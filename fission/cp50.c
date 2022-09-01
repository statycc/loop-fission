/**
 * This benchmark created on August 30, 2022.
 *
 * The table shift kernel appeared in MiBench Version 1.0,
 * office/ghostscript/src/gdevcp50.c, LOC 137-149,
 * and represents Aladdin Ghostscript print routine of
 * sending a page to Mitsubishi CP50 color printer.
 *
 * Web address: https://vhosts.eecs.umich.edu/mibench
 */
/* cp50.c */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include "cp50.h"

/* Array initialization. */
static
void init_array(int ls, int ol,
                DATA_TYPE POLYBENCH_2D(out,LS,OL,ls,ol)) {
    int i, j;

    // simulate gdev_prn_copy_scan_lines procedure
    for (i = 0; i < ol; i++)
        for (j = 0; j < ls; j++)
            out[i][j] = (DATA_TYPE) ((i*j+1));
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int xy,
            DATA_TYPE POLYBENCH_1D(RPLANE,XY,xy),
            DATA_TYPE POLYBENCH_1D(GPLANE,XY,xy),
            DATA_TYPE POLYBENCH_1D(BPLANE,XY,xy))
{
    int i, j;

    for (i = 0; i < xy; i++) {
        fprintf (stderr, DATA_PRINTF_MODIFIER, RPLANE[i]);
        fprintf (stderr, DATA_PRINTF_MODIFIER, GPLANE[i]);
        fprintf (stderr, DATA_PRINTF_MODIFIER, BPLANE[i]);
        if (i % 20 == 0) fprintf (stderr, "\n");
    }
    fprintf (stderr, "\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_cp50(
        int X_PIXEL, int FIRST_COLUMN, int FIRST_LINE, int LAST_LINE,
        DATA_TYPE POLYBENCH_2D(out,LS,OL,ls,ol),
        DATA_TYPE POLYBENCH_1D(RPLANE,XY,xy),
        DATA_TYPE POLYBENCH_1D(GPLANE,XY,xy),
        DATA_TYPE POLYBENCH_1D(BPLANE,XY,xy))
{
    int lnum = FIRST_LINE, last = LAST_LINE;
    int i, col, l;

#pragma scop

    /* Print lines of graphics */
#pragma omp parallel private(lnum,i)
{
    #pragma omp single nowait
    {
        lnum = FIRST_LINE;
        while (lnum <= last) {
            for (i = 0; i < X_PIXEL; i++) {
                col = (lnum - FIRST_LINE) * X_PIXEL + i;
                RPLANE[col] = out[lnum][i * 3 + FIRST_COLUMN];
            }
            lnum++;
        }
    }

    #pragma omp single nowait
    {
        lnum = FIRST_LINE;
        while (lnum <= last) {
            for (i = 0; i < X_PIXEL; i++) {
                col = (lnum - FIRST_LINE) * X_PIXEL + i;
                GPLANE[col] = out[lnum][i * 3 + 1 + FIRST_COLUMN];
            }
            lnum++;
        }
    }

    #pragma omp single nowait
    {
        lnum = FIRST_LINE;
        while (lnum <= last) {
            for (i = 0; i < X_PIXEL; i++) {
                col = (lnum - FIRST_LINE) * X_PIXEL + i;
                BPLANE[col] = out[lnum][i * 3 + 2 + FIRST_COLUMN];
            }
            lnum++;
        }
    }
}
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int x = X;
  int y = Y;
  int xy = XY;
  int fc = FC;
  int ls = LS;
  int fl = FL;
  int ll = LL;
  int ol = OL;

  /* Variable declaration/allocation. */
    POLYBENCH_2D_ARRAY_DECL(out, DATA_TYPE,LS,OL,ls,ol);
    POLYBENCH_1D_ARRAY_DECL(RPLANE,DATA_TYPE,XY,xy);
    POLYBENCH_1D_ARRAY_DECL(GPLANE,DATA_TYPE,XY,xy);
    POLYBENCH_1D_ARRAY_DECL(BPLANE,DATA_TYPE,XY,xy);

  /* Initialize array(s). */
  init_array (ls, ol, POLYBENCH_ARRAY(out));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_cp50(x, fc, fl, ll,
              POLYBENCH_ARRAY(out),
              POLYBENCH_ARRAY(RPLANE),
              POLYBENCH_ARRAY(GPLANE),
              POLYBENCH_ARRAY(BPLANE));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(xy,
              POLYBENCH_ARRAY(RPLANE),
              POLYBENCH_ARRAY(GPLANE),
              POLYBENCH_ARRAY(BPLANE)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(out);
  POLYBENCH_FREE_ARRAY(RPLANE);
  POLYBENCH_FREE_ARRAY(GPLANE);
  POLYBENCH_FREE_ARRAY(BPLANE);

  return 0;
}
