#ifndef _DGEMVT_H
# define _DGEMVT_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
#  define LARGE_DATASET
# endif

# if !defined(M) && !defined(N) && !defined(L) && !defined(P)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define M 256
#   define N 256
#   define L 128
#   define P (M*L+N)
#  endif

#  ifdef SMALL_DATASET
#   define M 512
#   define N 256
#   define L 128
#   define P (M*L+N)
#  endif

#  ifdef MEDIUM_DATASET
#   define M 1024
#   define N 512
#   define L 256
#   define P (M*L+N)
#  endif

#  ifdef LARGE_DATASET
#   define M 4096
#   define N 4096
#   define L 512
#   define P (M*L+N)
#  endif

#  ifdef EXTRALARGE_DATASET
#   define M 16384
#   define N 16384
#   define L 1024
#   define P (M*L+N)
#  endif


#endif /* !(M N L P) */

# define _PB_M POLYBENCH_LOOP_BOUND(M,m)
# define _PB_N POLYBENCH_LOOP_BOUND(N,n)


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

#endif /* !_DGEMVT_H */
