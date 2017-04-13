#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 1
#define N 100

double wallclock(void);
double funcion(double a);

int main(int argc, char *argv[])
{

    int n, i, nthreads;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, x;
    double total, t0, t1;

    n = N; //Número de intervalos
    h = 1.0 / (double)n;

    omp_set_num_threads(NUM_THREADS);

    t0 = wallclock();

//La variable pi es una variable de reducción (+)
#pragma omp parallel for private(x, i) reduction(+:pi) schedule(static)
    for (i = 1; i <= n; i++)
    {
	x = h * ((double)i - 0.5);
	pi += funcion(x);
    }

    t1 = wallclock();

    pi = h * pi;
    total = (t1 - t0);

    printf("\nTrabajando con %d threads y %d iteraciones\n", NUM_THREADS, n);
    printf("\n Pi es aproximadamente %.16f.El error cometido es %.16f\n", pi, fabs(pi - PI25DT));
    printf("Tiempo de ejecucion: %.6lf ms\n\n", (double) total * 1e3);

    return 0;
}

double funcion(double a)
{
    return (4.0 / (1.0 + a * a));
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
