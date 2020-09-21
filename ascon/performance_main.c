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
	unsigned char key[CRYPTO_KEYBYTES];
	unsigned char nonce[CRYPTO_NPUBBYTES];
	unsigned char msg[MAX_MESSAGE_LENGTH+1];
	unsigned char msg2[MAX_MESSAGE_LENGTH+1];
	unsigned char ad[MAX_ASSOCIATED_DATA_LENGTH+1];
	unsigned char ct[MAX_MESSAGE_LENGTH + CRYPTO_ABYTES];
	io_length  clen, mlen, mlen2, adlen;

	memsetzero(key, CRYPTO_KEYBYTES);
	memsetzero(nonce, CRYPTO_NPUBBYTES);
	memsetzero(msg, MAX_MESSAGE_LENGTH+1);
	memsetzero(msg2, MAX_MESSAGE_LENGTH+1);
	memsetzero(ad, MAX_ASSOCIATED_DATA_LENGTH+1);
	memsetzero(ct, MAX_MESSAGE_LENGTH + CRYPTO_ABYTES);

	#ifdef SPIKE
		srand(SEED);
	#endif

	mlen = MAX_MESSAGE_LENGTH;
	for (io_length i = 0; i < MAX_MESSAGE_LENGTH; i++) {
		// msg[i] = ((rand() % 26) + 'A');
		msg[i] = ((i % 26) + 'A');
	}
	if (MAX_MESSAGE_LENGTH > 0) { msg[MAX_MESSAGE_LENGTH-1] = '\0'; }
	msg[MAX_MESSAGE_LENGTH] = '\0';
	msg2[MAX_MESSAGE_LENGTH] = '\0';

	adlen = MAX_ASSOCIATED_DATA_LENGTH;
	for (io_length i = 0; i < MAX_ASSOCIATED_DATA_LENGTH; i++) {
		// ad[i] = ((rand() % 26) + 'A');
		ad[i] = ((i % 26) + 'A');
	}
	if (MAX_ASSOCIATED_DATA_LENGTH > 0) { ad[MAX_ASSOCIATED_DATA_LENGTH-1] = '\0'; }
	ad[MAX_ASSOCIATED_DATA_LENGTH] = '\0';

	for (io_length i = 0; i < CRYPTO_NPUBBYTES; i++) {
		// nonce[i] = (unsigned char) rand();
		// key[i]   = (unsigned char) rand();
		nonce[i] = (unsigned char) i;
		key[i]   = (unsigned char) i;
	}

	perfcounter cycle_before = -1;
	perfcounter cycle_after  = -1;
	perfcounter retinstr_before = -1;
	perfcounter retinstr_after  = -1;


// activate performance counters for RISCY
#ifdef RISCY
	__asm__ __volatile__ ("csrrwi x0, 0x7A1, 0x3;");
	__asm__ __volatile__ ("csrrwi x0, 0x7E0, 0x1F;");
#endif

	#define BENCHMARKFUNCTION crypto_aead_encrypt_asm_little_endian
	// #define BENCHMARKFUNCTION crypto_aead_encrypt_asm_big_endian
	// #define BENCHMARKFUNCTION crypto_aead_encrypt_c_HW
	// #define BENCHMARKFUNCTION crypto_aead_encrypt_c_SW
	// #define BENCHMARKFUNCTION crypto_aead_encrypt_c_bit_interleaved

#ifdef SPIKE
		int (*benchmark_func_ptr[NUM_BENCHMARKFUNCTIONS])(unsigned char*, io_length*,
		                        const unsigned char* , io_length ,
		                        const unsigned char* , io_length ,
		                        const unsigned char* , const unsigned char* ,
		                        const unsigned char*) = {
					crypto_aead_encrypt_c_SW,
	#if (LITTLE_ENDIAN_CONF == 1 || defined SPIKE)
					crypto_aead_encrypt_asm_little_endian,
	#endif
	#if (LITTLE_ENDIAN_CONF == 0 || defined SPIKE)
					crypto_aead_encrypt_asm_big_endian,
					crypto_aead_encrypt_c_HW,
	#endif
					// ,				crypto_aead_encrypt_c_bit_interleaved
		};

		char benchmark_func_names[NUM_BENCHMARKFUNCTIONS][40] =
		{
			"crypto_aead_encrypt_c_SW",
	#if (LITTLE_ENDIAN_CONF == 1 || defined SPIKE)
			"crypto_aead_encrypt_asm_little_endian",
	#endif
	#if (LITTLE_ENDIAN_CONF == 0 || defined SPIKE)
			"crypto_aead_encrypt_asm_big_endian",
			"crypto_aead_encrypt_c_HW",
	#endif
			// ,	"crypto_aead_encrypt_c_bit_interleaved"
		};

	#ifdef SPIKE
		FILE * fp;
		char benchmark_func_names_files[NUM_BENCHMARKFUNCTIONS][60] =
		{
			"crypto_aead_encrypt_c_SW.csv",
			"crypto_aead_encrypt_asm_little_endian.csv",
			"crypto_aead_encrypt_asm_big_endian.csv",
			"crypto_aead_encrypt_c_HW.csv"
			// , "crypto_aead_encrypt_c_bit_interleaved.csv"
		};
	#endif
#else
	int (*benchmark_func_ptr[NUM_BENCHMARKFUNCTIONS])(unsigned char*, io_length*,
													const unsigned char* , io_length ,
													const unsigned char* , io_length ,
													const unsigned char* , const unsigned char* ,
													const unsigned char*) = {
		#if (ASCON_INSTR == 1)
			#if (LITTLE_ENDIAN_CONF == 1)
				crypto_aead_encrypt_asm_little_endian
			#else
				crypto_aead_encrypt_asm_big_endian
			#endif
		#elif (ASCON_INSTR == 2)
			crypto_aead_encrypt_c_HW
		#else
			crypto_aead_encrypt_c_SW
		#endif
	};

	char benchmark_func_names[NUM_BENCHMARKFUNCTIONS][40] = {
		#if (ASCON_INSTR == 1)
			#if (LITTLE_ENDIAN_CONF == 1)
				"crypto_aead_encrypt_asm_little_endian"
			#else
				"crypto_aead_encrypt_asm_big_endian"
			#endif
		#elif (ASCON_INSTR == 2)
			"crypto_aead_encrypt_c_HW"
		#else
			"crypto_aead_encrypt_c_SW"
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
		fprintf(fp, "function;ad_bytes;m_bytes;retinstr_before;retinstr_after;retinstr;cycle_before;cycle_after;cycle\n");
#else
		myprintString("---BEGINCSV---\n");
		myprintString("function;ad_bytes;m_bytes;retinstr_before;retinstr_after;retinstr;cycle_before;cycle_after;cycle\n");
#endif

		// int encrypt_ret_test_run = BENCHMARKFUNCTION(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
		(*benchmark_func_ptr[i])(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
		for (io_length ad_bytes = MIN_ASSOCIATED_DATA_LENGTH; ad_bytes < MAX_ASSOCIATED_DATA_LENGTH; ad_bytes += STEP_ASSOCIATED_DATA) {
			for (io_length m_bytes = MIN_MESSAGE_LENGTH; m_bytes < MAX_MESSAGE_LENGTH; m_bytes += STEP_MESSAGE) {
				clen = 0;
				retinstr_before = instr_timer();
				cycle_before    = cycle_timer();

				(*benchmark_func_ptr[i])(ct, &clen, msg, m_bytes, ad, ad_bytes, NULL, nonce, key);

				retinstr_after = instr_timer();
				cycle_after    = cycle_timer();
#ifdef SPIKE
				fprintf(fp, "%s;%u;%u;%llu;%llu;%llu;%llu;%llu;%llu\n",
						// VALUE(BENCHMARKFUNCTION),
						benchmark_func_names[i],
						ad_bytes,
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
				myprintUnsignedWordPlain(ad_bytes);
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

				int decrypt_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, ad_bytes, nonce, key);
				if (decrypt_ret) {
					myprintString("ERROR WHEN DECRYPTING!\n");
					myprintString("Plaintext Hex  : ");
					printAsHex(msg, mlen);
					printTextAndNumberWNL("Message length : %lu\n", m_bytes);
					printTextAndNumberWNL("Associated Data length : %lu\n", ad_bytes);
					myprintString("AD Hex         : ");
					printAsHex(ad, adlen);
					myprintString("Key            : ");
					printAsHex(key, CRYPTO_KEYBYTES);
					myprintString("Nonce          : ");
					printAsHex(nonce, CRYPTO_NPUBBYTES);

					#ifdef RISCY
						endSimulation();
					#endif
					return decrypt_ret;
				}
				loop_counter++;

				if ((loop_counter % ALIVE_MESSAGE_INTERVAL) == 0) {
					myprintString("RUNNING");
					myprintUnsignedWord((i+1));
					myprintString("/");
					myprintUnsignedWord(NUM_BENCHMARKFUNCTIONS);
					myprintString(": ad_bytes: ");
					myprintUnsignedWord(ad_bytes);
					myprintString(": m_bytes: ");
					myprintUnsignedWord(m_bytes);
					// printf("RUNNING %u/%u: ad_bytes: %8u        m_bytes: %8u    %s \n", (i+1), NUM_BENCHMARKFUNCTIONS, ad_bytes, m_bytes, benchmark_func_names[i]);
				}

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
