#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <polybench.h>
#include <jacobi-1d.h>
/**
* This version is stamped on May 10, 2016
*
* Contact:
*   Louis-Noel Pouchet <pouchet.ohio-state.edu>
*   Tomofumi Yuki <tomofumi.yuki.fr>
*
* Web address: http://polybench.sourceforge.net
*/
/*jacobi-1d.c: this file is part of PolyBench/C*/
/*Include polybench common header.*/
/*Include benchmark-specific header.*/
/*Array initialization.*/
static void init_array(int n, double A[2000], double B[2000]) {
   int i;
   for(i = 0; i < n; i++) {
      A[i] = ((double) i + 2) / n;
      B[i] = ((double) i + 3) / n;
   }
}

/*DCE code. Must scan the entire live-out data.
Can be used also to check the correctness of the output.*/
static void print_array(int n, double A[2000]) {
   int i;
   fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
   fprintf(stderr, "begin dump: %s", "A");
   for(i = 0; i < n; i++) {
      if(i % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", A[i]);
   }
   fprintf(stderr, "\nend   dump: %s\n", "A");
   fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

/*Main computational kernel. The whole function will be timed,
including the call and return.*/
static void kernel_jacobi_1d(int tsteps, int n, double A[2000], double B[2000]) {
   int t, i;
   #pragma loop1
   for(t = 0; t < tsteps; t++) {
      for(i = 1; i < n - 1; i++)
         B[i] = 0.33333 * (A[i - 1] + A[i] + A[i + 1]);
   }
   #pragma loop2
   for(t = 0; t < tsteps; t++) {
      for(i = 1; i < n - 1; i++)
         A[i] = 0.33333 * (B[i - 1] + B[i] + B[i + 1]);
   }
}

int main(int argc, char **argv) {
   /*Retrieve problem size.*/
   int n = 2000;
   int tsteps = 500;
   /*Variable declaration/allocation.*/
   double (*A)[2000];
   A = (double (*)[2000]) polybench_alloc_data(2000 + 0, sizeof(double));
   ;
   double (*B)[2000];
   B = (double (*)[2000]) polybench_alloc_data(2000 + 0, sizeof(double));
   ;
   /*Initialize array(s).*/
   init_array(n, *A, *B);
   /*Start timer.*/
   ;
   /*Run kernel.*/
   kernel_jacobi_1d(tsteps, n, *A, *B);
   /*Stop and print timer.*/
   ;
   ;
   /*Prevent dead-code elimination. All live-out data must be printed
   by the function call in argument.*/
   if(argc > 42 && !strcmp(argv[0], "")) print_array(n, *A);
   /*Be clean.*/
   free((void *) A);
   ;
   free((void *) B);
   ;
   
   return 0;
}
