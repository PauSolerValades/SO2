#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
  int ret, i, *a;

  a = malloc(10 * sizeof(int));

  for(i = 0; i < 10; i++)
    a[i] = 1;
  
  ret = fork();
  
  if (ret == 0) {  // fill
     a[3] = 2;
     printf("Fill a = ");
     for(i = 0; i < 10; i++)
       printf("%d ", a[i]);
     printf("\n");
  } else { // pare
     sleep(1);
     printf("Pare a = ");
     for(i = 0; i < 10; i++)
       printf("%d ", a[i]);
     printf("\n");
  }

  free(a);
}
