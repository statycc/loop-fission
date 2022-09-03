/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* gemm.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
#include "gemm.h"
/* Array initialization. */

static void init_array(int ni,int nj,int nk,double *alpha,double *beta,double C[1000][1100],double A[1000][1200],double B[1200][1100])
{
  int i;
  int j;
   *alpha = 1.5;
   *beta = 1.2;
  for (i = 0; i <= -1 + ni; i += 1) {
    for (j = 0; j <= -1 + nj; j += 1) {
      C[i][j] = ((double )((i * j + 1) % ni)) / ni;
    }
  }
  for (i = 0; i <= -1 + ni; i += 1) {
    for (j = 0; j <= -1 + nk; j += 1) {
      A[i][j] = ((double )(i * (j + 1) % nk)) / nk;
    }
  }
  for (i = 0; i <= -1 + nk; i += 1) {
    for (j = 0; j <= -1 + nj; j += 1) {
      B[i][j] = ((double )(i * (j + 2) % nj)) / nj;
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int ni,int nj,double C[1000][1100])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","C");
  for (i = 0; i <= -1 + ni; i += 1) {
    for (j = 0; j <= -1 + nj; j += 1) {
      if ((i * ni + j) % 20 == 0) {
        fprintf(stderr,"\n");
      }
       else {
      }
      fprintf(stderr,"%0.2lf ",C[i][j]);
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","C");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_gemm(int ni,int nj,int nk,double alpha,double beta,double C[1000][1100],double A[1000][1200],double B[1200][1100])
{
  int i;
  int j;
  int k;
  double A_buf0;
  for (i = 0; i <= -1 + ni; i += 1) {
    for (j = 0; j <= -1 + nj; j += 1) {
      C[i][j] *= beta;
    }
  }
  for (i = 0; i <= -1 + ni; i += 1) {
    for (k = 0; k <= -1 + nk; k += 1) {
      A_buf0 = A[i][k];
      for (j = 0; j <= -1 + nj; j += 1) {
        C[i][j] += alpha * A_buf0 * B[k][j];
      }
    }
  }
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int ni = 1000;
  int nj = 1100;
  int nk = 1200;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double (*C)[1000][1100];
  C = ((double (*)[1000][1100])(polybench_alloc_data(((1000 + 0) * (1100 + 0)),(sizeof(double )))));
  ;
  double (*A)[1000][1200];
  A = ((double (*)[1000][1200])(polybench_alloc_data(((1000 + 0) * (1200 + 0)),(sizeof(double )))));
  ;
  double (*B)[1200][1100];
  B = ((double (*)[1200][1100])(polybench_alloc_data(((1200 + 0) * (1100 + 0)),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(ni,nj,nk,&alpha,&beta, *C, *A, *B);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_gemm(ni,nj,nk,alpha,beta, *C, *A, *B);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(ni,nj, *C);
/* Be clean. */
  free((void *)C);
  ;
  free((void *)A);
  ;
  free((void *)B);
  ;
  return 0;
}
