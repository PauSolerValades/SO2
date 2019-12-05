# Dockerfile reference
# https://docs.docker.com/engine/reference/builder/#format

FROM ubuntu:18.04
COPY tmp/* /home/appuser/
WORKDIR /home/appuser
RUN addgroup appgroup
RUN useradd -r -u 2000 -g appgroup appuser
USER appuser

