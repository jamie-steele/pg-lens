# Decisions

This document summarizes the current architectural decisions for `pg-lens`.
Detailed reasoning lives in `docs/adr/`.

## Accepted Decisions

- use standard SQL views as projection definitions
- keep projection maintenance asynchronous and post-commit
- model recomputation around a stable key
- implement v0 as a narrow vertical slice rather than a broad platform
- keep triggers out of the core architecture

## Consequences

- the system stays small and understandable
- the write path remains conceptually clean
- recomputation cost is explicit rather than hidden
- users must design projection views that behave well under key-filtered access

## Deferred Decisions

- exact WAL integration mechanism
- queue representation and checkpoint format
- worker concurrency model
- operational observability surface

## Related ADRs

- `docs/adr/0001-use-sql-views-for-projections.md`
- `docs/adr/0002-keep-maintenance-asynchronous.md`
- `docs/adr/0003-recompute-by-stable-key.md`
- `docs/adr/0004-ship-a-minimal-v0-slice.md`
