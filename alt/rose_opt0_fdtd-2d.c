/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* fdtd-2d.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
#include <fdtd-2d.h>
/* Array initialization. */

static void init_array(int tmax,int nx,int ny,double ex[1000][1200],double ey[1000][1200],double hz[1000][1200],double _fict_[500])
{
  int i;
  int j;
  for (i = 0; i <= -1 + tmax; i += 1) {
    _fict_[i] = ((double )i);
  }
  for (i = 0; i <= -1 + nx; i += 1) {
    for (j = 0; j <= -1 + ny; j += 1) {
      ex[i][j] = ((double )i) * (j + 1) / nx;
    }
  }
  for (i = 0; i <= -1 + nx; i += 1) {
    for (j = 0; j <= -1 + ny; j += 1) {
      ey[i][j] = ((double )i) * (j + 2) / ny;
    }
  }
  for (i = 0; i <= -1 + nx; i += 1) {
    for (j = 0; j <= -1 + ny; j += 1) {
      hz[i][j] = ((double )i) * (j + 3) / nx;
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int nx,int ny,double ex[1000][1200],double ey[1000][1200],double hz[1000][1200])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","ex");
  for (i = 0; i <= -1 + nx; i += 1) {
    for (j = 0; j <= -1 + ny; j += 1) {
      if ((i * nx + j) % 20 == 0) {
        fprintf(stderr,"\n");
      }
       else {
      }
      fprintf(stderr,"%0.2lf ",ex[i][j]);
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","ex");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","ey");
  for (i = 0; i <= -1 + nx; i += 1) {
    for (j = 0; j <= -1 + ny; j += 1) {
      if ((i * nx + j) % 20 == 0) {
        fprintf(stderr,"\n");
      }
       else {
      }
      fprintf(stderr,"%0.2lf ",ey[i][j]);
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","ey");
  fprintf(stderr,"begin dump: %s","hz");
  for (i = 0; i <= -1 + nx; i += 1) {
    for (j = 0; j <= -1 + ny; j += 1) {
      if ((i * nx + j) % 20 == 0) {
        fprintf(stderr,"\n");
      }
       else {
      }
      fprintf(stderr,"%0.2lf ",hz[i][j]);
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","hz");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_fdtd_2d(int tmax,int nx,int ny,double ex[1000][1200],double ey[1000][1200],double hz[1000][1200],double _fict_[500])
{
  int t;
  int i;
  int j;
  
#pragma scop
  t = 0;
  while(t < tmax){
    j = 0;
    while(j < ny){
      ey[0][j] = _fict_[t];
      j++;
    }
    i = 0;
    while(i < nx){
      j = 0;
      while(j < ny){
        ey[i][j] = ey[i][j] - 0.5 * (hz[i][j] - hz[i - 1][j]);
        j++;
      }
      i++;
    }
    i = 0;
    while(i < nx){
      j = 1;
      while(j < ny){
        ex[i][j] = ex[i][j] - 0.5 * (hz[i][j] - hz[i][j - 1]);
        j++;
      }
      i++;
    }
    i = 0;
    while(i < nx - 1){
      j = 0;
      while(j < ny - 1){
        hz[i][j] = hz[i][j] - 0.7 * (ex[i][j + 1] - ex[i][j] + ey[i + 1][j] - ey[i][j]);
        j++;
      }
      i++;
    }
    t++;
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int tmax = 500;
  int nx = 1000;
  int ny = 1200;
/* Variable declaration/allocation. */
  double (*ex)[1000][1200];
  ex = ((double (*)[1000][1200])(polybench_alloc_data(((1000 + 0) * (1200 + 0)),(sizeof(double )))));
  ;
  double (*ey)[1000][1200];
  ey = ((double (*)[1000][1200])(polybench_alloc_data(((1000 + 0) * (1200 + 0)),(sizeof(double )))));
  ;
  double (*hz)[1000][1200];
  hz = ((double (*)[1000][1200])(polybench_alloc_data(((1000 + 0) * (1200 + 0)),(sizeof(double )))));
  ;
  double (*_fict_)[500];
  _fict_ = ((double (*)[500])(polybench_alloc_data((500 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(tmax,nx,ny, *ex, *ey, *hz, *_fict_);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_fdtd_2d(tmax,nx,ny, *ex, *ey, *hz, *_fict_);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(nx,ny, *ex, *ey, *hz);
/* Be clean. */
  free((void *)ex);
  ;
  free((void *)ey);
  ;
  free((void *)hz);
  ;
  free((void *)_fict_);
  ;
  return 0;
}
