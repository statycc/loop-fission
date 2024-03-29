/**
 * This benchmark created on September 1, 2022.
 *
 * The (partial) conjugate gradient routine iteration loop
 * from NAS CG benchmark.
 * CG = Conjugate Gradient, irregular memory access and communication.
 *
 * Web address: https://www.nas.nasa.gov/software/npb.html
 */
/* conjgrad.c */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include <conjgrad.h>


/* Array initialization. */
static
void init_array(int na,
    DATA_TYPE POLYBENCH_1D(p,NA,na),
    DATA_TYPE POLYBENCH_1D(q,NA,na),
    DATA_TYPE POLYBENCH_1D(z,NA,na),
    DATA_TYPE POLYBENCH_1D(r,NA,na))
{
    int j;

    for (j = 0; j < na; j++) {
        q[j] = 0.00001 *j;
        z[j] = 0.00002 *j;
        r[j] = 0.00003 *j;
        p[j] = r[j];
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int na,
    DATA_TYPE POLYBENCH_1D(z,NA,na),
    DATA_TYPE POLYBENCH_1D(r,NA,na))
{
  int i;
  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("z");
  for (i = 0; i < na; i++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, z[i]);
      if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("z");
  POLYBENCH_DUMP_BEGIN("r");
  for (i = 0; i < na; i++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, r[i]);
      if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("r");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_conjgrad(int na, int niter,
     DATA_TYPE POLYBENCH_1D(p,NA,na),
     DATA_TYPE POLYBENCH_1D(q,NA,na),
     DATA_TYPE POLYBENCH_1D(z,NA,na),
     DATA_TYPE POLYBENCH_1D(r,NA,na))
{
  int i;
  int j;

  double rho;
  double d;
  double alpha;
  rho = 0;
  d = 0;
  for (i = 1; i <= niter; i += 1) {
    
#pragma omp parallel for private (j) reduction (+:rho)
    for (j = 0; j <= -1 + na; j += 1) {
      rho = rho + r[j] * r[j];
    }
    
#pragma omp parallel for private (j) reduction (+:d)
    for (j = 0; j <= -1 + na; j += 1) {
      d = d + p[j] * q[j];
    }
    alpha = rho / d;
    
#pragma omp parallel for private (j)
    for (j = 0; j <= -1 + na; j += 1) {
      z[j] = z[j] + alpha * p[j];
    }
    
#pragma omp parallel for private (j) firstprivate (alpha)
    for (j = 0; j <= -1 + na; j += 1) {
      r[j] = r[j] - alpha * q[j];
    }
  }
}

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int na = NA;
  int niter = NITER;

  /* Variable declaration/allocation. */
  POLYBENCH_1D_ARRAY_DECL(p,DATA_TYPE,NA,na);
  POLYBENCH_1D_ARRAY_DECL(q,DATA_TYPE,NA,na);
  POLYBENCH_1D_ARRAY_DECL(z,DATA_TYPE,NA,na);
  POLYBENCH_1D_ARRAY_DECL(r,DATA_TYPE,NA,na);

  /* Initialize array(s). */
  init_array (na,
              POLYBENCH_ARRAY(p),
              POLYBENCH_ARRAY(q),
              POLYBENCH_ARRAY(z),
              POLYBENCH_ARRAY(r));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_conjgrad (na, niter,
               POLYBENCH_ARRAY(p),
               POLYBENCH_ARRAY(q),
               POLYBENCH_ARRAY(z),
               POLYBENCH_ARRAY(r));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(na, POLYBENCH_ARRAY(z), POLYBENCH_ARRAY(r)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(p);
  POLYBENCH_FREE_ARRAY(q);
  POLYBENCH_FREE_ARRAY(z);
  POLYBENCH_FREE_ARRAY(r);

  return 0;
}
