/* Copyright (C) 1991-2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* We do support the IEC 559 math functionality, real and complex.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
int bitAnd(int, int);
int test_bitAnd(int, int);
int isAsciiDigit(int);
int test_isAsciiDigit(int);
int reverseBytes(int);
int test_reverseBytes(int);
int conditional(int, int, int);
int test_conditional(int, int, int);
int bitCount(int);
int test_bitCount(int);
int allEvenBits();
int test_allEvenBits();
int isGreater(int, int);
int test_isGreater(int, int);
int isNegative(int);
int test_isNegative(int);
int satMul3(int);
int test_satMul3(int);
int absVal(int);
int test_absVal(int);
unsigned float_abs(unsigned);
unsigned test_float_abs(unsigned);
unsigned float_twice(unsigned);
unsigned test_float_twice(unsigned);
