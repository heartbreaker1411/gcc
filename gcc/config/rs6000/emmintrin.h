/* Copyright (C) 2003-2018 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* Implemented from the specification included in the Intel C++ Compiler
   User Guide and Reference, version 9.0.  */

#ifndef NO_WARN_X86_INTRINSICS
/* This header is distributed to simplify porting x86_64 code that
   makes explicit use of Intel intrinsics to powerpc64le.
   It is the user's responsibility to determine if the results are
   acceptable and make additional changes as necessary.
   Note that much code that uses Intel intrinsics can be rewritten in
   standard C or GNU C extensions, which are more portable and better
   optimized across multiple targets.

   In the specific case of X86 SSE2 (__m128i, __m128d) intrinsics,
   the PowerPC VMX/VSX ISA is a good match for vector double SIMD
   operations.  However scalar double operations in vector (XMM)
   registers require the POWER8 VSX ISA (2.07) level. Also there are
   important differences for data format and placement of double
   scalars in the vector register.

   For PowerISA Scalar double is in FPRs (left most 64-bits of the
   low 32 VSRs), while X86_64 SSE2 uses the right most 64-bits of
   the XMM. These differences require extra steps on POWER to match
   the SSE2 scalar double semantics.

   Most SSE2 scalar double intrinsic operations can be performed more
   efficiently as C language double scalar operations or optimized to
   use vector SIMD operations.  We recommend this for new applications.

   Another difference is the format and details of the X86_64 MXSCR vs
   the PowerISA FPSCR / VSCR registers. We recommend applications
   replace direct access to the MXSCR with the more portable <fenv.h>
   Posix APIs. */
#error "Please read comment above.  Use -DNO_WARN_X86_INTRINSICS to disable this error."
#endif

#ifndef EMMINTRIN_H_
#define EMMINTRIN_H_

#include <altivec.h>
#include <assert.h>

/* We need definitions from the SSE header files.  */
#include <xmmintrin.h>

/* SSE2 */
typedef __vector double __v2df;
typedef __vector long long __v2di;
typedef __vector unsigned long long __v2du;
typedef __vector int __v4si;
typedef __vector unsigned int __v4su;
typedef __vector short __v8hi;
typedef __vector unsigned short __v8hu;
typedef __vector signed char __v16qi;
typedef __vector unsigned char __v16qu;

/* The Intel API is flexible enough that we must allow aliasing with other
   vector types, and their scalar components.  */
typedef long long __m128i __attribute__ ((__vector_size__ (16), __may_alias__));
typedef double __m128d __attribute__ ((__vector_size__ (16), __may_alias__));

/* Unaligned version of the same types.  */
typedef long long __m128i_u __attribute__ ((__vector_size__ (16), __may_alias__, __aligned__ (1)));
typedef double __m128d_u __attribute__ ((__vector_size__ (16), __may_alias__, __aligned__ (1)));

/* Create a vector with element 0 as F and the rest zero.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_sd (double __F)
{
  return __extension__ (__m128d){ __F, 0.0 };
}

/* Create a vector with both elements equal to F.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_pd (double __F)
{
  return __extension__ (__m128d){ __F, __F };
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_pd1 (double __F)
{
  return _mm_set1_pd (__F);
}

/* Create a vector with the lower value X and upper value W.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_pd (double __W, double __X)
{
  return __extension__ (__m128d){ __X, __W };
}

/* Create a vector with the lower value W and upper value X.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_pd (double __W, double __X)
{
  return __extension__ (__m128d){ __W, __X };
}

/* Create an undefined vector.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_undefined_pd (void)
{
  __m128d __Y = __Y;
  return __Y;
}

/* Create a vector of zeros.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setzero_pd (void)
{
  return (__m128d) vec_splats (0);
}

/* Sets the low DPFP value of A from the low value of B.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_move_sd (__m128d __A, __m128d __B)
{
  __v2df result = (__v2df) __A;
  result [0] = ((__v2df) __B)[0];
  return (__m128d) result;
}

/* Load two DPFP values from P.  The address must be 16-byte aligned.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_pd (double const *__P)
{
  assert(((unsigned long)__P & 0xfUL) == 0UL);
  return ((__m128d)vec_ld(0, (__v16qu*)__P));
}

/* Load two DPFP values from P.  The address need not be 16-byte aligned.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_pd (double const *__P)
{
  return (vec_vsx_ld(0, __P));
}

/* Create a vector with all two elements equal to *P.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load1_pd (double const *__P)
{
  return (vec_splats (*__P));
}

/* Create a vector with element 0 as *P and the rest zero.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_sd (double const *__P)
{
  return _mm_set_sd (*__P);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_pd1 (double const *__P)
{
  return _mm_load1_pd (__P);
}

/* Load two DPFP values in reverse order.  The address must be aligned.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadr_pd (double const *__P)
{
  __v2df __tmp = _mm_load_pd (__P);
  return (__m128d)vec_xxpermdi (__tmp, __tmp, 2);
}

/* Store two DPFP values.  The address must be 16-byte aligned.  */
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_pd (double *__P, __m128d __A)
{
  assert(((unsigned long)__P & 0xfUL) == 0UL);
  vec_st((__v16qu)__A, 0, (__v16qu*)__P);
}

/* Store two DPFP values.  The address need not be 16-byte aligned.  */
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeu_pd (double *__P, __m128d __A)
{
  *(__m128d *)__P = __A;
}

/* Stores the lower DPFP value.  */
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_sd (double *__P, __m128d __A)
{
  *__P = ((__v2df)__A)[0];
}

extern __inline double __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsd_f64 (__m128d __A)
{
  return ((__v2df)__A)[0];
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storel_pd (double *__P, __m128d __A)
{
  _mm_store_sd (__P, __A);
}

/* Stores the upper DPFP value.  */
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeh_pd (double *__P, __m128d __A)
{
  *__P = ((__v2df)__A)[1];
}
/* Store the lower DPFP value across two words.
   The address must be 16-byte aligned.  */
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store1_pd (double *__P, __m128d __A)
{
  _mm_store_pd (__P, vec_splat (__A, 0));
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_pd1 (double *__P, __m128d __A)
{
  _mm_store1_pd (__P, __A);
}

/* Store two DPFP values in reverse order.  The address must be aligned.  */
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storer_pd (double *__P, __m128d __A)
{
  _mm_store_pd (__P, vec_xxpermdi (__A, __A, 2));
}

/* Intel intrinsic.  */
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi128_si64 (__m128i __A)
{
  return ((__v2di)__A)[0];
}

/* Microsoft intrinsic.  */
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi128_si64x (__m128i __A)
{
  return ((__v2di)__A)[0];
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_pd (__m128d __A, __m128d __B)
{
  return (__m128d) ((__v2df)__A + (__v2df)__B);
}

/* Add the lower double-precision (64-bit) floating-point element in
   a and b, store the result in the lower element of dst, and copy
   the upper element from a to the upper element of dst. */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_sd (__m128d __A, __m128d __B)
{
  __A[0] = __A[0] + __B[0];
  return (__A);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_pd (__m128d __A, __m128d __B)
{
  return (__m128d) ((__v2df)__A - (__v2df)__B);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_sd (__m128d __A, __m128d __B)
{
  __A[0] = __A[0] - __B[0];
  return (__A);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_pd (__m128d __A, __m128d __B)
{
  return (__m128d) ((__v2df)__A * (__v2df)__B);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_sd (__m128d __A, __m128d __B)
{
  __A[0] = __A[0] * __B[0];
  return (__A);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_div_pd (__m128d __A, __m128d __B)
{
  return (__m128d) ((__v2df)__A / (__v2df)__B);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_div_sd (__m128d __A, __m128d __B)
{
  __A[0] = __A[0] / __B[0];
  return (__A);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sqrt_pd (__m128d __A)
{
  return (vec_sqrt (__A));
}

/* Return pair {sqrt (B[0]), A[1]}.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sqrt_sd (__m128d __A, __m128d __B)
{
  __v2df c;
  c = vec_sqrt ((__v2df) _mm_set1_pd (__B[0]));
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_pd (__m128d __A, __m128d __B)
{
  return (vec_min (__A, __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = vec_min (a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_pd (__m128d __A, __m128d __B)
{
  return (vec_max (__A, __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = vec_max (a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmpeq ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmplt ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmple ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmpgt ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmpge ((__v2df) __A,(__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_pd (__m128d __A, __m128d __B)
{
  __v2df temp = (__v2df) vec_cmpeq ((__v2df) __A, (__v2df)__B);
  return ((__m128d)vec_nor (temp, temp));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnlt_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmpge ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnle_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmpgt ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpngt_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmple ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnge_pd (__m128d __A, __m128d __B)
{
  return ((__m128d)vec_cmplt ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpord_pd (__m128d __A, __m128d __B)
{
#if _ARCH_PWR8
  __v2du c, d;
  /* Compare against self will return false (0's) if NAN.  */
  c = (__v2du)vec_cmpeq (__A, __A);
  d = (__v2du)vec_cmpeq (__B, __B);
#else
  __v2du a, b;
  __v2du c, d;
  const __v2du double_exp_mask  = {0x7ff0000000000000, 0x7ff0000000000000};
  a = (__v2du)vec_abs ((__v2df)__A);
  b = (__v2du)vec_abs ((__v2df)__B);
  c = (__v2du)vec_cmpgt (double_exp_mask, a);
  d = (__v2du)vec_cmpgt (double_exp_mask, b);
#endif
  /* A != NAN and B != NAN.  */
  return ((__m128d)vec_and(c, d));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpunord_pd (__m128d __A, __m128d __B)
{
#if _ARCH_PWR8
  __v2du c, d;
  /* Compare against self will return false (0's) if NAN.  */
  c = (__v2du)vec_cmpeq ((__v2df)__A, (__v2df)__A);
  d = (__v2du)vec_cmpeq ((__v2df)__B, (__v2df)__B);
  /* A == NAN OR B == NAN converts too:
     NOT(A != NAN) OR NOT(B != NAN).  */
  c = vec_nor (c, c);
  return ((__m128d)vec_orc(c, d));
#else
  __v2du c, d;
  /* Compare against self will return false (0's) if NAN.  */
  c = (__v2du)vec_cmpeq ((__v2df)__A, (__v2df)__A);
  d = (__v2du)vec_cmpeq ((__v2df)__B, (__v2df)__B);
  /* Convert the true ('1's) is NAN.  */
  c = vec_nor (c, c);
  d = vec_nor (d, d);
  return ((__m128d)vec_or(c, d));
#endif
}

extern __inline  __m128d  __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_sd(__m128d  __A, __m128d  __B)
{
  __v2df a, b, c;
  /* PowerISA VSX does not allow partial (for just lower double)
     results. So to insure we don't generate spurious exceptions
     (from the upper double values) we splat the lower double
     before we do the operation. */
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = (__v2df) vec_cmpeq(a, b);
  /* Then we merge the lower double result with the original upper
     double from __A.  */
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = (__v2df) vec_cmplt(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmple_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = (__v2df) vec_cmple(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = (__v2df) vec_cmpgt(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpge_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = (__v2df) vec_cmpge(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpneq_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  c = (__v2df) vec_cmpeq(a, b);
  c = vec_nor (c, c);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnlt_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  /* Not less than is just greater than or equal.  */
  c = (__v2df) vec_cmpge(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnle_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  /* Not less than or equal is just greater than.  */
  c = (__v2df) vec_cmpge(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpngt_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  /* Not greater than is just less than or equal.  */
  c = (__v2df) vec_cmple(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpnge_sd (__m128d __A, __m128d __B)
{
  __v2df a, b, c;
  a = vec_splats (__A[0]);
  b = vec_splats (__B[0]);
  /* Not greater than or equal is just less than.  */
  c = (__v2df) vec_cmplt(a, b);
  return (__m128d) _mm_setr_pd (c[0], __A[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpord_sd (__m128d __A, __m128d __B)
{
  __v2df r;
  r = (__v2df)_mm_cmpord_pd (vec_splats (__A[0]), vec_splats (__B[0]));
  return (__m128d) _mm_setr_pd (r[0], ((__v2df)__A)[1]);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpunord_sd (__m128d __A, __m128d __B)
{
  __v2df r;
  r = _mm_cmpunord_pd (vec_splats (__A[0]), vec_splats (__B[0]));
  return (__m128d) _mm_setr_pd (r[0], __A[1]);
}

/* FIXME
   The __mm_comi??_sd and __mm_ucomi??_sd implementations below are
   exactly the same because GCC for PowerPC only generates unordered
   compares (scalar and vector).
   Technically __mm_comieq_sp et all should be using the ordered
   compare and signal for QNaNs.  The __mm_ucomieq_sd et all should
   be OK.   */
extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comieq_sd (__m128d __A, __m128d __B)
{
  return (__A[0] == __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comilt_sd (__m128d __A, __m128d __B)
{
  return (__A[0] < __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comile_sd (__m128d __A, __m128d __B)
{
  return (__A[0] <= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comigt_sd (__m128d __A, __m128d __B)
{
  return (__A[0] > __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comige_sd (__m128d __A, __m128d __B)
{
  return (__A[0] >= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_comineq_sd (__m128d __A, __m128d __B)
{
  return (__A[0] != __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomieq_sd (__m128d __A, __m128d __B)
{
	return (__A[0] == __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomilt_sd (__m128d __A, __m128d __B)
{
	return (__A[0] < __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomile_sd (__m128d __A, __m128d __B)
{
	return (__A[0] <= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomigt_sd (__m128d __A, __m128d __B)
{
	return (__A[0] > __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomige_sd (__m128d __A, __m128d __B)
{
	return (__A[0] >= __B[0]);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_ucomineq_sd (__m128d __A, __m128d __B)
{
  return (__A[0] != __B[0]);
}

/* Create a vector of Qi, where i is the element number.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi64x (long long __q1, long long __q0)
{
  return __extension__ (__m128i)(__v2di){ __q0, __q1 };
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi64 (__m64 __q1,  __m64 __q0)
{
  return _mm_set_epi64x ((long long)__q1, (long long)__q0);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi32 (int __q3, int __q2, int __q1, int __q0)
{
  return __extension__ (__m128i)(__v4si){ __q0, __q1, __q2, __q3 };
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi16 (short __q7, short __q6, short __q5, short __q4,
	       short __q3, short __q2, short __q1, short __q0)
{
  return __extension__ (__m128i)(__v8hi){
    __q0, __q1, __q2, __q3, __q4, __q5, __q6, __q7 };
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set_epi8 (char __q15, char __q14, char __q13, char __q12,
	      char __q11, char __q10, char __q09, char __q08,
	      char __q07, char __q06, char __q05, char __q04,
	      char __q03, char __q02, char __q01, char __q00)
{
  return __extension__ (__m128i)(__v16qi){
    __q00, __q01, __q02, __q03, __q04, __q05, __q06, __q07,
    __q08, __q09, __q10, __q11, __q12, __q13, __q14, __q15
  };
}

/* Set all of the elements of the vector to A.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi64x (long long __A)
{
  return _mm_set_epi64x (__A, __A);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi64 (__m64 __A)
{
  return _mm_set_epi64 (__A, __A);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi32 (int __A)
{
  return _mm_set_epi32 (__A, __A, __A, __A);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi16 (short __A)
{
  return _mm_set_epi16 (__A, __A, __A, __A, __A, __A, __A, __A);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_set1_epi8 (char __A)
{
  return _mm_set_epi8 (__A, __A, __A, __A, __A, __A, __A, __A,
		       __A, __A, __A, __A, __A, __A, __A, __A);
}

/* Create a vector of Qi, where i is the element number.
   The parameter order is reversed from the _mm_set_epi* functions.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi64 (__m64 __q0, __m64 __q1)
{
  return _mm_set_epi64 (__q1, __q0);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi32 (int __q0, int __q1, int __q2, int __q3)
{
  return _mm_set_epi32 (__q3, __q2, __q1, __q0);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi16 (short __q0, short __q1, short __q2, short __q3,
	        short __q4, short __q5, short __q6, short __q7)
{
  return _mm_set_epi16 (__q7, __q6, __q5, __q4, __q3, __q2, __q1, __q0);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setr_epi8 (char __q00, char __q01, char __q02, char __q03,
	       char __q04, char __q05, char __q06, char __q07,
	       char __q08, char __q09, char __q10, char __q11,
	       char __q12, char __q13, char __q14, char __q15)
{
  return _mm_set_epi8 (__q15, __q14, __q13, __q12, __q11, __q10, __q09, __q08,
		       __q07, __q06, __q05, __q04, __q03, __q02, __q01, __q00);
}

/* Create a vector with element 0 as *P and the rest zero.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_load_si128 (__m128i const *__P)
{
  return *__P;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadu_si128 (__m128i_u const *__P)
{
  return (__m128i) (vec_vsx_ld(0, (signed int const *)__P));
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadl_epi64 (__m128i_u const *__P)
{
  return _mm_set_epi64 ((__m64)0LL, *(__m64 *)__P);
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_store_si128 (__m128i *__P, __m128i __B)
{
  assert(((unsigned long )__P & 0xfUL) == 0UL);
  vec_st ((__v16qu) __B, 0, (__v16qu*)__P);
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storeu_si128 (__m128i_u *__P, __m128i __B)
{
  *__P = __B;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_storel_epi64 (__m128i_u *__P, __m128i __B)
{
  *(long long *)__P = ((__v2di)__B)[0];
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movepi64_pi64 (__m128i_u __B)
{
  return (__m64) ((__v2di)__B)[0];
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movpi64_epi64 (__m64 __A)
{
  return _mm_set_epi64 ((__m64)0LL, __A);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_move_epi64 (__m128i __A)
{
  return _mm_set_epi64 ((__m64)0LL, (__m64)__A[0]);
}

/* Create an undefined vector.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_undefined_si128 (void)
{
  __m128i __Y = __Y;
  return __Y;
}

/* Create a vector of zeros.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setzero_si128 (void)
{
  return __extension__ (__m128i)(__v4si){ 0, 0, 0, 0 };
}

#ifdef _ARCH_PWR8
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepi32_pd (__m128i __A)
{
  __v2di val;
  /* For LE need to generate Vector Unpack Low Signed Word.
     Which is generated from unpackh.  */
  val = (__v2di)vec_unpackh ((__v4si)__A);

  return (__m128d)vec_ctf (val, 0);
}
#endif

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtepi32_ps (__m128i __A)
{
  return ((__m128)vec_ctf((__v4si)__A, 0));
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpd_epi32 (__m128d __A)
{
  __v2df rounded = vec_rint (__A);
  __v4si result, temp;
  const __v4si vzero =
    { 0, 0, 0, 0 };

  /* VSX Vector truncate Double-Precision to integer and Convert to
   Signed Integer Word format with Saturate.  */
  __asm__(
      "xvcvdpsxws %x0,%x1"
      : "=wa" (temp)
      : "wa" (rounded)
      : );

#ifdef _ARCH_PWR8
  temp = vec_mergeo (temp, temp);
  result = (__v4si)vec_vpkudum ((__vector long)temp, (__vector long)vzero);
#else
  {
    const __v16qu pkperm = {0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b,
	0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f };
    result = (__v4si) vec_perm ((__v16qu) temp, (__v16qu) vzero, pkperm);
  }
#endif
  return (__m128i) result;
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpd_pi32 (__m128d __A)
{
  __m128i result = _mm_cvtpd_epi32(__A);

  return (__m64) result[0];
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpd_ps (__m128d __A)
{
  __v4sf result;
  __v4si temp;
  const __v4si vzero = { 0, 0, 0, 0 };

  __asm__(
      "xvcvdpsp %x0,%x1"
      : "=wa" (temp)
      : "wa" (__A)
      : );

#ifdef _ARCH_PWR8
  temp = vec_mergeo (temp, temp);
  result = (__v4sf)vec_vpkudum ((__vector long)temp, (__vector long)vzero);
#else
  {
    const __v16qu pkperm = {0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b,
	0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f };
    result = (__v4sf) vec_perm ((__v16qu) temp, (__v16qu) vzero, pkperm);
  }
#endif
  return ((__m128)result);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttpd_epi32 (__m128d __A)
{
  __v4si result;
  __v4si temp;
  const __v4si vzero = { 0, 0, 0, 0 };

  /* VSX Vector truncate Double-Precision to integer and Convert to
   Signed Integer Word format with Saturate.  */
  __asm__(
      "xvcvdpsxws %x0,%x1"
      : "=wa" (temp)
      : "wa" (__A)
      : );

#ifdef _ARCH_PWR8
  temp = vec_mergeo (temp, temp);
  result = (__v4si)vec_vpkudum ((__vector long)temp, (__vector long)vzero);
#else
  {
    const __v16qu pkperm = {0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b,
	0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f };
    result = (__v4si) vec_perm ((__v16qu) temp, (__v16qu) vzero, pkperm);
  }
#endif

  return ((__m128i) result);
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttpd_pi32 (__m128d __A)
{
  __m128i result = _mm_cvttpd_epi32 (__A);

  return (__m64) result[0];
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi128_si32 (__m128i __A)
{
  return ((__v4si)__A)[0];
}

#ifdef _ARCH_PWR8
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtpi32_pd (__m64 __A)
{
  __v4si temp;
  __v2di tmp2;
  __v2df result;

  temp = (__v4si)vec_splats (__A);
  tmp2 = (__v2di)vec_unpackl (temp);
  result = vec_ctf ((__vector signed long)tmp2, 0);
  return (__m128d)result;
}
#endif

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtps_epi32 (__m128 __A)
{
  __v4sf rounded;
  __v4si result;

  rounded = vec_rint((__v4sf) __A);
  result = vec_cts (rounded, 0);
  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttps_epi32 (__m128 __A)
{
  __v4si result;

  result = vec_cts ((__v4sf) __A, 0);
  return (__m128i) result;
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtps_pd (__m128 __A)
{
  /* Check if vec_doubleh is defined by <altivec.h>. If so use that. */
#ifdef vec_doubleh
  return (__m128d) vec_doubleh ((__v4sf)__A);
#else
  /* Otherwise the compiler is not current and so need to generate the
     equivalent code.  */
  __v4sf a = (__v4sf)__A;
  __v4sf temp;
  __v2df result;
#ifdef __LITTLE_ENDIAN__
  /* The input float values are in elements {[0], [1]} but the convert
     instruction needs them in elements {[1], [3]}, So we use two
     shift left double vector word immediates to get the elements
     lined up.  */
  temp = __builtin_vsx_xxsldwi (a, a, 3);
  temp = __builtin_vsx_xxsldwi (a, temp, 2);
#elif __BIG_ENDIAN__
  /* The input float values are in elements {[0], [1]} but the convert
     instruction needs them in elements {[0], [2]}, So we use two
     shift left double vector word immediates to get the elements
     lined up.  */
  temp = vec_vmrghw (a, a);
#endif
  __asm__(
      " xvcvspdp %x0,%x1"
      : "=wa" (result)
      : "wa" (temp)
      : );
  return (__m128d) result;
#endif
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsd_si32 (__m128d __A)
{
  __v2df rounded = vec_rint((__v2df) __A);
  int result = ((__v2df)rounded)[0];

  return result;
}
/* Intel intrinsic.  */
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsd_si64 (__m128d __A)
{
  __v2df rounded = vec_rint ((__v2df) __A );
  long long result = ((__v2df) rounded)[0];

  return result;
}

/* Microsoft intrinsic.  */
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsd_si64x (__m128d __A)
{
  return _mm_cvtsd_si64 ((__v2df)__A);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttsd_si32 (__m128d __A)
{
  int result = ((__v2df)__A)[0];

  return result;
}

/* Intel intrinsic.  */
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttsd_si64 (__m128d __A)
{
  long long result = ((__v2df)__A)[0];

  return result;
}

/* Microsoft intrinsic.  */
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvttsd_si64x (__m128d __A)
{
  return _mm_cvttsd_si64 (__A);
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsd_ss (__m128 __A, __m128d __B)
{
  __v4sf result = (__v4sf)__A;

#ifdef __LITTLE_ENDIAN__
  __v4sf temp_s;
  /* Copy double element[0] to element [1] for conversion.  */
  __v2df temp_b = vec_splat((__v2df)__B, 0);

  /* Pre-rotate __A left 3 (logically right 1) elements.  */
  result = __builtin_vsx_xxsldwi (result, result, 3);
  /* Convert double to single float scalar in a vector.  */
  __asm__(
      "xscvdpsp %x0,%x1"
      : "=wa" (temp_s)
      : "wa" (temp_b)
      : );
  /* Shift the resulting scalar into vector element [0].  */
  result = __builtin_vsx_xxsldwi (result, temp_s, 1);
#else
  result [0] = ((__v2df)__B)[0];
#endif
  return (__m128) result;
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi32_sd (__m128d __A, int __B)
{
  __v2df result = (__v2df)__A;
  double db = __B;
  result [0] = db;
  return (__m128d)result;
}

/* Intel intrinsic.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64_sd (__m128d __A, long long __B)
{
  __v2df result = (__v2df)__A;
  double db = __B;
  result [0] = db;
  return (__m128d)result;
}

/* Microsoft intrinsic.  */
extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64x_sd (__m128d __A, long long __B)
{
  return _mm_cvtsi64_sd (__A, __B);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtss_sd (__m128d __A, __m128 __B)
{
#ifdef __LITTLE_ENDIAN__
  /* Use splat to move element [0] into position for the convert. */
  __v4sf temp = vec_splat ((__v4sf)__B, 0);
  __v2df res;
  /* Convert single float scalar to double in a vector.  */
  __asm__(
      "xscvspdp %x0,%x1"
      : "=wa" (res)
      : "wa" (temp)
      : );
  return (__m128d) vec_mergel (res, (__v2df)__A);
#else
  __v2df res = (__v2df)__A;
  res [0] = ((__v4sf)__B) [0];
  return (__m128d) res;
#endif
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_pd(__m128d __A, __m128d __B, const int __mask)
{
  __vector double result;
  const int litmsk = __mask & 0x3;

  if (litmsk == 0)
    result = vec_mergeh (__A, __B);
#if __GNUC__ < 6
  else if (litmsk == 1)
    result = vec_xxpermdi (__B, __A, 2);
  else if (litmsk == 2)
    result = vec_xxpermdi (__B, __A, 1);
#else
  else if (litmsk == 1)
    result = vec_xxpermdi (__A, __B, 2);
  else if (litmsk == 2)
    result = vec_xxpermdi (__A, __B, 1);
#endif
  else
    result = vec_mergel (__A, __B);

  return result;
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_pd (__m128d __A, __m128d __B)
{
  return (__m128d) vec_mergel ((__v2df)__A, (__v2df)__B);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_pd (__m128d __A, __m128d __B)
{
  return (__m128d) vec_mergeh ((__v2df)__A, (__v2df)__B);
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadh_pd (__m128d __A, double const *__B)
{
  __v2df result = (__v2df)__A;
  result [1] = *__B;
  return (__m128d)result;
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_loadl_pd (__m128d __A, double const *__B)
{
  __v2df result = (__v2df)__A;
  result [0] = *__B;
  return (__m128d)result;
}

#ifdef _ARCH_PWR8
/* Intrinsic functions that require PowerISA 2.07 minimum.  */

/* Creates a 2-bit mask from the most significant bits of the DPFP values.  */
extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movemask_pd (__m128d  __A)
{
  __vector __m64 result;
  static const __vector unsigned int perm_mask =
    {
#ifdef __LITTLE_ENDIAN__
	0x80800040, 0x80808080, 0x80808080, 0x80808080
#elif __BIG_ENDIAN__
      0x80808080, 0x80808080, 0x80808080, 0x80800040
#endif
    };

  result = (__vector __m64) vec_vbpermq ((__vector unsigned char) __A,
					 (__vector unsigned char) perm_mask);

#ifdef __LITTLE_ENDIAN__
  return result[1];
#elif __BIG_ENDIAN__
  return result[0];
#endif
}
#endif /* _ARCH_PWR8 */

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packs_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_packs ((__v8hi) __A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packs_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_packs ((__v4si)__A, (__v4si)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_packus_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_packsu ((__v8hi) __A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergel ((__v16qu)__A, (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergel ((__v8hu)__A, (__v8hu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergel ((__v4su)__A, (__v4su)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpackhi_epi64 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergel ((__vector long)__A, (__vector long)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergeh ((__v16qu)__A, (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergeh ((__v8hi)__A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergeh ((__v4si)__A, (__v4si)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_unpacklo_epi64 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_mergeh ((__vector long)__A, (__vector long)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v16qu)__A + (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v8hu)__A + (__v8hu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v4su)__A + (__v4su)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_add_epi64 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v2du)__A + (__v2du)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_adds ((__v16qi)__A, (__v16qi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_adds ((__v8hi)__A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_adds ((__v16qu)__A, (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_adds_epu16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_adds ((__v8hu)__A, (__v8hu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v16qu)__A - (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v8hu)__A - (__v8hu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v4su)__A - (__v4su)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sub_epi64 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__v2du)__A - (__v2du)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_subs ((__v16qi)__A, (__v16qi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_subs ((__v8hi)__A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_subs ((__v16qu)__A, (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_subs_epu16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_subs ((__v8hu)__A, (__v8hu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_madd_epi16 (__m128i __A, __m128i __B)
{
  __vector signed int zero = {0, 0, 0, 0};

  return (__m128i) vec_vmsumshm ((__v8hi)__A, (__v8hi)__B, zero);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mulhi_epi16 (__m128i __A, __m128i __B)
{
  __vector signed int w0, w1;

  __vector unsigned char xform1 = {
#ifdef __LITTLE_ENDIAN__
      0x02, 0x03, 0x12, 0x13,  0x06, 0x07, 0x16, 0x17,
      0x0A, 0x0B, 0x1A, 0x1B,  0x0E, 0x0F, 0x1E, 0x1F
#elif __BIG_ENDIAN__
      0x00, 0x01, 0x10, 0x11,  0x04, 0x05, 0x14, 0x15,
      0x08, 0x09, 0x18, 0x19,  0x0C, 0x0D, 0x1C, 0x1D
#endif
    };

  w0 = vec_vmulesh ((__v8hi)__A, (__v8hi)__B);
  w1 = vec_vmulosh ((__v8hi)__A, (__v8hi)__B);
  return (__m128i) vec_perm (w0, w1, xform1);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mullo_epi16 (__m128i __A, __m128i __B)
{
    return (__m128i) ((__v8hi)__A * (__v8hi)__B);
}

extern __inline __m64 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_su32 (__m64 __A, __m64 __B)
{
  unsigned int a = __A;
  unsigned int b = __B;

  return ((__m64)a * (__m64)b);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mul_epu32 (__m128i __A, __m128i __B)
{
#if __GNUC__ < 8
  __v2du result;

#ifdef __LITTLE_ENDIAN__
  /* VMX Vector Multiply Odd Unsigned Word.  */
  __asm__(
      "vmulouw %0,%1,%2"
      : "=v" (result)
      : "v" (__A), "v" (__B)
      : );
#elif __BIG_ENDIAN__
  /* VMX Vector Multiply Even Unsigned Word.  */
  __asm__(
      "vmuleuw %0,%1,%2"
      : "=v" (result)
      : "v" (__A), "v" (__B)
      : );
#endif
  return (__m128i) result;
#else
#ifdef __LITTLE_ENDIAN__
  return (__m128i) vec_mule ((__v4su)__A, (__v4su)__B);
#elif __BIG_ENDIAN__
  return (__m128i) vec_mulo ((__v4su)__A, (__v4su)__B);
#endif
#endif
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_epi16 (__m128i __A, int __B)
{
  __v8hu lshift;
  __v8hi result = { 0, 0, 0, 0, 0, 0, 0, 0 };

  if (__B < 16)
    {
      if (__builtin_constant_p(__B))
	  lshift = (__v8hu) vec_splat_s16(__B);
      else
	  lshift = vec_splats ((unsigned short) __B);

      result = vec_vslh ((__v8hi) __A, lshift);
    }

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_epi32 (__m128i __A, int __B)
{
  __v4su lshift;
  __v4si result = { 0, 0, 0, 0 };

  if (__B < 32)
    {
      if (__builtin_constant_p(__B))
	lshift = (__v4su) vec_splat_s32(__B);
      else
	lshift = vec_splats ((unsigned int) __B);

      result = vec_vslw ((__v4si) __A, lshift);
    }

  return (__m128i) result;
}

#ifdef _ARCH_PWR8
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_epi64 (__m128i __A, int __B)
{
  __v2du lshift;
  __v2di result = { 0, 0 };

  if (__B < 64)
    {
      if (__builtin_constant_p(__B))
	{
	  if (__B < 32)
	      lshift = (__v2du) vec_splat_s32(__B);
	    else
	      lshift = (__v2du) vec_splats((unsigned long long)__B);
	}
      else
	  lshift = (__v2du) vec_splats ((unsigned int) __B);

      result = vec_vsld ((__v2di) __A, lshift);
    }

  return (__m128i) result;
}
#endif

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srai_epi16 (__m128i __A, int __B)
{
  __v8hu rshift = { 15, 15, 15, 15, 15, 15, 15, 15 };
  __v8hi result;

  if (__B < 16)
    {
      if (__builtin_constant_p(__B))
	rshift = (__v8hu) vec_splat_s16(__B);
      else
	rshift = vec_splats ((unsigned short) __B);
    }
  result = vec_vsrah ((__v8hi) __A, rshift);

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srai_epi32 (__m128i __A, int __B)
{
  __v4su rshift = { 31, 31, 31, 31 };
  __v4si result;

  if (__B < 32)
    {
      if (__builtin_constant_p(__B))
	{
	  if (__B < 16)
	      rshift = (__v4su) vec_splat_s32(__B);
	    else
	      rshift = (__v4su) vec_splats((unsigned int)__B);
	}
      else
	rshift = vec_splats ((unsigned int) __B);
    }
  result = vec_vsraw ((__v4si) __A, rshift);

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_bslli_si128 (__m128i __A, const int __N)
{
  __v16qu result;
  const __v16qu zeros = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  if (__N < 16)
    result = vec_sld ((__v16qu) __A, zeros, __N);
  else
    result = zeros;

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_bsrli_si128 (__m128i __A, const int __N)
{
  __v16qu result;
  const __v16qu zeros = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  if (__N < 16)
    if (__builtin_constant_p(__N))
      /* Would like to use Vector Shift Left Double by Octet
	 Immediate here to use the immediate form and avoid
	 load of __N * 8 value into a separate VR.  */
      result = vec_sld (zeros, (__v16qu) __A, (16 - __N));
    else
      {
	__v16qu shift = vec_splats((unsigned char)(__N*8));
	result = vec_sro ((__v16qu)__A, shift);
      }
  else
    result = zeros;

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_si128 (__m128i __A, const int __N)
{
  return _mm_bsrli_si128 (__A, __N);
}

extern __inline  __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_slli_si128 (__m128i __A, const int _imm5)
{
  __v16qu result;
  const __v16qu zeros = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  if (_imm5 < 16)
#ifdef __LITTLE_ENDIAN__
    result = vec_sld ((__v16qu) __A, zeros, _imm5);
#elif __BIG_ENDIAN__
    result = vec_sld (zeros, (__v16qu) __A, (16 - _imm5));
#endif
  else
    result = zeros;

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))

_mm_srli_epi16 (__m128i  __A, int __B)
{
  __v8hu rshift;
  __v8hi result = { 0, 0, 0, 0, 0, 0, 0, 0 };

  if (__B < 16)
    {
      if (__builtin_constant_p(__B))
	rshift = (__v8hu) vec_splat_s16(__B);
      else
	rshift = vec_splats ((unsigned short) __B);

      result = vec_vsrh ((__v8hi) __A, rshift);
    }

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_epi32 (__m128i __A, int __B)
{
  __v4su rshift;
  __v4si result = { 0, 0, 0, 0 };

  if (__B < 32)
    {
      if (__builtin_constant_p(__B))
	{
	  if (__B < 16)
	      rshift = (__v4su) vec_splat_s32(__B);
	    else
	      rshift = (__v4su) vec_splats((unsigned int)__B);
	}
      else
	rshift = vec_splats ((unsigned int) __B);

      result = vec_vsrw ((__v4si) __A, rshift);
    }

  return (__m128i) result;
}

#ifdef _ARCH_PWR8
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srli_epi64 (__m128i __A, int __B)
{
  __v2du rshift;
  __v2di result = { 0, 0 };

  if (__B < 64)
    {
      if (__builtin_constant_p(__B))
	{
	  if (__B < 16)
	      rshift = (__v2du) vec_splat_s32(__B);
	    else
	      rshift = (__v2du) vec_splats((unsigned long long)__B);
	}
      else
	rshift = (__v2du) vec_splats ((unsigned int) __B);

      result = vec_vsrd ((__v2di) __A, rshift);
    }

  return (__m128i) result;
}
#endif

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sll_epi16 (__m128i __A, __m128i __B)
{
  __v8hu lshift, shmask;
  const __v8hu shmax = { 15, 15, 15, 15, 15, 15, 15, 15 };
  __v8hu result;

#ifdef __LITTLE_ENDIAN__
  lshift = vec_splat ((__v8hu)__B, 0);
#elif __BIG_ENDIAN__
  lshift = vec_splat ((__v8hu)__B, 3);
#endif
  shmask = lshift <= shmax;
  result = vec_vslh ((__v8hu) __A, lshift);
  result = vec_sel (shmask, result, shmask);

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sll_epi32 (__m128i __A, __m128i __B)
{
  __v4su lshift, shmask;
  const __v4su shmax = { 32, 32, 32, 32 };
  __v4su result;
#ifdef __LITTLE_ENDIAN__
  lshift = vec_splat ((__v4su)__B, 0);
#elif __BIG_ENDIAN__
  lshift = vec_splat ((__v4su)__B, 1);
#endif
  shmask = lshift < shmax;
  result = vec_vslw ((__v4su) __A, lshift);
  result = vec_sel (shmask, result, shmask);

  return (__m128i) result;
}

#ifdef _ARCH_PWR8
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sll_epi64 (__m128i __A, __m128i __B)
{
  __v2du lshift, shmask;
  const __v2du shmax = { 64, 64 };
  __v2du result;

  lshift = (__v2du) vec_splat ((__v2du)__B, 0);
  shmask = lshift < shmax;
  result = vec_vsld ((__v2du) __A, lshift);
  result = (__v2du) vec_sel ((__v2df) shmask, (__v2df) result,
			      (__v2df) shmask);

  return (__m128i) result;
}
#endif

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sra_epi16 (__m128i __A, __m128i __B)
{
  const __v8hu rshmax = { 15, 15, 15, 15, 15, 15, 15, 15 };
  __v8hu rshift;
  __v8hi result;

#ifdef __LITTLE_ENDIAN__
  rshift = vec_splat ((__v8hu)__B, 0);
#elif __BIG_ENDIAN__
  rshift = vec_splat ((__v8hu)__B, 3);
#endif
  rshift = vec_min (rshift, rshmax);
  result = vec_vsrah ((__v8hi) __A, rshift);

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sra_epi32 (__m128i __A, __m128i __B)
{
  const __v4su rshmax = { 31, 31, 31, 31 };
  __v4su rshift;
  __v4si result;

#ifdef __LITTLE_ENDIAN__
  rshift = vec_splat ((__v4su)__B, 0);
#elif __BIG_ENDIAN__
  rshift = vec_splat ((__v4su)__B, 1);
#endif
  rshift = vec_min (rshift, rshmax);
  result = vec_vsraw ((__v4si) __A, rshift);

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srl_epi16 (__m128i __A, __m128i __B)
{
  __v8hu rshift, shmask;
  const __v8hu shmax = { 15, 15, 15, 15, 15, 15, 15, 15 };
  __v8hu result;

#ifdef __LITTLE_ENDIAN__
  rshift = vec_splat ((__v8hu)__B, 0);
#elif __BIG_ENDIAN__
  rshift = vec_splat ((__v8hu)__B, 3);
#endif
  shmask = rshift <= shmax;
  result = vec_vsrh ((__v8hu) __A, rshift);
  result = vec_sel (shmask, result, shmask);

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srl_epi32 (__m128i __A, __m128i __B)
{
  __v4su rshift, shmask;
  const __v4su shmax = { 32, 32, 32, 32 };
  __v4su result;

#ifdef __LITTLE_ENDIAN__
  rshift = vec_splat ((__v4su)__B, 0);
#elif __BIG_ENDIAN__
  rshift = vec_splat ((__v4su)__B, 1);
#endif
  shmask = rshift < shmax;
  result = vec_vsrw ((__v4su) __A, rshift);
  result = vec_sel (shmask, result, shmask);

  return (__m128i) result;
}

#ifdef _ARCH_PWR8
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_srl_epi64 (__m128i __A, __m128i __B)
{
  __v2du rshift, shmask;
  const __v2du shmax = { 64, 64 };
  __v2du result;

  rshift = (__v2du) vec_splat ((__v2du)__B, 0);
  shmask = rshift < shmax;
  result = vec_vsrd ((__v2du) __A, rshift);
  result = (__v2du)vec_sel ((__v2du)shmask, (__v2du)result, (__v2du)shmask);

  return (__m128i) result;
}
#endif

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_and_pd (__m128d __A, __m128d __B)
{
  return (vec_and ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_andnot_pd (__m128d __A, __m128d __B)
{
  return (vec_andc ((__v2df) __B, (__v2df) __A));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_or_pd (__m128d __A, __m128d __B)
{
  return (vec_or ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_xor_pd (__m128d __A, __m128d __B)
{
  return (vec_xor ((__v2df) __A, (__v2df) __B));
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmpeq ((__v16qi) __A, (__v16qi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmpeq ((__v8hi) __A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpeq_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmpeq ((__v4si) __A, (__v4si)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmplt ((__v16qi) __A, (__v16qi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmplt ((__v8hi) __A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmplt_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmplt ((__v4si) __A, (__v4si)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epi8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmpgt ((__v16qi) __A, (__v16qi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmpgt ((__v8hi) __A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cmpgt_epi32 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_cmpgt ((__v4si) __A, (__v4si)__B);
}

extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_extract_epi16 (__m128i const __A, int const __N)
{
  return (unsigned short) ((__v8hi)__A)[__N & 7];
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_insert_epi16 (__m128i const __A, int const __D, int const __N)
{
  __v8hi result = (__v8hi)__A;

  result [(__N & 7)] = __D;

  return (__m128i) result;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_max ((__v8hi)__A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_max_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_max ((__v16qu) __A, (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_epi16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_min ((__v8hi) __A, (__v8hi)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_min_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_min ((__v16qu) __A, (__v16qu)__B);
}


#ifdef _ARCH_PWR8
/* Intrinsic functions that require PowerISA 2.07 minimum.  */

/* Creates a 4-bit mask from the most significant bits of the SPFP values.  */
extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_movemask_epi8 (__m128i __A)
{
  __vector __m64 result;
  static const __vector unsigned char perm_mask =
    {
#ifdef __LITTLE_ENDIAN__
	0x78, 0x70, 0x68, 0x60, 0x58, 0x50, 0x48, 0x40,
	0x38, 0x30, 0x28, 0x20, 0x18, 0x10, 0x08, 0x00
#elif __BIG_ENDIAN__
	0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
	0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78
#endif
    };

  result = (__vector __m64) vec_vbpermq ((__vector unsigned char) __A,
					 (__vector unsigned char) perm_mask);

#ifdef __LITTLE_ENDIAN__
  return result[1];
#elif __BIG_ENDIAN__
  return result[0];
#endif
}
#endif /* _ARCH_PWR8 */

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mulhi_epu16 (__m128i __A, __m128i __B)
{
  __v4su w0, w1;
  __v16qu xform1 = {
#ifdef __LITTLE_ENDIAN__
      0x02, 0x03, 0x12, 0x13,  0x06, 0x07, 0x16, 0x17,
      0x0A, 0x0B, 0x1A, 0x1B,  0x0E, 0x0F, 0x1E, 0x1F
#elif __BIG_ENDIAN__
      0x00, 0x01, 0x10, 0x11,  0x04, 0x05, 0x14, 0x15,
      0x08, 0x09, 0x18, 0x19,  0x0C, 0x0D, 0x1C, 0x1D
#endif
    };

  w0 = vec_vmuleuh ((__v8hu)__A, (__v8hu)__B);
  w1 = vec_vmulouh ((__v8hu)__A, (__v8hu)__B);
  return (__m128i) vec_perm (w0, w1, xform1);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shufflehi_epi16 (__m128i __A, const int __mask)
{
  unsigned long element_selector_98 = __mask & 0x03;
  unsigned long element_selector_BA = (__mask >> 2) & 0x03;
  unsigned long element_selector_DC = (__mask >> 4) & 0x03;
  unsigned long element_selector_FE = (__mask >> 6) & 0x03;
  static const unsigned short permute_selectors[4] =
    {
#ifdef __LITTLE_ENDIAN__
	      0x0908, 0x0B0A, 0x0D0C, 0x0F0E
#elif __BIG_ENDIAN__
	      0x0607, 0x0405, 0x0203, 0x0001
#endif
    };
  __v2du pmask =
#ifdef __LITTLE_ENDIAN__
      { 0x1716151413121110UL,  0x1f1e1d1c1b1a1918UL};
#elif __BIG_ENDIAN__
      { 0x1011121314151617UL,  0x18191a1b1c1d1e1fUL};
#endif
  __m64_union t;
  __v2du a, r;

#ifdef __LITTLE_ENDIAN__
  t.as_short[0] = permute_selectors[element_selector_98];
  t.as_short[1] = permute_selectors[element_selector_BA];
  t.as_short[2] = permute_selectors[element_selector_DC];
  t.as_short[3] = permute_selectors[element_selector_FE];
#elif __BIG_ENDIAN__
  t.as_short[3] = permute_selectors[element_selector_98];
  t.as_short[2] = permute_selectors[element_selector_BA];
  t.as_short[1] = permute_selectors[element_selector_DC];
  t.as_short[0] = permute_selectors[element_selector_FE];
#endif
#ifdef __LITTLE_ENDIAN__
  pmask[1] = t.as_m64;
#elif __BIG_ENDIAN__
  pmask[0] = t.as_m64;
#endif
  a = (__v2du)__A;
  r = vec_perm (a, a, (__vector unsigned char)pmask);
  return (__m128i) r;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shufflelo_epi16 (__m128i __A, const int __mask)
{
  unsigned long element_selector_10 = __mask & 0x03;
  unsigned long element_selector_32 = (__mask >> 2) & 0x03;
  unsigned long element_selector_54 = (__mask >> 4) & 0x03;
  unsigned long element_selector_76 = (__mask >> 6) & 0x03;
  static const unsigned short permute_selectors[4] =
    {
#ifdef __LITTLE_ENDIAN__
	      0x0100, 0x0302, 0x0504, 0x0706
#elif __BIG_ENDIAN__
	      0x0e0f, 0x0c0d, 0x0a0b, 0x0809
#endif
    };
  __v2du pmask = { 0x1011121314151617UL,  0x1f1e1d1c1b1a1918UL};
  __m64_union t;
  __v2du a, r;

#ifdef __LITTLE_ENDIAN__
  t.as_short[0] = permute_selectors[element_selector_10];
  t.as_short[1] = permute_selectors[element_selector_32];
  t.as_short[2] = permute_selectors[element_selector_54];
  t.as_short[3] = permute_selectors[element_selector_76];
#elif __BIG_ENDIAN__
  t.as_short[3] = permute_selectors[element_selector_10];
  t.as_short[2] = permute_selectors[element_selector_32];
  t.as_short[1] = permute_selectors[element_selector_54];
  t.as_short[0] = permute_selectors[element_selector_76];
#endif
#ifdef __LITTLE_ENDIAN__
  pmask[0] = t.as_m64;
#elif __BIG_ENDIAN__
  pmask[1] = t.as_m64;
#endif
  a = (__v2du)__A;
  r = vec_perm (a, a, (__vector unsigned char)pmask);
  return (__m128i) r;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_shuffle_epi32 (__m128i __A, const int __mask)
{
  unsigned long element_selector_10 = __mask & 0x03;
  unsigned long element_selector_32 = (__mask >> 2) & 0x03;
  unsigned long element_selector_54 = (__mask >> 4) & 0x03;
  unsigned long element_selector_76 = (__mask >> 6) & 0x03;
  static const unsigned int permute_selectors[4] =
    {
#ifdef __LITTLE_ENDIAN__
	0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C
#elif __BIG_ENDIAN__
      0x0C0D0E0F, 0x08090A0B, 0x04050607, 0x00010203
#endif
    };
  __v4su t;

#ifdef __LITTLE_ENDIAN__
  t[0] = permute_selectors[element_selector_10];
  t[1] = permute_selectors[element_selector_32];
  t[2] = permute_selectors[element_selector_54] + 0x10101010;
  t[3] = permute_selectors[element_selector_76] + 0x10101010;
#elif __BIG_ENDIAN__
  t[3] = permute_selectors[element_selector_10] + 0x10101010;
  t[2] = permute_selectors[element_selector_32] + 0x10101010;
  t[1] = permute_selectors[element_selector_54];
  t[0] = permute_selectors[element_selector_76];
#endif
  return (__m128i)vec_perm ((__v4si) __A, (__v4si)__A, (__vector unsigned char)t);
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskmoveu_si128 (__m128i __A, __m128i __B, char *__C)
{
  __v2du hibit = { 0x7f7f7f7f7f7f7f7fUL, 0x7f7f7f7f7f7f7f7fUL};
  __v16qu mask, tmp;
  __m128i *p = (__m128i*)__C;

  tmp = (__v16qu)_mm_loadu_si128(p);
  mask = (__v16qu)vec_cmpgt ((__v16qu)__B, (__v16qu)hibit);
  tmp = vec_sel (tmp, (__v16qu)__A, mask);
  _mm_storeu_si128 (p, (__m128i)tmp);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avg_epu8 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_avg ((__v16qu)__A, (__v16qu)__B);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_avg_epu16 (__m128i __A, __m128i __B)
{
  return (__m128i) vec_avg ((__v8hu)__A, (__v8hu)__B);
}


extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_sad_epu8 (__m128i __A, __m128i __B)
{
  __v16qu a, b;
  __v16qu vmin, vmax, vabsdiff;
  __v4si vsum;
  const __v4su zero = { 0, 0, 0, 0 };
  __v4si result;

  a = (__v16qu) __A;
  b = (__v16qu) __B;
  vmin = vec_min (a, b);
  vmax = vec_max (a, b);
  vabsdiff = vec_sub (vmax, vmin);
  /* Sum four groups of bytes into integers.  */
  vsum = (__vector signed int) vec_sum4s (vabsdiff, zero);
  /* Sum across four integers with two integer results.  */
  result = vec_sum2s (vsum, (__vector signed int) zero);
  /* Rotate the sums into the correct position.  */
#ifdef __LITTLE_ENDIAN__
  result = vec_sld (result, result, 4);
#elif __BIG_ENDIAN__
  result = vec_sld (result, result, 6);
#endif
  /* Rotate the sums into the correct position.  */
  return (__m128i) result;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_si32 (int *__A, int __B)
{
  /* Use the data cache block touch for store transient.  */
  __asm__ (
    "dcbtstt 0,%0"
    :
    : "b" (__A)
    : "memory"
  );
  *__A = __B;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_si64 (long long int *__A, long long int __B)
{
  /* Use the data cache block touch for store transient.  */
  __asm__ (
    "	dcbtstt	0,%0"
    :
    : "b" (__A)
    : "memory"
  );
  *__A = __B;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_si128 (__m128i *__A, __m128i __B)
{
  /* Use the data cache block touch for store transient.  */
  __asm__ (
    "dcbtstt 0,%0"
    :
    : "b" (__A)
    : "memory"
  );
  *__A = __B;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_stream_pd (double *__A, __m128d __B)
{
  /* Use the data cache block touch for store transient.  */
  __asm__ (
    "dcbtstt 0,%0"
    :
    : "b" (__A)
    : "memory"
  );
  *(__m128d*)__A = __B;
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_clflush (void const *__A)
{
  /* Use the data cache block flush.  */
  __asm__ (
    "dcbf 0,%0"
    :
    : "b" (__A)
    : "memory"
  );
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_lfence (void)
{
  /* Use light weight sync for load to load ordering.  */
  __atomic_thread_fence (__ATOMIC_RELEASE);
}

extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mfence (void)
{
  /* Use heavy weight sync for any to any ordering.  */
  __atomic_thread_fence (__ATOMIC_SEQ_CST);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi32_si128 (int __A)
{
  return _mm_set_epi32 (0, 0, 0, __A);
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64_si128 (long long __A)
{
  return __extension__ (__m128i)(__v2di){ __A, 0LL };
}

/* Microsoft intrinsic.  */
extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_cvtsi64x_si128 (long long __A)
{
  return __extension__ (__m128i)(__v2di){ __A, 0LL };
}

/* Casts between various SP, DP, INT vector types.  Note that these do no
   conversion of values, they just change the type.  */
extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_castpd_ps(__m128d __A)
{
  return (__m128) __A;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_castpd_si128(__m128d __A)
{
  return (__m128i) __A;
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_castps_pd(__m128 __A)
{
  return (__m128d) __A;
}

extern __inline __m128i __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_castps_si128(__m128 __A)
{
  return (__m128i) __A;
}

extern __inline __m128 __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_castsi128_ps(__m128i __A)
{
  return (__m128) __A;
}

extern __inline __m128d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_castsi128_pd(__m128i __A)
{
  return (__m128d) __A;
}

#endif /* EMMINTRIN_H_ */
