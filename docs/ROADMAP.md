# Roadmap

This roadmap is descriptive, not a promise of delivery. `pg-lens` is still an
experimental PostgreSQL extension, and some ideas listed here may change or be
removed as the design becomes clearer.

## Milestone 0: Architectural Vertical Slice

Goal: prove the core abstraction without overbuilding.

Included:

- extension packaging
- lens registration
- metadata storage
- target-table creation
- key-based recompute
- idempotent upsert-or-delete behavior
- SQL-driven regression tests

Not included:

- automatic change capture
- background processing loop
- checkpointing
- retries
- operator-facing observability

## Future Possibility 1: Post-Commit Change Capture

Goal: replace manual recompute calls with committed change ingestion.

Expected work:

- evaluate a PostgreSQL WAL or logical decoding approach
- define affected-key extraction contract
- enqueue committed recompute work
- preserve OLTP-path isolation

## Future Possibility 2: Projection Worker

Goal: execute recomputes asynchronously inside PostgreSQL.

Expected work:

- background worker registration
- work polling and batching
- checkpoint handling
- restart-safe processing rules

## Future Possibility 3: Operational Hardening

Goal: make the system easier to run safely.

Expected work:

- operator-facing diagnostics
- performance guidance
- clearer failure handling
- more restart and crash-recovery tests

## Ongoing Rule

Every roadmap step should preserve the same core idea:

`SQL view -> affected key -> recompute row -> maintain target table`
