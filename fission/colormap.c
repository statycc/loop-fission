/**
 * This benchmark created on August 29, 2022.
 *
 * The colormap kernel from MiBench Version 1.0,
 * consumer/tiff-v3.5.4/contrib/dbs/xtiff/xtiff.c, LOC 472-477.
 *
 * Web address: https://vhosts.eecs.umich.edu/mibench
 */
/* colormap.c */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include <colormap.h>

/* Array initialization. */
static
void init_array(int n,
    DATA_TYPE POLYBENCH_1D(R,N,n),
    DATA_TYPE POLYBENCH_1D(G,N,n),
    DATA_TYPE POLYBENCH_1D(B,N,n)) {

    int i;

    for (i = 0; i < n; i++)
        R[i] = (DATA_TYPE) ((i*i+1) % n) / (5*n);
    for (i = 0; i < n; i++)
        G[i] = (DATA_TYPE) ((i*(i+1)+2) % n) / (5*n);
    for (i = 0; i < n; i++)
        B[i] = (DATA_TYPE) (i*(i+3) % n) / (5*n);

}

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
     DATA_TYPE POLYBENCH_1D(R,N,n),
     DATA_TYPE POLYBENCH_1D(G,N,n),
     DATA_TYPE POLYBENCH_1D(B,N,n))
{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("R");
  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, R[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("R");
  POLYBENCH_DUMP_BEGIN("G");
  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, G[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("G");
  POLYBENCH_DUMP_BEGIN("B");
  for (i = 0; i < n; i++) {
    fprintf (stderr, DATA_PRINTF_MODIFIER, B[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("B");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_colormap(int n,
    DATA_TYPE POLYBENCH_1D(R,N,n),
    DATA_TYPE POLYBENCH_1D(G,N,n),
    DATA_TYPE POLYBENCH_1D(B,N,n))
{
  int i;

#pragma scop

#pragma omp parallel private(i)
{
    #pragma omp single nowait
    {
        i = 0;
        while (i < _PB_N) {
            R[i] = (DATA_TYPE) (((R[i]) * 65535L) / 255);
            i++;
        }
    }

    #pragma omp single nowait
    {
        i = 0;
        while (i < _PB_N) {
            G[i] = (DATA_TYPE) (((G[i]) * 65535L) / 255);
            i++;
        }
    }

    #pragma omp single nowait
    {
        i = 0;
        while (i < _PB_N) {
            B[i] = (DATA_TYPE) (((B[i]) * 65535L) / 255);
            i++;
        }
    }
}

#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  POLYBENCH_1D_ARRAY_DECL(R,DATA_TYPE,N,n);
  POLYBENCH_1D_ARRAY_DECL(G,DATA_TYPE,N,n);
  POLYBENCH_1D_ARRAY_DECL(B,DATA_TYPE,N,n);

  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(R), POLYBENCH_ARRAY(G), POLYBENCH_ARRAY(B));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_colormap (n, POLYBENCH_ARRAY(R), POLYBENCH_ARRAY(G), POLYBENCH_ARRAY(B));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(R), POLYBENCH_ARRAY(G), POLYBENCH_ARRAY(B)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(R);
  POLYBENCH_FREE_ARRAY(G);
  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
