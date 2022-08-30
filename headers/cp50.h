#ifndef _CP50_H
# define _CP50_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
#  define LARGE_DATASET
# endif

# if !defined(X) && !defined(Y) && !defined(FL) && !defined(LL) && !defined(XY) && !defined(FC) && !defined(LS) && !defined(OL)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define X 237           /* paper X pixels */
#   define Y 400           /* paper Y pixels */
#   define FC 90           /* first column */
#   define FL 70           /* first line */
#   define LL 472          /* last line; LL-FL+1 should be close to Y */
#   define LS (X*3+FC*2)   /* line size */
#   define XY (X*Y)
#   define OL (LL-FL)      /* printer data Y */
#  endif

#  ifdef SMALL_DATASET
#   define X 330
#   define Y 560
#   define FC 126
#   define FL 98
#   define LL 661
#   define LS (X*3+FC*2)
#   define XY (X*Y)
#   define OL (LL-FL)
#  endif

#  ifdef MEDIUM_DATASET
#   define X 474
#   define Y 800
#   define FC 180
#   define FL 140
#   define LL 933
#   define LS (X*3+FC*2)
#   define XY (X*Y)
#   define OL (LL-FL)
#  endif

#  ifdef LARGE_DATASET
#   define X 592
#   define Y 1000
#   define FC 225
#   define FL 175
#   define LL 1175
#   define LS (X*3+FC*2)
#   define XY (X*Y)
#   define OL (LL-FL)
#  endif

#  ifdef EXTRALARGE_DATASET
#   define X 1184
#   define Y 2000
#   define FC 450
#   define FL 350
#   define LL 2350
#   define LS (X*3+FC*2)
#   define XY (X*Y)
#   define OL (LL-FL)
#  endif

#endif /* !(X Y XY FC LS FL LL OL) */

/* Default data type */
# if !defined(DATA_TYPE_IS_INT) && !defined(DATA_TYPE_IS_FLOAT) && !defined(DATA_TYPE_IS_DOUBLE)
#  define DATA_TYPE_IS_DOUBLE
# endif

#ifdef DATA_TYPE_IS_INT
#  define DATA_TYPE int
#  define DATA_PRINTF_MODIFIER "%d "
#endif

#ifdef DATA_TYPE_IS_FLOAT
#  define DATA_TYPE float
#  define DATA_PRINTF_MODIFIER "%0.2f "
#  define SCALAR_VAL(x) x##f
#  define SQRT_FUN(x) sqrtf(x)
#  define EXP_FUN(x) expf(x)
#  define POW_FUN(x,y) powf(x,y)
# endif

#ifdef DATA_TYPE_IS_DOUBLE
#  define DATA_TYPE double
#  define DATA_PRINTF_MODIFIER "%0.2lf "
#  define SCALAR_VAL(x) x
#  define SQRT_FUN(x) sqrt(x)
#  define EXP_FUN(x) exp(x)
#  define POW_FUN(x,y) pow(x,y)
# endif

#endif /* !_CP50_H */
