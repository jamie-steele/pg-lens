# PostgreSQL Projections With pg-lens

`pg-lens` is an experimental PostgreSQL extension for SQL view projections and
PostgreSQL read models.

This page explains the projection model the project is trying to support.

## What A Projection Means Here

In `pg-lens`, a projection is a denormalized read model defined by a SQL view
and stored in a physical table.

That makes the model:

- SQL-native
- inspectable with normal PostgreSQL tools
- easy to reason about for database engineers

## Projection Flow

The intended flow is:

1. define a SQL view that expresses the read model
2. register the view as a lens
3. detect committed source-table changes
4. derive affected keys
5. recompute only the affected projection rows
6. update the target projection table

## Why Use SQL Views

Using SQL views for PostgreSQL projections keeps the system aligned with normal
database practice:

- no custom projection language
- no application-only mapping layer
- no trigger-authored business logic as the core design

This makes `pg-lens` relevant to teams exploring:

- PostgreSQL read models
- SQL view projections
- PostgreSQL materialized read models
- CQRS read models in PostgreSQL

## Current v0 Projection Model

The current implementation is intentionally narrow.

It assumes:

- one logical projection row per key
- a stable key column
- deterministic recomputation by key
- a target table shape copied from the SQL view at registration time

The current repository proves the recompute behavior directly with a manual
recompute function. It does not yet provide automatic async maintenance.

## When This Model Fits

This approach may fit if you want:

- denormalized read tables inside PostgreSQL
- explicit SQL-defined read models
- a smaller alternative to custom projection pipelines

## When This Model Does Not Fit

This approach does not fit well if you need:

- general-purpose incremental SQL maintenance
- arbitrary query-shape support
- production-ready guarantees today
- synchronous projection updates during writes

## Important Limitation

Projection maintenance still has a cost. `pg-lens` does not make projection
work disappear. It only aims to move it into a more controlled async path.
