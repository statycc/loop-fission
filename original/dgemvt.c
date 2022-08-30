/**
 * This benchmark created on August 30, 2022.
 *
 * Simplified version of DGEMVT from LAPACK: Linear
 * Algebra PACKage, pperforming matrix algebra, as is
 * appears in the ROSE compiler test suite.
 *
 * Web address: https://github.com/rose-compiler/rose/blob/release/tests/roseTests/loopProcessingTests/dgemvT.C
 */
/* dgemvt.c */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include "dgemvt.h"


/* Array initialization. */
static
void init_array(int m, int n, int p,
        DATA_TYPE *beta,
        DATA_TYPE POLYBENCH_1D(Y,M,m),
        DATA_TYPE POLYBENCH_1D(X,N,n),
        DATA_TYPE POLYBENCH_1D(A,P,p))
{
  int i, j;
  *beta = 1.2;

    for (i = 0; i < m; i++)
        Y[i] = 42.f + (DATA_TYPE)i;
    for (i = 0; i < n; i++)
        X[i] = log(i);
    for (i = 0; i < p; i++)
        A[i] = sin(i);
}

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int p, DATA_TYPE POLYBENCH_1D(A,P,p))
{
  int j;

  for (j = 0; j < p; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, A[j]);
	if (j % 20 == 0) fprintf (stderr, "\n");
  }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_dgemvt(int m, int n, int lda,
        DATA_TYPE beta,
        DATA_TYPE POLYBENCH_1D(Y,M,m),
        DATA_TYPE POLYBENCH_1D(X,N,n),
        DATA_TYPE POLYBENCH_1D(A,P,p))
{
  int i, j;

#pragma scop
for (i = 0; i < _PB_M; i += 1) {
    Y[i] = beta * Y[i];
    for (j = 0; j < _PB_N; j += 1) {
        Y[i] += A[i * lda + j] * X[j];
    }
}
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int m = M;
  int n = N;
  int l = L;
  int p = P;

  /* Variable declaration/allocation. */
  DATA_TYPE beta;
  POLYBENCH_1D_ARRAY_DECL(Y,DATA_TYPE,M,M);
  POLYBENCH_1D_ARRAY_DECL(X,DATA_TYPE,N,n);
  POLYBENCH_1D_ARRAY_DECL(A,DATA_TYPE,P,p);

  /* Initialize array(s). */
  init_array (m,n,p,&beta,
              POLYBENCH_ARRAY(Y),
              POLYBENCH_ARRAY(X),
              POLYBENCH_ARRAY(A));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_dgemvt (m,n,l,beta,
                 POLYBENCH_ARRAY(Y),
                 POLYBENCH_ARRAY(X),
                 POLYBENCH_ARRAY(A));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n,  POLYBENCH_ARRAY(A)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(Y);
  POLYBENCH_FREE_ARRAY(X);
  POLYBENCH_FREE_ARRAY(A);

  return 0;
}
