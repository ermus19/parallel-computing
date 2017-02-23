#include <omp.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
  int nthreads, tid;

  printf("Working with 4 threads\n");
  omp_set_num_threads(4);

  nthreads = omp_get_num_threads();
  printf("Number of current threads working = %d\n", nthreads);

  #pragma omp parallel private(tid)
    {

      tid = omp_get_thread_num();
      printf("Hello from thread number = %d !\n",tid);

      if (tid == 0)
      {
        printf("Hello from the master thread!\n");
      }

      nthreads = omp_get_num_threads();
      printf("Number of current threads working = %d\n", nthreads);
    }

  printf("Master thread exitting!\n");
  return 0;
}
