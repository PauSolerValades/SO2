#include <stdio.h>
#include <strings.h>

int main(void)
{
  char *str1 = "STRING";
  char *str2 = "strinnn";
  int result;

  result = strcasecmp(str1, str2);

  if (result == 0)
    printf("Strings compared equal.\n");
  else if (result < 0)
    printf("\"%s\" is less than \"%s\".\n", str1, str2);
  else
    printf("\"%s\" is greater than \"%s\".\n", str1, str2);
  
  printf("%d\n", result);

  return 0;
}
