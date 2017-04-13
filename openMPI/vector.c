#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	int size, rank, i, j, from, to, ndat, part, tag, VA[N];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	//Inicializo el vector
	for (i=0; i<N; i++) {
		VA[i] = 0;
	}
	
	//Si soy maestro
	if (rank == 0){
		for (i=1; i<N; i++) {
			VA[i] = i;
		}

        int tam = N/(size-1);

        for (j = 1; j < size; j++){
			to = j; tag = 0;
			if(to == (size - 1)){
                ndat = N - (j - 1)*tam;
			}else{
				ndat = tam;
			}
			MPI_Send(&VA[(j - 1)*tam], ndat, MPI_INT,to,tag,MPI_COMM_WORLD);

		}
		
	} else {		

		printf("Proceso %d: VA antes de recibir datos: \n",rank);
		for (i=0; i<N; i++) {
			printf("%d  ",VA[i]);
		}
		printf("\n\n");
		from = 0; tag = 0;
		MPI_Recv(&VA[0],N,MPI_INT,from,tag,MPI_COMM_WORLD,&info);
		MPI_Get_count(&info,MPI_INT,&ndat);
		printf("Proceso %d recibe VA de %d: tag %d, ndat %d; \nVA = ", rank, info.MPI_SOURCE, info.MPI_TAG, ndat);
		
		for (i=0; i<ndat; i++) {
			printf("%d  ",VA[i]);
		}
		printf("\n\n");
	}

	MPI_Finalize();
	return 0;
}