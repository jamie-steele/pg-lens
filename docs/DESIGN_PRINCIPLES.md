# Design Principles

## 1. Use SQL Views As The Projection Definition

The view is the contract.

`pg-lens` should not replace normal PostgreSQL query authoring with a custom
projection language unless a future need is overwhelming and well-justified.

## 2. Keep Projection Work Off The Write Path

Projection maintenance belongs after commit. The benefit of `pg-lens` is not
 eliminating recomputation. The benefit is moving that cost into a controlled
 async path.

## 3. Prefer PostgreSQL Primitives

Use native PostgreSQL mechanisms before inventing framework-specific machinery:

- extension APIs
- background workers
- WAL/logical decoding
- SQL-callable functions
- normal tables, views, and indexes

## 4. Constrain Scope Aggressively

v0 should prove the architecture, not solve every projection problem.

Prefer:

- one row per entity
- stable keys
- deterministic recompute
- simple joins

Defer:

- arbitrary SQL support
- generalized dependency planners
- distributed coordination

## 5. Keep C Code Small And Explicit

- small modules
- clear header boundaries
- minimal global state
- readable control flow
- defensive checks around critical assumptions

Avoid:

- clever macros
- pointer tricks used for style rather than need
- layering that hides the real PostgreSQL behavior

## 6. Make Tradeoffs Visible

Important limitations should be explicit in:

- code comments
- architecture documents
- ADRs
- tests

If a feature is scaffolded, say that it is scaffolded.

## 7. Favor Honest Interfaces

Do not pretend async change capture exists before it does. A smaller truthful
system is better than a broader misleading one.

## 8. Optimize For Maintenance

This project is infrastructure software. Future contributors should be able to
understand:

- where each responsibility lives
- why a boundary exists
- what is intentionally unsupported
