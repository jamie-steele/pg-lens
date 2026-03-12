# Contributing

Thanks for contributing to `pg-lens`.

This project is intentionally small in scope and should stay that way. The goal
is production-quality infrastructure software with a narrow, explicit contract.

## Working Principles

- keep the architecture view-driven and SQL-native
- keep projection work out of the synchronous transaction path
- do not introduce triggers as the core mechanism
- prefer simple, explicit C over clever abstractions
- document decisions, tradeoffs, and limitations as you change the system
- defer features that do not strengthen the current milestone

## Development Workflow

1. read `README.md`, `AGENTS.md`, and the documents in `docs/`
2. use the Docker workflow unless you already have PostgreSQL development tooling installed
3. build with `make docker-build`
4. run the regression suite with `make docker-test`
5. if using a local PGXS environment instead, run `make`, `make install`, and `make installcheck`
6. update or add tests for behavioral changes
7. update documentation when architecture, scope, or behavior changes

## Code Expectations

- keep modules small and focused
- use function-level comments for non-obvious logic
- avoid hidden control flow and surprising macros
- preserve clear boundaries between domain concepts and PostgreSQL plumbing
- prefer failing explicitly over silently accepting invalid state

## Documentation Expectations

Every meaningful change should answer:

- why does this exist?
- what tradeoff does it make?
- what is still intentionally unsupported?

Use `docs/adr/` when a change introduces or revises an architectural decision.

## Scope Discipline

Before adding a feature, ask:

- does it help the minimal async projection model?
- is it required for the current milestone?
- can it be expressed with normal PostgreSQL primitives?

If the answer is no, defer it and document the reason.
