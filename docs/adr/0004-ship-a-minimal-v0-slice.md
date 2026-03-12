# ADR 0004: Ship A Minimal v0 Slice

## Decision

Implement a narrow vertical slice first: registration, metadata, target-table
creation, and key-based recompute, while explicitly deferring WAL integration
and background processing.

## Context

The full product vision includes post-commit change capture, affected-key
derivation, queueing, and worker execution. Implementing all of that up front
would create a large and risky bootstrap surface.

## Alternatives Considered

- build the entire async pipeline before publishing the repository
- ship documentation only with no executable slice
- prototype with shortcuts that hide deferred work

## Reasoning

The chosen slice is the smallest honest proof of the architecture because it:

- validates the lens abstraction
- validates projection-table creation
- validates recompute semantics
- leaves the async seam explicit
- avoids misleading claims of completeness

This ADR favors truthfulness and maintainability over breadth.
