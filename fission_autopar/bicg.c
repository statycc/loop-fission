#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <polybench.h>
#include <bicg.h>
/**
* This version is stamped on May 10, 2016
*
* Contact:
*   Louis-Noel Pouchet <pouchet.ohio-state.edu>
*   Tomofumi Yuki <tomofumi.yuki.fr>
*
* Web address: http://polybench.sourceforge.net
*/
/*bicg.c: this file is part of PolyBench/C*/
/*Include polybench common header.*/
/*Include benchmark-specific header.*/
/*Array initialization.*/
static void init_array(int m, int n, double A[2100][1900], double r[2100], double p[1900]) {
   int i, j;
   for(i = 0; i < m; i++)
      p[i] = (double) (i % m) / m;
   for(i = 0; i < n; i++) {
      r[i] = (double) (i % n) / n;
      for(j = 0; j < m; j++)
         A[i][j] = (double) (i * (j + 1) % n) / n;
   }
}

/*DCE code. Must scan the entire live-out data.
Can be used also to check the correctness of the output.*/
static void print_array(int m, int n, double s[1900], double q[2100]) {
   int i;
   fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
   fprintf(stderr, "begin dump: %s", "s");
   for(i = 0; i < m; i++) {
      if(i % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", s[i]);
   }
   fprintf(stderr, "\nend   dump: %s\n", "s");
   fprintf(stderr, "begin dump: %s", "q");
   for(i = 0; i < n; i++) {
      if(i % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2lf ", q[i]);
   }
   fprintf(stderr, "\nend   dump: %s\n", "q");
   fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

/*Main computational kernel. The whole function will be timed,
including the call and return.*/
static void kernel_bicg(int m, int n, double A[2100][1900], double s[1900], double q[2100], double p[1900], double r[2100]) {
   int i, j;
   #pragma loop1
   #pragma omp parallel for default(shared) private(i) firstprivate(m)
   for(i = 0; i < m; i++)
      s[i] = 0;
   #pragma loop2
   #pragma omp parallel for default(shared) private(i, j) firstprivate(n, m, r, A) reduction(+ : s[:1900])
   for(i = 0; i < n; i++) {
      for(j = 0; j < m; j++) {
         s[j] = s[j] + r[i] * A[i][j];
      }
   }
   #pragma loop3
   #pragma omp parallel for default(shared) private(i, j) firstprivate(n, m, A, p)
   for(i = 0; i < n; i++) {
      q[i] = 0.0;
      for(j = 0; j < m; j++) {
         q[i] = q[i] + A[i][j] * p[j];
      }
   }
}

int main(int argc, char **argv) {
   /*Retrieve problem size.*/
   int n = 2100;
   int m = 1900;
   /*Variable declaration/allocation.*/
   double (*A)[2100][1900];
   A = (double (*)[2100][1900]) polybench_alloc_data((2100 + 0) * (1900 + 0), sizeof(double));
   ;
   double (*s)[1900];
   s = (double (*)[1900]) polybench_alloc_data(1900 + 0, sizeof(double));
   ;
   double (*q)[2100];
   q = (double (*)[2100]) polybench_alloc_data(2100 + 0, sizeof(double));
   ;
   double (*p)[1900];
   p = (double (*)[1900]) polybench_alloc_data(1900 + 0, sizeof(double));
   ;
   double (*r)[2100];
   r = (double (*)[2100]) polybench_alloc_data(2100 + 0, sizeof(double));
   ;
   /*Initialize array(s).*/
   init_array(m, n, *A, *r, *p);
   /*Start timer.*/
   ;
   /*Run kernel.*/
   kernel_bicg(m, n, *A, *s, *q, *p, *r);
   /*Stop and print timer.*/
   ;
   ;
   /*Prevent dead-code elimination. All live-out data must be printed
   by the function call in argument.*/
   if(argc > 42 && !strcmp(argv[0], "")) print_array(m, n, *s, *q);
   /*Be clean.*/
   free((void *) A);
   ;
   free((void *) s);
   ;
   free((void *) q);
   ;
   free((void *) p);
   ;
   free((void *) r);
   ;
   
   return 0;
}
