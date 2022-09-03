/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* 3mm.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
#include <3mm.h>
/* Array initialization. */
#include <omp.h> 

static void init_array(int ni,int nj,int nk,int nl,int nm,double A[800][1000],double B[1000][900],double C[900][1200],double D[1200][1100])
{
  int i;
  int j;
  
#pragma omp parallel for private (i,j) firstprivate (ni)
  for (i = 0; i <= -1 + ni; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= -1 + nk; j += 1) {
      A[i][j] = ((double )((i * j + 1) % ni)) / (5 * ni);
    }
  }
  
#pragma omp parallel for private (i,j) firstprivate (nk)
  for (i = 0; i <= -1 + nk; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= -1 + nj; j += 1) {
      B[i][j] = ((double )((i * (j + 1) + 2) % nj)) / (5 * nj);
    }
  }
  
#pragma omp parallel for private (i,j) firstprivate (nj)
  for (i = 0; i <= -1 + nj; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= -1 + nm; j += 1) {
      C[i][j] = ((double )(i * (j + 3) % nl)) / (5 * nl);
    }
  }
  
#pragma omp parallel for private (i,j) firstprivate (nl,nm)
  for (i = 0; i <= -1 + nm; i += 1) {
    
#pragma omp parallel for private (j) firstprivate (nk)
    for (j = 0; j <= -1 + nl; j += 1) {
      D[i][j] = ((double )((i * (j + 2) + 2) % nk)) / (5 * nk);
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int ni,int nl,double G[800][1100])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","G");
  for (i = 0; i <= -1 + ni; i += 1) {
    for (j = 0; j <= -1 + nl; j += 1) {
      if ((i * ni + j) % 20 == 0) {
        fprintf(stderr,"\n");
      }
       else {
      }
      fprintf(stderr,"%0.2lf ",G[i][j]);
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","G");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_3mm(int ni,int nj,int nk,int nl,int nm,double E[800][900],double A[800][1000],double B[1000][900],double F[900][1100],double C[900][1200],double D[1200][1100],double G[800][1100])
{
  int i;
  int j;
  int k;
  
#pragma scop
/* E := A*B */
  i = 0;
  while(i < ni){
    j = 0;
    while(j < nj){
      E[i][j] = 0.0;
      k = 0;
      while(k < nk){
        E[i][j] += A[i][k] * B[k][j];
        k++;
      }
      j++;
    }
    i++;
  }
/* F := C*D */
  i = 0;
  while(i < nj){
    j = 0;
    while(j < nl){
      F[i][j] = 0.0;
      k = 0;
      while(k < nm){
        F[i][j] += C[i][k] * D[k][j];
        k++;
      }
      j++;
    }
    i++;
  }
/* G := E*F */
  i = 0;
  while(i < ni){
    j = 0;
    while(j < nl){
      G[i][j] = 0.0;
      k = 0;
      while(k < nj){
        G[i][j] += E[i][k] * F[k][j];
        k++;
      }
      j++;
    }
    i++;
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int ni = 800;
  int nj = 900;
  int nk = 1000;
  int nl = 1100;
  int nm = 1200;
/* Variable declaration/allocation. */
  double (*E)[800][900];
  E = ((double (*)[800][900])(polybench_alloc_data(((800 + 0) * (900 + 0)),(sizeof(double )))));
  ;
  double (*A)[800][1000];
  A = ((double (*)[800][1000])(polybench_alloc_data(((800 + 0) * (1000 + 0)),(sizeof(double )))));
  ;
  double (*B)[1000][900];
  B = ((double (*)[1000][900])(polybench_alloc_data(((1000 + 0) * (900 + 0)),(sizeof(double )))));
  ;
  double (*F)[900][1100];
  F = ((double (*)[900][1100])(polybench_alloc_data(((900 + 0) * (1100 + 0)),(sizeof(double )))));
  ;
  double (*C)[900][1200];
  C = ((double (*)[900][1200])(polybench_alloc_data(((900 + 0) * (1200 + 0)),(sizeof(double )))));
  ;
  double (*D)[1200][1100];
  D = ((double (*)[1200][1100])(polybench_alloc_data(((1200 + 0) * (1100 + 0)),(sizeof(double )))));
  ;
  double (*G)[800][1100];
  G = ((double (*)[800][1100])(polybench_alloc_data(((800 + 0) * (1100 + 0)),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(ni,nj,nk,nl,nm, *A, *B, *C, *D);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_3mm(ni,nj,nk,nl,nm, *E, *A, *B, *F, *C, *D, *G);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(ni,nl, *G);
/* Be clean. */
  free((void *)E);
  ;
  free((void *)A);
  ;
  free((void *)B);
  ;
  free((void *)F);
  ;
  free((void *)C);
  ;
  free((void *)D);
  ;
  free((void *)G);
  ;
  return 0;
}
