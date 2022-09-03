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

static void init_array()
{
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int n,double R[16777216],double G[16777216],double B[16777216])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","R");
  for (i = 0; i <= -1 + n; i += 1) {
    fprintf(stderr,"%0.2lf ",R[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","R");
  fprintf(stderr,"begin dump: %s","G");
  for (i = 0; i <= -1 + n; i += 1) {
    fprintf(stderr,"%0.2lf ",G[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","G");
  fprintf(stderr,"begin dump: %s","B");
  for (i = 0; i <= -1 + n; i += 1) {
    fprintf(stderr,"%0.2lf ",B[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","B");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_colormap(int n,double R[16777216],double G[16777216],double B[16777216])
{
  int i;
  
#pragma scop
  i = 0;
  while(i < n){
    R[i] = ((double )(i * 65535L / (n - 1)));
    G[i] = ((double )(i * 65535L / (n - 1)));
    B[i] = ((double )(i * 65535L / (n - 1)));
    i++;
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16777216;
/* Variable declaration/allocation. */
  double (*R)[16777216];
  R = ((double (*)[16777216])(polybench_alloc_data((16777216 + 0),(sizeof(double )))));
  ;
  double (*G)[16777216];
  G = ((double (*)[16777216])(polybench_alloc_data((16777216 + 0),(sizeof(double )))));
  ;
  double (*B)[16777216];
  B = ((double (*)[16777216])(polybench_alloc_data((16777216 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array();
/* Start timer. */
  ;
/* Run kernel. */
  kernel_colormap(n, *R, *G, *B);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(n, *R, *G, *B);
/* Be clean. */
  free((void *)R);
  ;
  free((void *)G);
  ;
  free((void *)B);
  ;
  return 0;
}
