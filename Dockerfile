ARG PG_MAJOR=15

FROM debian:bookworm-slim

ARG PG_MAJOR
ARG LOCAL_UID=1000
ARG LOCAL_GID=1000

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        git \
        make \
        postgresql-${PG_MAJOR} \
        postgresql-server-dev-${PG_MAJOR} \
        sudo \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd --gid "${LOCAL_GID}" dev \
    && useradd --uid "${LOCAL_UID}" --gid "${LOCAL_GID}" --create-home --shell /bin/bash dev

RUN echo 'dev ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/dev \
    && chmod 0440 /etc/sudoers.d/dev

ENV PG_CONFIG=/usr/lib/postgresql/${PG_MAJOR}/bin/pg_config
ENV PATH=/usr/lib/postgresql/${PG_MAJOR}/bin:${PATH}

USER dev
WORKDIR /workspace

CMD ["bash"]
