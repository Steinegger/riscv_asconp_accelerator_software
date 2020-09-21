#ifndef CONFIG_H_
#define CONFIG_H_

#define CRYPTO_AEAD_ENCRYPT_ASM_BIG_ENDIAN 0
#define CRYPTO_AEAD_ENCRYPT_ASM_LITTLE_ENDIAN 1
#define CRYPTO_AEAD_ENCRYPT_C_HW 2
#define CRYPTO_AEAD_ENCRYPT_C_SW 3
#define CRYPTO_AEAD_ENCRYPT_C_BIT_INTERLEAVED 4

#define CRYPTO_HASH_C_SW 5
#define CRYPTO_HASH_C_HW 6
#define CRYPTO_HASH_ASM_BIG_ENDIAN 7
#define CRYPTO_HASH_ASM_LITTLE_ENDIAN 8

#ifndef IMPLEMENTATION_CHOICE
	#if (ASCON_INSTR == 1)
		#if (LITTLE_ENDIAN_CONF == 1)
			#pragma message "Compiling CRYPTO_AEAD_ENCRYPT_ASM_LITTLE_ENDIAN implementation"
			#define IMPLEMENTATION_CHOICE CRYPTO_AEAD_ENCRYPT_ASM_LITTLE_ENDIAN
		#else
			#pragma message "Compiling CRYPTO_AEAD_ENCRYPT_ASM_BIG_ENDIAN implementation"
			#define IMPLEMENTATION_CHOICE CRYPTO_AEAD_ENCRYPT_ASM_BIG_ENDIAN
		#endif
	#elif (ASCON_INSTR == 2)
		#pragma message "Compiling CRYPTO_AEAD_ENCRYPT_C_HW implementation"
		#define IMPLEMENTATION_CHOICE CRYPTO_AEAD_ENCRYPT_C_HW
		#undef LITTLE_ENDIAN_CONF
		#define LITTLE_ENDIAN_CONF 0
	#else
		#pragma message "Compiling CRYPTO_AEAD_ENCRYPT_C_SW implementation"
		#define IMPLEMENTATION_CHOICE CRYPTO_AEAD_ENCRYPT_C_SW
	#endif
#endif


// The hardware can only run either in little or big endian mode
#ifdef SPIKE
	#define NUM_BENCHMARKFUNCTIONS 4
#else
	#define NUM_BENCHMARKFUNCTIONS 1
// #if (LITTLE_ENDIAN_CONF == 1)
// 	#define NUM_BENCHMARKFUNCTIONS 2
// #else
// 	#define NUM_BENCHMARKFUNCTIONS 3
// #endif
#endif

#endif  // CONFIG_H_