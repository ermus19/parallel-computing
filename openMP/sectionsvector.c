#include <omp.h>
#include <stdio.h>

//EXPORT OMP_NESTED=TRUE
void sumparcial(double *a, int n);
double sumtotal(double *a, int n, int THREADS);
double sum(double *a, int n, int THREADS);

void multparcial(double *a, int n);
double multtotal(double *a, int n, int THREADS);
double multiplication(double *a, int n, int THREADS);

int main(){
	int nthreads, tid;
	double vector[200],sumres,prodres;
	int i,size=200;
	srand(time(NULL));

    omp_set_num_threads(8);


	for (i=0; i<size; i++) 
    {
        vector[i]=rand()%5 + 1;
    }

    #pragma omp parallel sections firstprivate(vector)
    {

        #pragma omp section
        {

            nthreads = omp_get_num_threads();
            tid = omp_get_thread_num();
			printf("Thread %d of %d, working on section 1\n", tid, nthreads);
			sumres=sum(vector, size, nthreads);
			printf("Result of sum: %f\n",sumres);

        }

        #pragma omp section
        {
            nthreads = omp_get_num_threads();
            tid = omp_get_thread_num();
            printf("Thread %d of %d, working on section 2\n", tid, nthreads);
			prodres=multiplication(vector, size, nthreads);
			printf("Result of multiplication: %f\n",prodres);
        }


    }
}
	


void sumparcial(double *a, int n){
	int i;
	double s=0;
    #pragma omp parallel for  private(i)
    
        for(i=0;i<n;i++){
		    s+=a[i];
        }
	    a[0]=s;
	
}

double sumtotal(double *a, int n, int THREADS){
	int i;
	double s=0;
    #pragma omp parallel for private(i)
    
	    for(i=0;i<n;i+=n/THREADS)
		    s+=a[i];
	    return s;

}

double sum(double *a, int n, int THREADS){
	int i;
    #pragma omp parallel for private(i)
    
	    for (i=0;i<THREADS;i++){
			sumparcial(&a[i*n/THREADS], n/THREADS);
			printf("Thread %d summing\n", omp_get_thread_num());	
	    }
	    return sumtotal(a, n, THREADS);

}

void multparcial(double *a, int n){
	int i;
	double s=1;

    #pragma omp parallel for private(i)
    	for(i=0;i<n;i++)
        {
		    s*=a[i];
        }
	    a[0]=s;
}

double multtotal(double *a, int n, int THREADS){
	int i;
	double s=1;
    #pragma omp parallel for private(i)
	    for(i=0;i<n;i+=n/THREADS){
		    s*=a[i];
        }
	    return s;
}

double multiplication(double *a, int n, int THREADS){
	int i;
    #pragma omp parallel for private(i)
	    for (i=0;i<THREADS;i++)
        {
		    multparcial(&a[i*n/THREADS], n/THREADS);
			printf("Thread %d multiplicating\n", omp_get_thread_num());
        }
	    return multtotal(a, n, THREADS);
}


