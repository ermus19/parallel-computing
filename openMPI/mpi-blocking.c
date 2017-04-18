#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define ORDEN 10000

int main(int argc, char *argv[])
{

    int size, rank, i, j, from, to, ndat, part, tag, mA[ORDEN * ORDEN], sumaTotal = 0;
    double total, t0, t1;
    MPI_Status info;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));

    //Inicializo matriz
    for (i = 0; i < ORDEN * ORDEN; i++)
    {
        mA[i] = 0;
    }

    //Inicializo matriz con numeros entre 0 y 10 si soy maestro
    if (rank == 0)
    {
        for (i = 0; i < ORDEN * ORDEN; i++)
        {
            mA[i] = rand() % 10;
        }
    }

    /*
    printf("Proceso %d: mA antes de enviar datos: \n", rank);
    for (i = 0; i < ORDEN * ORDEN; i++)
    {
        printf("%d  ", mA[i]);
    }
    printf("\n\n");
    */

    //Calculamos cuantos datos corresponden a cada proceso
    ndat = (ORDEN * ORDEN) / size;
    int VA[ndat];

    for (i = 0; i < ndat; i++)
    {
        VA[i] = 0;
    }

    //Tomamos tiempo inicial
    if (rank == 0)
    {
        t0 = MPI_Wtime();
    }

    //Dividimos el vector en partes, y las distribuimos entre los procesos
    MPI_Scatter(&mA[0], ndat, MPI_INT, &VA, ndat, MPI_INT, 0, MPI_COMM_WORLD);

    for (i = 0; i < ndat; i++)
    {
        sumaTotal = sumaTotal + VA[i];
    }

    printf("Proceso %d: calcula suma de elementos: %d\n", rank, sumaTotal);

    //todos los procesos conocen la suma del total de elementos de la matriz
    MPI_Allreduce(&sumaTotal, &sumaTotal, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    
    printf("Proceso %d: valor de suma total: %d\n", rank, sumaTotal);
    
    /*
    for (i = 0; i < ndat; i++)
    {
        VA[i] = VA[i] - sumaTotal;
        printf("%d  ", VA[i]);
    }
    printf("\n\n");
    */

    //Recolectamos valores de cada proceso actualizados
    MPI_Gather(&VA, ndat, MPI_INT, &mA, ndat, MPI_INT, 0, MPI_COMM_WORLD);

    //Tomamos tiempo final y calculamos tiempo ejecucion
    if (rank == 0)
    {
        t1 = MPI_Wtime();
        total = (t1 - t0);

        /*
        printf("Proceso %d: mA despues de recolectar datos: \n", rank);
        for (i = 0; i < ORDEN * ORDEN; i++)
        {
            printf("%d  ", mA[i]);
        }
        printf("\n\n");
        */

        printf("\nSe ha tardado %f s\n", total);

    }

    MPI_Finalize();
    return 0;
}