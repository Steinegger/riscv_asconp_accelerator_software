#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

#include "types.h"
#include "util.h"
#include "riscv_asm_macros.h"

#if (MULTIROUND_CONF <= 6)
#define AD_ENC_LOOP_LEN_LE (5 + CEIL_DIV(6, MULTIROUND_CONF))
#define AD_ENC_LOOP_LEN_BE (23 + CEIL_DIV(6, MULTIROUND_CONF))

#define M_ENC_LOOP_LEN_LE (6 + CEIL_DIV(6, MULTIROUND_CONF))
#define M_ENC_LOOP_LEN_BE (36 + CEIL_DIV(6, MULTIROUND_CONF))
#define M_HASH_LOOP_LEN_LE (5 + CEIL_DIV(12, MULTIROUND_CONF))
#define M_HASH_LOOP_LEN_BE (23 + CEIL_DIV(12, MULTIROUND_CONF))
#else
#define AD_ENC_LOOP_LEN_LE (5 + 1)
#define AD_ENC_LOOP_LEN_BE (23 + 1)

#define M_ENC_LOOP_LEN_LE (6 + 1)
#define M_ENC_LOOP_LEN_BE (36 + 1)
#define M_HASH_LOOP_LEN_LE (5 + 2)
#define M_HASH_LOOP_LEN_BE (23 + 2)

#endif

#if (MULTIROUND_CONF == 8)
  #define P12_LE \
    ASCON_LExN(0xf0, 8) \
    ASCON_LExN(0x78, 4)

  #define P12_BE \
    ASCON_BExN(0x0f0, 8) \
    ASCON_BExN(0x078, 4)

  #define P6_BE \
    ASCON_BExN(0x096, 6)

  #define P6_LE \
    ASCON_LExN(0x96, 6)
#elif (MULTIROUND_CONF == 7)
  #define P12_LE \
    ASCON_LExN(0x0f0, 7) \
    ASCON_LExN(0x087, 5)

  #define P12_BE \
    ASCON_BExN(0x0f0, 7) \
    ASCON_BExN(0x087, 5)

  #define P6_BE \
    ASCON_BExN(0x096, 6)

  #define P6_LE \
    ASCON_LExN(0x96, 6)
#elif (MULTIROUND_CONF == 6)
  #define P12_LE \
    ASCON_LExN(0x0f0, 6) \
    ASCON_LExN(0x096, 6)

  #define P12_BE \
    ASCON_BExN(0x0f0, 6) \
    ASCON_BExN(0x096, 6)

  #define P6_BE \
    ASCON_BExN(0x096, 6)

  #define P6_LE \
    ASCON_LExN(0x96, 6)
#elif (MULTIROUND_CONF == 5)
  #define P12_LE \
    ASCON_LExN(0x0f0, 5) \
    ASCON_LExN(0x0a5, 5) \
    ASCON_LExN(0x05a, 2)

  #define P12_BE \
    ASCON_BExN(0x0f0, 5) \
    ASCON_LExN(0x0a5, 5) \
    ASCON_LExN(0x05a, 2)

  #define P6_BE \
    ASCON_BExN(0x096, 5) \
    ASCON_BExN(0x04b, 1)

  #define P6_LE \
    ASCON_LExN(0x096, 5) \
    ASCON_LExN(0x04b, 1)
#elif (MULTIROUND_CONF == 4)
  #define P12_LE \
    ASCON_LExN(0x0f0, 4) \
    ASCON_LExN(0x0b4, 4) \
    ASCON_LExN(0x078, 4)

  #define P12_BE \
    ASCON_BExN(0x0f0, 4) \
    ASCON_LExN(0x0b4, 4) \
    ASCON_LExN(0x078, 4)

  #define P6_BE \
    ASCON_BExN(0x096, 4) \
    ASCON_BExN(0x05a, 2)

  #define P6_LE \
    ASCON_LExN(0x096, 4) \
    ASCON_LExN(0x05a, 2)
#elif (MULTIROUND_CONF == 3)
  #define P12_LE \
    ASCON_LExN(0x0f0, 3) \
    ASCON_LExN(0x0c3, 3) \
    ASCON_LExN(0x096, 3) \
    ASCON_LExN(0x069, 3)

  #define P12_BE \
    ASCON_BExN(0x0f0, 3) \
    ASCON_LExN(0x0c3, 3) \
    ASCON_LExN(0x096, 3) \
    ASCON_LExN(0x069, 3)

  #define P6_BE \
    ASCON_BExN(0x096, 3) \
    ASCON_BExN(0x069, 3)

  #define P6_LE \
    ASCON_LExN(0x096, 3) \
    ASCON_LExN(0x069, 3)
#elif (MULTIROUND_CONF == 2)
  #define P12_LE \
    ASCON_LExN(0x0f0, 2) \
    ASCON_LExN(0x0d2, 2) \
    ASCON_LExN(0x0b4, 2) \
    ASCON_LExN(0x096, 2) \
    ASCON_LExN(0x078, 2) \
    ASCON_LExN(0x05a, 2)

  #define P12_BE \
    ASCON_BExN(0x0f0, 2) \
    ASCON_LExN(0x0d2, 2) \
    ASCON_LExN(0x0b4, 2) \
    ASCON_LExN(0x096, 2) \
    ASCON_LExN(0x078, 2) \
    ASCON_LExN(0x05a, 2)

  #define P6_BE \
    ASCON_BExN(0x096, 2) \
    ASCON_BExN(0x078, 2) \
    ASCON_BExN(0x05a, 2)

  #define P6_LE \
    ASCON_LExN(0x096, 2) \
    ASCON_LExN(0x078, 2) \
    ASCON_LExN(0x05a, 2)
#elif (MULTIROUND_CONF == 1)
  #define P12_LE \
    ASCON_LE(0x0f0) \
    ASCON_LE(0x0e1) \
    ASCON_LE(0x0d2) \
    ASCON_LE(0x0c3) \
    ASCON_LE(0x0b4) \
    ASCON_LE(0x0a5) \
    ASCON_LE(0x096) \
    ASCON_LE(0x087) \
    ASCON_LE(0x078) \
    ASCON_LE(0x069) \
    ASCON_LE(0x05a) \
    ASCON_LE(0x04b)

  #define P12_BE \
    ASCON_BE(0x0f0) \
    ASCON_BE(0x0e1) \
    ASCON_BE(0x0d2) \
    ASCON_BE(0x0c3) \
    ASCON_BE(0x0b4) \
    ASCON_BE(0x0a5) \
    ASCON_BE(0x096) \
    ASCON_BE(0x087) \
    ASCON_BE(0x078) \
    ASCON_BE(0x069) \
    ASCON_BE(0x05a) \
    ASCON_BE(0x04b)

  #define P6_BE \
    ASCON_BE(0x096) \
    ASCON_BE(0x087) \
    ASCON_BE(0x078) \
    ASCON_BE(0x069) \
    ASCON_BE(0x05a) \
    ASCON_BE(0x04b)

  #define P6_LE \
    ASCON_LE(0x096) \
    ASCON_LE(0x087) \
    ASCON_LE(0x078) \
    ASCON_LE(0x069) \
    ASCON_LE(0x05a) \
    ASCON_LE(0x04b)
#else
#error "Invalid MULTIROUND_CONF setting config."

#define P12_LE "nop;"

#define P12_BE "nop;"

#define P6_BE "nop;"

#define P6_LE "nop;"
#endif

typedef struct __attribute__((__packed__)) {
  u64 x0, x1, x2, x3, x4;
} state;

typedef struct __attribute__((__packed__)) {
  u32 x0_0, x0_1, x1_0, x1_1, x2_0, x2_1, x3_0, x3_1, x4_0, x4_1;
} state_u32;

typedef union {
  state     s64;
  state_u32 s32;
} state_union;

typedef struct {
  u32 e;
  u32 o;
} u32_2;

static inline void printstate(const char* text, const state s) {
#ifdef DEBUG
  #ifdef SPIKE
    printf("%s\n", text);
    printf("  x0=%016llx\n", s.x0);
    printf("  x1=%016llx\n", s.x1);
    printf("  x2=%016llx\n", s.x2);
    printf("  x3=%016llx\n", s.x3);
    printf("  x4=%016llx\n", s.x4);
  #endif
#else
  // disable warning about unused parameters
  (void)text;
  (void)s;
#endif
}

static inline u64 BYTES_TO_U64(const u8* bytes, int n) {
  int i;
  u64 x = 0;
  for (i = 0; i < n; i++) x |= ((u64)bytes[i]) << (56 - 8 * i);
  return x;
}

static inline void U64_TO_BYTES(u8* bytes, const u64 x, int n) {
  int i;
  for (i = 0; i < n; i++) bytes[i] = (u8)(x >> (56 - 8 * i));
}

static inline u64 BYTE_MASK(int n) {
  int i;
  u64 x = 0;
  for (i = 0; i < n; i++) x |= 0xffull << (56 - 8 * i);
  return x;
}

static inline void print_implementation() {
  #ifdef DEBUG
    #ifdef SPIKE
      printf("Using SOFTWARE implementation\n");
    #endif
  #endif
}

// #if (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_C_SW || IMPLEMENTATION_CHOICE == CRYPTO_HASH_C_SW)
static inline u64 ROTR64(u64 x, int n) { return (x << (64 - n)) | (x >> n); }

static inline void ROUND(u8 C, state* p) {
  state s = *p;
  state t;
  // addition of round constant
  s.x2 ^= C;
  // substitution layer
  s.x0 ^= s.x4;
  s.x4 ^= s.x3;
  s.x2 ^= s.x1;
  // start of keccak s-box
  t.x0 = ~s.x0;
  t.x1 = ~s.x1;
  t.x2 = ~s.x2;
  t.x3 = ~s.x3;
  t.x4 = ~s.x4;
  t.x0 &= s.x1;
  t.x1 &= s.x2;
  t.x2 &= s.x3;
  t.x3 &= s.x4;
  t.x4 &= s.x0;
  s.x0 ^= t.x1;
  s.x1 ^= t.x2;
  s.x2 ^= t.x3;
  s.x3 ^= t.x4;
  s.x4 ^= t.x0;
  // end of keccak s-box
  s.x1 ^= s.x0;
  s.x0 ^= s.x4;
  s.x3 ^= s.x2;
  s.x2 = ~s.x2;
  // linear diffusion layer
  s.x0 ^= ROTR64(s.x0, 19) ^ ROTR64(s.x0, 28);
  s.x1 ^= ROTR64(s.x1, 61) ^ ROTR64(s.x1, 39);
  s.x2 ^= ROTR64(s.x2,  1) ^ ROTR64(s.x2,  6);
  s.x3 ^= ROTR64(s.x3, 10) ^ ROTR64(s.x3, 17);
  s.x4 ^= ROTR64(s.x4,  7) ^ ROTR64(s.x4, 41);
  *p = s;
}

static inline void P12(state* s) {
  ROUND(0xf0, s);
  ROUND(0xe1, s);
  ROUND(0xd2, s);
  ROUND(0xc3, s);
  ROUND(0xb4, s);
  ROUND(0xa5, s);
  ROUND(0x96, s);
  ROUND(0x87, s);
  ROUND(0x78, s);
  ROUND(0x69, s);
  ROUND(0x5a, s);
  ROUND(0x4b, s);
}

static inline void P8(state* s) {
  ROUND(0xb4, s);
  ROUND(0xa5, s);
  ROUND(0x96, s);
  ROUND(0x87, s);
  ROUND(0x78, s);
  ROUND(0x69, s);
  ROUND(0x5a, s);
  ROUND(0x4b, s);
}

static inline void P6(state* s) {
  ROUND(0x96, s);
  ROUND(0x87, s);
  ROUND(0x78, s);
  ROUND(0x69, s);
  ROUND(0x5a, s);
  ROUND(0x4b, s);
}
// #endif

// #if (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_C_HW || IMPLEMENTATION_CHOICE == CRYPTO_HASH_C_HW)
static inline void P12_HW(state* s) {
  state_union* su = (state_union*) s;
  __asm__ __volatile__ ("lw a2, %10;"
                        "lw a3, %11;"
                        "lw a4, %12;"
                        "lw a5, %13;"
                        "lw a6, %14;"
                        "lw a7, %15;"
                        "lw t3, %16;"
                        "lw t4, %17;"
                        "lw t5, %18;"
                        "lw t6, %19;"
                        "nop;"
                        "nop;"
                        P12_BE
                        "sw a2, %0;"
                        "sw a3, %1;"
                        "sw a4, %2;"
                        "sw a5, %3;"
                        "sw a6, %4;"
                        "sw a7, %5;"
                        "sw t3, %6;"
                        "sw t4, %7;"
                        "sw t5, %8;"
                        "sw t6, %9;"
                        :
                        "=m" ( su->s32.x0_1 ),
                        "=m" ( su->s32.x0_0 ),
                        "=m" ( su->s32.x1_1 ),
                        "=m" ( su->s32.x1_0 ),
                        "=m" ( su->s32.x2_1 ),
                        "=m" ( su->s32.x2_0 ),
                        "=m" ( su->s32.x3_1 ),
                        "=m" ( su->s32.x3_0 ),
                        "=m" ( su->s32.x4_1 ),
                        "=m" ( su->s32.x4_0 )
                        :
                        "m" ( su->s32.x0_1  ),
                        "m" ( su->s32.x0_0  ),
                        "m" ( su->s32.x1_1  ),
                        "m" ( su->s32.x1_0  ),
                        "m" ( su->s32.x2_1  ),
                        "m" ( su->s32.x2_0  ),
                        "m" ( su->s32.x3_1  ),
                        "m" ( su->s32.x3_0  ),
                        "m" ( su->s32.x4_1  ),
                        "m" ( su->s32.x4_0  )
                        : "%a2", "%a3", "%a4", "%a5", "%a6", "%a7", "%t3", "%t4", "%t5", "%t6");
}

static inline void P8_HW(state* s) {
  state_union* su = (state_union*) s;
  __asm__ __volatile__ ("lw a2, %10;"
                        "lw a3, %11;"
                        "lw a4, %12;"
                        "lw a5, %13;"
                        "lw a6, %14;"
                        "lw a7, %15;"
                        "lw t3, %16;"
                        "lw t4, %17;"
                        "lw t5, %18;"
                        "lw t6, %19;"
                        "nop;"
                        "nop;"
                        ASCON_BE(0xb4)
                        ASCON_BE(0xa5)
                        ASCON_BE(0x96)
                        ASCON_BE(0x87)
                        ASCON_BE(0x78)
                        ASCON_BE(0x69)
                        ASCON_BE(0x5a)
                        ASCON_BE(0x4b)
                        "sw a2, %0;"
                        "sw a3, %1;"
                        "sw a4, %2;"
                        "sw a5, %3;"
                        "sw a6, %4;"
                        "sw a7, %5;"
                        "sw t3, %6;"
                        "sw t4, %7;"
                        "sw t5, %8;"
                        "sw t6, %9;"
                        :
                        "=m" ( su->s32.x0_1 ),
                        "=m" ( su->s32.x0_0 ),
                        "=m" ( su->s32.x1_1 ),
                        "=m" ( su->s32.x1_0 ),
                        "=m" ( su->s32.x2_1 ),
                        "=m" ( su->s32.x2_0 ),
                        "=m" ( su->s32.x3_1 ),
                        "=m" ( su->s32.x3_0 ),
                        "=m" ( su->s32.x4_1 ),
                        "=m" ( su->s32.x4_0 )
                        :
                        "m" ( su->s32.x0_1 ),
                        "m" ( su->s32.x0_0 ),
                        "m" ( su->s32.x1_1 ),
                        "m" ( su->s32.x1_0 ),
                        "m" ( su->s32.x2_1 ),
                        "m" ( su->s32.x2_0 ),
                        "m" ( su->s32.x3_1 ),
                        "m" ( su->s32.x3_0 ),
                        "m" ( su->s32.x4_1 ),
                        "m" ( su->s32.x4_0 )
                        : "%t2", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7", "%t3", "%t4", "%t5", "%t6");
}

static inline void P6_HW(state* s) {
  state_union* su = (state_union*) s;
  __asm__ __volatile__ ("lw a2, %10;"
                        "lw a3, %11;"
                        "lw a4, %12;"
                        "lw a5, %13;"
                        "lw a6, %14;"
                        "lw a7, %15;"
                        "lw t3, %16;"
                        "lw t4, %17;"
                        "lw t5, %18;"
                        "lw t6, %19;"
                        "nop;"
                        "nop;"
                        P6_BE
                        "sw a2, %0;"
                        "sw a3, %1;"
                        "sw a4, %2;"
                        "sw a5, %3;"
                        "sw a6, %4;"
                        "sw a7, %5;"
                        "sw t3, %6;"
                        "sw t4, %7;"
                        "sw t5, %8;"
                        "sw t6, %9;"
                        :
                        "=m" ( su->s32.x0_1 ),
                        "=m" ( su->s32.x0_0 ),
                        "=m" ( su->s32.x1_1 ),
                        "=m" ( su->s32.x1_0 ),
                        "=m" ( su->s32.x2_1 ),
                        "=m" ( su->s32.x2_0 ),
                        "=m" ( su->s32.x3_1 ),
                        "=m" ( su->s32.x3_0 ),
                        "=m" ( su->s32.x4_1 ),
                        "=m" ( su->s32.x4_0 )
                        :
                        "m" ( su->s32.x0_1 ),
                        "m" ( su->s32.x0_0 ),
                        "m" ( su->s32.x1_1 ),
                        "m" ( su->s32.x1_0 ),
                        "m" ( su->s32.x2_1 ),
                        "m" ( su->s32.x2_0 ),
                        "m" ( su->s32.x3_1 ),
                        "m" ( su->s32.x3_0 ),
                        "m" ( su->s32.x4_1 ),
                        "m" ( su->s32.x4_0 )
                        : "%t2", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7", "%t3", "%t4", "%t5", "%t6");
}
// #endif


#endif  // PERMUTATIONS_H_
