# pg-lens

`pg-lens` is an experimental PostgreSQL extension for maintaining projection
tables, also called read models, from SQL views using asynchronous change
processing.

The idea is intentionally simple:

- define a SQL view projection for a denormalized read model
- register that view as a lens
- recompute affected rows after commit
- maintain a physical projection table for reads

For engineers evaluating PostgreSQL projections, PostgreSQL read models, CQRS
read models in PostgreSQL, or SQL view projections, `pg-lens` is an attempt to
package familiar PostgreSQL primitives into a narrower database projection
engine.

## Experimental Status

> `pg-lens` is experimental.
>
> The architecture is still evolving, the APIs may change, and it should not be
> used in production yet. The current goal is to explore the design, validate
> the core abstraction, and get feedback from database engineers and extension
> contributors.

## What pg-lens Is

`pg-lens` is:

- a PostgreSQL extension
- SQL-native
- view-driven
- focused on maintaining PostgreSQL materialized read models
- designed around PostgreSQL async processing
- intended to simplify projection workflows inside PostgreSQL

In practical terms, a lens is a registered SQL view plus a maintained target
table and a recompute key.

## What pg-lens Is Not

`pg-lens` is not:

- a trigger-based projection system
- a synchronous write-path feature
- a general streaming platform
- a Kafka replacement
- a SQL optimizer
- a full CDC framework
- a complex incremental SQL engine

It is also not trying to eliminate computation. The point is to control where
and how projection work happens so the OLTP path stays fast.

## Why pg-lens Exists

Many PostgreSQL applications want denormalized read models for dashboards,
search-friendly tables, CQRS-style query paths, or application-specific
projections.

Today, teams often choose between:

- hand-built projection code in the application
- trigger-based maintenance in the database
- external pipelines built around WAL, CDC, or queueing infrastructure

`pg-lens` exists to explore a narrower middle ground:

- use standard SQL views to define projections
- keep projection maintenance outside the synchronous transaction path
- recompute only the affected rows when possible
- integrate naturally with PostgreSQL extension and WAL architecture

## Architecture Overview

The intended conceptual flow is:

```text
source tables
-> SQL view
-> lens registration
-> async processing
-> projection table
```

The steady-state design is:

1. base tables change
2. changes are observed after commit
3. affected keys are derived when possible
4. the lens view is re-run for those keys
5. the projection table is updated with upsert-or-delete semantics

For the first milestone, the repository proves the recompute contract directly
and documents the future async path. It does not yet implement the PostgreSQL
WAL architecture or a background worker loop.

## Example Workflow

```sql
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

INSERT INTO accounts VALUES (1, 'a@example.com', 'active');

SELECT pglens.recompute_lens('account_projection', '1');

TABLE account_projection;
```

That example is intentionally manual. In the full design, recomputation would be
driven by committed changes and async processing rather than direct calls from
the write path.

## Development Status

Current v0 implementation:

- extension skeleton built with PGXS
- lens registration metadata
- projection table creation from a SQL view
- deterministic recompute of a single row by key
- idempotent upsert-or-delete behavior

Explicitly deferred:

- WAL decoding and committed change capture
- automatic affected-key discovery
- background worker execution
- checkpointing and restart recovery
- support for arbitrary projection SQL

Current limitations:

- only plain SQL views are supported
- the key column must be stable and identify one projection row
- the target table shape is copied from the view at registration time
- view changes after registration are not handled automatically
- negative-path coverage is still limited compared with the happy path

## When To Use pg-lens

`pg-lens` may be worth evaluating if you want:

- a PostgreSQL extension for read-model maintenance
- SQL view projections instead of a custom mapping DSL
- PostgreSQL async processing for projections after commit
- a small, explicit model for PostgreSQL materialized read models

## When Not To Use pg-lens

`pg-lens` is probably the wrong tool if you need:

- production-ready guarantees today
- full CDC platform capabilities
- synchronous consistency in the write transaction
- arbitrary incremental SQL maintenance
- a general event streaming system

## Build And Test

The recommended workflow is Docker-first so PostgreSQL development tooling does
not need to be installed on the host.

```bash
make docker-build
make docker-test
```

Open a shell inside the container:

```bash
make docker-shell
```

Inside the container:

```bash
make
sudo make install
pg_virtualenv make installcheck
```

If you already have local PostgreSQL extension tooling installed, the normal
PGXS workflow is also supported:

```bash
make
make install
make installcheck
```

## Contributing

Contributions are welcome, especially from engineers evaluating:

- PostgreSQL extension architecture
- PostgreSQL async processing patterns
- PostgreSQL read models and SQL view projections
- WAL-backed projection maintenance

Before contributing, read:

- `CONTRIBUTING.md`
- `AGENTS.md`
- `docs/ARCHITECTURE.md`
- `docs/DESIGN_PRINCIPLES.md`
- `docs/TESTING.md`

Please keep changes small, explicit, and honest about scope. If a feature is
deferred, document it rather than scaffolding around it.

## Future Possibilities

If the core idea continues to hold up, plausible next steps include:

- committed change capture using PostgreSQL WAL or logical decoding
- affected-key derivation for common projection patterns
- a background worker for async recompute execution
- better failure handling and restart semantics
- broader projection behavior tests

Those are possibilities, not promises. The project is still deciding which
parts are practical, maintainable, and consistent with a small extension.

## Additional Documentation

- `docs/architecture.md`
- `docs/terminology.md`
- `docs/projections.md`
- `docs/wal-design.md`
- `docs/ARCHITECTURE.md`
- `docs/DESIGN_PRINCIPLES.md`
- `docs/ROADMAP.md`
- `docs/DECISIONS.md`
- `docs/NON_GOALS.md`
- `docs/TERMINOLOGY.md`
- `docs/TESTING.md`
- `docs/adr/`

## License

MIT. See `LICENSE`.
