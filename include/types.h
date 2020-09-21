#ifndef TYPES_H_
#define TYPES_H_

typedef unsigned char u8;
typedef unsigned long u32;
typedef unsigned long long u64;

#ifdef REFERENCE_LENGTH
	typedef unsigned long long io_length;
#else
	#ifdef SPIKE
		#include <stdio.h>
		typedef size_t io_length;
	#else
		typedef unsigned int io_length;
		typedef unsigned int size_t;
	#endif
#endif


#ifdef SPIKE
typedef uint64_t perfcounter;
#define PERFORMANCE_FS "%llu"
#elif defined RISCY
typedef u32 perfcounter;
#define PERFORMANCE_FS "%u"

#else 
	#error "Neither RISCY nor SPIKE defined"
#endif

#endif // TYPES_H_