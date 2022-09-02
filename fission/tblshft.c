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
static
void init_array(int tsz,
    DATA_TYPE POLYBENCH_1D(TLF,TSZ,tsz))
{

    int i, j, nlin;
    double b, c, linstep, v;
    int TSIZE = tsz - 1;

    j = 0;
    c = log(RATIO);
    nlin = 1. / c;
    c = 1. / nlin;
    b = exp(-c * ONE);
    linstep = b * c * exp(1.);

    for (i = 0; i < nlin; i++) {
        v = i * linstep;
        TLF[j++] = (DATA_TYPE) v;
    }

    for (i = nlin; i < TSIZE; i++)
        TLF[j++] = b * exp(c * i);

    TLF[TSIZE] = TLF[TSIZE - 1];
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int f8sz, int f14sz,
    DATA_TYPE POLYBENCH_1D(F8,F8SZ,f8sz),
    DATA_TYPE POLYBENCH_1D(F14,F14SZ,f14sz))
{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("F8");
  for (i = 0; i < f8sz; i++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, F8[i]);
	if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("F8");
  POLYBENCH_DUMP_BEGIN("F14");
  for (j = 0; j < f14sz; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, F14[j]);
	if (j % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("F14");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_tblshft(int f8sz, int f14sz, int tsz,
        DATA_TYPE POLYBENCH_1D(F8,F8SZ,f8sz),
        DATA_TYPE POLYBENCH_1D(F14,F14SZ,f14sz),
        DATA_TYPE POLYBENCH_1D(TLF,TSZ,tsz))
{
  int i, j;
  DATA_TYPE F14SZM1 = (DATA_TYPE)(f14sz - 1);
  DATA_TYPE F8SZM1 = (DATA_TYPE)(f8sz-1);

#pragma scop

#pragma omp parallel private(i,j)
{
    #pragma omp single nowait
    {
        j = 0;
        for (i = 0; i < f14sz; i++) {
            while (((DATA_TYPE) i / F14SZM1) * ((DATA_TYPE) i / F14SZM1) > TLF[j] * TLF[j + 1])
                j++;
            F14[i] = (DATA_TYPE) j;
        }
    }
    #pragma omp single nowait
    {
        j = 0;
        for (i = 0; i < f8sz; i++) {
            while (((DATA_TYPE) i / F8SZM1) * ((DATA_TYPE) i / F8SZM1) > TLF[j] * TLF[j + 1])
                j++;
            F8[i] = (DATA_TYPE) j;
        }
    }
}

#pragma endscop

}


int main(int argc, char** argv)
{

  /* Retrieve problem size. */
  int f8sz = F8SZ;
  int f14sz = F14SZ;
  int tsz = TSZ;

  /* Variable declaration/allocation. */
  POLYBENCH_1D_ARRAY_DECL(F8,DATA_TYPE,F8SZ,f8sz);
  POLYBENCH_1D_ARRAY_DECL(F14,DATA_TYPE,F14SZ,f14sz);
  POLYBENCH_1D_ARRAY_DECL(TLF,DATA_TYPE,TSZ,tsz);

  /* Initialize array(s). */
  init_array (tsz, POLYBENCH_ARRAY(TLF));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_tblshft (f8sz, f14sz, tsz, POLYBENCH_ARRAY(F8), POLYBENCH_ARRAY(F14), POLYBENCH_ARRAY(TLF));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(f8sz, f14sz, POLYBENCH_ARRAY(F8), POLYBENCH_ARRAY(F14)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(TLF);
  POLYBENCH_FREE_ARRAY(F14);
  POLYBENCH_FREE_ARRAY(F8);

  return 0;
}
