#ifndef CRYPTO_AEAD_H_
#define CRYPTO_AEAD_H_

int crypto_aead_encrypt_asm_big_endian(
	unsigned char *c, io_length *clen,
	const unsigned char *m, io_length mlen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
);

int crypto_aead_encrypt_asm_little_endian(
	unsigned char *c, io_length *clen,
	const unsigned char *m, io_length mlen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
);

int crypto_aead_encrypt_c_HW(
	unsigned char *c, io_length *clen,
	const unsigned char *m, io_length mlen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
);
int crypto_aead_encrypt_c_SW(
	unsigned char *c, io_length *clen,
	const unsigned char *m, io_length mlen,
	const unsigned char *ad, io_length adlen,
	const unsigned char *nsec,
	const unsigned char *npub,
	const unsigned char *k
);
int crypto_aead_encrypt_c_bit_interleaved(
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

unsigned long long test_bytes_to_u64(unsigned char* m, unsigned long long mlen);


int crypto_hash_c_SW(
	unsigned char *out,
	const unsigned char *in,
	io_length inlen
);

int crypto_hash_c_HW(
	unsigned char *out,
	const unsigned char *in,
	io_length inlen
);

int crypto_hash_asm_big_endian(
	unsigned char *out,
	const unsigned char *in,
	io_length inlen
);

int crypto_hash_asm_little_endian(
	unsigned char *out,
	const unsigned char *in,
	io_length inlen
);

#endif // CRYPTO_AEAD_H_