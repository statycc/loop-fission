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
static
void init_array (int w, int h, DATA_TYPE* alpha,
		 DATA_TYPE POLYBENCH_2D(imgIn,W,H,w,h),
		 DATA_TYPE POLYBENCH_2D(imgOut,W,H,w,h))
{
  int i, j;

  *alpha=0.25; //parameter of the filter

  //input should be between 0 and 1 (grayscale image pixel)
  for (i = 0; i < w; i++)
     for (j = 0; j < h; j++)
	imgIn[i][j] = (DATA_TYPE) ((313*i+991*j)%65536) / 65535.0f;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int w, int h,
		 DATA_TYPE POLYBENCH_2D(imgOut,W,H,w,h))

{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("imgOut");
  for (i = 0; i < w; i++)
    for (j = 0; j < h; j++) {
      if ((i * h + j) % 20 == 0) fprintf(POLYBENCH_DUMP_TARGET, "\n");
      fprintf(POLYBENCH_DUMP_TARGET, DATA_PRINTF_MODIFIER, imgOut[i][j]);
    }
  POLYBENCH_DUMP_END("imgOut");
  POLYBENCH_DUMP_FINISH;
}



/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/* Original code provided by Gael Deest */
static
void kernel_deriche(int w, int h, DATA_TYPE alpha,
       DATA_TYPE POLYBENCH_2D(imgIn, W, H, w, h),
       DATA_TYPE POLYBENCH_2D(imgOut, W, H, w, h),
       DATA_TYPE POLYBENCH_2D(y1, W, H, w, h),
       DATA_TYPE POLYBENCH_2D(y2, W, H, w, h)) 
{
   int i, j;
   float xm1, tm1, ym1, ym2;
   float xp1, xp2;
   float tp1, tp2;
   float yp1, yp2;
   float k;
   float a1, a2, a3, a4, a5, a6, a7, a8;
   float b1, b2, c1, c2;
   #pragma scop
   k = (1.0f - expf(-alpha)) * (1.0f - expf(-alpha)) / (1.0f + 2.0f * alpha * expf(-alpha) - expf(2.0f * alpha));
   a1 = a5 = k;
   a2 = a6 = k * expf(-alpha) * (alpha - 1.0f);
   a3 = a7 = k * expf(-alpha) * (alpha + 1.0f);
   a4 = a8 = -k * expf(-2.0f * alpha);
   b1 = powf(2.0f, -alpha);
   b2 = -expf(-2.0f * alpha);
   c1 = c2 = 1;
   #pragma omp parallel for default(shared) private(i, j, ym1, ym2, xm1) firstprivate(w, h, a1, a2, b1, b2, imgIn)
   for(i = 0; i < w; i++) {
      ym1 = 0.0f;
      ym2 = 0.0f;
      xm1 = 0.0f;
      /*************** Clava msgError **************
      		Variable xm1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable ym1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable ym2 could not be categorized into any OpenMP Variable Scopeuse : RW
      ****************************************/
      for(j = 0; j < h; j++) {
         y1[i][j] = a1 * imgIn[i][j] + a2 * xm1 + b1 * ym1 + b2 * ym2;
         xm1 = imgIn[i][j];
         ym2 = ym1;
         ym1 = y1[i][j];
      }
   }
   #pragma omp parallel for default(shared) private(i, j, yp1, yp2, xp1, xp2) firstprivate(w, h, a3, a4, b1, b2, imgIn)
   for(i = 0; i < w; i++) {
      yp1 = 0.0f;
      yp2 = 0.0f;
      xp1 = 0.0f;
      xp2 = 0.0f;
      /*************** Clava msgError **************
      		Variable xp1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable xp2 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable yp1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable yp2 could not be categorized into any OpenMP Variable Scopeuse : RW
      ****************************************/
      for(j = h - 1; j >= 0; j--) {
         y2[i][j] = a3 * xp1 + a4 * xp2 + b1 * yp1 + b2 * yp2;
         xp2 = xp1;
         xp1 = imgIn[i][j];
         yp2 = yp1;
         yp1 = y2[i][j];
      }
   }
   #pragma omp parallel for default(shared) private(i, j) firstprivate(w, h, c1, y1, y2)
   for(i = 0; i < w; i++) {
      // #pragma omp parallel for default(shared) private(j) firstprivate(h, i, c1, y1, y2)
      for(j = 0; j < h; j++) {
         imgOut[i][j] = c1 * (y1[i][j] + y2[i][j]);
      }
   }
   #pragma omp parallel for default(shared) private(j, i, tm1, ym1, ym2) firstprivate(h, w, a5, a6, b1, b2, imgOut)
   for(j = 0; j < h; j++) {
      tm1 = 0.0f;
      ym1 = 0.0f;
      ym2 = 0.0f;
      /*************** Clava msgError **************
      		Variable tm1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable ym1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable ym2 could not be categorized into any OpenMP Variable Scopeuse : RW
      ****************************************/
      for(i = 0; i < w; i++) {
         y1[i][j] = a5 * imgOut[i][j] + a6 * tm1 + b1 * ym1 + b2 * ym2;
         tm1 = imgOut[i][j];
         ym2 = ym1;
         ym1 = y1[i][j];
      }
   }
   #pragma omp parallel for default(shared) private(j, i, tp1, tp2, yp1, yp2) firstprivate(h, w, a7, a8, b1, b2, imgOut)
   for(j = 0; j < h; j++) {
      tp1 = 0.0f;
      tp2 = 0.0f;
      yp1 = 0.0f;
      yp2 = 0.0f;
      /*************** Clava msgError **************
      		Variable tp1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable tp2 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable yp1 could not be categorized into any OpenMP Variable Scopeuse : RW
      		Variable yp2 could not be categorized into any OpenMP Variable Scopeuse : RW
      ****************************************/
      for(i = w - 1; i >= 0; i--) {
         y2[i][j] = a7 * tp1 + a8 * tp2 + b1 * yp1 + b2 * yp2;
         tp2 = tp1;
         tp1 = imgOut[i][j];
         yp2 = yp1;
         yp1 = y2[i][j];
      }
   }
   #pragma omp parallel for default(shared) private(i, j) firstprivate(w, h, c2, y1, y2)
   for(i = 0; i < w; i++) {
      // #pragma omp parallel for default(shared) private(j) firstprivate(h, i, c2, y1, y2)
      for(j = 0; j < h; j++)
         imgOut[i][j] = c2 * (y1[i][j] + y2[i][j]);
   }
   #pragma endscop
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int w = W;
  int h = H;

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  POLYBENCH_2D_ARRAY_DECL(imgIn, DATA_TYPE, W, H, w, h);
  POLYBENCH_2D_ARRAY_DECL(imgOut, DATA_TYPE, W, H, w, h);
  POLYBENCH_2D_ARRAY_DECL(y1, DATA_TYPE, W, H, w, h);
  POLYBENCH_2D_ARRAY_DECL(y2, DATA_TYPE, W, H, w, h);


  /* Initialize array(s). */
  init_array (w, h, &alpha, POLYBENCH_ARRAY(imgIn), POLYBENCH_ARRAY(imgOut));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_deriche (w, h, alpha, POLYBENCH_ARRAY(imgIn), POLYBENCH_ARRAY(imgOut), POLYBENCH_ARRAY(y1), POLYBENCH_ARRAY(y2));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(w, h, POLYBENCH_ARRAY(imgOut)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(imgIn);
  POLYBENCH_FREE_ARRAY(imgOut);
  POLYBENCH_FREE_ARRAY(y1);
  POLYBENCH_FREE_ARRAY(y2);

  return 0;
}
