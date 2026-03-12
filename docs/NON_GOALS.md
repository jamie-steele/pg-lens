# Non-Goals

`pg-lens` is intentionally narrow.

It is not:

- a trigger framework
- a synchronous write-path feature
- a general streaming platform
- a Kafka replacement
- a custom SQL DSL
- a query optimizer
- a full CDC platform
- a general-purpose incremental view maintenance engine

## v0 Non-Goals

The first milestone does not attempt to provide:

- arbitrary SQL support
- automatic dependency inference across complex query shapes
- multi-node coordination
- zero-cost recomputation
- hidden magic around performance

## Performance Reality

Projection recomputation still costs CPU, IO, and query-planning effort.

The intended value is:

- controlled async execution
- simpler operational shape than custom projection pipelines
- explicit, SQL-native projection definitions

Users are expected to optimize their projection views and indexes.
