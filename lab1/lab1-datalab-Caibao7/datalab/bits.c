/* 
 * CS:APP Data Lab 
 * 
 * <蔡亦扬 23307130258>
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
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


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

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

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
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
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
#include "bits.h"

// P1
/* 
 * tmax - return the biggest positive number that an integer can represent
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int tmax(void) {
  return ~(1 << 31);
}

// P2
/* 
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(4, 5) = -6, bitNor(-1,-2) = 0
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 2
 */
int bitNor(int x, int y) {
	return (~x & ~y);
}

// P3
/*
 * getByte - return the nth byte of x
 *   You can assume 0 <= n <= 3
 *   Example: getByte(0x01020304, 2) = 0x2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x,int n) {
  x = x >> (n << 3);
  return (x & 255);
}

// P4
/* 
 * roundUp - round up x to the nearest multiple of 256 that is bigger than x
 *   Example: roundUp(0x117f) = 0x1200
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int roundUp(int x) {
  x = (x + 255) & ~255 ;
  return x;
}

// P5
/*
 * absVal - return the absolute value of x
 *   Examples: absVal(-10) = 10
 *			       absVal(5) = 5
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int absVal(int x) {
    int mask = x >> 31;
    return (x ^ mask) + (~mask + 1);
}

// P6
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 4
 */
int isLessOrEqual(int x, int y) {
  int signx = x >> 31;
  int signy = y >> 31;
  int signe = !(signx ^ signy);
  int cmpsign = signx & !signy; /*符号不同时，x负y正时为1，x正y负时为0*/
  int t = y + (~x + 1);
  int calsign = t >> 31; /*若符号相同，得到相减后的符号位*/ 
  return (signe & (!calsign)) | ((!signe) & cmpsign);
}

// P7
/* 
 * logicalShift - shift x to the right by n bits, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int logicalShift(int x, int n) {
  int mask = ~(((1 << 31) >> n) << 1); 
  return (x >> n) & mask;
}


// P8
/*
 * swapOddandEven - swap the odd bits and even bits in x
 *   Examples: swapOddandEven(0xAA) = 0x55
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 4
 */
int swapOddandEven(int x) {
  int tl = x << 1;
  int tr = x >> 1;
  int oddMask = (0xAA | (0xAA << 8)) | ((0xAA | (0xAA << 8)) << 16); /*1010*/
  int evenMask = (0x55 | (0x55 << 8)) | ((0x55 | (0x55 << 8)) << 16); /*0101*/
  return (evenMask & tr) | (oddMask & tl);
}

// P9
/*
 * secondLowBit - return a mask that marks the position of the second least significant 1 bit
 *   Examples: secondLowBit(0x00000110) = 0x00000100
 *			       secondLowBit(0xFEDC1a80) = 0x00000200
 *             secondLowBit(0)  = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 4
 */
int secondLowBit(int x) {
  int lowest1 = x & (~x + 1);
  x = x ^ lowest1;
  return x & (~x + 1);
}

// P10
/* 
 * rotateNbits - rotate x to left by n bits
 *   you can assume n >= 0
 *   Examples: rotateNbits(0x12345678, 8) = 0x34567812
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 5
 */
int rotateNBits(int x, int n) {
  n = n & 31;
  return (x << n) | ((x >> (32 + (~n + 1))) & ~(((1 << 31) >> (~n + 1)) << 1));
}

// P11
/* 
 * fractions - return floor(x*7/16), for 0 <= x <= (1 << 28), x is an integer 
 *   Example: fractions(20) = 8
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 5
 */
int fractions(int x) {
  x = (x << 2) + (x << 1) + x;
  x = x >> 4;
  return x;
}


// P12
/* 
 * overflowCalc - given binary representations of three 32-bit positive numbers and add them together, 
 *      return the binary representation of the part where bits are higher than 32.
 *   Examples: overflowCalc(0xffffffff, 0xffffffff, 0xffffffff) = 2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 7 
 */
int overflowCalc(int x, int y, int z) {
  int sum1 = x + y;
  int sum2 = sum1 + z;
  int carry1 = ((x & y) | ((x ^ y) & ~sum1)) >> 31 & 1;
  int carry2 = ((sum1 & z) | ((sum1 ^ z) & ~sum2)) >> 31 & 1;
  return carry1 + carry2;
}


// P13
/* 
 * mul3 - return x*3, and if x*3 overflow, change the result to 
 * INT_MAX(0x7fffffff) or INT_MIN(0x80000000) correspondingly
 *   Examples: mul3(1) = 0x3, mul3(0x7ffffff0) = 0x7fffffff
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 36
 *   Rating: 7
 */
int mul3(int x) {
  int x2 = x << 1;
  int sum = x2 + x;
  int of_in_x2 = ((x ^ x2) >> 31);
  int of_in_sum = ((x ^ sum) >> 31);
  int overflow = of_in_x2 | of_in_sum;/*溢出为全1，没有溢出为0*/
  int sign = x >> 31;
  int INT_MIN = 1 << 31;
  int INT_MAX = ~INT_MIN;
  int of_value = (~sign & INT_MAX) | (sign & INT_MIN);
  return (overflow & of_value) | (~overflow & sum);
}


// P14
/* 
 * float_abs - Return bit-level equivalent of expression |f| (absolute value of f) for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer / unsigned operations incl. ||, &&. also if, while
 *   Max ops: 20
 *   Rating: 3
 */
unsigned float_abs(unsigned uf) {
  unsigned mask = ~(1 << 31);
  unsigned abs_uf = uf & mask;   

  unsigned exponent = (uf >> 23) & 0xFF;
  unsigned fraction = uf & ((1 << 23) - 1);

  if (exponent == 0xFF && fraction != 0) {
    return uf;
  } else {
    return abs_uf;
  }
}

// P15
/* 
 * float_half - Return bit-level equivalent of expression f/2 for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer / unsigned operations incl. ||, &&. also if, while
 *   Max ops: 32
 *   Rating: 4
 */
unsigned float_half(unsigned f) {
    unsigned sign = f & (1 << 31);
    unsigned exp = (f >> 23) & 0xFF;
    unsigned frac = f & ((1 << 23) - 1);
    unsigned rounding;

    if (exp == 0xFF) {
        return f; // NaN 或 Infinity 返回自身
    }

    if (exp == 0 || exp == 1) {
      // 非标准化数或指数为1的情况，需要处理尾数并考虑舍入
      if (exp == 1) {
          frac = frac | (1 << 23);
      }

      // 检查被移出的位以决定是否需要舍入
      rounding = (frac & 3); // 取尾数的最低两位

      frac = frac >> 1; // 尾数右移一位

      if (rounding == 3) { // 11就加1
          frac = frac + 1;
      }

      // 指数变为0（非标准化数）
      exp = 0;

      return sign | (exp << 23) | (frac & ((1 << 23) - 1));
    } else {
      exp = exp - 1;
      return sign | (exp << 23) | frac;
    }
}

// P16
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x.
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer / unsigned operations incl. ||, &&. also if, while
 *   Max ops: 40
 *   Rating: 7
 */
unsigned float_i2f(int x) {
    unsigned sign = x & 0x80000000;
    unsigned abs_x = x;
    unsigned exp = 0;
    unsigned frac = 0;
    int shift = 0;

    if (x == 0) {
        return 0;
    }
    if (x < 0) {
        abs_x = -x;
    }

    // exp的计算
    while ((abs_x & 0x80000000) == 0) {
        abs_x <<= 1;
        shift++;
    }
    exp = 158 - shift;

    // frac计算
    frac = (abs_x & 0x7FFFFFFF) >> 8;

    // 舍入处理
    if ((abs_x & 0x80) && ((abs_x & 0x7F) || (frac & 1))) {
        frac++;
        if (frac >> 23) {
            exp++;
            frac = 0;
        }
    }

    return sign | (exp << 23) | frac;
}

#ifdef NOT_SUPPOSED_TO_BE_DEFINED
#   __          __  _                          
#   \ \        / / | |                         
#    \ \  /\  / /__| | ___ ___  _ __ ___   ___ 
#     \ \/  \/ / _ \ |/ __/ _ \| '_ ' _ \ / _ \
#      \  /\  /  __/ | (_| (_) | | | | | |  __/       
#       \/  \/ \___|_|\___\___/|_| |_| |_|\___|
#                                              

#  ██╗  ██╗ ██████╗ ███╗   ██╗ ██████╗ ██████╗     ██████╗  █████╗ ██████╗ ████████╗
#  ██║  ██║██╔═══██╗████╗  ██║██╔═══██╗██╔══██╗    ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝
#  ███████║██║   ██║██╔██╗ ██║██║   ██║██████╔╝    ██████╔╝███████║██████╔╝   ██║   
#  ██╔══██║██║   ██║██║╚██╗██║██║   ██║██╔══██╗    ██╔═══╝ ██╔══██║██╔══██╗   ██║   
#  ██║  ██║╚██████╔╝██║ ╚████║╚██████╔╝██║  ██║    ██║     ██║  ██║██║  ██║   ██║   
#  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝    ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   
#                                                                                   
#endif

// P17
/*
 * oddParity - return the odd parity bit of x, that is, 
 *      when the number of 1s in the binary representation of x is even, then the return 1, otherwise return 0.
 *   Examples: oddParity(5) = 1, oddParity(7) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 56
 *   Rating: 2
 */
int oddParity(int x) {
  x ^= (x >> 16);
  x ^= (x >> 8);
  x ^= (x >> 4);
  x ^= (x >> 2);
  x ^= (x >> 1);
  return !(x & 1);
}

// P18
/*
 * bitCount - return count of number of 1's in the binary representation of x
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 2
 */
int bitCount(int x) {
  int mask1, mask2, mask3, mask4, mask5;
  mask1 = 0x55 | (0x55 << 8); 
  mask1 = mask1 | (mask1 << 16);  
  mask2 = 0x33 | (0x33 << 8);   
  mask2 = mask2 | (mask2 << 16);  
  mask3 = 0x0F | (0x0F << 8);  
  mask3 = mask3 | (mask3 << 16); 
  mask4 = 0xFF | (0xFF << 16);
  mask5 = 0xFF | (0xFF << 8);      

  x = (x & mask1) + ((x >> 1) & mask1);
  x = (x & mask2) + ((x >> 2) & mask2);
  x = (x & mask3) + ((x >> 4) & mask3);
  x = (x & mask4) + ((x >> 8) & mask4);
  x = (x & mask5) + ((x >> 16) & mask5);

  return x;
}
