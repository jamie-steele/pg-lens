# Architecture

## Purpose

`pg-lens` is an experimental PostgreSQL extension for maintaining projection
tables from SQL views while keeping projection maintenance off the synchronous
write path.

It is not a general incremental view maintenance engine. It is an exploration of
whether a small, view-driven PostgreSQL projection engine can provide useful
PostgreSQL read models without moving work into the OLTP transaction path.

Because the project is early, this document describes both the current vertical
slice and the intended architectural direction. Those are not the same thing.

## Core Model

- a SQL view defines the logical projection
- a target table stores the maintained read model
- a lens metadata row binds the view, target table, and recompute key
- a worker eventually recomputes affected keys after commit

In the steady-state design:

1. committed base-table changes are observed after commit
2. affected projection keys are derived from those changes
3. a worker runs a key-filtered query against the source view
4. the target table is updated with upsert-or-delete semantics

Conceptually:

```text
source tables
-> SQL view
-> lens registration
-> async processing
-> projection table
```

## v0 Boundary

Implemented in this repository:

- extension and SQL API skeleton
- metadata table for registered lenses
- target table creation from the registered view
- deterministic recompute of a single key
- idempotent target-row maintenance

Deferred beyond v0:

- WAL decoding
- change queueing
- background worker event loop
- checkpoint persistence
- restart recovery logic
- automatic affected-key derivation

This distinction matters. The current codebase demonstrates the recompute model
for one key. It does not yet provide the full PostgreSQL async processing path.

## Modules

### `src/pglens.c`

Extension entrypoints and SQL-callable wrappers. This file should stay small.
It owns argument conversion and SPI lifecycle, not business logic.

### `src/catalog.c`

Lens metadata and relation inspection. This module knows how to:

- verify that a registered source object is a plain view
- verify key-column presence
- create the target table
- insert and load lens metadata

### `src/recompute.c`

Key-based recompute behavior. This is the core of the v0 proof:

- load one registered lens
- select a source row by key
- upsert into the target table
- delete the target row when the source row disappears

## Data Model

`pglens.lenses` stores:

- lens name
- source view OID and identity
- target table OID and identity
- recompute key column
- registration timestamp

This is intentionally small. v0 does not yet store worker checkpoints, retry
state, or dependency graphs.

## Deferred Async Path

The async model is part of the product definition, but it does not need a code
module in this milestone.

For v0, the repository proves the recompute contract directly and documents the
future path:

`committed change -> affected key -> async worker -> recompute by key`

Keeping the future worker out of the codebase for now is a deliberate
simplification. It avoids exposing APIs for behavior that does not exist yet.

## Recompute Semantics

The recompute path is whole-row by key, not partial incremental SQL
maintenance.

Given a lens and one key:

1. query the source view for that key
2. if one row exists, upsert it into the target table
3. if no row exists, delete the existing target row

This model is chosen because it is simple, explicit, and compatible with the
goal of preserving a fast OLTP path.

It does not eliminate computation. A PostgreSQL projection still costs planning,
execution, CPU, and IO. The point is to control when and where that work runs.

## Current Assumptions

- a lens view yields at most one row per key
- the key column is stable
- the target table shape matches the source view shape at registration time
- view changes after registration are unsupported in v0

## Why This Shape

This architecture keeps the product definition clear:

`pg-lens` is packaged PostgreSQL infrastructure for async, key-based projection
maintenance driven by SQL views.

That makes it easier to evaluate for engineers searching for:

- PostgreSQL extension approaches to read models
- PostgreSQL projections from SQL views
- PostgreSQL materialized read models
- CQRS read models in PostgreSQL
