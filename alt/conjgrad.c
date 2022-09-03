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
#include <omp.h> 

static void init_array(int na,double p[75000],double q[75000],double z[75000],double r[75000])
{
  int j;
  
#pragma omp parallel for private (j)
  for (j = 0; j <= -1 + na; j += 1) {
    q[j] = 0.00001 * j;
  }
  
#pragma omp parallel for private (j)
  for (j = 0; j <= -1 + na; j += 1) {
    z[j] = 0.00002 * j;
  }
  
#pragma omp parallel for private (j)
  for (j = 0; j <= -1 + na; j += 1) {
    r[j] = 0.00003 * j;
  }
  
#pragma omp parallel for private (j) firstprivate (na)
  for (j = 0; j <= -1 + na; j += 1) {
    p[j] = r[j];
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int na,double z[75000],double r[75000])
{
  int i;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","z");
  for (i = 0; i <= -1 + na; i += 1) {
    fprintf(stderr,"%0.2lf ",z[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","z");
  fprintf(stderr,"begin dump: %s","r");
  for (i = 0; i <= -1 + na; i += 1) {
    fprintf(stderr,"%0.2lf ",r[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","r");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_conjgrad(int na,int niter,double p[75000],double q[75000],double z[75000],double r[75000])
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

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int na = 75000;
  int niter = 75;
/* Variable declaration/allocation. */
  double (*p)[75000];
  p = ((double (*)[75000])(polybench_alloc_data((75000 + 0),(sizeof(double )))));
  ;
  double (*q)[75000];
  q = ((double (*)[75000])(polybench_alloc_data((75000 + 0),(sizeof(double )))));
  ;
  double (*z)[75000];
  z = ((double (*)[75000])(polybench_alloc_data((75000 + 0),(sizeof(double )))));
  ;
  double (*r)[75000];
  r = ((double (*)[75000])(polybench_alloc_data((75000 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(na, *p, *q, *z, *r);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_conjgrad(na,niter, *p, *q, *z, *r);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(na, *z, *r);
/* Be clean. */
  free((void *)p);
  ;
  free((void *)q);
  ;
  free((void *)z);
  ;
  free((void *)r);
  ;
  return 0;
}
