/* { dg-do compile } */
/* { dg-options "-O2 -std=c11 -DNO_WARN_X86_INTRINSICS" } */

/* Test to ensure that "bool" gets undef'd in xmmintrin.h when
   we require strict ANSI.  Subsequent use of bool needs stdbool.h.
   altivec.h should eventually avoid defining bool, vector, and
   pixel, following distro testing.  */

#include <xmmintrin.h>

bool foo (int x) /* { dg-error "unknown type name 'bool'; did you mean '_Bool'?" } */
{
  return x == 2;
}

