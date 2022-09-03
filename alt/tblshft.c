/**
 * This benchmark created on August 29, 2022.
 *
 * The table shift kernel appeared in MiBench Version 1.0,
 * consumer/tiff-v3.5.4/libtiff/tif_pixarlog.c, LOC 565-576,
 * with following explanation:
 * > Since we lose info anyway on 16-bit data, we set up a 14-bit
 * > table and shift 16-bit values down two bits on input.
 * > saves a little table space.
 *
 * Web address: https://vhosts.eecs.umich.edu/mibench
 */
/* tblshft.c */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include <tblshft.h>
#define ONE      1250       /* token value of 1.0 exactly */
#define RATIO	 1.004		/* nominal ratio for log part */
/* Array initialization. */
#include <omp.h> 

static void init_array(int tsz,double TLF[2049])
{
  int i;
  int j;
  int nlin;
  double b;
  double c;
  double linstep;
  double v;
  int TSIZE = tsz - 1;
  j = 0;
  c = log(1.004);
  nlin = (1. / c);
  c = 1. / nlin;
  b = exp(-c * 1250);
  linstep = b * c * exp(1.);
  for (i = 0; i <= -1 + nlin; i += 1) {
    v = i * linstep;
    TLF[j++] = ((double )v);
  }
  for (i = nlin; i <= -1 + TSIZE; i += 1) {
    TLF[j++] = b * exp(c * i);
  }
  TLF[TSIZE] = TLF[TSIZE - 1];
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int f8sz,int f14sz,double F8[65536],double F14[4194304])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","F8");
  for (i = 0; i <= -1 + f8sz; i += 1) {
    fprintf(stderr,"%0.2lf ",F8[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","F8");
  fprintf(stderr,"begin dump: %s","F14");
  for (j = 0; j <= -1 + f14sz; j += 1) {
    fprintf(stderr,"%0.2lf ",F14[j]);
    if (j % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","F14");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_tblshft(int f8sz,int f14sz,int tsz,double F8[65536],double F14[4194304],double TLF[2049])
{
  int i;
  int j;
  double F14SZM1 = (double )(f14sz - 1);
  double F8SZM1 = (double )(f8sz - 1);
  
#pragma scop
  j = 0;
  
#pragma omp parallel for private (i) firstprivate (f14sz,j,F14SZM1)
  for (i = 0; i <= f14sz - 1; i += 1) {
    while(((double )i) / F14SZM1 * (((double )i) / F14SZM1) > TLF[j] * TLF[j + 1])
      j++;
    F14[i] = ((double )j);
  }
  j = 0;
  
#pragma omp parallel for private (i) firstprivate (f8sz,j,F8SZM1)
  for (i = 0; i <= f8sz - 1; i += 1) {
    while(((double )i) / F8SZM1 * (((double )i) / F8SZM1) > TLF[j] * TLF[j + 1])
      j++;
    F8[i] = ((double )j);
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int f8sz = 65536;
  int f14sz = 4194304;
  int tsz = 2049;
/* Variable declaration/allocation. */
  double (*F8)[65536];
  F8 = ((double (*)[65536])(polybench_alloc_data((65536 + 0),(sizeof(double )))));
  ;
  double (*F14)[4194304];
  F14 = ((double (*)[4194304])(polybench_alloc_data((4194304 + 0),(sizeof(double )))));
  ;
  double (*TLF)[2049];
  TLF = ((double (*)[2049])(polybench_alloc_data((2049 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(tsz, *TLF);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_tblshft(f8sz,f14sz,tsz, *F8, *F14, *TLF);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(f8sz,f14sz, *F8, *F14);
/* Be clean. */
  free((void *)TLF);
  ;
  free((void *)F14);
  ;
  free((void *)F8);
  ;
  return 0;
}
