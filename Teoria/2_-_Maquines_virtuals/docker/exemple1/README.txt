 
docker build -t prova1 .
docker run --ulimit nproc=32:64 prova1

docker container ls
docker kill <container>
