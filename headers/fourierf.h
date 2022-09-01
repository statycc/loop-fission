#ifndef _FOURIERF_H
# define _FOURIERF_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
#  define LARGE_DATASET
# endif

# if !defined(M) && !defined(W)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define M 8
#   define W 32
#  endif

#  ifdef SMALL_DATASET
#   define M 64
#   define W 128
#  endif

#  ifdef MEDIUM_DATASET
#   define M 512
#   define W 256
#  endif

#  ifdef LARGE_DATASET
#   define M 2048
#   define W 512
#  endif

#  ifdef EXTRALARGE_DATASET
#   define M 16384
#   define W 1024
#  endif


#endif /* !(M W) */

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

#endif /* !_FOURIERF_H */
