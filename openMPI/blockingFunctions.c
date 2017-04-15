#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define ORDEN 10000

double wallclock(void);

int main(int argc, char *argv[])
{

    int size, rank, i, from, to, ndat, part, tag, mA[ORDEN][ORDEN], mB[ORDEN][ORDEN], sumaTotal;
    double total, t0, t1;
    MPI_Status info;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));

    //Inicializo matriz si soy maestro
    if (rank == 0)
    {
        for (i = 0; i < ORDEN; i++)
        {
            for (j = 0; j < ORDEN; j++)
            {
                mA[i][j] = rand() % 10;
            }
        }
    }

    //Calculamos cuántos datos corresponden a cada proceso
    ndat = ORDEN * ORDEN / size;

    //Tomamos tiempo inicial
    t0 = wallclock();

    //Dividimos el vector en partes, y las distribuimos entre los procesos
    MPI_Scatter(&mA[0][0], ndat, MPI_INT, &mA[0][0], ORDEN, MPI_INT, rank, MPI_COMM_WORLD);

    //Sincronizamos todos los procesos
    MPI_Barrier(MPI_COMM_WORLD);

    for (i = 0; i < ndat; i++)
    {
        for (j = 0; j < ndat; j++)
        {
            sumaTotal += mA[i][j];
        }
    }

    printf("Proceso %d: calculando suma de elementos: %f\n", rank, sumaTotal);

    //todos los procesos conocen la suma del total de elementos de la matriz
    MPI_Bcast(&sumaTotal, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Sincronizamos todos los procesos
    MPI_Barrier(MPI_COMM_WORLD);

    printf("Proceso %d: valor de suma tras broadcast: %f\n", rank, sumaTotal);

    for (i = 0; i < ndat; i++)
    {
        for (j = 0; j < ndat; j++)
        {
            mA[i][j] = mA[i][j] - sumaTotal;
        }
    }

    //Recolectamos valores de cada proceso actualizados
    MPI_Gather(&mA[0][0], ndat, MPI_INT, &m[0][0], ndat, MPI_INT, 0, MPI_COMM_WORLD);

    //Tomamos tiempo final y calculamos tiempo ejecucion
    t1 = wallclock();
    total = (t1 - t0);

    printf("\nSe ha tardado %f ms\n", (double)total * 1e3);

    MPI_Finalize();
    return 0;
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