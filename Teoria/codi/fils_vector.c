#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int a[10];

void imprimir(char *str, int *b)
{
  int i;
  printf("%s a = ", str);
  for(i = 0; i < 10; i++)
    printf("%d ", b[i]);
  printf("\n");
}

void *thread_function(void *arg)
{
  a[3] = 2;
  imprimir("Fil creat", a);

  return ((void *)0);
}

int main(void)
{
  pthread_t ntid;
  int i;

  for(i = 0; i < 10; i++)
    a[i] = 1;

  pthread_create(&ntid, NULL, thread_function, NULL);

  sleep(1);
  imprimir("Fil original", a);
}
