# Dockerfile reference
# https://docs.docker.com/engine/reference/builder/#format

FROM gcc:7
COPY tmp/* /home/appuser/
WORKDIR /home/appuser
RUN make
RUN addgroup appgroup
RUN useradd -r -u 2000 -g appgroup appuser
USER appuser
CMD ["./prova"]

