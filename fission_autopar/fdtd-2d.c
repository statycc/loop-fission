#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <polybench.h>
#include <fdtd-2d.h>
/**
* This version is stamped on May 10, 2016
*
* Contact:
*   Louis-Noel Pouchet <pouchet.ohio-state.edu>
*   Tomofumi Yuki <tomofumi.yuki.fr>
*
* Web address: http://polybench.sourceforge.net
*/
/*fdtd-2d.c: this file is part of PolyBench/C*/
/*Include polybench common header.*/
/*Include benchmark-specific header.*/
/*Array initialization.*/
static void init_array(int tmax, int nx, int ny, double ex[1000][1200], double ey[1000][1200], double hz[1000][1200], double _fict_[500]) {
   int i, j;
   for(i = 0; i < tmax; i++)
      _fict_[i] = (double) i;
   for(i = 0; i < nx; i++)
      for(j = 0; j < ny; j++) {
         ex[i][j] = ((double) i * (j + 1)) / nx;
         ey[i][j] = ((double) i * (j + 2)) / ny;
         hz[i][j] = ((double) i * (j + 3)) / nx;
      }
}

/*DCE code. Must scan the entire live-out data.
Can be used also to check the correctness of the output.*/
static void print_array(int nx, int ny, double ex[1000][1200], double ey[1000][1200], double hz[1000][1200]) {
   int i, j;
   fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
   fprintf(stderr, "begin dump: %s", "ex");
   for(i = 0; i < nx; i++)
      for(j = 0; j < ny; j++) {
         if((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
         fprintf(stderr, "%0.2lf ", ex[i][j]);
      }
   fprintf(stderr, "\nend   dump: %s\n", "ex");
   fprintf(stderr, "==END   DUMP_ARRAYS==\n");
   fprintf(stderr, "begin dump: %s", "ey");
   for(i = 0; i < nx; i++)
      for(j = 0; j < ny; j++) {
         if((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
         fprintf(stderr, "%0.2lf ", ey[i][j]);
      }
   fprintf(stderr, "\nend   dump: %s\n", "ey");
   fprintf(stderr, "begin dump: %s", "hz");
   for(i = 0; i < nx; i++)
      for(j = 0; j < ny; j++) {
         if((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
         fprintf(stderr, "%0.2lf ", hz[i][j]);
      }
   fprintf(stderr, "\nend   dump: %s\n", "hz");
}

/*Main computational kernel. The whole function will be timed,
including the call and return.*/
static void kernel_fdtd_2d(int tmax, int nx, int ny, double ex[1000][1200], double ey[1000][1200], double hz[1000][1200], double _fict_[500]) {
   int t, i, j;
   #pragma scop
   #pragma omp parallel for default(shared) private(t, j, i) firstprivate(tmax, ny, nx, _fict_, hz) reduction(- : ey[:1000][:1200])
   for(t = 0; t < tmax; t++) {
      // #pragma omp parallel for default(shared) private(j) firstprivate(ny, t, _fict_)
      for(j = 0; j < ny; j++)
         ey[0][j] = _fict_[t];
      // #pragma omp parallel for default(shared) private(i, j) firstprivate(nx, ny, hz)
      for(i = 1; i < nx; i++) {
         // #pragma omp parallel for default(shared) private(j) firstprivate(ny, i, hz)
         for(j = 0; j < ny; j++)
            ey[i][j] = ey[i][j] - 0.5 * (hz[i][j] - hz[i - 1][j]);
      }
   }
   #pragma omp parallel for default(shared) private(t, i, j) firstprivate(tmax, nx, ny, hz) reduction(- : ex[:1000][:1200])
   for(t = 0; t < tmax; t++) {
      // #pragma omp parallel for default(shared) private(i, j) firstprivate(nx, ny, hz)
      for(i = 0; i < nx; i++) {
         // #pragma omp parallel for default(shared) private(j) firstprivate(ny, i, hz)
         for(j = 1; j < ny; j++)
            ex[i][j] = ex[i][j] - 0.5 * (hz[i][j] - hz[i][j - 1]);
      }
   }
   #pragma omp parallel for default(shared) private(t, i, j) firstprivate(tmax, nx, ny, ex, ey) reduction(- : hz[:1000][:1200])
   for(t = 0; t < tmax; t++) {
      // #pragma omp parallel for default(shared) private(i, j) firstprivate(nx, ny, ex, ey)
      for(i = 0; i < nx - 1; i++) {
         // #pragma omp parallel for default(shared) private(j) firstprivate(ny, i, ex, ey)
         for(j = 0; j < ny - 1; j++)
            hz[i][j] = hz[i][j] - 0.7 * (ex[i][j + 1] - ex[i][j] + ey[i + 1][j] - ey[i][j]);
      }
   }
   #pragma endscop
}

int main(int argc, char **argv) {
   /*Retrieve problem size.*/
   int tmax = 500;
   int nx = 1000;
   int ny = 1200;
   /*Variable declaration/allocation.*/
   double (*ex)[1000][1200];
   ex = (double (*)[1000][1200]) polybench_alloc_data((1000 + 0) * (1200 + 0), sizeof(double));
   ;
   double (*ey)[1000][1200];
   ey = (double (*)[1000][1200]) polybench_alloc_data((1000 + 0) * (1200 + 0), sizeof(double));
   ;
   double (*hz)[1000][1200];
   hz = (double (*)[1000][1200]) polybench_alloc_data((1000 + 0) * (1200 + 0), sizeof(double));
   ;
   double (*_fict_)[500];
   _fict_ = (double (*)[500]) polybench_alloc_data(500 + 0, sizeof(double));
   ;
   /*Initialize array(s).*/
   init_array(tmax, nx, ny, *ex, *ey, *hz, *_fict_);
   /*Start timer.*/
   ;
   /*Run kernel.*/
   kernel_fdtd_2d(tmax, nx, ny, *ex, *ey, *hz, *_fict_);
   /*Stop and print timer.*/
   polybench_stop_instruments;
   polybench_print_instruments;
   ;
   ;
   /*Prevent dead-code elimination. All live-out data must be printed
   by the function call in argument.*/
   if(argc > 42 && !strcmp(argv[0], "")) print_array(nx, ny, *ex, *ey, *hz);
   /*Be clean.*/
   free((void *) ex);
   ;
   free((void *) ey);
   ;
   free((void *) hz);
   ;
   free((void *) _fict_);
   ;
   
   return 0;
}
