/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* deriche.c: this file is part of PolyBench/C */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
#include <deriche.h>
/* Array initialization. */
#include <omp.h> 

static void init_array(int w,int h,float *alpha,float imgIn[4096][2160],float imgOut[4096][2160])
{
  int i;
  int j;
   *alpha = 0.25;
  
#pragma omp parallel for private (i,j) firstprivate (w,h)
  for (i = 0; i <= -1 + w; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= -1 + h; j += 1) {
      imgIn[i][j] = ((float )((313 * i + 991 * j) % 65536)) / 65535.0f;
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int w,int h,float imgOut[4096][2160])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","imgOut");
  for (i = 0; i <= -1 + w; i += 1) {
    for (j = 0; j <= -1 + h; j += 1) {
      if ((i * h + j) % 20 == 0) {
        fprintf(stderr,"\n");
      }
       else {
      }
      fprintf(stderr,"%0.2f ",imgOut[i][j]);
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","imgOut");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/* Original code provided by Gael Deest */

static void kernel_deriche(int w,int h,float alpha,float imgIn[4096][2160],float imgOut[4096][2160],float y1[4096][2160],float y2[4096][2160])
{
  int i;
  int j;
  float xm1;
  float tm1;
  float ym1;
  float ym2;
  float xp1;
  float xp2;
  float tp1;
  float tp2;
  float yp1;
  float yp2;
  float k;
  float a1;
  float a2;
  float a3;
  float a4;
  float a5;
  float a6;
  float a7;
  float a8;
  float b1;
  float b2;
  float c1;
  float c2;
  k = (1.0f - expf(-alpha)) * (1.0f - expf(-alpha)) / (1.0f + 2.0f * alpha * expf(-alpha) - expf(2.0f * alpha));
  a1 = a5 = k;
  a2 = a6 = k * expf(-alpha) * (alpha - 1.0f);
  a3 = a7 = k * expf(-alpha) * (alpha + 1.0f);
  a4 = a8 = -k * expf(- 2.0f * alpha);
  b1 = powf(2.0f,-alpha);
  b2 = -expf(- 2.0f * alpha);
  c1 = c2 = 1;
  
#pragma omp parallel for private (xm1,ym1,ym2,i,j)
  for (i = 0; i <= -1 + w; i += 1) {
    ym1 = 0.0f;
    ym2 = 0.0f;
    xm1 = 0.0f;
    for (j = 0; j <= -1 + h; j += 1) {
      y1[i][j] = a1 * imgIn[i][j] + a2 * xm1 + b1 * ym1 + b2 * ym2;
      xm1 = imgIn[i][j];
      ym2 = ym1;
      ym1 = y1[i][j];
    }
  }
  
#pragma omp parallel for private (xp1,xp2,yp1,yp2,i,j)
  for (i = 0; i <= -1 + w; i += 1) {
    yp1 = 0.0f;
    yp2 = 0.0f;
    xp1 = 0.0f;
    xp2 = 0.0f;
    for (j = - 1 + h; j >= 0; j += -1) {
      y2[i][j] = a3 * xp1 + a4 * xp2 + b1 * yp1 + b2 * yp2;
      xp2 = xp1;
      xp1 = imgIn[i][j];
      yp2 = yp1;
      yp1 = y2[i][j];
    }
  }
  
#pragma omp parallel for private (i,j)
  for (i = 0; i <= -1 + w; i += 1) {
    
#pragma omp parallel for private (j) firstprivate (c1)
    for (j = 0; j <= -1 + h; j += 1) {
      imgOut[i][j] = c1 * (y1[i][j] + y2[i][j]);
    }
  }
  
#pragma omp parallel for private (tm1,ym1,ym2,i,j)
  for (j = 0; j <= -1 + h; j += 1) {
    tm1 = 0.0f;
    ym1 = 0.0f;
    ym2 = 0.0f;
    for (i = 0; i <= -1 + w; i += 1) {
      y1[i][j] = a5 * imgOut[i][j] + a6 * tm1 + b1 * ym1 + b2 * ym2;
      tm1 = imgOut[i][j];
      ym2 = ym1;
      ym1 = y1[i][j];
    }
  }
  
#pragma omp parallel for private (tp1,tp2,yp1,yp2,i,j)
  for (j = 0; j <= -1 + h; j += 1) {
    tp1 = 0.0f;
    tp2 = 0.0f;
    yp1 = 0.0f;
    yp2 = 0.0f;
    for (i = - 1 + w; i >= 0; i += -1) {
      y2[i][j] = a7 * tp1 + a8 * tp2 + b1 * yp1 + b2 * yp2;
      tp2 = tp1;
      tp1 = imgOut[i][j];
      yp2 = yp1;
      yp1 = y2[i][j];
    }
  }
  
#pragma omp parallel for private (i,j) firstprivate (w,h)
  for (i = 0; i <= -1 + w; i += 1) {
    
#pragma omp parallel for private (j) firstprivate (c2)
    for (j = 0; j <= -1 + h; j += 1) {
      imgOut[i][j] = c2 * (y1[i][j] + y2[i][j]);
    }
  }
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int w = 4096;
  int h = 2160;
/* Variable declaration/allocation. */
  float alpha;
  float (*imgIn)[4096][2160];
  imgIn = ((float (*)[4096][2160])(polybench_alloc_data(((4096 + 0) * (2160 + 0)),(sizeof(float )))));
  ;
  float (*imgOut)[4096][2160];
  imgOut = ((float (*)[4096][2160])(polybench_alloc_data(((4096 + 0) * (2160 + 0)),(sizeof(float )))));
  ;
  float (*y1)[4096][2160];
  y1 = ((float (*)[4096][2160])(polybench_alloc_data(((4096 + 0) * (2160 + 0)),(sizeof(float )))));
  ;
  float (*y2)[4096][2160];
  y2 = ((float (*)[4096][2160])(polybench_alloc_data(((4096 + 0) * (2160 + 0)),(sizeof(float )))));
  ;
/* Initialize array(s). */
  init_array(w,h,&alpha, *imgIn, *imgOut);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_deriche(w,h,alpha, *imgIn, *imgOut, *y1, *y2);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
       by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(w,h, *imgOut);
/* Be clean. */
  free((void *)imgIn);
  ;
  free((void *)imgOut);
  ;
  free((void *)y1);
  ;
  free((void *)y2);
  ;
  return 0;
}
