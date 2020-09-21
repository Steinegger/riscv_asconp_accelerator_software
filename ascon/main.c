#include "util.h"
#include "api.h"
#include "benchmark_config.h"
#include "io_macros.h"
#include "crypto_aead.h"
#include "permutations.h"
#include "config.h"
#include "riscv_asm_macros.h"
#include <string.h>
#include "isap.h"
#include "common_macros.h"

#ifdef SPIKE
	#include <stdlib.h>
#endif


int main(int argc, char* argv[]) {

	// Init
	myprintString("---------- ASCON TEST ----------\n");
	state teststate = {0};
	printstate("teststate before", teststate);
	ROUND(0xF0, &teststate);
	printstate("teststate after", teststate);


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

	// Generate dummy data
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

#if   (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_ASM_LITTLE_ENDIAN)
	#define BENCHMARKFUNCTION crypto_aead_encrypt_asm_little_endian
#elif (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_ASM_BIG_ENDIAN)
	#define BENCHMARKFUNCTION crypto_aead_encrypt_asm_big_endian
#elif (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_C_HW)
	#define BENCHMARKFUNCTION crypto_aead_encrypt_c_HW
#elif (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_C_SW)
	#define BENCHMARKFUNCTION crypto_aead_encrypt_c_SW
#elif (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_C_BIT_INTERLEAVED)
	#define BENCHMARKFUNCTION crypto_aead_encrypt_c_bit_interleaved
#endif

	// dry-run to make sure the code is loaded
	int encrypt_ret_test_run = BENCHMARKFUNCTION(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
	// int encrypt_ret_test_run = crypto_aead_encrypt_c_HW(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
	// int encrypt_ret_test_run = crypto_aead_encrypt_c_SW(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);

	// Benchmark section
	clen = 0;
	myprintString("---------- START TIMING ----------\n");
	retinstr_before = instr_timer();
	cycle_before    = cycle_timer();
	int encrypt_ret = BENCHMARKFUNCTION(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);

	retinstr_after = instr_timer();
	cycle_after    = cycle_timer();
	myprintString("----------  END TIMING  ----------\n");


	// Print results
	int decrypt_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, adlen, nonce, key);

	printTextAndNumberWNL("Sizeof size_t           : ", sizeof(size_t));
	printTextAndNumberWNL("Sizeof io_length        : ", sizeof(io_length));
	printTextAndText     ("Plaintext               : ", (char*) msg);
	myprintString          ("Plaintext Hex           : ");
	printAsHex(msg, mlen);
	printTextAndNumberWNL("Associated Data length  : ", adlen);
	printTextAndNumberWNL("Message length          : ", mlen);
	printTextAndText     ("Associated Data         : ", (char*) ad);
	myprintString          ("AD Hex                  : ");
	printAsHex(ad, adlen);
	myprintString          ("Ciphertext              : ");
	printAsHex(ct, clen);
	printTextAndNumberWNL("Ciphert length          : ", clen);
	myprintString          ("Key                     : ");
	printAsHex(key, CRYPTO_KEYBYTES);
	myprintString          ("Nonce                   : ");
	printAsHex(nonce, CRYPTO_NPUBBYTES);
	printTextAndText     ("Decrypted               : ", (char*) msg2);
	myprintString          ("Decrypted  Hex          : ");
	printAsHex(msg2, mlen2);
	myprintString          ("Should be               : ");
	printAsHex(msg, mlen);

	PRINT_CYCLES(cycle_before, cycle_after, retinstr_before, retinstr_after)

	myprintString("\nEncryption Return Warumup: ");
	myprintUnsignedWord(encrypt_ret_test_run);
	myprintString("\nEncryption Return        : ");
	myprintUnsignedWord(encrypt_ret);
	myprintString("\nDecryption Return        : ");
	myprintUnsignedWord(decrypt_ret);

	myprintString("\n\n");

	// Repurpose for ISAP test
	// unsigned char isap_key[CRYPTO_KEYBYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	unsigned char isap_key[CRYPTO_KEYBYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	// unsigned char isap_nonce[CRYPTO_NPUBBYTES] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	unsigned char isap_nonce[CRYPTO_NPUBBYTES] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	unsigned char isap_output_ref[ISAP_STATE_SZ-CRYPTO_NPUBBYTES] = {0};
	unsigned char isap_iv[ISAP_IV_SZ] = {0x03, 0x80, 0x40, 0x01, 0x0c, 0x0c, 0x0c, 0x0c};
	unsigned char isap_output_asm_le[ISAP_STATE_SZ-CRYPTO_NPUBBYTES] = {0};
	// ISAP-test
	myprintString("---------- START ISAP SW TIMING ----------\n");
	retinstr_before = instr_timer();
	cycle_before    = cycle_timer();
	isap_rk(isap_key,
		isap_iv,
		isap_nonce,
		CRYPTO_NPUBBYTES,
		isap_output_ref,
		ISAP_STATE_SZ-CRYPTO_NPUBBYTES
	);
	retinstr_after = instr_timer();
	cycle_after    = cycle_timer();
	myprintString("----------  END ISAP SW TIMING  ----------\n");

	PRINT_CYCLES(cycle_before, cycle_after, retinstr_before, retinstr_after)
	myprintString("\nISAP key        : ");
	printAsHex(isap_key, CRYPTO_KEYBYTES);
	myprintString("\nISAP iv      : ");
	printAsHex(isap_iv, ISAP_IV_SZ);
	myprintString("\nISAP nonce      : ");
	printAsHex(isap_nonce, CRYPTO_NPUBBYTES);
	myprintString("\nISAP out        : ");
	printAsHex(isap_output_ref, ISAP_STATE_SZ-CRYPTO_NPUBBYTES);

	myprintString("\nAssembler optimized version: ");

#if (ASCON_INSTR == 1)
	#if (LITTLE_ENDIAN_CONF == 1)
		myprintString("---------- START ISAP ASM TIMING ----------\n");
		retinstr_before = instr_timer();
		cycle_before    = cycle_timer();
		isap_rk_asm_le(isap_key,
			isap_iv,
			isap_nonce,
			CRYPTO_NPUBBYTES,
			isap_output_asm_le,
			ISAP_STATE_SZ-CRYPTO_NPUBBYTES
		);
		retinstr_after = instr_timer();
		cycle_after    = cycle_timer();
		myprintString("----------  END ISAP ASM TIMING  ----------\n");

		myprintString("\nISAP key        : ");
		printAsHex(isap_key, CRYPTO_KEYBYTES);
		myprintString("\nISAP iv      : ");
		printAsHex(isap_iv, ISAP_IV_SZ);
		myprintString("\nISAP nonce      : ");
		printAsHex(isap_nonce, CRYPTO_NPUBBYTES);
		myprintString("\nISAP out        : ");
		printAsHex(isap_output_asm_le, ISAP_STATE_SZ-CRYPTO_NPUBBYTES);
		PRINT_CYCLES(cycle_before, cycle_after, retinstr_before, retinstr_after)
	#endif
#endif

	#if defined RISCY || defined IBEX
		endSimulation();
	#endif
}
