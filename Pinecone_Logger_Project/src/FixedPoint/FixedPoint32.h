/*
* FixedPoint32.h
*
* Created: 9/14/2017 4:20:43 PM
*  Author: tim.anderson
*/


#include <stdint.h>
#include <stdlib.h>
#include <asf.h>


#ifndef FIXEDPOINT32_H_
#define FIXEDPOINT32_H_


/*FixedPoint32 structure
stores 32 bits and how many decimal digits are represented.
data, and decimalDigits MUST be valid if isValid is true.
if isValid is false, the rest of the data is undefined.

bit 31 is treated as a sign bit.

*/
struct FixedPoint32{
	uint32_t data;
	uint8_t decimalDigits;
	bool isValid;
};
typedef struct FixedPoint32 FixedPoint32;

/* FixedPoint32ToString
Takes the given fixed point structure, and writes it to the given string.
Written string will always write a digit before and after the decimal point.
Please make sure that dest has enough size to allow for up to
a decimal point, negative sign, 10 digits, and a null terminator.
use smaller dest strings at your own risk!
*/
void FixedPoint32ToString(const struct FixedPoint32 *fixedPoint, char *dest);


/*FixedPoint32ToString
reads the src string and attempts to parse it into a fixed point structure.
parsing ends when encountering anything other than a digit, decimal, plus sign, or minus sign.
check the struct's isValid member for success.
*/
char *StringToFixedPoint32(struct FixedPoint32 *fixedPoint, char *src);


/*FixedPoint32UnitTests
tests the FixedPoint32ToString and StringToFixedPoint32 functions for correct parsing.
returns 0 if all tests pass, or the index of the first test that failed.
*/
uint32_t FixedPoint32UnitTests(void);

#endif /* FIXEDPOINT32_H_ */