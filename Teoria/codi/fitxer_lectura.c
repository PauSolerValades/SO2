/*
 *   Aquest codi hauria de llegir les 10000 primeres linies del fitxer,
 *   sumar els valors i imprimir el resultat per pantalla. Alguns cops
 *   pot no funcionar correctament. Per què? 
 * 
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_FILS  2 
#define N         10000 

int nelem, total;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_fn(void *arg)
{
	int i, total_local;
	FILE *fitxer = (FILE *) arg;

	total_local = 0;
	while (nelem < N)
	{
		fscanf(fitxer, "%d", &i);
        
        pthread_mutex_lock(&mutex);
		nelem++;
		pthread_mutex_unlock(&mutex);

		total_local += i;
	}

	pthread_mutex_lock(&mutex);
	total += total_local;
	pthread_mutex_unlock(&mutex);

	return ((void *)0);
}

int main(void)
{
	pthread_t ntid[NUM_FILS];
	FILE *fp;
	int i;

	nelem = 0;
	total = 0;

	fp = fopen("fitxer.data", "r");
	if (!fp) {
		printf("No he pogut obrir fitxer fitxer.data\n");
		exit(1);
	}

	for(i = 0; i < NUM_FILS; i++)
		pthread_create(&(ntid[i]), NULL, thread_fn, (void *) fp);

	for(i = 0; i < NUM_FILS; i++)
		pthread_join(ntid[i], NULL);

	printf("Nombre total d'elements llegits: %d\n", nelem);
	printf("Suma total: %d\n", total);
}
