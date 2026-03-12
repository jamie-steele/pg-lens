# WAL Design Notes

`pg-lens` is intended to become a WAL-backed PostgreSQL async processing system
for maintaining projection tables from SQL views.

This document describes the direction of that design. It is not a description of
completed functionality.

## Why WAL Matters

The long-term design avoids triggers and avoids synchronous projection work in
the write transaction.

That pushes the project toward PostgreSQL WAL architecture and related
post-commit mechanisms because the system needs a way to observe committed
changes without turning projection maintenance into a trigger-based feature.

## Intended Model

The intended flow is:

```text
WAL or committed change source
-> affected key extraction
-> async recompute work
-> SELECT from SQL view by key
-> upsert or delete in projection table
```

## Why This Is Interesting

If it works well, this design could provide:

- a PostgreSQL extension approach to async projections
- SQL-defined PostgreSQL read models
- a simpler projection pipeline than application-managed fanout code

## What Is Not Decided Yet

The project has not yet committed to:

- a specific logical decoding strategy
- a queue representation
- a checkpoint format
- a worker concurrency model
- failure recovery semantics

Those details are intentionally deferred because the project is still validating
the smaller recompute-by-key model first.

## What Exists Today

Today, `pg-lens` does not implement PostgreSQL WAL integration.

The current repository only proves:

- lens registration
- projection table creation
- metadata storage
- manual recomputation by key

## Evaluation Guidance

If you are evaluating `pg-lens` for PostgreSQL WAL architecture work, treat the
current state as an architectural exploration, not an operational solution.

The project is trying to answer whether WAL-backed async projection maintenance
can stay small, explicit, and understandable inside PostgreSQL.
