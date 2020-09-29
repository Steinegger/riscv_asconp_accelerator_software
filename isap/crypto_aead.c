//#include <stdio.h>
//#include <string.h>
#include "api.h"
#include "types.h"
#include "isap.h"
#include "crypto_aead.h"
#include "util.h"
#include "io_macros.h"

int crypto_aead_encrypt(
	unsigned char *c, io_length *clen,
	const unsigned char *m, io_length mlen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
){
    (void)nsec;  
    
	// Ciphertext length is mlen + tag length
	*clen = mlen+ISAP_TAG_SZ;

	// Encrypt plaintext
	if (mlen > 0) {
		isap_enc(k,npub,m,mlen,c);
	}

	// Generate tag
	unsigned char *tag = c+mlen;
	isap_mac(k,npub,ad,adlen,c,mlen,tag);
	return 0;
}

int crypto_aead_decrypt(
	unsigned char *m, io_length *mlen,
	unsigned char *nsec,
	const unsigned char *c, io_length clen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *npub,
	const unsigned char *k
){
    (void)nsec;

	// Plaintext length is clen - tag length
	*mlen = clen-ISAP_TAG_SZ;

	// Generate tag
	unsigned char tag[ISAP_TAG_SZ];
	isap_mac(k,npub,ad,adlen,c,*mlen,tag);

	// Compare tag
	unsigned long eq_cnt = 0;
	for(unsigned int i = 0; i < ISAP_TAG_SZ; i++) {
		eq_cnt += (tag[i] == c[(*mlen)+i]);
	}

	myprintString("============\n");
	myprintUnsignedWord(eq_cnt);
	myprintString("============\n");
	// Perform decryption if tag is correct
	if(eq_cnt == (unsigned long)ISAP_TAG_SZ){
		myprintString("---------- TAG compared correctly\n");
		if (*mlen > 0) {
			isap_enc(k,npub,c,*mlen,m);
		}

		myprintString("---------- DECRYPTED\n");
		return 0;
	} else {
		return -1;
	}
}
