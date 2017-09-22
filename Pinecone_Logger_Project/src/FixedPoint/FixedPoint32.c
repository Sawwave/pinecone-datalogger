/*
* FixedPoint32.c
*
* Created: 9/14/2017 4:20:28 PM
*  Author: tim.anderson
*/

#include "FixedPoint/FixedPoint32.h"
/* FixedPoint32ToString
Takes the given fixed point structure, and writes it to the given string.
Written string will always write a digit before and after the decimal point.
Please make sure that dest has enough size to allow for up to
a decimal point, negative sign, 10 digits, and a null terminator.
use smaller dest strings at your own risk!

*/
void FixedPoint32ToString(const struct FixedPoint32 *fixedPoint, char *dest){

	uint32_t value = fixedPoint->data;
	//save the sign bit
	bool signBit = (value>>31);
	value = value & (~(1 << 31));

	if(fixedPoint->isValid){
		uint8_t charStackSize = 14;
		char charStack[charStackSize];
		uint8_t charStackIndex = charStackSize;
		
		charStack[--charStackIndex] = 0;	//null terminator

		if(fixedPoint->decimalDigits){
			//loop and print each decimal place
			const uint8_t lastDecimalIndex = (charStackSize - fixedPoint->decimalDigits);
			while (charStackIndex >= lastDecimalIndex){
				charStack[--charStackIndex] = '0' + (value % 10);
				value /= 10;
			}
		}
		else{	//if decimal digits was 0, just write a 0 in the tenths for pretty printing
			charStack[--charStackIndex] = '0';
		}

		charStack[--charStackIndex] = '.';

		//write the integer part of the string, with a 0 written if this part is 0.
		do{
			charStack[--charStackIndex] = '0' + (value % 10);
			value /= 10;
		}while(value);

		//add the sign character if the sign bit was set
		if(signBit){
			charStack[--charStackIndex] = '-';
		}
		

		//copy the stack to dest
		strcpy(dest, &charStack[charStackIndex]);
	}
	else{
		char nanStr[4] = "NaN";
		strcpy(dest, nanStr);
	}
}


/*FixedPoint32ToString
reads the src string and attempts to parse it into a fixed point structure.
parsing ends when encountering anything other than a digit, decimal, plus sign, or minus sign.
check the struct's isValid member for success.

returns the address of the next character in src after the number.
*/
char *StringToFixedPoint32(struct FixedPoint32 *fixedPoint, char *src){
	char srcChar = src[0];
	uint8_t currentSrcIndex = 0;
	bool storedSignBit = false;
	//if the first character is a + or -, set the sign bit accordingly and move onto the next char.
	if(srcChar == '-'){
		storedSignBit = true;
		currentSrcIndex = 1;
	}
	else if(srcChar == '+'){
		currentSrcIndex = 1;
	}

	//check to make sure the first non-sign character is valid
	//returns the address of the character after this illegal char.
	else if((srcChar < '0' || srcChar > '9')&& srcChar != '.'){
		fixedPoint->isValid = false;
		return src + currentSrcIndex + 1;

	}

	//initialize the fixed point
	fixedPoint->data = 0;
	fixedPoint->decimalDigits = 0;
	bool hasEncounteredDecimal = false;
	fixedPoint->isValid = true;

	//iterate through src, starting on the first char if there was no + or - at the beginning,
	// or the second if there was
	while(true){
		srcChar = src[currentSrcIndex++];
		if(srcChar >= '0' && srcChar <= '9'){
			fixedPoint->data *= 10;
			fixedPoint->data += (srcChar - '0');
			if(hasEncounteredDecimal){
				fixedPoint->decimalDigits++;
			}
		}
		else if(srcChar == '.'){
			hasEncounteredDecimal = true;
		}
		else{
			//if the next character's not a valid continuation of the number, end here.
			if(storedSignBit){
				fixedPoint->data |= (1<<31);
			}
			return src + currentSrcIndex;
		}

	}
	


}



struct FP32UnitTest{
	char src[26];
	char dest[26];
	char expectedDest[26];
	uint32_t expectedData;
	uint8_t expectedDecimalDigits;
	bool expectedIsValid;
};

uint32_t FixedPoint32UnitTests(void){


	struct FP32UnitTest tests[26];

	uint8_t testNum = 0;
	strcpy(tests[testNum].src, "9.9");
	strcpy(tests[testNum].expectedDest, "9.9");
	tests[testNum].expectedData = (99 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 1;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "+9.9");
	strcpy(tests[testNum].expectedDest, "9.9");
	tests[testNum].expectedData = (99 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 1;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "-9.9");
	strcpy(tests[testNum].expectedDest, "-9.9");
	tests[testNum].expectedData = (99 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 1;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "2345.9");
	strcpy(tests[testNum].expectedDest, "2345.9");
	tests[testNum].expectedData = (23459 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 1;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "6.12345");
	strcpy(tests[testNum].expectedDest, "6.12345");
	tests[testNum].expectedData = (612345 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 5;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "6123.45");
	strcpy(tests[testNum].expectedDest, "6123.45");
	tests[testNum].expectedData = (612345 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 2;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "0.123");
	strcpy(tests[testNum].expectedDest, "0.123");
	tests[testNum].expectedData = (123 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 3;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "-0.123");
	strcpy(tests[testNum].expectedDest, "-0.123");
	tests[testNum].expectedData = (123 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 3;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "-456.0");
	strcpy(tests[testNum].expectedDest, "-456.0");
	tests[testNum].expectedData = (4560 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 1;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, ".123");
	strcpy(tests[testNum].expectedDest, "0.123");
	tests[testNum].expectedData = (123 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 3;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "-.123");
	strcpy(tests[testNum].expectedDest, "-0.123");
	tests[testNum].expectedData = (123 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 3;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "456.");
	strcpy(tests[testNum].expectedDest, "456.0");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "-.456");
	strcpy(tests[testNum].expectedDest, "-0.456.0");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 3;
	tests[testNum].expectedIsValid = true;
	testNum++;
	
	strcpy(tests[testNum].src, "000456.3");
	strcpy(tests[testNum].expectedDest, "456.3");
	tests[testNum].expectedData = (4563 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 1;
	tests[testNum].expectedIsValid = true;
	testNum++;

	
	strcpy(tests[testNum].src, "a456");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;
	
	strcpy(tests[testNum].src, "+a456");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;
	
	strcpy(tests[testNum].src, "-a456");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;
	
	strcpy(tests[testNum].src, "+451a6.0");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;
	
	strcpy(tests[testNum].src, "+451a.0");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;
	
	strcpy(tests[testNum].src, "+451.a");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;
	
	strcpy(tests[testNum].src, "+451.4a");
	tests[testNum].expectedData = (456 | (tests[testNum].src[0] == '-'? (1<<31):0));
	tests[testNum].expectedDecimalDigits = 0;
	tests[testNum].expectedIsValid = false;
	testNum++;

	testNum = 14;
	while(testNum--){
		struct FixedPoint32 fp;
		
		const char * endptr = StringToFixedPoint32(&fp, tests[testNum].src);

		if(fp.isValid == tests[testNum].expectedIsValid){
			if(fp.isValid){
				if(fp.data != tests[testNum].expectedData){
					return testNum | 0x0100;
				}
				else if (fp.decimalDigits != tests[testNum].expectedDecimalDigits){
					return testNum | 0x0200;
				}
				else if(endptr[0] != 0){
					return testNum | 0x0400;
				}
			}

			//fp to string
			
			char dest[40];
			memset(dest, 'f', 40);
			FixedPoint32ToString(&fp, dest);
			if(fp.isValid){
				uint8_t counter = 0;
				do{
					if(dest[counter] != tests[testNum].expectedDest[counter]){
						return  testNum | 0x1000| dest[counter]<< 16;
					}
				}while(dest[++counter]);
			}
			else{
				if(dest[0] != 'N' || dest[1] != 'a' || dest[2] != 'N' || dest[3] != 0){
					return testNum | 0x2000;
				}
			}

		}
		else{
			return testNum | 0x4000;
		}
	}
	return 0;
}