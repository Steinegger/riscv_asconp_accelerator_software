#ifndef ISAP_H_
#define ISAP_H_
//--------------- ISAP tests
// // Number of rounds ISAPa128
#define ISAP_sH 12
#define ISAP_sB 12
#define ISAP_sE 12
#define ISAP_sK 12

// Number of rounds //ISAPa128a
// #define ISAP_sH 12
// #define ISAP_sB 1
// #define ISAP_sE 6
// #define ISAP_sK 12

// State size in bytes
#define ISAP_STATE_SZ 40
// Size of rate in bytes
#define ISAP_rH_SZ ((ISAP_rH+7)/8)
// Size of zero truncated IV in bytes
#define ISAP_IV_SZ 8
// Size of tag in bytes
#define ISAP_TAG_SZ 16
// Security level
#define ISAP_K 128

/* ---------------------------------------------------------------- */

#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))


void load64(u64* x, u8* S);

void store64(u8* S, u64 x);

void Ascon_AddBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length);

void Ascon_ExtractBytes(const void *state, unsigned char *data, unsigned int offset, unsigned int length);

void Ascon_Permute_Nrounds(u8 *S, unsigned int rounds);

void isap_rk(const unsigned char *k, const unsigned char *iv, const unsigned char *in, const unsigned long long inlen, unsigned char *out, const unsigned long long outlen);

void isap_rk_asm_le(const unsigned char *k, const unsigned char *iv, const unsigned char *in, const unsigned long long inlen, unsigned char *out, const unsigned long long outlen);


#endif  // ISAP_H_
