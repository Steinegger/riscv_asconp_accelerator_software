#ifndef CRYPTO_AEAD_H
#define CRYPTO_AEAD_H

//#include <stdio.h>
//#include <string.h>
#include "types.h"

int crypto_aead_encrypt(
	unsigned char *c, io_length *clen,
	const unsigned char *m, io_length mlen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
);

int crypto_aead_decrypt(
	unsigned char *m, io_length *mlen,
	unsigned char *nsec,
	const unsigned char *c, io_length clen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *npub,
	const unsigned char *k
);


#endif // CRYPTO_AEAD_H