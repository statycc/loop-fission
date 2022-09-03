/**
 * This benchmark created on August 31, 2022.
 *
 * The remap procedure from NAS UA benchmark.
 * UA = Unstructured Adaptive mesh, dynamic and irregular memory access.
 *
 * Description of remap:
 * After a refinement, map the solution from the parent (x) to
 * the eight children. y is the solution on the first child
 * (front-bottom-left) and y1 is the solution on the next 7 children.
 *
 * Web address: https://www.nas.nasa.gov/software/npb.html
 */
/* remap.c */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is N=1024. */
#include <remap.h>

/* these dimensions are fixed for all problem sizes */
#define YONE_SZ 2
#define YTWO_SZ 4
#define Y1_SIZE 7

/* Array initialization. */
static
void init_array(int lx,
        DATA_TYPE POLYBENCH_4D(YONE,  YONE_SZ,LX,LX,LX,YONE_SZ,lx,lx,lx),
        DATA_TYPE POLYBENCH_4D(YTWO,  YTWO_SZ,LX,LX,LX,YTWO_SZ,lx,lx,lx),
        DATA_TYPE POLYBENCH_3D(X,     LX,LX,LX,lx,lx,lx),
        DATA_TYPE POLYBENCH_3D(Y,     LX,LX,LX,lx,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixmc1, LX,LX,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixmc2, LX,LX,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixtmc1,LX,LX,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixtmc2,LX,LX,lx,lx))
{
    int i, j, k, h;

    for (i = 0; i < lx; i++)
        ixtmc1[0][i] = ixtmc1[lx-1][i] = 0.0;
    ixtmc1[0][0] = 1.0;
    ixtmc1[lx-1][lx/2] = 1.0;

    for(i = 1; i < ((lx-2)/3); i += 3){
        for(j = 0; j < lx/5; j += 5){
            ixtmc1[1+i][(j+0) % lx] = j + i *  0.3385078435248143;
            ixtmc1[1+i][(j+1) % lx] = j + i *  0.7898516348912331;
            ixtmc1[1+i][(j+2) % lx] = j + i * -0.1884018684471238;
            ixtmc1[1+i][(j+3) % lx] = j + i *  9.202967302175333e-02;
            ixtmc1[1+i][(j+4) % lx] = j + i * -3.198728299067715e-02;
        }
        for(j = 0; j < lx/5; j += 5){
            ixtmc1[2+i][(j+0) % lx] = j + i * -0.1171875;
            ixtmc1[2+i][(j+1) % lx] = j + i *  0.8840317166357952;
            ixtmc1[2+i][(j+2) % lx] = j + i *  0.3125;
            ixtmc1[2+i][(j+3) % lx] = j + i * -0.118406716635795;
            ixtmc1[2+i][(j+4) % lx] = j + i *  0.0390625;
        }
        for(j = 0; j < lx/5; j += 5){
            ixtmc1[3+i][(j+0) % lx] = j + i * -7.065070066767144e-02;
            ixtmc1[3+i][(j+1) % lx] = j + i *  0.2829703269782467;
            ixtmc1[3+i][(j+2) % lx] = j + i *  0.902687582732838;
            ixtmc1[3+i][(j+3) % lx] = j + i * -0.1648516348912333;
            ixtmc1[3+i][(j+4) % lx] = j + i *  4.984442584781999e-02;
        }
    }
    for (j = 0; j < lx; j++)
        for (i = 0; i < lx; i++)
            ixtmc2[j][i] = ixtmc1[lx - 1 - j][lx - 1 - i];

    for (i = 0; i < lx; i++)
        for (j = 0; j < lx; j++)
            ixmc1[j][i] = ixtmc1[i][j];

    for (i = 0; i < lx; i++)
        for (j = 0; j < lx; j++)
            ixmc2[j][i] = ixtmc2[i][j];

    for (i = 0; i < lx; i++)
        for (j = 0; j < lx; j++)
            for (k = 0; k < lx; k++)
                X[i][j][k] = sin(i+j+k);

    for (i = 0; i < lx; i++)
        for (j = 0; j < lx; j++)
            for (k = 0; k < lx; k++)
                Y[i][j][k] = cos(i+j+k);

    for (i = 0; i < YONE_SZ; i++)
        for (j = 0; j < lx; j++)
            for (k = 0; k < lx; k++)
                for (h = 0; h < lx; h++)
                    YONE[i][j][k][h] = 0;

    for (i = 0; i < YTWO_SZ; i++)
        for (j = 0; j < lx; j++)
            for (k = 0; k < lx; k++)
                for (h = 0; h < lx; h++)
                    YTWO[i][j][k][h] = 0;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int lx, DATA_TYPE POLYBENCH_4D(Y1,Y1_SIZE,LX,LX,LX,Y1_SIZE,lx,lx,lx))
{
  int i, j, k, l;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("Y1");
  for (i = 0; i < Y1_SIZE; i++)
    for (j = 0; j < lx; j++)
      for (k = 0; k < lx; k++)
        for (l = 0; l < lx; l++) {
          fprintf (stderr, DATA_PRINTF_MODIFIER, Y1[i][j][k][l]);
          if (i % 20 == 0) fprintf (stderr, "\n");
        }
  POLYBENCH_DUMP_END("Y1");
  POLYBENCH_DUMP_FINISH;
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_remap(int lx,
        DATA_TYPE POLYBENCH_4D(YONE,  YONE_SZ,LX,LX,LX,YONE_SZ,lx,lx,lx),
        DATA_TYPE POLYBENCH_4D(YTWO,  YTWO_SZ,LX,LX,LX,YTWO_SZ,lx,lx,lx),
        DATA_TYPE POLYBENCH_4D(Y1,    Y1_SIZE,LX,LX,LX,Y1_SIZE,lx,lx,lx),
        DATA_TYPE POLYBENCH_3D(X,     LX,LX,LX,lx,lx,lx),
        DATA_TYPE POLYBENCH_3D(Y,     LX,LX,LX,lx,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixmc1, LX,LX,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixmc2, LX,LX,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixtmc1,LX,LX,lx,lx),
        DATA_TYPE POLYBENCH_2D(ixtmc2,LX,LX,lx,lx))
{
   int i, iz, ii, jj, kk;;

#pragma scop

    for (i = 0; i < _PB_LX; i++)
    {
        for (kk = 0; kk < _PB_LX; kk++)
        {
            for (jj = 0; jj < _PB_LX; jj++)
            {
                for (ii = 0; ii < _PB_LX; ii++)
                {
                    YONE[0][i][jj][ii] = YONE[0][i][jj][ii] + ixmc1[kk][ii] * X[i][jj][kk];
                    YONE[1][i][jj][ii] = YONE[1][i][jj][ii] + ixmc2[kk][ii] * X[i][jj][kk];
                }
            }
        }
        for (kk = 0; kk < _PB_LX; kk++)
        {
            for (jj = 0; jj < _PB_LX; jj++)
            {
                for (ii = 0; ii < _PB_LX; ii++)
                {
                    YTWO[0][jj][i][ii] = YTWO[0][jj][i][ii] +
                                         YONE[0][i][kk][ii] * ixtmc1[jj][kk];
                    YTWO[1][jj][i][ii] = YTWO[1][jj][i][ii] +
                                         YONE[0][i][kk][ii] * ixtmc2[jj][kk];
                    YTWO[2][jj][i][ii] = YTWO[2][jj][i][ii] +
                                         YONE[1][i][kk][ii] * ixtmc1[jj][kk];
                    YTWO[3][jj][i][ii] = YTWO[3][jj][i][ii] +
                                         YONE[1][i][kk][ii] * ixtmc2[jj][kk];
                }
            }
        }
    }
    for (iz = 0; iz < _PB_LX; iz++)
    {
        for (kk = 0; kk < _PB_LX; kk++)
        {
            for (jj = 0; jj < _PB_LX; jj++)
            {
                for (ii = 0; ii < _PB_LX; ii++)
                {
                    Y[jj][iz][ii] = Y[jj][iz][ii] +
                                    YTWO[0][iz][kk][ii] * ixtmc1[jj][kk];
                    Y1[0][jj][iz][ii] = Y1[0][jj][iz][ii] +
                                        YTWO[2][iz][kk][ii] * ixtmc1[jj][kk];
                    Y1[1][jj][iz][ii] = Y1[1][jj][iz][ii] +
                                        YTWO[1][iz][kk][ii] * ixtmc1[jj][kk];
                    Y1[2][jj][iz][ii] = Y1[2][jj][iz][ii] +
                                        YTWO[3][iz][kk][ii] * ixtmc1[jj][kk];
                    Y1[3][jj][iz][ii] = Y1[3][jj][iz][ii] +
                                        YTWO[0][iz][kk][ii] * ixtmc2[jj][kk];
                    Y1[4][jj][iz][ii] = Y1[4][jj][iz][ii] +
                                        YTWO[2][iz][kk][ii] * ixtmc2[jj][kk];
                    Y1[5][jj][iz][ii] = Y1[5][jj][iz][ii] +
                                        YTWO[1][iz][kk][ii] * ixtmc2[jj][kk];
                    Y1[6][jj][iz][ii] = Y1[6][jj][iz][ii] +
                                        YTWO[3][iz][kk][ii] * ixtmc2[jj][kk];
                }
            }
        }
    }

#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int lx = LX;

  /* Variable declaration/allocation. */
  POLYBENCH_4D_ARRAY_DECL(YONE,DATA_TYPE,YONE_SZ,LX,LX,LX,YONE_SZ,lx,lx,lx);
  POLYBENCH_4D_ARRAY_DECL(YTWO,DATA_TYPE,YTWO_SZ,LX,LX,LX,YTWO_SZ,lx,lx,lx);
  POLYBENCH_4D_ARRAY_DECL(Y1, DATA_TYPE,Y1_SIZE,LX,LX,LX,Y1_SIZE,lx,lx,lx);
  POLYBENCH_3D_ARRAY_DECL(Y,DATA_TYPE,LX,LX,LX,lx,lx,lx);
  POLYBENCH_3D_ARRAY_DECL(X,DATA_TYPE,LX,LX,LX,lx,lx,lx);
  POLYBENCH_2D_ARRAY_DECL(ixmc1,DATA_TYPE,LX,LX,lx,lx);
  POLYBENCH_2D_ARRAY_DECL(ixmc2,DATA_TYPE,LX,LX,lx,lx);
  POLYBENCH_2D_ARRAY_DECL(ixtmc1,DATA_TYPE,LX,LX,lx,lx);
  POLYBENCH_2D_ARRAY_DECL(ixtmc2,DATA_TYPE,LX,LX,lx,lx);

  /* Initialize array(s). */
  init_array (lx,
              POLYBENCH_ARRAY(YONE),
              POLYBENCH_ARRAY(YTWO),
              POLYBENCH_ARRAY(X),
              POLYBENCH_ARRAY(Y),
              POLYBENCH_ARRAY(ixmc1),
              POLYBENCH_ARRAY(ixmc2),
              POLYBENCH_ARRAY(ixtmc1),
              POLYBENCH_ARRAY(ixtmc2));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_remap (lx,
                  POLYBENCH_ARRAY(YONE),
                  POLYBENCH_ARRAY(YTWO),
                  POLYBENCH_ARRAY(Y1),
                  POLYBENCH_ARRAY(X),
                  POLYBENCH_ARRAY(Y),
                  POLYBENCH_ARRAY(ixmc1),
                  POLYBENCH_ARRAY(ixmc2),
                  POLYBENCH_ARRAY(ixtmc1),
                  POLYBENCH_ARRAY(ixtmc2));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(lx,  POLYBENCH_ARRAY(Y1)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(YONE);
  POLYBENCH_FREE_ARRAY(YTWO);
  POLYBENCH_FREE_ARRAY(Y1);
  POLYBENCH_FREE_ARRAY(Y);
  POLYBENCH_FREE_ARRAY(X);
  POLYBENCH_FREE_ARRAY(ixmc1);
  POLYBENCH_FREE_ARRAY(ixmc2);
  POLYBENCH_FREE_ARRAY(ixtmc1);
  POLYBENCH_FREE_ARRAY(ixtmc2);
  return 0;
}
