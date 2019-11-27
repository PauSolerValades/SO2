#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

#define NUM_PROCS 2 /* No canviar */ 
#define NUM_RECTS 100000000

double integral(int id)
{
  int i;
  double mid, height, width, sum = 0.0;
  double area;

  width = 1.0 / (double) NUM_RECTS;
  for(i = id; i < NUM_RECTS; i += NUM_PROCS) {
    mid = (i + 0.5) * width;
    height = 4.0 / (1.0 + mid * mid);
    sum += height;
  }
  area  = width * sum;
  return area;
}

int main(int argc, char *argv[])
{
  int ret;
  double valor_pi, *a;                 

  a = mmap(NULL, sizeof(double) * 2, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  ret = fork();

  if (ret == 0) {  // fill
    a[1] = integral(1);
    exit(1);
  } else { // pare
    a[0] = integral(0);
  }
  
  wait(NULL);

  valor_pi = a[0] + a[1];
  
  printf("Valor de pi = %f\n", valor_pi);

  munmap(a, sizeof(double) * 2);
}
