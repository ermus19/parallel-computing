#include <omp.h>
#include <stdio.h>
#define CHUNKSIZE 2
#define N 10

/*
* for loop static sheduled
*/

int main ()
{
	int i, chunk;
	int nthreads, tid;
	int a[N],b[N],c[N];

	for (i=0;i<N;i++)
  {
    a[i]=b[i]=i*1.0;
  }
	chunk=CHUNKSIZE;

	//a, b, c, chunk shared variables
	// i, tid private variables

	#pragma omp parallel private(tid, i) shared(a, b, c, chunk)
	{
		//Asignación estática de iteraciones en la que el
		//tamaño del bloque se fija a chunk
    #pragma omp for schedule(static, chunk)

    for (i=0;i<N;i++)
    {
  		tid = omp_get_thread_num();
      nthreads = omp_get_num_threads();
  		c[i]=a[i]+b[i];
  		printf("Thread %d of %d threads, iteration i = %d\n", tid, nthreads, i);
  	}

    printf("Thread %d finished\n", tid);

	} //Fin de la región paralela
}
