# Testing

## Testing Philosophy

`pg-lens` is infrastructure software. Tests should focus on behavior,
correctness, and architectural promises rather than only code coverage.

## v0 Test Strategy

The first milestone uses SQL-driven integration tests via `pg_regress`.

This is the right default for a PostgreSQL extension because it exercises:

- extension installation
- SQL-callable APIs
- metadata persistence
- projection-table behavior
- DDL and SPI interactions inside PostgreSQL

## Current Coverage

- lens registration
- target-table creation
- metadata storage
- recompute insert behavior
- recompute update behavior
- recompute delete behavior
- idempotent upsert semantics

## Current Gaps

The current suite is intentionally small and mostly exercises the happy path.

It does not yet cover:

- registering a non-view source object
- registering a lens with a missing key column
- duplicate lens or target-table registration failures
- behavior after source-view shape changes

## Deferred But Required

As async processing is implemented, the suite should grow to include:

- restart and recovery behavior
- checkpoint handling
- worker crash scenarios
- duplicate event handling
- retry and idempotency guarantees

## Unit Tests

Pure C unit tests are possible for isolated string-building helpers, but they
should not replace SQL integration tests for extension behavior.

For now, integration tests are the main test surface because they validate the
actual PostgreSQL extension contract.

## Running Tests

Recommended host-clean workflow:

```bash
make docker-build
make docker-test
```

If you want an interactive container shell:

```bash
make docker-shell
```

From inside the container:

```bash
make
sudo make install
pg_virtualenv make installcheck
```

The repository also supports direct local PGXS commands for contributors who
already have PostgreSQL development tooling installed.

## Testing Rule

When changing recompute behavior or metadata semantics:

- add or update a regression test
- document any new invariant or limitation
