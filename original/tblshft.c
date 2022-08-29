/**
 * This benchmark created on August 29, 2022.
 *
 * The table shift kernel appeared in MiBench Version 1.0,
 * consumer/tiff-v3.5.4/libtiff/tif_pixarlog.c, LOC 565-576.
 * The code block includes the following explanation:
 *
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
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include <tblshft.h>


/* Array initialization. */
static
void init_array(int ni, int nj, int nk,
    DATA_TYPE POLYBENCH_1D(TLF,NK,nk))
{

  int i, j=0;
  int c = log(1.004);
  int nlin = 1./c;
  c = 1./nlin;
  int  b = exp(-c * 1250);
  int linstep = b * c * exp(1.);

  for (i = 0; i < nlin; i++)
	TLF[j++] = i * linstep;

  for (i = nlin; i < nk; i++)
	TLF[j++] = b * exp(c * i);

  TLF[nk-1] = TLF[nk-2];
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nj,
    DATA_TYPE POLYBENCH_1D(F8,NI,ni),
    DATA_TYPE POLYBENCH_1D(F14,NJ,nj))
{
  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("F8");
  for (i = 0; i < ni; i++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, F8[i]);
	if (i % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("F8");

  POLYBENCH_DUMP_BEGIN("F14");
  for (j = 0; j < nj; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, F14[j]);
	if (j % 20 == 0) fprintf (stderr, "\n");
  }
  POLYBENCH_DUMP_END("F14");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_tblshft(int ni, int nj, int nk,
    DATA_TYPE POLYBENCH_1D(F8,NI,ni),
    DATA_TYPE POLYBENCH_1D(F14,NJ,nj),
    DATA_TYPE POLYBENCH_1D(TLF,NK,nk))
{
  int i, j;

#pragma scop

    j = 0;
    for (i = 0; i < _PB_NJ; i++)  {
	while ((i/(nj-1.))*(i/(nj-1.)) > TLF[j]*TLF[j+1])
	    j++;
 	F14[i] = j;
    }

    j = 0;
    for (i = 0; i < _PB_NI; i++)  {
	while ((i/(ni-1.))*(i/(ni-1.)) > TLF[j]*TLF[j+1])
	    j++;
	F8[i] = j;
    }

#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;
  int nk = NK;

  /* Variable declaration/allocation. */
  POLYBENCH_1D_ARRAY_DECL(F8,DATA_TYPE,NI,ni);
  POLYBENCH_1D_ARRAY_DECL(F14,DATA_TYPE,NJ,nj);
  POLYBENCH_1D_ARRAY_DECL(TLF,DATA_TYPE,NK,nk);

  /* Initialize array(s). */
  init_array (ni, nj, nk, POLYBENCH_ARRAY(TLF));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  //kernel_tblshft (ni, nj, nk, POLYBENCH_ARRAY(F8), POLYBENCH_ARRAY(F14), POLYBENCH_ARRAY(TLF));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(ni, nj, POLYBENCH_ARRAY(F8), POLYBENCH_ARRAY(F14)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(TLF);
  POLYBENCH_FREE_ARRAY(F14);
  POLYBENCH_FREE_ARRAY(F8);

  return 0;
}
