# pg-lens Architecture

`pg-lens` is an experimental PostgreSQL extension for maintaining PostgreSQL
projections and PostgreSQL read models from SQL views.

This page is a search-friendly architecture overview. For the more detailed
repository architecture document, see `docs/ARCHITECTURE.md`.

## Summary

The intended model is:

```text
source tables
-> SQL view
-> lens registration
-> async processing
-> projection table
```

A SQL view defines the logical projection. A maintained table stores the
physical read model. `pg-lens` aims to recompute only the affected rows, usually
by primary key, after commit.

## Why This Architecture

The project is exploring a narrow design for:

- PostgreSQL projections
- SQL view projections
- PostgreSQL materialized read models
- CQRS read models in PostgreSQL
- PostgreSQL async processing for read-model maintenance

The goal is not to eliminate computation. The goal is to control where and when
that computation runs so the synchronous OLTP path stays fast.

## Current Status

This architecture is only partially implemented today.

Current v0 covers:

- lens registration
- target table creation
- metadata storage
- recompute of one projection row by key
- upsert-or-delete semantics

Still deferred:

- PostgreSQL WAL architecture integration
- committed change capture
- affected-key derivation
- background worker execution
- checkpointing and recovery

## Design Boundaries

`pg-lens` is not:

- a trigger-based projection system
- a synchronous write-path feature
- a general streaming platform
- a Kafka replacement
- a full CDC framework
- a complex incremental SQL engine

It is an experimental database projection engine built as a PostgreSQL
extension, with a deliberately small scope.
