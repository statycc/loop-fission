#ifndef _TBLSHFT_H
# define _TBLSHFT_H

/* Default to LARGE_DATASET. */
# if !defined(MINI_DATASET) && !defined(SMALL_DATASET) && !defined(MEDIUM_DATASET) && !defined(LARGE_DATASET) && !defined(EXTRALARGE_DATASET)
#  define LARGE_DATASET
#  define LARGE_DATASET
# endif

# if !defined(F8SZ) && !defined(F14SZ) && !defined(TSZ)
/* Define sample dataset sizes. */
#  ifdef MINI_DATASET
#   define F8SZ 256      /* 2^8 */
#   define F14SZ 16384   /* 2^14 */
#   define TSZ 2049
#  endif

#  ifdef SMALL_DATASET
#   define F8SZ 1024     /* 2^10 */
#   define F14SZ 65536   /* 2^16 */
#   define TSZ 2049
#  endif

#  ifdef MEDIUM_DATASET
#   define F8SZ 4096     /* 2^12 */
#   define F14SZ 262144  /* 2^18 */
#   define TSZ 2049
#  endif

#  ifdef LARGE_DATASET
#   define F8SZ 65536     /* 2^16 */
#   define F14SZ 4194304  /* 2^22 */
#   define TSZ 2049
#  endif

#  ifdef EXTRALARGE_DATASET
#   define F8SZ 4194304     /* 2^22 */
#   define F14SZ 268435456  /* 2^28 */
#   define TSZ 2049
#  endif


#endif /* !(F8SZ F14SZ TSZ) */

# define _PB_F8 POLYBENCH_LOOP_BOUND(F8SZ,f8sz)
# define _PB_F14 POLYBENCH_LOOP_BOUND(F14SZ,f14sz)

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
