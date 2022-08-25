/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* bicg.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
#include <bicg.h>
/* Array initialization. */

static void init_array(int m,int n,double A[2100][1900],double r[2100],double p[1900])
{
  int i;
  int j;
  for (i = 0; i <= -1 + m; i += 1) {
    p[i] = ((double )(i % m)) / m;
  }
  for (i = 0; i <= -1 + n; i += 1) {
    r[i] = ((double )(i % n)) / n;
  }
  for (i = 0; i <= -1 + n; i += 1) {
    for (j = 0; j <= -1 + m; j += 1) {
      A[i][j] = ((double )(i * (j + 1) % n)) / n;
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int m,int n,double s[1900],double q[2100])
{
  int i;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","s");
  for (i = 0; i <= -1 + m; i += 1) {
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
    fprintf(stderr,"%0.2lf ",s[i]);
  }
  fprintf(stderr,"\nend   dump: %s\n","s");
  fprintf(stderr,"begin dump: %s","q");
  for (i = 0; i <= -1 + n; i += 1) {
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
    fprintf(stderr,"%0.2lf ",q[i]);
  }
  fprintf(stderr,"\nend   dump: %s\n","q");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_bicg(int m,int n,double A[2100][1900],double s[1900],double q[2100],double p[1900],double r[2100])
{
  int i;
  int j;
  
#pragma scop
  i = 0;
  while(i < m){
    s[i] = 0;
    i++;
  }
  i = 0;
  while(i < n){
    q[i] = 0.0;
    j = 0;
    while(j < m){
      s[j] = s[j] + r[i] * A[i][j];
      q[i] = q[i] + A[i][j] * p[j];
      j++;
    }
    i++;
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 2100;
  int m = 1900;
/* Variable declaration/allocation. */
  double (*A)[2100][1900];
  A = ((double (*)[2100][1900])(polybench_alloc_data(((2100 + 0) * (1900 + 0)),(sizeof(double )))));
  ;
  double (*s)[1900];
  s = ((double (*)[1900])(polybench_alloc_data((1900 + 0),(sizeof(double )))));
  ;
  double (*q)[2100];
  q = ((double (*)[2100])(polybench_alloc_data((2100 + 0),(sizeof(double )))));
  ;
  double (*p)[1900];
  p = ((double (*)[1900])(polybench_alloc_data((1900 + 0),(sizeof(double )))));
  ;
  double (*r)[2100];
  r = ((double (*)[2100])(polybench_alloc_data((2100 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(m,n, *A, *r, *p);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_bicg(m,n, *A, *s, *q, *p, *r);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(m,n, *s, *q);
/* Be clean. */
  free((void *)A);
  ;
  free((void *)s);
  ;
  free((void *)q);
  ;
  free((void *)p);
  ;
  free((void *)r);
  ;
  return 0;
}
