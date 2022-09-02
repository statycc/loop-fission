/**
 * This benchmark created on August 29, 2022.
 *
 * The colormap kernel appeared in MiBench Version 1.0,
 * consumer/tiff-v3.5.4/contrib/dbs/xtiff/xtiff.c, LOC 472-476.
 * It has been extracted and modified to fit this benchmark suite.
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
void init_array() { }

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

    i = 0;
    while (i < _PB_N) {
          R[i] = (DATA_TYPE) (((i) * 65535L) / (n-1));
          G[i] = (DATA_TYPE) (((i) * 65535L) / (n-1));
          B[i] = (DATA_TYPE) (((i) * 65535L) / (n-1));
          i++;
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
  init_array ();

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
