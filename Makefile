EXTENSION = pglens
MODULE_big = pglens
OBJS = \
	src/pglens.o \
	src/catalog.o \
	src/recompute.o

DATA = sql/pglens--0.1.0.sql

REGRESS = register_lens recompute_lens
REGRESS_OPTS = --inputdir=test

DOCKER_COMPOSE ?= docker compose
DOCKER_SERVICE ?= dev
DOCKER_RUN = $(DOCKER_COMPOSE) run --rm $(DOCKER_SERVICE)
SKIP_PGXS_TARGETS = docker-build docker-shell docker-test docker-clean help

PG_CONFIG ?= pg_config

PG_CFLAGS += -Wall -Wextra

DOCS = \
	README.md \
	CONTRIBUTING.md \
	AGENTS.md \
	docs/ARCHITECTURE.md \
	docs/architecture.md \
	docs/DESIGN_PRINCIPLES.md \
	docs/ROADMAP.md \
	docs/DECISIONS.md \
	docs/NON_GOALS.md \
	docs/TERMINOLOGY.md \
	docs/terminology.md \
	docs/projections.md \
	docs/wal-design.md \
	docs/TESTING.md

ifneq ($(filter $(SKIP_PGXS_TARGETS),$(MAKECMDGOALS)),)
.PHONY: help docker-build docker-shell docker-test docker-clean

help:
	@printf '%s\n' \
		'Local PostgreSQL toolchain targets:' \
		'  make' \
		'  make install' \
		'  make installcheck' \
		'' \
		'Docker-first targets:' \
		'  make docker-build' \
		'  make docker-test' \
		'  make docker-shell' \
		'  make docker-clean'

docker-build:
	$(DOCKER_COMPOSE) build $(DOCKER_SERVICE)

docker-shell:
	$(DOCKER_RUN) bash

docker-test:
	$(DOCKER_RUN) bash -lc "make clean && make && sudo -E make install && pg_virtualenv make installcheck"

docker-clean:
	$(DOCKER_COMPOSE) down --remove-orphans
else
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
endif
