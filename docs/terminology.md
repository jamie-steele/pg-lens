# pg-lens Terminology

This page defines the core terms used in `pg-lens`.

For the canonical terminology reference used by the repository, see
`docs/TERMINOLOGY.md`.

## Lens

A registered projection definition. A lens binds a SQL view, a target table,
and the recompute key used to maintain that projection.

## Projection

A denormalized read model derived from one or more source tables. In `pg-lens`,
the logical definition of the projection is a standard SQL view.

## SQL View Projection

A projection defined with normal PostgreSQL SQL rather than a custom DSL. This
is a central design choice in `pg-lens`.

## Target Table

The physical table that stores the maintained read model. This is the table
queried by readers after projection maintenance has run.

## Recompute Key

The stable identifier used to recompute one projection row. In v0, the intended
shape is one projection row per key.

## Change Event

A committed database change that may require one or more projection rows to be
recomputed. This is part of the intended async design and is not fully
implemented yet.

## Async Processing

Projection work that happens after commit and outside the synchronous write
transaction. This is essential to the long-term design of `pg-lens`.

## PostgreSQL WAL Architecture

The PostgreSQL write-ahead log and related decoding mechanisms that may
eventually provide committed change information for `pg-lens`. This part of the
design is still under exploration.

## Projection Worker

The future background component that would consume committed change information
and run recompute operations asynchronously. This is deferred beyond the current
vertical slice.
