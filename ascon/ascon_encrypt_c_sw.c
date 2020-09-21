#include "api.h"
#include "benchmark_config.h"
#include "permutations.h"
#include "util.h"

#if (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_C_SW || PERFORMANCE_TEST == 1)
int crypto_aead_encrypt_c_SW(unsigned char* c, io_length* clen,
                        const unsigned char* m, io_length mlen,
                        const unsigned char* ad, io_length adlen,
                        const unsigned char* nsec, const unsigned char* npub,
                        const unsigned char* k) {
  const u64 K0 = BYTES_TO_U64(k, 8);
  const u64 K1 = BYTES_TO_U64(k + 8, 8);
  const u64 N0 = BYTES_TO_U64(npub, 8);
  const u64 N1 = BYTES_TO_U64(npub + 8, 8);
  state s;
  (void)nsec;

  // set ciphertext size
  *clen = mlen + CRYPTO_ABYTES;

  // initialization
  s.x0 = IV;
  s.x1 = K0;
  s.x2 = K1;
  s.x3 = N0;
  s.x4 = N1;
  // printstate("initial value:", s);
  P12(&s);
  s.x3 ^= K0;
  s.x4 ^= K1;
  // printstate("initialization:", s);
  // process associated data
  if (adlen) {
    while (adlen >= RATE) {
      s.x0 ^= BYTES_TO_U64(ad, 8);
      // printstate("before ad blocks permut:", s);
      P6(&s);
      adlen -= RATE;
      ad += RATE;
    }
    // printstate("before remaining ad blocks:", s);
    s.x0 ^= BYTES_TO_U64(ad, adlen);
    s.x0 ^= 0x80ull << (56 - 8 * adlen);
    P6(&s);
  }
  s.x4 ^= 1;
  // printstate("process associated data:", s);

  // process plaintext
  while (mlen >= RATE) {
    s.x0 ^= BYTES_TO_U64(m, 8);
    U64_TO_BYTES(c, s.x0, 8);
    P6(&s);
    mlen -= RATE;
    m += RATE;
    c += RATE;
  }


  // printstate("state  plaintext:", s);
  s.x0 ^= BYTES_TO_U64(m, mlen);
  s.x0 ^= 0x80ull << (56 - 8 * mlen);
  U64_TO_BYTES(c, s.x0, mlen);
  c += mlen;
  // printstate("process plaintext:", s);

  // finalization
  s.x1 ^= K0;
  s.x2 ^= K1;
  P12(&s);
  s.x3 ^= K0;
  s.x4 ^= K1;
  // printstate("finalization:", s);

  // set tag
  U64_TO_BYTES(c, s.x3, 8);
  U64_TO_BYTES(c + 8, s.x4, 8);

  // print_implementation();
  return 0;
}
#endif