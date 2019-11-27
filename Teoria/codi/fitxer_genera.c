#include <stdio.h>
#include <stdlib.h>

#define N 10010

int main(void)
{
	FILE *fp;
	int i;

	fp = fopen("fitxer.data", "w");
	if (!fp) {
		printf("No he pogut obrir fitxer.data\n");
		exit(1);
	}

	for(i = 1; i <= N; i++) {
		fprintf(fp, "%d\n", i);
	}

	fclose(fp);

	return 0;
}


