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
#include <cp50.h>
/* Array initialization. */

static void init_array(int ls,int ol,double out[2226][996])
{
  int i;
  int j;
  for (i = 0; i <= -1 + ls; i += 1) {
    for (j = 0; j <= -1 + ol; j += 1) {
      out[i][j] = ((double )(i * j + 1));
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int xy,double RPLANE[592000],double GPLANE[592000],double BPLANE[592000])
{
  int i;
  int j;
  fprintf(stderr,"==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr,"begin dump: %s","RPLANE");
  for (i = 0; i <= -1 + xy; i += 1) {
    fprintf(stderr,"%0.2lf ",RPLANE[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","RPLANE");
  fprintf(stderr,"begin dump: %s","GPLANE");
  for (i = 0; i <= -1 + xy; i += 1) {
    fprintf(stderr,"%0.2lf ",GPLANE[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","GPLANE");
  fprintf(stderr,"begin dump: %s","BPLANE");
  for (i = 0; i <= -1 + xy; i += 1) {
    fprintf(stderr,"%0.2lf ",BPLANE[i]);
    if (i % 20 == 0) {
      fprintf(stderr,"\n");
    }
     else {
    }
  }
  fprintf(stderr,"\nend   dump: %s\n","BPLANE");
  fprintf(stderr,"==END   DUMP_ARRAYS==\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_cp50(int X_PIXEL,int FIRST_COLUMN,int FIRST_LINE,int LAST_LINE,double out[2226][996],double RPLANE[592000],double GPLANE[592000],double BPLANE[592000])
{
  int lnum = FIRST_LINE;
  int last = LAST_LINE;
  int i;
  int col;
  
#pragma scop
/* Print lines of graphics */
  while(lnum <= last){
    for (i = 0; i <= -1 + X_PIXEL; i += 1) {
      col = (lnum - FIRST_LINE) * X_PIXEL + i;
      RPLANE[col] = out[lnum][i * 3 + FIRST_COLUMN];
      GPLANE[col] = out[lnum][i * 3 + 1 + FIRST_COLUMN];
      BPLANE[col] = out[lnum][i * 3 + 2 + FIRST_COLUMN];
    }
    lnum++;
  }
  
#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int x = 592;
  int y = 1000;
  int xy = 592 * 1000;
  int fc = 225;
  int ls = 592 * 3 + 225 * 2;
  int fl = 179;
  int ll = 1175;
  int ol = 1175 - 179;
/* Variable declaration/allocation. */
  double (*out)[2226][996];
  out = ((double (*)[2226][996])(polybench_alloc_data(((592 * 3 + 225 * 2 + 0) * (1175 - 179 + 0)),(sizeof(double )))));
  ;
  double (*RPLANE)[592000];
  RPLANE = ((double (*)[592000])(polybench_alloc_data((592 * 1000 + 0),(sizeof(double )))));
  ;
  double (*GPLANE)[592000];
  GPLANE = ((double (*)[592000])(polybench_alloc_data((592 * 1000 + 0),(sizeof(double )))));
  ;
  double (*BPLANE)[592000];
  BPLANE = ((double (*)[592000])(polybench_alloc_data((592 * 1000 + 0),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(ls,ol, *out);
/* Start timer. */
  ;
/* Run kernel. */
  kernel_cp50(x,fc,fl,ll, *out, *RPLANE, *GPLANE, *BPLANE);
/* Stop and print timer. */
  ;
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(xy, *RPLANE, *GPLANE, *BPLANE);
/* Be clean. */
  free((void *)out);
  ;
  free((void *)RPLANE);
  ;
  free((void *)GPLANE);
  ;
  free((void *)BPLANE);
  ;
  return 0;
}
