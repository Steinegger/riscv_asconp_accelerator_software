#ifndef IO_MACROS_H_
#define IO_MACROS_H_

#ifdef SPIKE
	#include <stdio.h>
	#include <stdlib.h>

	#define myprintString(str) printf("%s", str);
	#define myprintHexChar(hex) printf("%02x", hex);
	#define myprintChar(ch) printf("%c", (char) ch);
	#define myprintUnsignedWord(word) printf("%u", (unsigned int) word);
	#define myprintUnsignedWordPlain(word) printf("%u", (unsigned int) word);
	#define myprintUnsignedLongWord(word) printf("%llu", (unsigned long long int) word);
#else
	#define myprintString(str) printString(str);
	#define myprintHexChar(hex) printHexChar(hex);
	#define myprintChar(ch) printChar(ch);
	#define myprintUnsignedWord(word) printInt(word);
	#define myprintUnsignedWordPlain(word) printIntPlain(word);
	#define myprintUnsignedLongWord(word) printInt(word);
#endif

#define printTextAndNumberWNL(str, num) myprintString(str); myprintUnsignedWord(num); myprintChar('\n')
#define printTextAndText(str1, str2) myprintString(str1); myprintString(str2); myprintChar('\n')


#define PRINT_CYCLES(cycle_before, cycle_after, retinstr_before, retinstr_after) \
myprintString("\nCycle before            : "); \
myprintUnsignedLongWord(cycle_before); \
myprintString("\nCycle after             : "); \
myprintUnsignedLongWord(cycle_after); \
myprintString("\nCycle difference        : "); \
myprintUnsignedLongWord((cycle_after - cycle_before)); \
myprintString("\nInstr before            : "); \
myprintUnsignedLongWord(retinstr_before); \
myprintString("\nInstr after             : "); \
myprintUnsignedLongWord(retinstr_after); \
myprintString("\nInstr difference        : "); \
myprintUnsignedLongWord((retinstr_after - retinstr_before));

#endif // IO_MACROS_H_