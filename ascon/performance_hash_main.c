#ifdef SPIKE
	#include <stdio.h>
	#include <stdlib.h>
	#include <errno.h>
#endif

#include <string.h>
#include "benchmark_config.h"
#include "api.h"
#include "util.h"
#include "io_macros.h"
#include "crypto_aead.h"
#include "config.h"
#include "riscv_asm_macros.h"
#include "common_macros.h"

int main(int argc, char* argv[]) {
	unsigned char msg[MAX_MESSAGE_LENGTH+1];
	unsigned char hash[CRYPTO_BYTES];
	unsigned char hash_ref[CRYPTO_BYTES];
	io_length  mlen;

	memsetzero(msg, MAX_MESSAGE_LENGTH);
	memsetzero(hash, CRYPTO_BYTES);
	memsetzero(hash_ref, CRYPTO_BYTES);

	#ifdef SPIKE
		srand(SEED);
	#endif

	mlen = MAX_MESSAGE_LENGTH;
	for (io_length i = 0; i < MAX_MESSAGE_LENGTH; i++) {
		msg[i] = ((i % 26) + 'A');
	}
	if (MAX_MESSAGE_LENGTH > 0) { msg[MAX_MESSAGE_LENGTH-1] = '\0'; }
	msg[MAX_MESSAGE_LENGTH] = '\0';

	perfcounter cycle_before = -1;
	perfcounter cycle_after  = -1;
	perfcounter retinstr_before = -1;
	perfcounter retinstr_after  = -1;

	// activate performance counters for RISCY
	#ifdef RISCY
		__asm__ __volatile__ ("csrrwi x0, 0x7A1, 0x3;");
		__asm__ __volatile__ ("csrrwi x0, 0x7E0, 0x1F;");
	#endif

#ifdef SPIKE
	int (*benchmark_func_ptr[NUM_BENCHMARKFUNCTIONS])(unsigned char*, const unsigned char*,
	                io_length) = {
			crypto_hash_c_SW,
#if (LITTLE_ENDIAN_CONF == 1 || defined SPIKE)
			crypto_hash_asm_little_endian,
#endif
#if (LITTLE_ENDIAN_CONF == 0 || defined SPIKE)
			crypto_hash_asm_big_endian,
			crypto_hash_c_HW
#endif
	};

	char benchmark_func_names[NUM_BENCHMARKFUNCTIONS][40] =
	{
		"crypto_hash_c_SW",
#if (LITTLE_ENDIAN_CONF == 1 || defined SPIKE)
		"crypto_hash_asm_little_endian",
#endif
#if (LITTLE_ENDIAN_CONF == 0 || defined SPIKE)
		"crypto_hash_asm_big_endian",
		"crypto_hash_c_HW"
#endif
	};

#ifdef SPIKE
	FILE * fp;
	char benchmark_func_names_files[NUM_BENCHMARKFUNCTIONS][60] =
	{
		"crypto_hash_c_SW.csv"
		"crypto_hash_asm_little_endian.csv",
		"crypto_hash_asm_big_endian.csv",
		"crypto_hash_c_HW.csv",
	};
#endif
#else
	int (*benchmark_func_ptr[NUM_BENCHMARKFUNCTIONS])(unsigned char*, const unsigned char*,
								io_length) = {
	#if (ASCON_INSTR == 1)
		#if (LITTLE_ENDIAN_CONF == 1)
			crypto_hash_asm_little_endian
		#else
			crypto_hash_asm_big_endian
		#endif
	#elif (ASCON_INSTR == 2)
		crypto_hash_c_HW
	#else
		crypto_hash_c_SW
	#endif
};

	char benchmark_func_names[NUM_BENCHMARKFUNCTIONS][40] = {
		#if (ASCON_INSTR == 1)
			#if (LITTLE_ENDIAN_CONF == 1)
				"crypto_hash_asm_little_endian"
			#else
				"crypto_hash_asm_big_endian"
			#endif
		#elif (ASCON_INSTR == 2)
			"crypto_hash_c_HW"
		#else
			"crypto_hash_c_SW"
		#endif
	};

#endif

unsigned long long int loop_counter = 0;


	for (unsigned char i = 0; i < NUM_BENCHMARKFUNCTIONS; i++) {
#ifdef SPIKE
				fp = fopen(benchmark_func_names_files[i], "w");

				if (fp == NULL) {
					myprintString("ERROR Opening file. Exit\n Error ");
					myprintUnsignedWord(errno);
					return -1;
				}
#endif

#ifdef SPIKE
		fprintf(fp, "function;m_bytes;retinstr_before;retinstr_after;retinstr;cycle_before;cycle_after;cycle\n");
#else
		myprintString("---BEGINCSV---\n");
		myprintString("function;m_bytes;retinstr_before;retinstr_after;retinstr;cycle_before;cycle_after;cycle\n");
#endif
		(*benchmark_func_ptr[i])(hash, msg, MAX_MESSAGE_LENGTH);
		for (io_length m_bytes = MIN_MESSAGE_LENGTH; m_bytes < MAX_MESSAGE_LENGTH; m_bytes += STEP_MESSAGE) {
			retinstr_before = instr_timer();
			cycle_before    = cycle_timer();
			
			(*benchmark_func_ptr[i])(hash, msg, m_bytes);

			retinstr_after = instr_timer();
			cycle_after    = cycle_timer();
#ifdef SPIKE
			fprintf(fp, "%s;%u;%llu;%llu;%llu;%llu;%llu;%llu\n",
					// VALUE(BENCHMARKFUNCTION),
					benchmark_func_names[i],
					m_bytes,
					retinstr_before,
					retinstr_after,
					(retinstr_after - retinstr_before),
					cycle_before,
					cycle_after,
					(cycle_after - cycle_before)
				);
#else
			myprintString(benchmark_func_names[i]);
			myprintString(";");
			myprintUnsignedWordPlain(m_bytes);
			myprintString(";");
			myprintUnsignedWordPlain(retinstr_before);
			myprintString(";");
			myprintUnsignedWordPlain(retinstr_after);
			myprintString(";");
			myprintUnsignedWordPlain((retinstr_after - retinstr_before));
			myprintString(";");
			myprintUnsignedWordPlain(cycle_before);
			myprintString(";");
			myprintUnsignedWordPlain(cycle_after);
			myprintString(";");
			myprintUnsignedWordPlain((cycle_after - cycle_before));
			myprintString("\n");
#endif

			crypto_hash_c_SW(hash_ref, msg, m_bytes);

			if (memcmp(hash, hash_ref, CRYPTO_BYTES)) {
				myprintString("ERROR WHEN HASHING!\n");
				myprintString("Plaintext Hex  : ");
				printAsHex(msg, mlen);
				printTextAndNumberWNL("Message length : %lu\n", m_bytes);
				myprintString("Hash Hex       : ");
				printAsHex(hash, CRYPTO_BYTES);
				myprintString("Ref Hash Hex   : ");
				printAsHex(hash_ref, CRYPTO_BYTES);

#ifdef RISCY
				endSimulation();
#endif
				return -1;
			}
			loop_counter++;

			if ((loop_counter % ALIVE_MESSAGE_INTERVAL) == 0) {
				myprintString("RUNNING");
				myprintUnsignedWordPlain((i+1));
				myprintString("/");
				myprintUnsignedWord(NUM_BENCHMARKFUNCTIONS);
				myprintString(": m_bytes: ");
				myprintUnsignedWord(m_bytes);
				myprintString(benchmark_func_names[i]);
			}

		}
#ifdef SPIKE
		fclose(fp);
#endif
	myprintString("---ENDCSV---\n");
	}
#ifdef RISCY
	endSimulation();
#endif
}
