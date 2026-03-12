# AGENTS.md

This repository hosts `pg-lens`, a serious PostgreSQL extension project for
maintaining projection tables from SQL views using asynchronous change
processing.

## Non-Negotiable Architecture Rules

- preserve the core model: `view = logical projection`, `table = maintained storage`
- keep projection maintenance asynchronous and post-commit
- never move projection work into the synchronous OLTP transaction path
- never introduce triggers as the core architecture
- do not invent a custom DSL when normal SQL views are sufficient
- keep v0 minimal and honest about what is implemented versus deferred

## Design Priorities

- clarity
- maintainability
- correctness
- minimalism
- strong documentation
- explicit architecture boundaries

## Implementation Guidance

- keep modules small and focused
- prefer explicit code over cleverness or magic
- avoid unnecessary abstractions, frameworks, or enterprise-style layering
- use PostgreSQL primitives directly where they fit
- separate domain concepts from infrastructure plumbing
- add defensive checks around assumptions that matter to correctness

## Domain Concepts To Preserve

- Lens
- Projection Definition
- Projection State
- Change Event
- Recompute Key
- Projection Worker
- Checkpoint
- Target Table

Each concept should have one clear responsibility. Do not collapse unrelated
responsibilities into a single module unless doing so clearly reduces
complexity.

## Documentation Requirements

When changing behavior or structure:

- explain why the change exists
- describe tradeoffs and limitations
- update the relevant document in `docs/`
- add or update an ADR when the change affects architecture

## v0 Discipline

For the first milestone, prefer:

- one-row-per-entity projections
- stable primary-key recomputation
- simple joins
- deterministic recompute behavior

Do not add:

- arbitrary incremental SQL maintenance
- distributed systems complexity
- speculative abstractions for future scale
- fake completeness around WAL capture or worker internals

If a feature is deferred, say so clearly in code and documentation.
