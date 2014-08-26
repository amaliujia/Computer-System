/* 
 * CS:APP Data Lab 
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
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int evenBits(void) {
/*
	do or operation between 0x00 and 0x55, then we can 0x55. Keep shifting, every time the least 8 bits are 0, do or.
 */
	int result = 0x55;
	result <<= 8;	
	result |= 0x55;
	return result | (result << 16);
}
/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
/*
 * xor means noting in common, x ^ y == 0 means x is equal to y. Then neg it, we can get answer.
 */
	return !(x ^ y);	
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
/*
 * basic idea: firstly, I figure out how many bits are n, m bytes. 
 * 				Then,as I know m, n's offset, I shift parts which needs to be swapped to the end of x, in order to store info.
 * 				after that, I set swapping parts of x to 0.
 * 				Finally, I transfer n part to m part, and vice versa 
 */
	int noffset = n << 3;
	int moffset = m << 3;
	int nr = (x >> noffset) & 0xff;
	int mr = (x >> moffset) & 0xff;
	x = x & (~(0xff << noffset));
	x = x | (mr << noffset);
	x = x & (~(0xff << moffset)); 
	x = x | (nr << moffset);
	return x;	
}
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x18765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateRight(int x, int n) {
/*
 * Basic idea is shift x twice, one is to shift right n, another is to shift left 32-n, then merge these two num.
 * However, trouble is I cannot merge them directly, because right shift is arithmetic shift. Therefore before mergeing them, it is necessary to turn n bits at most significant side into 0, no matter 0 or 1 they originally are.
 * mask is ~(((1 << n) + (~0x1 + 1)) << (32 - n)), this is the most difficult part and cost me an half hour to think.
 */
	int xright = x >> n;
	int ncom = 0x21 + ~n;
	int xleft = x << ncom;	
	int mask = ~(((1 << n) + ~0x00) << ncom);
	xright &= mask;
	return xleft | xright;
}
/* 
 * logicalNeg - implement the ! operator using any of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
/*
 * The difference between zero and nonzero numbers is that zero'opposite number is still zero. Therefore I use or operation to distuiguish zero and nonzero. zero or zero is 0x00. For nonzero number, the result of it or its opposite number is helpful, beacause the most significant bit of this result must be 1.
 * Then I shift the most significant bit to least significant position, and use 0x01 as mask to clear other bits.
 * Finally, if the least significant bit is 0, I know x is 0, otherwise x is nonzero. 
 */
	int result = (~x + 1) | x;
	result >>= 0x1f;	
	result &= 0x01;
	return result ^ 0x01;
}
/* 
 * TMax - return maximum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmax(void) {
/*
 *	if we need 0111, we can use 1000 - 1 to get the anwser.
 */
	int result = 0x01;
	result <<= 0x1f;
	return result + ~0x00;	
}
/* 
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x) {
/*
 * For logicalNeg, it only needs 1 bit info, but for this one, it needs two: sign and zero. I need sign info because I need to know if I have to return -1(0xffffffff), and I need zero info cause I need to decide return 0 or 1
 * For sign info, right shift x is enough. By doing this, I get 0x00000000 or 0xffffffff.
 * For zero info, I use the same way as I did in logicalNeg. As we know, (~x + 1) | x can give us anwser if x is 0 or not, because if x is nonzero, the second most significant bit must be one. So I move this resullt 30 bits and & it with 0x01.
 * Finally, sign info | zero info, I get the final answer.
 */
 	int c = (~x + 1) | x;
	return ((c >> 0x1e) & 0x01) | (x  >> 0x1f);
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
/*
 * There are four case
 * 1. x is neg but y is pos, return 0
 * 2. x is pos but y is neg, return 1
 * 3 and 4. x and y have same sign, then x - y, if result > 0, return 1, else return 0
 */
	int xsign = (x >> 0x1f) & 0x01;
	int ysign = (y >> 0x1f) & 0x01;
	int var1 = ~(x + (~y + 1)) + 1;
	int var2 = ((var1 >> 0x1f) & 0x01) & (var1 >> 0x1f); 
	return ((!xsign) & ysign) | (var2 & (!(xsign ^ ysign)));
}
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
/*
 * Only when x and y have opposite sign, x - y could overflow.
 * In this case, the flag of overflow is if sub of x and y has a opposite sign against x
 */	
	int negy = ~y + 1;
	int sum = x + negy;
	return !( ((x ^ y) >> 0x1f) & ((sum ^ x) >> 0x1f) );
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum possible value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
	int sum = x + y;
    int sumSign = (sum >> 0x1f) & 0x01;
	int specialNum = 1 << 0x1f;		
	return ((~(((sum ^ x) >> 0x1f) & ((sum ^ y) >> 0x1f))) & (x + y)) | ((((sum ^ x) & (sum ^ y)) >> 0x1f) & (specialNum + (~(sumSign) + 1))); 
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
	int offset = 16;
	int mask = 0xff;
	int n,nn,judge,offset2, sign;

    sign = x >> 31;	
	x = (sign & (~x)) | ((~sign) & x);
	
	mask = (mask << 8) | 0xff;
	n = (x >> offset) & mask; 
	nn = ~n + 1;
	judge = (n | nn) >> 31;
	offset = offset & judge;	
	
	offset2 = 8;
    n = (x >> (offset + offset2)) & 0xff;
    nn = ~n + 1;
    judge = (n | nn) >> 31;
    offset = (offset2 & judge) + offset;

	offset2 = 4;
    n = (x >> (offset + offset2)) & 0x0f;
    nn = ~n + 1;
    judge = (n | nn) >> 31;
    offset = (offset2 & judge) + offset; 

    offset2 = 2;
    n = (x >> (offset + offset2)) & 0x03;
    nn = ~n + 1;
    judge = (n | nn) >> 31;
    offset = (offset2 & judge) + offset;

    offset2 = 1;
    n = (x >> (offset + offset2)) & 0x01;
    nn = ~n + 1;
    judge = (n | nn) >> 31;
    offset = (offset2 & judge) + offset;
	offset += 1;

	return (!!x) + offset;

}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
	unsigned sign = uf & 0x80000000;
	unsigned exp = (uf >> 23) & 0xff;
	unsigned frac = uf & 0x007fffff;
	if(exp == 0xff){
		return uf; 
	}
	if(exp == 0x00){
		frac = frac  >> 1;
		if((uf & 0x03) == 0x03){
			frac++;
		}
		return sign | frac;
	}
	if(exp == 0x01){
		frac = (frac >> 1) | 0x400000;
        if((uf & 0x03) == 0x03){
            frac++;
        }
		exp--;
	}else{
		exp--;
	}
	sign =  sign | (exp << 23) | frac; 
	return sign;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
    unsigned sign = (uf >> 31) & 0x01;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & 0x007fffff;
    if(exp == 0xff){
        return 0x80000000u;
    }
	if(exp <= 126){
		return 0;
	}
	frac = frac | 0x00800000;	
	if(exp > 126 && exp <= 150){
		frac =	frac >> (150 - exp);
	}else if(exp > 157){
		return 0x80000000u;
	}else{
		frac = frac << (exp - 150);	
	}
	if(sign == 1){
		frac = ~frac + 1;
	}
	return frac;
}
