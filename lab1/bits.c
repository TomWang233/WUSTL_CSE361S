/* 
 * CS:APP Data Lab 
 * 
 * <Chenyang Wang id:456970>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
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
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~(~x | ~y);/* Write up some truth tables and easy to see */
}
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int test_30 = (x >> 4) ^ 0x3; /* if x is 0x3?, return 0, else return 1 */
  int test_8 = (x >> 3) & 0x1; 
  int test_4and2 = !((x >> 1) & 0x3);
  /* write out 4-bit patterns from 0 to 9
   * and observe that we can use the 8th bit and 4th,2th bits
   * to test whether x is in the range of 0x?0 to 0x?9 */  
  return (!test_30) & !(test_8 & !(test_4and2));/* making it only satisfy the above two tests will return 1 */
}
/* 
 * reverseBytes - reverse the bytes of x
 *   Example: reverseBytes(0x01020304) = 0x04030201
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3
 */
int reverseBytes(int x) {
  int mask = 0xff; /* Create a mask of all 1s */
  int new_tail = (x >> 24) & mask; /* divide the input into four sections by using the mask */
  int new_third = (x >> 8) & (mask << 8);
  int new_second = (x << 8) & (mask << 16);
  int new_head = x << 24; 
  return  new_tail + new_third + new_second + new_head; /* regroup the sections to reverse the bytes */
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) { /* note: if x is true, y is evaluated, otherwise z is evaluated */
  int condition_test = ((!x) << 31) >> 31; /* first test whether x = 0, create mask accordingly */
  return (y & ~condition_test) + (z & condition_test); /* if x = 0, return z, if x != 0, return y */
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
 int bitCount(int x) { 
  int mask = 0x1;
  int result = 0;
  result = result + (x & mask);
  result = result + (x >> 1 & mask);
  result = result + (x >> 2 & mask);
  result = result + (x >> 3 & mask);
  result = result + (x >> 4 & mask);
  result = result + (x >> 5 & mask);
  result = result + (x >> 6 & mask);
  result = result + (x >> 7 & mask);
  result = result + (x >> 8 & mask);
  result = result + (x >> 9 & mask);
  result = result + (x >> 10 & mask);
  result = result + (x >> 11 & mask);
  result = result + (x >> 12 & mask);
  result = result + (x >> 13 & mask);
  result = result + (x >> 14 & mask);
  result = result + (x >> 15 & mask);
  result = result + (x >> 16 & mask);
  result = result + (x >> 17 & mask);
  result = result + (x >> 18 & mask);
  result = result + (x >> 19 & mask);
  result = result + (x >> 20 & mask);
  result = result + (x >> 21 & mask);
  result = result + (x >> 22 & mask);
  result = result + (x >> 23 & mask);
  result = result + (x >> 24 & mask);
  result = result + (x >> 25 & mask);
  result = result + (x >> 26 & mask);
  result = result + (x >> 27 & mask);
  result = result + (x >> 28 & mask);
  result = result + (x >> 29 & mask);
  result = result + (x >> 30 & mask);
  result = result + (x >> 31 & mask); /* brute force by comparing every bit to the mask 
                                       * and add up the result */
  return result;                                     
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word are set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) { /* note that 0xaa = 10101010, all odd bits are set to 1*/
  int odd_pos = (0xaa << 8) + 0xaa;
  int odd_pos_mask = (odd_pos << 16) + odd_pos; /* create a mask of 0xaaaaaaaa */
  int test = (x | odd_pos_mask) + 1; /*if all even bits are set to 1, test will go to 0 */
  return !test; /*if all even bits are 1, return 1 */
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
  int test = (y + ((~x)+1)); /* take (y-x)'s sign bit */
  int x_sign = x >> 31; /* take x's sign bit */
  int y_sign = y >> 31; /* take y's sign bit */
  int differ_sign = x_sign ^ y_sign; /* test if x and y are same sign */
  return ((differ_sign & y_sign) + ((~differ_sign) & (test >> 31))) & 0x1; /* if same sign, differ_sign = 0x00000000,
                                                                            * otherwise differ_sign = 0xffffffff */  
}
/* 
 * isNegative - return 1 if x < 0, return 0 otherwise 
 *   Example: isNegative(-1) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNegative(int x) {
  int test = x >> 31; /* test the sign of x; x negative, test = 0xffffffff; x positive, test = 0 */
  return !(!test);
}
/*
 * satMul3 - multiplies by 3, saturating to Tmin or Tmax if overflow
 *  Examples: satMul3(0x10000000) = 0x30000000
 *            satMul3(0x30000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0x70000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0xD0000000) = 0x80000000 (Saturate to TMin)
 *            satMul3(0xA0000000) = 0x80000000 (Saturate to TMin)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 3
 */
int satMul3(int x) {
    int test_x = x >> 31; /* test sign bit of x */
    int double_x = x + x; /* double x */
    int test_2x = double_x >> 31; /* test sign bit of 2x */
    int triple_x = double_x + x; /* triple x */
    int test_3x = triple_x >> 31; /* test sign bit of 3x */ 
    int t_min = 0x1 << 31; /* setup TMin */
	int t_max = ~t_min; /* setup TMax */
    int overflow = (((test_x ^ test_2x) | (test_x ^ test_3x)) << 31) >> 31; 
	/* remember that when checking overflow, check if signs of input and output differ */
    return ((~overflow) & triple_x) + (overflow & ~(test_x) & t_max) + (overflow & test_x & t_min);
	/* if no overflow, return 3x, if overflow return Tmin or Tmax */
}
/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
  int sign_mask = x >> 31; /* sign bit's mask, positive 0x00000000, negative 0xffffffff */
  return (x & ~sign_mask) + ((~x+1) & sign_mask); /* if positive, do nothing, if negative, negate x */ 
}
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
  unsigned mask = 0x7fffffff; /* mask with sign bit = 0 */
  unsigned result = uf & mask;/* sign bit of input should change if it's a 1 */
  unsigned exp = (uf & 0x7f800000) >> 23; /* extract two useful parts from the result */
  unsigned fraction = result & 0x7fffff;
  
  if (exp == 255 && fraction > 0) /* if goes into NaN, return uf */
	  return uf;
  
  else
  
  return result; /*otherwise return result */
  
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  unsigned sign = uf & 0x80000000;
  unsigned exp = (uf & 0x7f800000) >> 23;
  unsigned fraction = uf & 0x7fffff; /* separate input into three parts that defined floating-point */
  
  if (exp == 255 || (exp == 0 && fraction == 0))/* if NaN or Zero, return argument */
	  return uf;
  /* we need to consider several cases below: (see CSAPP 3e Figure 2.35 p.116)*/
  if (exp){ /*if exp is not 255 or 0 */
	  exp ++; /* to double the float, just increase exp by 1 */
  }else if (fraction != 0x7fffff){ /* if exp is 0, and is not denormalized */
	  fraction = fraction << 1; /*from the figure, conclude: left shift fraction by 1 */
  }else {/*if exp is 0 and denormalized */
	exp ++;
	fraction --; /*from the figure, conclude: exp+1,fraction-1 */
  }
  return sign + (exp << 23) + fraction; /* combine these parts for final result */
}
