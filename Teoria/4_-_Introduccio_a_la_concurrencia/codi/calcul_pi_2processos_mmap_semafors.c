#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

#define NUM_PROCS 2 /* No canviar */ 
#define NUM_RECTS 100000000

typedef struct shared_mem {
    sem_t clau;
    double valor_pi;
} shared_mem;

shared_mem *s;

void integral(int id)
{
  int i;
  double mid, height, width;

  width = 1.0 / (double) NUM_RECTS;
  for(i = id; i < NUM_RECTS; i += NUM_PROCS) {
    mid = (i + 0.5) * width;
    height = 4.0 / (1.0 + mid * mid);
    sem_wait(&s->clau); // lock
    s->valor_pi += height * width;
    sem_post(&s->clau); // unlock
  }
}

int main(int argc, char *argv[])
{
  int ret;
  
  /* Compartim el valor de pi i la clau */
  
  s = mmap(NULL, sizeof(shared_mem), PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  
  s->valor_pi = 0.0;
  sem_init(&s->clau, 1, 1); /* Indiquem que esta compartida entre procesos */
  
  ret = fork();

  if (ret == 0) {  // fill
    integral(1);
    exit(1);
  } else { // pare
    integral(0);
  }
  
  wait(NULL);
  
  printf("Valor de pi = %f\n", s->valor_pi);

  munmap(s, sizeof(shared_mem));
}
