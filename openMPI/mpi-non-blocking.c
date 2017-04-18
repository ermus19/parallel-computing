#include <mpi.h>
#include <time.h>
#include <stdio.h>

#define N 1000

int calculaSuma(int V[]);

int main(int argc, char *argv[])
{

    int size, rank, tag1 = 1, VA[N];

    MPI_Request reqs[2];
    MPI_Status stats[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));

    //Inicializacion de vectore
    for (int i = 0; i < N; i++)
    {
        VA[i] = 0;
    }

    if (rank == 0)
    {
        //Inicializacion de vector maestro con valores aleatorios
        for (int i = 0; i < N; i++)
        {
            VA[i] = rand() % 10000;
        }

        //Proceso maestro envía los datos al esclavo
        MPI_Isend(&VA[0], N, MPI_INT, 1, tag1, MPI_COMM_WORLD, &reqs[0]);

        //Proceso maestro calcula suma total de elementos
        printf("Proceso %d calcula suma de elementos: %d\n", rank, calculaSuma(VA));

        //Proceso maestro espera a que se envíen los datos
        MPI_Wait(&reqs[0], &stats[0]);
    }
    else
    {

        //Proceso esclavo se prepara para recibir datos
        MPI_Irecv(&VA[0], N, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[1]);

        //Proceso esclavo espera a que se reciban los datos
        MPI_Wait(&reqs[1], &stats[1]);

        //Proceso esclavo calcula suma total despues de recibir los datos
        printf("Proceso %d calcula suma de elementos: %d\n", rank, calculaSuma(VA));
    }

    MPI_Finalize();
    return 0;
}

//Funcion que calcula la suma de los elementos de un vector
int calculaSuma(int V[])
{
    int sumaTotal = 0;

    for (int i = 0; i < N; i++)
    {
        sumaTotal += V[i];
    }

    return sumaTotal;
}