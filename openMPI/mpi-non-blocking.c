#include <mpi.h>
#include <time.h>
#include <stdio.h>

#define N 1000

int calculaSuma(int V[]);

int main(int argc, char *argv[])
{

    int numtasks, rank, next, prev, tag1 = 1, longitud, VA[N], VB[N];

    MPI_Request reqs[2];
    MPI_Status stats[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));

    for (int i = 0; i < N; i++)
    {
        VA[i] = 0;
        VB[i] = 0;
    }

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            VA[i] = rand() % 10000;
        }
    }

    if (rank == 0)
    {

        //Proceso maestro envía los datos al esclavo
        MPI_Isend(&VA[0], N, MPI_INT, 1, tag1, MPI_COMM_WORLD, &reqs[0]);

        //Proceso maestro calcula suma total de elementos
        printf("Proceso %d calcula suma de elementos: %d\n", rank, calculaSuma(VA));

        //Proceso maestro espera a que se envíen los datos?
        MPI_Wait(&reqs[0], &stats[0]);
    }
    else
    {

        //Proceso esclavo se prepara para recibir datos
        MPI_Irecv(&VB[0], N, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[1]);

        //Proceso esclavo espera a que se reciban los datos
        MPI_Wait(&reqs[1], &stats[1]);

        //Proceso esclavo calcula suma total despues de recibir los datos
        printf("Proceso %d calcula suma de elementos: %d\n", rank, calculaSuma(VB));
    }

    MPI_Finalize();
    return 0;
}

int calculaSuma(int V[])
{
    int sumaTotal = 0;

    for (int i = 0; i < N; i++)
    {
        sumaTotal += V[i];
    }

    return sumaTotal;
}