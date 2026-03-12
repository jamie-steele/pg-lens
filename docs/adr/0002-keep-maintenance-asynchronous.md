# ADR 0002: Keep Maintenance Asynchronous

## Decision

Projection maintenance must happen after commit and outside the synchronous
transaction path.

## Context

The main architectural value of `pg-lens` is separating OLTP writes from
projection maintenance work. If projection updates run synchronously during the
write transaction, the system collapses back into the class of trigger-style
solutions that this project intentionally avoids.

## Alternatives Considered

- synchronous triggers
- synchronous application-side projection writes
- optional mixed sync/async maintenance

## Reasoning

Asynchronous maintenance:

- protects write-path latency
- makes recompute cost explicit
- keeps projection work operationally separable
- fits PostgreSQL background-worker and WAL-based mechanisms

This choice intentionally leaves eventual-consistency tradeoffs visible.
