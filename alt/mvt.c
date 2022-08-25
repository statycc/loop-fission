/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* mvt.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
#include <mvt.h>
/* Array initialization. */

static void init_array(int n,double x1[2000],double x2[2000],double y_1[2000],double y_2[2000],double A[2000][2000])
{
  int i;
  int j;
  for (i = 0; i <= -1 + n; i += 1) {
    x1[i] = ((double )(i % n)) / n;
  }
  for (i = 0; i <= -1 + n; i += 1) {
    x2[i] = ((double )((i + 1) % n)) / n;
  }
  for (i = 0; i <= -1 + n; i += 1) {
    y_1[i] = ((double )((i + 3) % n)) / n;
  }
  for (i = 0; i <= -1 + n; i += 1) {
    y_2[i] = ((double )((i + 4) % n)) / n;
  }
  for (i = 0; i <= -1 + n; i += 1) {
    for (j = 0; j <= -1 + n; j += 1) {
      A[i][j] = ((double )(i * j % n)) / n;
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int n,double x1[2000],double x2[2000])
{
  int i;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","x1");
  for (i = 0; i <= -1 + n; i += 1) {
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
    fprintf(stderr,"%0.2lf ",x1[i]);
  }
  fprintf(stderr,"\nend   dump: %s\n","x1");
  fprintf(stderr,"begin dump: %s","x2");
  for (i = 0; i <= -1 + n; i += 1) {
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
    fprintf(stderr,"%0.2lf ",x2[i]);
  }
  fprintf(stderr,"\nend   dump: %s\n","x2");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_mvt(int n,double x1[2000],double x2[2000],double y_1[2000],double y_2[2000],double A[2000][2000])
{
  int i;
  int j;
  
#pragma scop
  i = 0;
  while(i < n){
    j = 0;
    while(j < n){
      x1[i] = x1[i] + A[i][j] * y_1[j];
      j++;
    }
    i++;
  }
  i = 0;
  while(i < n){
    j = 0;
    while(j < n){
      x2[i] = x2[i] + A[j][i] * y_2[j];
      j++;
    }
    i++;
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 2000;
/* Variable declaration/allocation. */
  double (*A)[2000][2000];
  A = ((double (*)[2000][2000])(polybench_alloc_data(((2000 + 0) * (2000 + 0)),(sizeof(double )))));
  ;
  double (*x1)[2000];
  x1 = ((double (*)[2000])(polybench_alloc_data((2000 + 0),(sizeof(double )))));
  ;
  double (*x2)[2000];
  x2 = ((double (*)[2000])(polybench_alloc_data((2000 + 0),(sizeof(double )))));
  ;
  double (*y_1)[2000];
  y_1 = ((double (*)[2000])(polybench_alloc_data((2000 + 0),(sizeof(double )))));
  ;
  double (*y_2)[2000];
  y_2 = ((double (*)[2000])(polybench_alloc_data((2000 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(n, *x1, *x2, *y_1, *y_2, *A);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_mvt(n, *x1, *x2, *y_1, *y_2, *A);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(n, *x1, *x2);
/* Be clean. */
  free((void *)A);
  ;
  free((void *)x1);
  ;
  free((void *)x2);
  ;
  free((void *)y_1);
  ;
  free((void *)y_2);
  ;
  return 0;
}
