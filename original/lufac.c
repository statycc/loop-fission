/**
 * lufac.C - from ROSE compiler LoopProcessing test suite
 * <https://github.com/rose-compiler/rose/blob/dab37577feb8eb129c8fc15f6972222c03171c9f/tests/roseTests/loopProcessingTests/lufac.C>
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include "lufac.h"


/* Array initialization. */
static
void init_array(int n, DATA_TYPE POLYBENCH_2D(C,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      C[i][j] = 42;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n, DATA_TYPE POLYBENCH_2D(C,N,N,n,n))
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, C[i][j]);
	if (i % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_template(int n, DATA_TYPE POLYBENCH_2D(C,N,N,n,n))
{
    int p[n], i, j, k;
    double a[n][n], mu, t;

#pragma scop
    for (k = 0; k<=n-2; k+=1) {
        p[k] = k;
        mu =  (a[k][k] < 0 ? -1 : 1) * a[k][k];

        for (i = k+1; i <= n-1; i+=1) {
          if (mu < (a[i][k] < 0 ? -1 : 1) * a[i][k] ) {
            mu = (a[i][k] < 0 ? -1 : 1) * a[i][k];
            p[k] = i;
          }
        }
        for (j = k; j <= n-1; j+=1) {
           t = a[k][j];
           a[k][j] = a[p[k]][j];
           a[p[k]][j] = t;
        }
        for (i = k+1; i <= n-1; i+=1) {
            a[i][k] = a[i][k]/a[k][k];
        }
        for (j = k+1; j <=n-1; j+=1) {
           for (i = k+1; i <=n-1; i+=1) {
                a[i][j] = a[i][j] - a[i][k]*a[k][j];
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
  POLYBENCH_2D_ARRAY_DECL(C,DATA_TYPE,N,N,n,n);

  /* Initialize array(s). */
  init_array (n, POLYBENCH_ARRAY(C));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_template (n, POLYBENCH_ARRAY(C));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(n,  POLYBENCH_ARRAY(C)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(C);

  return 0;
}
