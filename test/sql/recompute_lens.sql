CREATE TABLE accounts_recompute (
    id bigint PRIMARY KEY,
    email text NOT NULL,
    status text NOT NULL
);

CREATE VIEW account_projection_view_recompute AS
SELECT
    id,
    email,
    status
FROM accounts_recompute;

SELECT pglens.register_lens(
    'account_projection_recompute',
    'account_projection_view_recompute'::regclass,
    'public',
    'account_projection_recompute',
    'id'
);

INSERT INTO accounts_recompute VALUES (1, 'a@example.com', 'active');

SELECT pglens.recompute_lens('account_projection_recompute', '1');

SELECT id || '|' || email || '|' || status AS projection_row
FROM account_projection_recompute
ORDER BY id;

UPDATE accounts_recompute
SET status = 'disabled'
WHERE id = 1;

SELECT pglens.recompute_lens('account_projection_recompute', '1');

SELECT id || '|' || email || '|' || status AS projection_row
FROM account_projection_recompute
ORDER BY id;

DELETE FROM accounts_recompute
WHERE id = 1;

SELECT pglens.recompute_lens('account_projection_recompute', '1');

SELECT count(*) FROM account_projection_recompute;
