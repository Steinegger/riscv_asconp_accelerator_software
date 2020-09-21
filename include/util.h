#ifndef UTIL_H_
#define UTIL_H_

#define START 0
#define END 1
#include "api.h"
#include "types.h"

#define STR(x) #x
#define VALUE(x) STR(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

#define CEIL_DIV(x, y) (x + y - 1) / y
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define SWAP_ENDIANESS(x) ((((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24)))

void memsetzero(unsigned char* array, size_t size_bytes);

void printAsHex(unsigned char* array, size_t length);

void printTestNum(unsigned int testnum, char start_end);

void printChar(char c);

void printHexChar(char c);

void printInt(unsigned int number);

void printIntPlain(unsigned int number);

unsigned long long strlen_minimal(const char* string);

void printString(const char* string);

void endSimulation();

void endiantest();

#endif  // UTIL_H_
