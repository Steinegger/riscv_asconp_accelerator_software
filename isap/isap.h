#ifndef ISAP_H
#define ISAP_H

#include "types.h"

// Rate in bits
#define ISAP_rH 64
#define ISAP_rB 1

// Number of rounds
#define ISAP_sH 12
#define ISAP_sB 1
#define ISAP_sE 6
#define ISAP_sK 12

// State size in bytes
#define ISAP_STATE_SZ 40

// Size of rate in bytes
#define ISAP_rH_SZ ((ISAP_rH + 7) / 8)

// Size of zero truncated IV in bytes
#define ISAP_IV_SZ 8

// Size of tag in bytes
#define ISAP_TAG_SZ 16

// Security level
#define ISAP_K 128

void isap_mac(
	const unsigned char *k,
	const unsigned char *npub,
	const unsigned char *ad, const io_length adlen,
	const unsigned char *c, const io_length clen,
	unsigned char *tag);

void isap_rk(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	unsigned char *out,
	const unsigned long outlen);

void isap_enc(
	const unsigned char *k,
	const unsigned char *npub,
	const unsigned char *m, const io_length mlen,
	unsigned char *c);

#endif