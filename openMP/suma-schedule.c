#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define ORDEN 1000000

void sumaparcial(double *a, int n);
double sumatotal(double *a, int n, int THREADS);
double suma(double *a, int n, int THREADS);

int main()
{
    double vector[ORDEN], resultado;
    int i, tam = ORDEN;
    srand(time(NULL));
    for (i = 0; i < tam; i++)
        vector[i] = rand() % 100;
    resultado = suma(vector, tam, NUM_THREADS);
    printf("El resultado es %f\n", resultado);
}

double wallclock(void)
{
    struct timeval tv;
    double t;

    gettimeofday(&tv, NULL);

    t = (double)tv.tv_sec;
    t += ((double)tv.tv_usec) / 1000000.0;

    return t;
}

void sumaparcial(double *a, int n)
{
    int i;
    double s = 0;
    for (i = 0; i < n; i++)
        s += a[i];
    a[0] = s;
}
double sumatotal(double *a, int n, int THREADS)
{
    int i;
    double s = 0;
    for (i = 0; i < n; i += n / THREADS)
        s += a[i];
    return s;
}
//a es el array donde están los datos
//n es el número de datos
//THREADS es el número de threads que intervienen
double suma(double *a, int n, int THREADS)
{
    int i, nthreads;
    double total, t0, t1;

    omp_set_num_threads(THREADS);

    t0 = wallclock();

//Incluir aquí la directiva correspondiente para parallelizar el bucle for siguiente
#pragma omp parallel for private(i) schedule(static)
    //todos los threas realizan sumas parciales (sumas de los subvectores)
    for (i = 0; i < THREADS; i++)
    {
        sumaparcial(&a[i * n / THREADS], n / THREADS);
    }

    t1 = wallclock();
    total = (t1 - t0);
    printf("\nTrabajando con %d threads\n", THREADS);
    printf("Tiempo de ejecucion: %.6lf ms\n\n", (double)total * 1e3);

    //y el maestro acumula los resultados parciales
    return sumatotal(a, n, THREADS);
}
