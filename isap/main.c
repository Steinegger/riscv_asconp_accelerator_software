#include "util.h"
#include "api.h"
#include "types.h"
#include "benchmark_config.h"
#include "crypto_aead.h"
#include "asconp.h"
#include "isap.h"
#include "io_macros.h"
#include "common_macros.h"
#include <string.h>

int main(int argc, char* argv[]) {
	myprintString("---------- ISAP SW ONLY TEST ----------\n");


	unsigned char key[CRYPTO_KEYBYTES];
	unsigned char nonce[CRYPTO_NPUBBYTES];
	unsigned char msg[MAX_MESSAGE_LENGTH+1];
	unsigned char msg2[MAX_MESSAGE_LENGTH+1];
	unsigned char ad[MAX_ASSOCIATED_DATA_LENGTH+1];
	unsigned char ct[MAX_MESSAGE_LENGTH + CRYPTO_ABYTES];
	io_length  clen, mlen, mlen2, adlen;

	memsetzero(key, CRYPTO_KEYBYTES);
	memsetzero(nonce, CRYPTO_NPUBBYTES);
	memsetzero(msg, MAX_MESSAGE_LENGTH);
	memsetzero(msg2, MAX_MESSAGE_LENGTH);
	memsetzero(ad, MAX_ASSOCIATED_DATA_LENGTH);
	memsetzero(ct, MAX_MESSAGE_LENGTH + CRYPTO_ABYTES);

	#ifdef SPIKE
		srand(SEED);
	#endif

	mlen = MAX_MESSAGE_LENGTH;
	for (int i = 0; i < MAX_MESSAGE_LENGTH-1; i++) {
		// msg[i] = ((rand() % 26) + 'A');
		msg[i] = ((i % 26) + 'A');
	}
	if (MAX_MESSAGE_LENGTH > 0) { msg[MAX_MESSAGE_LENGTH-1] = '\0'; }
	msg[MAX_MESSAGE_LENGTH] = '\0';
	msg2[MAX_MESSAGE_LENGTH] = '\0';

	adlen = MAX_ASSOCIATED_DATA_LENGTH;
	for (int i = 0; i < MAX_ASSOCIATED_DATA_LENGTH-1; i++) {
		// ad[i] = ((rand() % 26) + 'A');
		ad[i] = ((i % 26) + 'A');
	}
	if (MAX_ASSOCIATED_DATA_LENGTH > 0) { ad[MAX_ASSOCIATED_DATA_LENGTH-1] = '\0'; }
	ad[MAX_ASSOCIATED_DATA_LENGTH] = '\0';

	for (int i = 0; i < CRYPTO_NPUBBYTES; i++) {
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

#define BENCHMARKFUNCTION crypto_aead_encrypt

	int encrypt_ret_test_run = BENCHMARKFUNCTION(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
	clen = 0;


	myprintString("---------- START TIMING ----------\n");
	retinstr_before = instr_timer();
	cycle_before = cycle_timer();

	int encrypt_ret = BENCHMARKFUNCTION(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);

	retinstr_after = instr_timer();
	cycle_after = cycle_timer();
	myprintString("----------  END TIMING  ----------\n");

	int decrypt_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, adlen, nonce, key);

	printTextAndNumberWNL("Sizeof size_t           : ", sizeof(size_t));
	printTextAndNumberWNL("Sizeof io_length        : ", sizeof(io_length));
	printTextAndText     ("Plaintext               : ", (char*) msg);
	myprintString        ("Plaintext Hex           : ");
	printAsHex           (msg, mlen);
	printTextAndNumberWNL("Associated Data length  : ", adlen);
	printTextAndNumberWNL("Message length          : ", mlen);
	printTextAndText     ("Associated Data         : ", (char*) ad);
	myprintString        ("AD Hex                  : ");
	printAsHex           (ad, adlen);
	myprintString        ("Ciphertext              : ");
	printAsHex           (ct, clen );
	printTextAndNumberWNL("Ciphert length          : ", clen);
	myprintString        ("Key  : ");
	printAsHex           (key, CRYPTO_KEYBYTES);
	myprintString        ("Nonce: ");
	printAsHex           (nonce, CRYPTO_NPUBBYTES);
	printTextAndText     ("Decrypted               : ", (char*) msg2);
	myprintString        ("Decrypted  Hex          : ");
	printAsHex           (msg2, mlen2);
	myprintString        ("Should be               : ");
	printAsHex           (msg, mlen);
	
	PRINT_CYCLES(cycle_before, cycle_after, retinstr_before, retinstr_after)

	myprintString("\nEncryption Return        : ");
	myprintUnsignedWord(encrypt_ret);
	myprintString("\nDecryption Return        : ");
	myprintUnsignedWord(decrypt_ret);

	myprintString("\n\n");

	#if defined RISCY || defined IBEX
		endSimulation();
	#endif

}
