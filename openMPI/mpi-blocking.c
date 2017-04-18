#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define ORDEN 10000

int main(int argc, char *argv[])
{

    int size, rank, i, j, ndat, sumaTotal = 0;
    double t0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));

    //Calculamos cuantos datos corresponden a cada proceso e inicializamos el vector
    ndat = (ORDEN * ORDEN) / size;
    int VA[ndat];

    for (i = 0; i < ndat; i++)
    {
        VA[i] = 0;
    }

    //El maestro inicializa la matriz y la divide
    if (rank == 0)
    {
        int mA[ORDEN][ORDEN];

        for (i = 0; i < ORDEN; i++)
        {
            for (j = 0; j < ORDEN; j++)
            {
                mA[i][j] = rand() % 10;
            }
        }

        /*printf("Proceso %d: mA antes de enviar datos: \n", rank);
        for (i = 0; i < ORDEN; i++)
        {
            for (j = 0; j < ORDEN; j++)
            {
                printf("%d  ", mA[i][j]);
            }
        }
        printf("\n\n");
        */

        //Tomamos tiempo inicial
        t0 = MPI_Wtime();

        //Dividimos el vector en partes, y las distribuimos entre los procesos
        MPI_Scatter(&mA[0], ndat, MPI_INT, &VA, ndat, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        //Los procesos esclavos no envian pero reciben datos de la llamada a MPI_Scatter
        MPI_Scatter(NULL, ndat, MPI_INT, &VA, ndat, MPI_INT, 0, MPI_COMM_WORLD);
    }

    for (i = 0; i < ndat; i++)
    {
        sumaTotal = sumaTotal + VA[i];
    }

    printf("Proceso %d: calcula suma de elementos: %d\n", rank, sumaTotal);

    //todos los procesos conocen la suma del total de elementos de la matriz
    MPI_Allreduce(&sumaTotal, &sumaTotal, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    printf("Proceso %d: valor de suma total: %d\n", rank, sumaTotal);

    //printf("Proceso %d: resta de elementos:\n");
    for (i = 0; i < ndat; i++)
    {
        VA[i] = abs(VA[i] - sumaTotal);
        //printf("%d ", VA[i]);
    }
    //printf("\n\n");

    //Tomamos tiempo final y calculamos tiempo ejecucion
    if (rank == 0)
    {
        int mA[ORDEN][ORDEN];
        double total, t1;

        //Recolectamos valores de cada proceso actualizados
        MPI_Gather(&VA, ndat, MPI_INT, &mA, ndat, MPI_INT, 0, MPI_COMM_WORLD);

        //Tomamos tiempo final y calculamos tiempo total
        t1 = MPI_Wtime();
        total = (t1 - t0);

        /*printf("Proceso %d: mA despues de recolectar datos: \n", rank);
        for (i = 0; i < ORDEN; i++)
        {
            for(j = 0; j < ORDEN; j++)
            {
                printf("%d  ", mA[i][j]);
            }
            
        }
        printf("\n\n");
        */

        printf("\nSe ha tardado %f s\n", total);
    }
    else
    {

        MPI_Gather(&VA, ndat, MPI_INT, NULL, ndat, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}