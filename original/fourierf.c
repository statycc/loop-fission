/**
 * This benchmark created on August 29, 2022.
 *
 * Inverse Fourier transforms (fft_float) method from MiBench Version 1.0.
 * telecomm/FFT/fourierf.c, LOC 39-146.
 *
 * Web address: https://vhosts.eecs.umich.edu/mibench
 */
/* fourierf.c */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include <fourierf.h>


/* Array initialization. */
static
void init_array(int m, int w,
        DATA_TYPE POLYBENCH_1D(RealIn,M,m),
        DATA_TYPE POLYBENCH_1D(RealOut,M,m),
        DATA_TYPE POLYBENCH_1D(ImagIn,M,m),
        DATA_TYPE POLYBENCH_1D(ImagOut,M,m),
        DATA_TYPE POLYBENCH_1D(coeff,W,w),
        DATA_TYPE POLYBENCH_1D(amp,W,w))
{
    int i, j, nb;
    srand(1);

    for ( i=0; ; i++ ){
        if ( m & (1 << i) ) { nb = i; break; }
    }
    for (i = 0; i < w; i++) {
        coeff[i] = rand() % 1000;
        amp[i] = rand() % 1000;
    }
    for (i = 0; i < m; i++) {
        RealIn[i] = 0;
        for (j = 0; j < w; j++) {
            /* randomly select sin or cos */
            if (rand() % 2) {
                RealIn[i] += coeff[j] * cos(amp[j] * i);
            } else {
                RealIn[i] += coeff[j] * sin(amp[j] * i);
            }
            ImagIn[i] = 0;
        }
    }
    for ( i=0; i < m; i++ )
    {
        unsigned index=i; // reverse bits
        for ( unsigned ii=j=0; ii < nb; ii++ )
        {
            j = (j << 1) | (index & 1);
            index >>= 1;
        }
        RealOut[j] = RealIn[i];
        ImagOut[j] = (ImagIn == NULL) ? 0.0f : ImagIn[i];
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int m,
       DATA_TYPE POLYBENCH_1D(RealOut,M,m),
       DATA_TYPE POLYBENCH_1D(ImagOut,M,m))
{
    int i;

    POLYBENCH_DUMP_START;
    POLYBENCH_DUMP_BEGIN("RealOut");
    for (i = 0; i < m; i++) {
        fprintf (stderr, DATA_PRINTF_MODIFIER, RealOut[i]);
        if (i % 20 == 0) fprintf (stderr, "\n");
    }
    POLYBENCH_DUMP_END("RealOut");
    POLYBENCH_DUMP_BEGIN("ImagOut");
    for (i = 0; i < m; i++) {
        fprintf (stderr, DATA_PRINTF_MODIFIER, ImagOut[i]);
        if (i % 20 == 0) fprintf (stderr, "\n");
    }
    POLYBENCH_DUMP_END("ImagOut");
    POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_fourierf(int m,
        DATA_TYPE POLYBENCH_1D(RealIn,M,m),
        DATA_TYPE POLYBENCH_1D(RealOut,M,m),
        DATA_TYPE POLYBENCH_1D(ImagIn,M,m),
        DATA_TYPE POLYBENCH_1D(ImagOut,M,m))
{
    DATA_TYPE denom = (DATA_TYPE)m;
    double angle_numerator = -2.0 * 3.14159265358979323846; // inverse transform
    unsigned i, j, k, n, BlockSize, BlockEnd;
    double tr, ti; // temp real, temp imaginary

    BlockEnd = 1;

#pragma scop

    for ( BlockSize = 2; BlockSize <= m; BlockSize <<= 1 )
    {
        double delta_angle = angle_numerator / (double)BlockSize;
        double sm2 = sin ( -2 * delta_angle );
        double sm1 = sin ( -delta_angle );
        double cm2 = cos ( -2 * delta_angle );
        double cm1 = cos ( -delta_angle );
        double w = 2 * cm1;
        double ar[3], ai[3];

        for ( i=0; i < m; i += BlockSize )
        {
            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;

            for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {
                ar[0] = w*ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w*ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];

                k = j + BlockEnd;
                tr = ar[0]*RealOut[k] - ai[0]*ImagOut[k];
                ti = ar[0]*ImagOut[k] + ai[0]*RealOut[k];

                RealOut[k] = (DATA_TYPE)(RealOut[j] - tr);
                ImagOut[k] = (DATA_TYPE)(ImagOut[j] - ti);

                RealOut[j] += (DATA_TYPE)(tr);
                ImagOut[j] += (DATA_TYPE)(ti);
            }
        }

        BlockEnd = BlockSize;
    }

    // normalize if inverse transform
    for ( i=0; i < m; i++ )
    {
        RealOut[i] /= (DATA_TYPE)(denom);
        ImagOut[i] /= (DATA_TYPE)(denom);
    }

#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int m = M;
  int w = W;

  /* Variable declaration/allocation. */
  POLYBENCH_1D_ARRAY_DECL(RealIn,DATA_TYPE,M,m);
  POLYBENCH_1D_ARRAY_DECL(RealOut,DATA_TYPE,M,m);
  POLYBENCH_1D_ARRAY_DECL(ImagIn,DATA_TYPE,M,m);
  POLYBENCH_1D_ARRAY_DECL(ImagOut,DATA_TYPE,M,m);
  POLYBENCH_1D_ARRAY_DECL(coeff,DATA_TYPE,W,w);
  POLYBENCH_1D_ARRAY_DECL(amp,DATA_TYPE,W,w);

  /* Initialize array(s). */
  init_array (m, w,
              POLYBENCH_ARRAY(RealIn),
              POLYBENCH_ARRAY(RealOut),
              POLYBENCH_ARRAY(ImagIn),
              POLYBENCH_ARRAY(ImagOut),
              POLYBENCH_ARRAY(coeff),
              POLYBENCH_ARRAY(amp));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_fourierf (m,
                   POLYBENCH_ARRAY(RealIn),
                   POLYBENCH_ARRAY(RealOut),
                   POLYBENCH_ARRAY(ImagIn),
                   POLYBENCH_ARRAY(ImagOut));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
   polybench_prevent_dce(print_array(m, POLYBENCH_ARRAY(RealOut), POLYBENCH_ARRAY(ImagOut)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(RealIn);
  POLYBENCH_FREE_ARRAY(RealOut);
  POLYBENCH_FREE_ARRAY(ImagIn);
  POLYBENCH_FREE_ARRAY(ImagOut);
  POLYBENCH_FREE_ARRAY(coeff);
  POLYBENCH_FREE_ARRAY(amp);

  return 0;
}
