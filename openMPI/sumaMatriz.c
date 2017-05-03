
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ORDEN 1200

void inicializarMatriz(int ***A, int rows, int cols);
void crearMatriz(int **A, int rows, int cols);
void distribuir(int **IN, int rows_in, int cols_in, int **OUT, int rows_out, int cols_out, MPI_Comm cart);
void sumar(int **A, int tam, int *suma);
void pintarMatriz(int **A, int rows, int cols, int rank);

int main(int argc, char *argv[])
{

    //Declaracion de variables
    int size, rank, i, j, procs_dim;
    double t0, total;
    int suma = 0;
    int suma_total;
    int dims[2], periods[2];
    int **mA, **mB;

    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm cart;

    //Calculo del numero de procesos por dimension
    procs_dim = sqrt(size);

    //Creacion de la topologia de procesadores
    dims[0] = procs_dim;
    dims[1] = procs_dim;
    periods[0] = periods[1] = 0;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart);

    //El maestro inicializa la matriz principal
    if (rank == 0)
    {
        inicializarMatriz(&mA, ORDEN, ORDEN);
        crearMatriz(mA, ORDEN, ORDEN);
        //pintarMatriz(mA, ORDEN, ORDEN, rank);
    }

    //Todos inicializan la matriz auxiliar 
    inicializarMatriz(&mB, ORDEN / procs_dim, ORDEN / procs_dim);

    //Esperamos sincronizacion de todos los procesos
    MPI_Barrier(MPI_COMM_WORLD);

    //Se toma tiempo inicial
    t0 = MPI_Wtime();

    //Distribucion matriz principal entre procesos
    distribuir(mA, ORDEN, ORDEN, mB, ORDEN / procs_dim, ORDEN / procs_dim, cart);

    //pintarMatriz(mB, ORDEN / procs_dim, ORDEN / procs_dim, rank);

    //Suma de los elementos de la submatriz
    sumar(mB, ORDEN / procs_dim, &suma);

    printf("Proceso %d: valor de la suma: %d\n", rank, suma);

    //Suma total de elementos
    MPI_Reduce(&suma, &suma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        //Se toma tiempo total y se calcula tiempo total
        total = MPI_Wtime() - t0;

        printf("\nValor de la suma total: %d\n", suma_total);
        printf("Tiempo invertido: %f segundos\n", total);
    }

    MPI_Finalize();
    return 0;
}

//Metodo que inicializa matrices
void inicializarMatriz(int ***A, int rows, int cols)
{
    int i, j;
    int **a;
    *A = malloc(sizeof(int *) * rows);
    a = *A;
    a[0] = malloc(sizeof(int) * rows * cols);
    for (i = 1; i < rows; ++i)
        a[i] = &a[0][i * cols];

    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            a[i][j] = 0;
}

//Metodo que rellana matriz con numeros aleatorios
void crearMatriz(int **A, int rows, int cols)
{
    int i, j;

    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            A[i][j] = rand() % 10;
}

//Metodo que distribuye la matriz principal mA entre los procesos
void distribuir(int **mA, int rows_in, int cols_in, int **mB, int rows_out, int cols_out, MPI_Comm cart)
{

    int i, j, k, m, size, rank, cart_rank, coords[2], posicion = 0, procs_dim;

    //Tipo de datos que representa subbloque de la matriz principal
    MPI_Datatype matrix_block;
    MPI_Type_vector(rows_out, cols_out, cols_in, MPI_INT, &matrix_block);
    MPI_Type_commit(&matrix_block);

    //Tipo de datos que representa el bloque que reciben los procesos
    MPI_Datatype sub_matrix_block;
    MPI_Type_vector(rows_out, cols_out, rows_out, MPI_INT, &sub_matrix_block);
    MPI_Type_commit(&sub_matrix_block);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    procs_dim = sqrt(size);

    //El proceso principal reparte la matriz entre procesos
    if (rank == 0)
    {

        for (i = 0; i < procs_dim; i++)
        {
            for (j = 0; j < procs_dim; j++)
            {
                coords[0] = i;
                coords[1] = j;

                //Calculo del rank correspondiente a las coordenadas
                MPI_Cart_rank(cart, coords, &cart_rank);

                //Se envia subbloque a procesos diferentes del maestro
                if (cart_rank != 0)
                {
                    //Se envia bloque de la submatriz principal 
                    MPI_Send(&mA[coords[0] * cols_out][coords[1] * rows_out], 1, matrix_block, cart_rank, 0, MPI_COMM_WORLD);
                }
                else
                {
                    //Se asignan los valores de la submatriz del proceso maestro respecto a su bloque correspondiente de la matriz principal
                    for (k = 0; k < cols_out; k++)
                    {
                        for (m = 0; m < rows_out; m++)
                        {
                            mB[k][m] = mA[k][m];
                        }
                    }
                }
            }
        }
        MPI_Type_free(&matrix_block);
    }
    else
    {
        //Procesos diferentes al maetro reciben los bloques en mB
        MPI_Recv(&mB[0][0], 1, sub_matrix_block, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Type_free(&sub_matrix_block);
    }
}

//Metodo que suma los elementos de una matriz
void sumar(int **A, int tam, int *suma)
{
    int i, j;
    for (i = 0; i < tam; ++i)
    {
        for (j = 0; j < tam; j++)
        {
            *suma += A[i][j];
        }
    }
}

//Metodo que genera representacion por consola de una matriz
void pintarMatriz(int **A, int rows, int cols, int rank)
{
    int i, j;
    printf("\nProceso: %d\n", rank);
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }
}