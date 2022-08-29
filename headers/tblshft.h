#ifndef _TBLSHFT_H
# define _TBLSHFT_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
#  define LARGE_DATASET
# endif

# if !defined(NI) && !defined(NJ) && !defined(NK)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define NI 256
#   define NJ 16384
#   define NK 2049
#  endif

#  ifdef SMALL_DATASET
#   define NI 256
#   define NJ 16384
#   define NK 4098
#  endif

#  ifdef MEDIUM_DATASET
#   define NI 4096
#   define NJ 65535
#   define NK 2049
#  endif

#  ifdef LARGE_DATASET
#   define NI 8192
#   define NJ 262144
#   define NK 2049
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NI 16384
#   define NJ 1048576
#   define NK 2049
#  endif


#endif /* !(NI NJ NK) */

# define _PB_NI POLYBENCH_LOOP_BOUND(NI,ni)
# define _PB_NJ POLYBENCH_LOOP_BOUND(NJ,nj)
# define _PB_NK POLYBENCH_LOOP_BOUND(NK,nk)

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

#endif /* !_TBLSHFT_H */
