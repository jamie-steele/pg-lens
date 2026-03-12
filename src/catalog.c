#include "postgres.h"

#include "access/table.h"
#include "catalog/namespace.h"
#include "catalog/pg_attribute.h"
#include "catalog/pg_class.h"
#include "catalog/pg_type_d.h"
#include "executor/spi.h"
#include "lib/stringinfo.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"

#include "pglens.h"

/*
 * Catalog helpers isolate metadata access and relation inspection so the
 * projection logic can stay focused on recomputation behavior.
 */

static void
pglens_require_spi_result(int rc, int expected, const char *context)
{
	if (rc != expected)
		ereport(ERROR,
				(errmsg("pg-lens %s failed", context),
				 errdetail("SPI returned %d, expected %d.", rc, expected)));
}

char *
pglens_catalog_qualified_name(const char *schema_name, const char *object_name)
{
	return quote_qualified_identifier(schema_name, object_name);
}

void
pglens_catalog_assert_key_column_exists(Oid relid, const char *column_name)
{
	Relation	rel;
	TupleDesc	desc;
	int			attnum;

	rel = table_open(relid, AccessShareLock);
	desc = RelationGetDescr(rel);
	attnum = get_attnum(relid, column_name);

	if (attnum == InvalidAttrNumber)
		ereport(ERROR,
				(errmsg("pg-lens key column \"%s\" does not exist", column_name),
				 errdetail("Relation \"%s\" has no visible column named \"%s\".",
						   RelationGetRelationName(rel),
						   column_name)));

	if (TupleDescAttr(desc, attnum - 1)->attisdropped)
		ereport(ERROR,
				(errmsg("pg-lens key column \"%s\" is dropped", column_name)));

	table_close(rel, AccessShareLock);
}

PGLensColumnList
pglens_catalog_load_columns(Oid relid)
{
	PGLensColumnList columns = {0};
	Relation		rel;
	TupleDesc		desc;
	int				i;
	int				visible_columns = 0;

	rel = table_open(relid, AccessShareLock);
	desc = RelationGetDescr(rel);

	for (i = 0; i < desc->natts; i++)
	{
		Form_pg_attribute attr = TupleDescAttr(desc, i);

		if (!attr->attisdropped && attr->attnum > 0)
			visible_columns++;
	}

	columns.count = visible_columns;
	columns.names = palloc0(sizeof(char *) * visible_columns);

	visible_columns = 0;

	for (i = 0; i < desc->natts; i++)
	{
		Form_pg_attribute attr = TupleDescAttr(desc, i);

		if (attr->attisdropped || attr->attnum <= 0)
			continue;

		columns.names[visible_columns++] = pstrdup(NameStr(attr->attname));
	}

	table_close(rel, AccessShareLock);

	return columns;
}

char *
pglens_catalog_lookup_column_type_sql(Oid relid, const char *column_name)
{
	Relation	rel;
	TupleDesc	desc;
	int			i;
	char	   *type_sql = NULL;

	rel = table_open(relid, AccessShareLock);
	desc = RelationGetDescr(rel);

	for (i = 0; i < desc->natts; i++)
	{
		Form_pg_attribute attr = TupleDescAttr(desc, i);

		if (attr->attisdropped || attr->attnum <= 0)
			continue;

		if (strcmp(NameStr(attr->attname), column_name) == 0)
		{
			type_sql = format_type_be(attr->atttypid);
			break;
		}
	}

	table_close(rel, AccessShareLock);

	if (type_sql == NULL)
		ereport(ERROR,
				(errmsg("pg-lens key column \"%s\" does not exist", column_name)));

	return type_sql;
}

void
pglens_catalog_insert_lens(const PGLensDefinition *lens)
{
	static const char *sql =
		"INSERT INTO pglens.lenses ("
		"  lens_name, view_oid, view_schema, view_name,"
		"  target_oid, target_schema, target_name, key_column"
		") VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
	Oid			argtypes[8] = {TEXTOID, REGCLASSOID, TEXTOID, TEXTOID,
								REGCLASSOID, TEXTOID, TEXTOID, TEXTOID};
	Datum		values[8];
	char		nulls[8] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
	int			rc;

	values[0] = CStringGetTextDatum(lens->lens_name);
	values[1] = ObjectIdGetDatum(lens->view_oid);
	values[2] = CStringGetTextDatum(lens->view_schema);
	values[3] = CStringGetTextDatum(lens->view_name);
	values[4] = ObjectIdGetDatum(lens->target_oid);
	values[5] = CStringGetTextDatum(lens->target_schema);
	values[6] = CStringGetTextDatum(lens->target_name);
	values[7] = CStringGetTextDatum(lens->key_column);

	rc = SPI_execute_with_args(sql, 8, argtypes, values, nulls, false, 0);
	pglens_require_spi_result(rc, SPI_OK_INSERT, "metadata insert");
}

void
pglens_catalog_load_lens(const char *lens_name, PGLensDefinition *lens)
{
	static const char *sql =
		"SELECT lens_name, view_oid, view_schema, view_name,"
		"       target_oid, target_schema, target_name, key_column "
		"FROM pglens.lenses "
		"WHERE lens_name = $1";
	Oid			argtypes[1] = {TEXTOID};
	Datum		values[1];
	char		nulls[1] = {' '};
	int			rc;
	HeapTuple	tuple;
	TupleDesc	tupdesc;
	bool		isnull;

	values[0] = CStringGetTextDatum(lens_name);

	rc = SPI_execute_with_args(sql, 1, argtypes, values, nulls, true, 1);
	pglens_require_spi_result(rc, SPI_OK_SELECT, "metadata lookup");

	if (SPI_processed != 1)
		ereport(ERROR,
				(errmsg("pg-lens lens \"%s\" is not registered", lens_name)));

	tuple = SPI_tuptable->vals[0];
	tupdesc = SPI_tuptable->tupdesc;

	lens->lens_name = SPI_getvalue(tuple, tupdesc, 1);
	lens->view_oid = DatumGetObjectId(SPI_getbinval(tuple, tupdesc, 2, &isnull));
	lens->view_schema = SPI_getvalue(tuple, tupdesc, 3);
	lens->view_name = SPI_getvalue(tuple, tupdesc, 4);
	lens->target_oid = DatumGetObjectId(SPI_getbinval(tuple, tupdesc, 5, &isnull));
	lens->target_schema = SPI_getvalue(tuple, tupdesc, 6);
	lens->target_name = SPI_getvalue(tuple, tupdesc, 7);
	lens->key_column = SPI_getvalue(tuple, tupdesc, 8);
}

void
pglens_register_lens_internal(const char *lens_name,
							  Oid view_oid,
							  const char *target_schema,
							  const char *target_name,
							  const char *key_column)
{
	char			   *view_schema;
	char			   *view_name;
	char			   *qualified_view;
	char			   *qualified_target;
	StringInfoData		sql;
	char				relkind;
	Oid					target_namespace_oid;
	Oid					target_oid;
	PGLensDefinition	lens;
	int					rc;

	relkind = get_rel_relkind(view_oid);

	if (relkind != RELKIND_VIEW)
		ereport(ERROR,
				(errmsg("pg-lens only supports SQL views in v0"),
				 errdetail("Object %u is not a plain view.", view_oid)));

	view_schema = get_namespace_name(get_rel_namespace(view_oid));
	view_name = get_rel_name(view_oid);

	if (view_schema == NULL || view_name == NULL)
		ereport(ERROR,
				(errmsg("pg-lens could not resolve view metadata")));

	pglens_catalog_assert_key_column_exists(view_oid, key_column);

	qualified_view = pglens_catalog_qualified_name(view_schema, view_name);
	qualified_target = pglens_catalog_qualified_name(target_schema, target_name);

	initStringInfo(&sql);
	appendStringInfo(&sql,
					 "CREATE TABLE %s AS TABLE %s WITH NO DATA",
					 qualified_target,
					 qualified_view);

	rc = SPI_execute(sql.data, false, 0);
	pglens_require_spi_result(rc, SPI_OK_UTILITY, "projection table creation");

	resetStringInfo(&sql);
	appendStringInfo(&sql,
					 "ALTER TABLE %s ADD PRIMARY KEY (%s)",
					 qualified_target,
					 quote_identifier(key_column));

	rc = SPI_execute(sql.data, false, 0);
	pglens_require_spi_result(rc, SPI_OK_UTILITY, "projection key constraint");

	target_namespace_oid = get_namespace_oid(target_schema, false);
	target_oid = get_relname_relid(target_name, target_namespace_oid);

	if (!OidIsValid(target_oid))
		ereport(ERROR,
				(errmsg("pg-lens could not resolve target table \"%s.%s\"",
						target_schema,
						target_name)));

	lens.lens_name = pstrdup(lens_name);
	lens.view_oid = view_oid;
	lens.view_schema = pstrdup(view_schema);
	lens.view_name = pstrdup(view_name);
	lens.target_oid = target_oid;
	lens.target_schema = pstrdup(target_schema);
	lens.target_name = pstrdup(target_name);
	lens.key_column = pstrdup(key_column);

	pglens_catalog_insert_lens(&lens);
}
