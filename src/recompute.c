#include "postgres.h"

#include "executor/spi.h"
#include "lib/stringinfo.h"
#include "utils/builtins.h"

#include "pglens.h"

/*
 * Recompute remains deliberately explicit in v0:
 * 1. load lens metadata
 * 2. select the current row for one key from the source view
 * 3. upsert into the maintained projection table
 * 4. delete the projection row when the source view no longer returns one
 */

static void
pglens_require_spi_result(int rc, int expected, const char *context)
{
	if (rc != expected)
		ereport(ERROR,
				(errmsg("pg-lens %s failed", context),
				 errdetail("SPI returned %d, expected %d.", rc, expected)));
}

static char *
pglens_join_column_list(const PGLensColumnList *columns)
{
	StringInfoData list;
	int				i;

	initStringInfo(&list);

	for (i = 0; i < columns->count; i++)
	{
		if (i > 0)
			appendStringInfoString(&list, ", ");

		appendStringInfoString(&list, quote_identifier(columns->names[i]));
	}

	return list.data;
}

static char *
pglens_build_conflict_clause(const PGLensColumnList *columns,
							 const char *key_column)
{
	StringInfoData clause;
	int				i;
	bool			has_updates = false;

	initStringInfo(&clause);
	appendStringInfo(&clause,
					 "ON CONFLICT (%s) ",
					 quote_identifier(key_column));

	for (i = 0; i < columns->count; i++)
	{
		char *column_name = columns->names[i];

		if (strcmp(column_name, key_column) == 0)
			continue;

		if (!has_updates)
		{
			appendStringInfoString(&clause, "DO UPDATE SET ");
			has_updates = true;
		}
		else
		{
			appendStringInfoString(&clause, ", ");
		}

		appendStringInfo(&clause,
						 "%s = EXCLUDED.%s",
						 quote_identifier(column_name),
						 quote_identifier(column_name));
	}

	if (!has_updates)
		appendStringInfoString(&clause, "DO NOTHING");

	return clause.data;
}

static bool
pglens_relation_contains_key(const char *qualified_relation,
							 const char *quoted_key_column,
							 const char *key_literal)
{
	StringInfoData	sql;
	int				rc;
	HeapTuple		tuple;
	TupleDesc		tupdesc;
	bool			isnull;
	bool			exists;

	initStringInfo(&sql);
	appendStringInfo(&sql,
					 "SELECT EXISTS ("
					 "  SELECT 1 FROM %s WHERE %s = %s"
					 ")",
					 qualified_relation,
					 quoted_key_column,
					 key_literal);

	rc = SPI_execute(sql.data, true, 1);
	pglens_require_spi_result(rc, SPI_OK_SELECT, "projection existence check");

	tuple = SPI_tuptable->vals[0];
	tupdesc = SPI_tuptable->tupdesc;
	exists = DatumGetBool(SPI_getbinval(tuple, tupdesc, 1, &isnull));

	return exists;
}

bool
pglens_recompute_lens_internal(const char *lens_name, const char *key_text)
{
	PGLensDefinition	lens;
	PGLensColumnList	columns;
	char			   *qualified_view;
	char			   *qualified_target;
	char			   *column_list;
	char			   *conflict_clause;
	char			   *key_type_sql;
	char			   *key_literal;
	const char		   *quoted_key_column;
	StringInfoData		sql;
	int					rc;

	pglens_catalog_load_lens(lens_name, &lens);

	columns = pglens_catalog_load_columns(lens.target_oid);
	key_type_sql = pglens_catalog_lookup_column_type_sql(lens.target_oid,
														 lens.key_column);
	qualified_view = pglens_catalog_qualified_name(lens.view_schema,
												   lens.view_name);
	qualified_target = pglens_catalog_qualified_name(lens.target_schema,
													 lens.target_name);
	column_list = pglens_join_column_list(&columns);
	conflict_clause = pglens_build_conflict_clause(&columns, lens.key_column);
	quoted_key_column = quote_identifier(lens.key_column);
	key_literal = psprintf("%s::%s",
						   quote_literal_cstr(key_text),
						   key_type_sql);

	initStringInfo(&sql);
	appendStringInfo(&sql,
					 "WITH source_rows AS ("
					 "  SELECT %s FROM %s WHERE %s = %s"
					 "), upserted AS ("
					 "  INSERT INTO %s (%s) "
					 "  SELECT %s FROM source_rows "
					 "  %s "
					 "  RETURNING 1"
					 ") "
					 "DELETE FROM %s "
					 "WHERE %s = %s "
					 "  AND NOT EXISTS (SELECT 1 FROM source_rows)",
					 column_list,
					 qualified_view,
					 quoted_key_column,
					 key_literal,
					 qualified_target,
					 column_list,
					 column_list,
					 conflict_clause,
					 qualified_target,
					 quoted_key_column,
					 key_literal);

	rc = SPI_execute(sql.data, false, 0);
	pglens_require_spi_result(rc, SPI_OK_DELETE, "projection recompute");

	return pglens_relation_contains_key(qualified_view,
										quoted_key_column,
										key_literal);
}
