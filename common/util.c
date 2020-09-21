#include "util.h"
#include "io_macros.h"

void memsetzero(unsigned char* array, size_t size_bytes){
	for (size_t i = 0; i < size_bytes; i++) {
		array[i] = 0;
	}
}

void printAsHex(unsigned char* array, size_t length) {
	for (size_t i = 0; i < length; i++) {
		if ( (i % 4) == 0) {
			myprintChar(' ');
		}
		myprintHexChar(array[i]);
	}
	myprintChar('\n');
}

void printTestNum(unsigned int testnum, char start_end) {
  if (start_end == START) {
    __asm__ __volatile__ (
        "   li t1, 0xFFFFFF20;"
        "   sw %[testnum], 0(t1);"
        :
        : [testnum] "r" ( testnum )
        : "%t1"
    );
  } else if (start_end == END) {
    __asm__ __volatile__ (
        "   li t1, 0xFFFFFF30;"
        "   sw %[testnum], 0(t1);"
        :
        : [testnum] "r" ( testnum )
        : "%t1"
    );
  }
}

void printChar(char c) {
  __asm__ __volatile__ (
      "   li t1, 0xFFFFFF00;"
      "   sb %[character], 0(t1);"
      :
      : [character] "r" ( c )
      : "%t1"
  );
}

void printHexChar(char c) {
  __asm__ __volatile__ (
      "   li t1, 0xFFFFFF04;"
      "   sb %[hex_character], 0(t1);"
      :
      : [hex_character] "r" ( c )
      : "%t1"
  );
}

void printInt(unsigned int number) {
  __asm__ __volatile__ (
      "   li t1, 0xFFFFFF10;"
      "   sw %[num], 0(t1);"
      :
      : [num] "r" ( number )
      : "%t1"
  );
}

void printIntPlain(unsigned int number) {
  __asm__ __volatile__ (
      "   li t1, 0xFFFFFF14;"
      "   sw %[num], 0(t1);"
      :
      : [num] "r" ( number )
      : "%t1"
  );
}


unsigned long long strlen_minimal(const char* string) {
  unsigned long long i = 0;
  while (string[i] != '\0') {
    i++;
  }
  return i;
}

void printString(const char* string) {
  size_t i = 0;
  while (string[i] != '\0') {
    printChar(string[i]);
    i++;
  }
}

void endSimulation() {
  __asm__ __volatile__ (
      "   li t1, 0xFFFFFF40;"
      "   li t2, 0x1;"
      "   sw t2, 0(t1);"
      :
      :
      : "%t1", "%t2"
  );
}

