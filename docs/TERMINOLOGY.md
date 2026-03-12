# Terminology

## Lens

A registered projection definition. A lens binds a source SQL view, a target
table, and the key used for recomputation.

## Projection Definition

The logical mapping from normalized source tables into a read model. In
`pg-lens`, the projection definition is a standard SQL view.

## Target Table

The physical table maintained by `pg-lens`. It stores the projection rows used
for reads.

## Projection State

The stored state of a lens, including the target table contents and metadata
needed to maintain it.

## Change Event

A committed database change that may affect one or more projection keys. This
is part of the intended async design and is not fully implemented in v0.

## Recompute Key

The stable identifier used to recompute one projection row. v0 assumes a
one-row-per-key model.

## Projection Worker

The async execution component that consumes committed change information and
runs recomputes outside the OLTP transaction path.

## Checkpoint

Persisted progress information used to resume async processing safely after
restart. This is deferred beyond v0.
