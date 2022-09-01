#ifndef _CONJGRAD_H
# define _CONJGRAD_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
#  define LARGE_DATASET
# endif

# if !defined(NA) && !defined(NITER)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define NA 1400
#   define NITER 15
#  endif

#  ifdef SMALL_DATASET
#   define NA 7000
#   define NITER 15
#  endif

#  ifdef MEDIUM_DATASET
#   define NA 14000
#   define NITER 15
#  endif

#  ifdef LARGE_DATASET
#   define NA 75000
#   define NITER 75
#  endif

#  ifdef EXTRALARGE_DATASET
#   define NA 150000
#   define NITER 75
#  endif

#endif /* !(NA NITER */

# define _PB_NITER POLYBENCH_LOOP_BOUND(NITER,niter)
# define _PB_NA POLYBENCH_LOOP_BOUND(NA,na)

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

#endif /* !_CONJGRAD_H */
