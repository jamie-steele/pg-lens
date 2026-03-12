CREATE EXTENSION pglens;

CREATE TABLE accounts (
    id bigint PRIMARY KEY,
    email text NOT NULL,
    status text NOT NULL
);

CREATE VIEW account_projection_view AS
SELECT
    id,
    email,
    status
FROM accounts;

SELECT pglens.register_lens(
    'account_projection',
    'account_projection_view'::regclass,
    'public',
    'account_projection',
    'id'
);

SELECT
    lens_name || '|' || view_name || '|' || target_name || '|' || key_column
        AS lens_summary
FROM pglens.lenses
ORDER BY lens_name;

SELECT
    string_agg(attname, ',' ORDER BY attnum) AS projection_columns
FROM pg_attribute
WHERE attrelid = 'public.account_projection'::regclass
  AND attnum > 0
  AND NOT attisdropped;
