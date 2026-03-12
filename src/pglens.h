#ifndef PGLENS_H
#define PGLENS_H

#include "postgres.h"

/*
 * A registered lens ties together the logical view, the maintained target
 * table, and the primary-key column used for deterministic recomputation.
 */
typedef struct PGLensDefinition
{
	char   *lens_name;
	Oid		view_oid;
	char   *view_schema;
	char   *view_name;
	Oid		target_oid;
	char   *target_schema;
	char   *target_name;
	char   *key_column;
} PGLensDefinition;

/*
 * v0 keeps column handling deliberately simple: the projection row shape is
 * the ordered column list of the target table created from the source view.
 */
typedef struct PGLensColumnList
{
	int		count;
	char  **names;
} PGLensColumnList;

extern void pglens_register_lens_internal(const char *lens_name,
										  Oid view_oid,
										  const char *target_schema,
										  const char *target_name,
										  const char *key_column);
extern bool pglens_recompute_lens_internal(const char *lens_name,
										   const char *key_text);

extern void pglens_catalog_load_lens(const char *lens_name,
									 PGLensDefinition *lens);
extern void pglens_catalog_insert_lens(const PGLensDefinition *lens);
extern void pglens_catalog_assert_key_column_exists(Oid relid,
													const char *column_name);
extern PGLensColumnList pglens_catalog_load_columns(Oid relid);
extern char *pglens_catalog_lookup_column_type_sql(Oid relid,
												   const char *column_name);
extern char *pglens_catalog_qualified_name(const char *schema_name,
										   const char *object_name);

#endif
