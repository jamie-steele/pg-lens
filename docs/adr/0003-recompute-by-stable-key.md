# ADR 0003: Recompute By Stable Key

## Decision

Use primary-key-based recomputation as the default maintenance model.

## Context

Arbitrary incremental SQL maintenance is large, difficult, and far outside the
intended scope of `pg-lens` v0. The project needs a simple, deterministic model
that supports common read-model use cases.

## Alternatives Considered

- general incremental expression maintenance
- table-level full refreshes
- trigger-authored row mutation logic

## Reasoning

Stable-key recomputation provides:

- simple mental model
- explicit failure modes
- compatibility with view-defined projections
- reasonable efficiency when views and indexes are designed well

The chosen model is:

`change -> affected key -> select view row by key -> upsert/delete target row`

This does not eliminate recomputation cost. It constrains it.
