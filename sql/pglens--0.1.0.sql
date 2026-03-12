CREATE SCHEMA pglens;

COMMENT ON SCHEMA pglens IS
'Metadata and SQL API for pg-lens.';

CREATE TABLE pglens.lenses (
	lens_name text PRIMARY KEY,
	view_oid regclass NOT NULL UNIQUE,
	view_schema text NOT NULL,
	view_name text NOT NULL,
	target_oid regclass NOT NULL UNIQUE,
	target_schema text NOT NULL,
	target_name text NOT NULL,
	key_column text NOT NULL,
	created_at timestamptz NOT NULL DEFAULT clock_timestamp()
);

COMMENT ON TABLE pglens.lenses IS
'Registered projection definitions. Each row ties a source view to a maintained target table.';

COMMENT ON COLUMN pglens.lenses.key_column IS
'Primary recompute key for v0. pg-lens re-selects a single projection row by this column.';

CREATE FUNCTION pglens.register_lens(
	lens_name text,
	source_view regclass,
	target_schema text,
	target_name text,
	key_column text
) RETURNS void
AS 'MODULE_PATHNAME', 'pglens_register_lens'
LANGUAGE C
STRICT;

COMMENT ON FUNCTION pglens.register_lens(text, regclass, text, text, text) IS
'Registers a view-backed lens, creates the physical target table, and records lens metadata.';

CREATE FUNCTION pglens.recompute_lens(
	lens_name text,
	key_text text
) RETURNS boolean
AS 'MODULE_PATHNAME', 'pglens_recompute_lens'
LANGUAGE C
STRICT;

COMMENT ON FUNCTION pglens.recompute_lens(text, text) IS
'Recomputes one projection row by key and upserts or deletes the maintained target row.';
