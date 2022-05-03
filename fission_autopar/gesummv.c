#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <polybench.h>
#include <gesummv.h>
/**
* This version is stamped on May 10, 2016
*
* Contact:
*   Louis-Noel Pouchet <pouchet.ohio-state.edu>
*   Tomofumi Yuki <tomofumi.yuki.fr>
*
* Web address: http://polybench.sourceforge.net
*/
/*gesummv.c: this file is part of PolyBench/C*/
/*Include polybench common header.*/
/*Include benchmark-specific header.*/
/*Array initialization.*/
static void init_array(int n, double *alpha, double *beta, double A[1300][1300], double B[1300][1300], double x[1300]) {
   int i, j;
   *alpha = 1.5;
   *beta = 1.2;
   for(i = 0; i < n; i++) {
      x[i] = (double) (i % n) / n;
      for(j = 0; j < n; j++) {
         A[i][j] = (double) ((i * j + 1) % n) / n;
         B[i][j] = (double) ((i * j + 2) % n) / n;
      }
   }
}

/*DCE code. Must scan the entire live-out data.
Can be used also to check the correctness of the output.*/
static void print_array(int n, double y[1300]) {
   int i;
   fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
   fprintf(stderr, "begin dump: %s", "y");
   for(i = 0; i < n; i++) {
      if(i % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", y[i]);
   }
   fprintf(stderr, "\nend   dump: %s\n", "y");
   fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

/*Main computational kernel. The whole function will be timed,
including the call and return.*/
static void kernel_gesummv(int n, double alpha, double beta, double A[1300][1300], double B[1300][1300], double tmp[1300], double x[1300], double y[1300]) {
   int i, j;
   #pragma scop
   #pragma omp parallel for default(shared) private(i, j) firstprivate(n, A, x)
   for(i = 0; i < n; i++) {
      tmp[i] = 0.0;
      #pragma omp parallel for default(shared) private(j) firstprivate(n, i, A, x) reduction(+ : tmp[i])
      for(j = 0; j < n; j++) {
         tmp[i] = A[i][j] * x[j] + tmp[i];
      }
   }
   #pragma omp parallel for default(shared) private(i, j) firstprivate(n, B, x)
   for(i = 0; i < n; i++) {
      y[i] = 0.0;
      #pragma omp parallel for default(shared) private(j) firstprivate(n, i, B, x) reduction(+ : y[i])
      for(j = 0; j < n; j++) {
         y[i] = B[i][j] * x[j] + y[i];
      }
   }
   // this has to come after because it reads both arrays, this transform maybe wrong
   #pragma omp parallel for default(shared) private(i) firstprivate(n, alpha, beta, tmp)
   for(i = 0; i < n; i++) {
      y[i] = alpha * tmp[i] + beta * y[i];
   }
   #pragma endscop
}

int main(int argc, char **argv) {
   /*Retrieve problem size.*/
   int n = 1300;
   /*Variable declaration/allocation.*/
   double alpha;
   double beta;
   double (*A)[1300][1300];
   A = (double (*)[1300][1300]) polybench_alloc_data((1300 + 0) * (1300 + 0), sizeof(double));
   ;
   double (*B)[1300][1300];
   B = (double (*)[1300][1300]) polybench_alloc_data((1300 + 0) * (1300 + 0), sizeof(double));
   ;
   double (*tmp)[1300];
   tmp = (double (*)[1300]) polybench_alloc_data(1300 + 0, sizeof(double));
   ;
   double (*x)[1300];
   x = (double (*)[1300]) polybench_alloc_data(1300 + 0, sizeof(double));
   ;
   double (*y)[1300];
   y = (double (*)[1300]) polybench_alloc_data(1300 + 0, sizeof(double));
   ;
   /*Initialize array(s).*/
   init_array(n, &alpha, &beta, *A, *B, *x);
   /*Start timer.*/
   ;
   /*Run kernel.*/
   kernel_gesummv(n, alpha, beta, *A, *B, *tmp, *x, *y);
   /*Stop and print timer.*/
   ;
   ;
   /*Prevent dead-code elimination. All live-out data must be printed
   by the function call in argument.*/
   if(argc > 42 && !strcmp(argv[0], "")) print_array(n, *y);
   /*Be clean.*/
   free((void *) A);
   ;
   free((void *) B);
   ;
   free((void *) tmp);
   ;
   free((void *) x);
   ;
   free((void *) y);
   ;
   
   return 0;
}
