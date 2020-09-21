#include "api.h"
#include "benchmark_config.h"
#include "permutations.h"
#include "util.h"


#if (IMPLEMENTATION_CHOICE == CRYPTO_HASH_C_SW || PERFORMANCE_TEST == 1)
int crypto_hash_c_SW(unsigned char *out, const unsigned char *in,
                io_length inlen) {
  state s;
  u64 outlen;

  // initialization
  s.x0 = IV_HASH;
  s.x1 = 0;
  s.x2 = 0;
  s.x3 = 0;
  s.x4 = 0;
  // printstate("initial value:", s);
  P12(&s);
  // printstate("initialization:", s);

  // absorb plaintext
  inlen = inlen;
  while (inlen >= RATE) {
    s.x0 ^= BYTES_TO_U64(in, 8);
    // printstate("message loop absorbed 8 byte", s);
    P12(&s);
    inlen -= RATE;
    in += RATE;
  }
  // printstate("before remaining bytes plaintext:", s);
  s.x0 ^= BYTES_TO_U64(in, inlen);
  s.x0 ^= 0x80ull << (56 - 8 * inlen);
  // printstate("absorb plaintext:", s);

  P12(&s);
  // printstate("finalization:", s);

  // set hash output
  outlen = CRYPTO_BYTES;
  while (outlen > RATE) {
    U64_TO_BYTES(out, s.x0, 8);
    P12(&s);
    outlen -= RATE;
    out += RATE;
  }
  U64_TO_BYTES(out, s.x0, 8);

  return 0;
}
#endif