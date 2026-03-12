# ADR 0001: Use SQL Views For Projections

## Decision

Use standard SQL views as the logical definition of a projection.

## Context

`pg-lens` exists to package PostgreSQL primitives into a clean projection
maintenance abstraction. A custom mapping language would add complexity,
fragment tooling compatibility, and create a second query authoring surface.

## Alternatives Considered

- custom projection DSL
- imperative projection callbacks
- trigger-authored per-table projection logic

## Reasoning

SQL views are already:

- expressive
- familiar to PostgreSQL users
- compatible with existing tooling
- easy to inspect and optimize

Using views keeps the product definition narrow and avoids unnecessary
abstraction.
