#include "api.h"
#include "benchmark_config.h"
#include "permutations.h"

#include "util.h"
#include <string.h>
#include "isap.h"


#if (ISAP_sB == 12)
#define ISAP_sB_PERMUTATION_LE  P12_LE
#define ISAP_RP_LOOP_LEN_PERM CEIL_DIV(12, MULTIROUND_CONF)
#elif (ISAP_sB == 1)
#define ISAP_sB_PERMUTATION_LE  ASCON_LExN(0x04b, 1)
#define ISAP_RP_LOOP_LEN_PERM 1
#endif

#define ISAP_sK_PERMUTATION_LE  P12_LE

#define ISAP_RK_LOOP_LEN_LE (1 + 9 + 2 * ISAP_RP_LOOP_LEN_PERM + 6 * (4 + ISAP_RP_LOOP_LEN_PERM))
// #define ISAP_RK_LOOP_LEN_LE 131

// 5 + (12 or 1) instructions
#define ISAP_RK_BITLOOP_UNROLLED(shift, dest_reg, temp_reg, byte_src_reg) \
												"srli " temp_reg ", " byte_src_reg ", " STR(shift) ";" \
												"andi " temp_reg ", " temp_reg ", 1;" \
												"slli " temp_reg ", " temp_reg ", 7;" \
												"xor " dest_reg ",  " dest_reg ", " temp_reg ";" \
												"nop;" \
												ISAP_sB_PERMUTATION_LE

// 4 + (12 or 1) instructions
#define ISAP_RK_BITLOOP_OPTIMIZED(shift, dest_reg, temp_reg, byte_src_reg) \
												"andi " temp_reg ", " temp_reg ", 1;" \
												"slli " temp_reg ", " temp_reg ", 7;" \
												"xor " dest_reg ",  " dest_reg ", " temp_reg ";" \
												"srli " temp_reg ", " byte_src_reg ", " STR(shift) ";" \
												ISAP_sB_PERMUTATION_LE \

// 9 + 2 * (12 or 1) + 6 * ISAP_RK_BITLOOP_OPTIMIZED instructions
#define ISAP_RK_FULL_BITLOOP_UNROLLED_OPTIMIZED(dest_reg, temp_reg, byte_src_reg) \
												"srli " temp_reg ", " byte_src_reg ", 7;" \
												ISAP_RK_BITLOOP_OPTIMIZED(6, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(5, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(4, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(3, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(2, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(1, dest_reg, temp_reg, byte_src_reg) \
												"andi " temp_reg ", " temp_reg ", 1;" \
												"slli " temp_reg ", " temp_reg ", 7;" \
												"xor " dest_reg ",  " dest_reg ", " temp_reg ";" \
												"nop;"\
												ISAP_sB_PERMUTATION_LE \
												"andi " temp_reg ", " byte_src_reg ", 1;" \
												"slli " temp_reg ", " temp_reg ", 7;" \
												"xor " dest_reg ",  " dest_reg ", " temp_reg ";" \
												"nop;" \
												ISAP_sB_PERMUTATION_LE

// 21 + (12 or 1) + 6 * ISAP_RK_BITLOOP_OPTIMIZED instructions
#define ISAP_RK_LAST_BYTE_UNROLLED_OPTIMIZED(dest_reg, temp_reg, byte_src_reg) \
												"srli " temp_reg ", " byte_src_reg ", 7;" \
												ISAP_RK_BITLOOP_OPTIMIZED(6, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(5, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(4, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(3, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(2, dest_reg, temp_reg, byte_src_reg) \
												ISAP_RK_BITLOOP_OPTIMIZED(1, dest_reg, temp_reg, byte_src_reg) \
												"andi " temp_reg ", " temp_reg ", 1;" \
												"slli " temp_reg ", " temp_reg ", 7;" \
												"xor " dest_reg ",  " dest_reg ", " temp_reg ";" \
												"nop;"\
												ISAP_sB_PERMUTATION_LE \
												"andi " temp_reg ", " byte_src_reg ", 1;" \
												"slli " temp_reg ", " temp_reg ", 7;" \
												"xor " dest_reg ",  " dest_reg ", " temp_reg ";" \
												"nop;" \
												ISAP_sK_PERMUTATION_LE

// 8 * ISAP_RK_BITLOOP_UNROLLED
#define ISAP_RK_FULL_BITLOOP_UNROLELD_NAIV(dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(7, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(6, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(5, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(4, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(3, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(2, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(1, dest_reg, temp_reg, byte_src_reg) \
											ISAP_RK_BITLOOP_UNROLLED(0, dest_reg, temp_reg, byte_src_reg) \

/* ---------------------------------------------------------------- */

void load64(u64* x, u8* S) {
  int i;
  *x = 0;
  for (i = 0; i < 8; ++i)
    *x |= ((u64) S[i]) << (56 - i * 8);
}

/* ---------------------------------------------------------------- */

void store64(u8* S, u64 x) {
  int i;
  for (i = 0; i < 8; ++i)
    S[i] = (u8) (x >> (56 - i * 8));
}

void Ascon_AddBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length){
  unsigned int i;
  for(i=0; i<length; i++){
    ((unsigned char *)state)[offset+i] ^= data[i];
  }
}

void Ascon_ExtractBytes(const void *state, unsigned char *data, unsigned int offset, unsigned int length){
  memcpy(data, (unsigned char*)state+offset, length);
}

void Ascon_Permute_Nrounds(u8 *S, unsigned int rounds){
	// printf("--------\n");
  int start = 12 - rounds;
  unsigned int i;
  u64 x0, x1, x2, x3, x4;
  u64 t0, t1, t2, t3, t4;
  load64(&x0, S + 0);
  load64(&x1, S + 8);
  load64(&x2, S + 16);
  load64(&x3, S + 24);
  load64(&x4, S + 32);
  for (i = start; i < rounds + start; ++i) {
    // addition of round constant
		// myprintString("Roundconstant: ")
		// printf("%llx \n", ((0xfull - i) << 4) | i);
    x2 ^= ((0xfull - i) << 4) | i;
    // substitution layer
    x0 ^= x4;    x4 ^= x3;    x2 ^= x1;
    t0  = x0;    t1  = x1;    t2  = x2;    t3  = x3;    t4  = x4;
    t0 =~ t0;    t1 =~ t1;    t2 =~ t2;    t3 =~ t3;    t4 =~ t4;
    t0 &= x1;    t1 &= x2;    t2 &= x3;    t3 &= x4;    t4 &= x0;
    x0 ^= t1;    x1 ^= t2;    x2 ^= t3;    x3 ^= t4;    x4 ^= t0;
    x1 ^= x0;    x0 ^= x4;    x3 ^= x2;    x2 =~ x2;
    // linear diffusion layer
    x0 ^= ROTR(x0, 19) ^ ROTR(x0, 28);
    x1 ^= ROTR(x1, 61) ^ ROTR(x1, 39);
    x2 ^= ROTR(x2,  1) ^ ROTR(x2,  6);
    x3 ^= ROTR(x3, 10) ^ ROTR(x3, 17);
    x4 ^= ROTR(x4,  7) ^ ROTR(x4, 41);
  }
  store64(S + 0, x0);
  store64(S + 8, x1);
  store64(S + 16, x2);
  store64(S + 24, x3);
  store64(S + 32, x4);
}

void isap_rk(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	const unsigned long long inlen,
	unsigned char *out,
	const unsigned long long outlen
){
	// Init State
	unsigned char state[ISAP_STATE_SZ] = {0};
	// unsigned char state[ISAP_STATE_SZ] = {
	// 	0x1e, 0xda, 0xac, 0xbc, 0x2a, 0x28, 0xe8, 0xa3,
	// 	0xaf, 0x6f, 0x28, 0xa9, 0x06, 0x9f, 0x02, 0x4e,
	// 	0x68, 0xaa, 0x75, 0x5f, 0xc0, 0x68, 0xb7, 0x7c,
	// 	0x59, 0x4a, 0x76, 0x39, 0x82, 0xa0, 0x4e, 0x2d,
	// 	0x6c, 0xca, 0x30, 0xcb, 0x13, 0x58, 0x9c, 0x00
	// };
	// Ascon_Initialize(state);
	// myprintString("after Ascon_Initialize\n");
	// printAsHex(state, ISAP_STATE_SZ);
	Ascon_AddBytes(state,k,0,CRYPTO_KEYBYTES);
	// myprintString("after Ascon_AddBytes k\n");
	// printAsHex(state, ISAP_STATE_SZ);
	Ascon_AddBytes(state,iv,CRYPTO_KEYBYTES,ISAP_IV_SZ);
	// myprintString("after Ascon_AddBytes iv\n");
	// printAsHex(state, ISAP_STATE_SZ);
	Ascon_Permute_Nrounds(state,ISAP_sK);

	// myprintString("before absorb\n");
	// printAsHex(state, ISAP_STATE_SZ);
	// Absorb
	for (size_t i = 0; i < inlen*8-1; i++){
		size_t cur_byte_pos = i/8;
		size_t cur_bit_pos = 7-(i%8);
		unsigned char cur_bit = ((in[cur_byte_pos] >> (cur_bit_pos)) & 0x01) << 7;
		// printf("Loop %d: \n", i);
		Ascon_AddBytes(state,(const unsigned char*)&cur_bit,0,1);
		// printAsHex(state, ISAP_STATE_SZ);
		Ascon_Permute_Nrounds(state,ISAP_sB);
		// printAsHex(state, ISAP_STATE_SZ);
	}
	// myprintString("after for\n");
	// printAsHex(state, ISAP_STATE_SZ);
	unsigned char cur_bit = ((in[inlen-1]) & 0x01) << 7;
	Ascon_AddBytes(state,(const unsigned char*)&cur_bit,0,1);
	Ascon_Permute_Nrounds(state,ISAP_sK);

	// myprintString("before squeeze\n");
	// printAsHex(state, ISAP_STATE_SZ);
	// Squeeze K*
	Ascon_ExtractBytes(state,out,0,outlen);
	// myprintString("final state\n");
	// printAsHex(state, ISAP_STATE_SZ);
}

void isap_rk_asm_le(
	const unsigned char *k,
	const unsigned char *iv,
	const unsigned char *in,
	const unsigned long long inlen,
	unsigned char *out,
	const unsigned long long outlen
){  
	u32* inlen_32  = (u32*) &inlen;
	u32* outlen_32 = (u32*) &outlen;
	__asm__ __volatile__ (
												// a2,a3,a4,a5,a6,a7,t3,t4,t5,t6 are used for ASCON state
												// a2 = x0_h      a3 = x0_l      |   x0 =  [ a2  ,  a3 ]
												// a4 = x1_h      a5 = x1_l      |   x1 =  [ a4  ,  a5 ]
												// a6 = x2_h      a7 = x2_l      |   x2 =  [ a6  ,  a7 ]
												// t3 = x3_h      t4 = x3_l      |   x3 =  [ t3  ,  t4 ]
												// t5 = x4_h      t6 = x4_l      |   x4 =  [ t5  ,  t6 ]
												// const u64 K0 = BYTES_TO_U64(k, 8);
												"lw t0, %[key_p];"
												"lw a2, 0(t0);"
												"lw a3, 4(t0);"
												"lw a4, 8(t0);"
												"lw a5, 12(t0);"
												"lw t0, %[iv_p];"
												"lw a6, 0(t0);"
												"lw a7, 4(t0);"
												"li t3, 0;"
												"li t4, 0;"
												"li t5, 0;"
												"li t6, 0;"
												"nop;"
							"isap_before_isap_perm_le:"
												P12_LE
#if (RISCY_HIGH_PERFORMANCE==1)
							"isap_after_init_le:"
												"lw t0, %[in_p];"
												"lw t1, %[inlen_l];"
												"add t1, t1, -1;"
												HWLOOPI(ISAP_RK_LOOP_LEN_LE, REG_T1)
							"isap_outer_loop_le:"
												// load next byte
												// "lbu t2, 0(t0);"
												// "addi t0, t0, 1;"
												LOADBUI_INCR(REG_T2, 1, REG_T0)
												ISAP_RK_FULL_BITLOOP_UNROLLED_OPTIMIZED("a2", "s2", "t2")
							"isap_last_byte_le:"
												"lbu t2, 0(t0);"
												ISAP_RK_LAST_BYTE_UNROLLED_OPTIMIZED("a2", "s2", "t2")
#else
							"isap_after_init_le:"
												"lw t0, %[in_p];"
												"lw t1, %[inlen_l];"
												"add t1, t0, t1;"
												"add t1, t1, -1;"
							"isap_outer_loop_le:"
												// load next byte
												"lbu t2, 0(t0);"
												"addi t0, t0, 1;"
												ISAP_RK_FULL_BITLOOP_UNROLLED_OPTIMIZED("a2", "s2", "t2")
												"bne t0, t1, isap_outer_loop_le;"
							"isap_last_byte_le:"
												"lbu t2, 0(t0);"
												ISAP_RK_LAST_BYTE_UNROLLED_OPTIMIZED("a2", "s2", "t2")
#endif
							"isap_after_for_le:"
												"lw t0, %[out_p];"
												"sw a2, 0(t0);"
												"sw a3, 4(t0);"
												"sw a4, 8(t0);"
												"sw a5, 12(t0);"
												"sw a6, 16(t0);"
												"sw a7, 20(t0);"
												: \
												:
												[key_p]            "m"  ( k ),
												[iv_p]             "m"  ( iv ),
												[in_p]             "m"  ( in ),
												[out_p]            "m"  ( out ),
												[outlen_l]         "m"  (  *outlen_32  ),
												// [adlen_h]          "m"  ( ( (u32*) &(adlen) )[1] ),  // 32-bit system cannot address more blocks in memory
												[inlen_l]          "m"  (  *inlen_32 ),
												// [mlen_h]           "m"  ( ( (u32*) &(mlen ) )[1] ),  // 32-bit system cannot address more blocks in memory
												[crypto_abytes]    "sn" (CRYPTO_ABYTES),
												[isap_iv_sz]       "sn" (ISAP_IV_SZ)
												: "%t0", "%t1", "%t2", "%s2", "%s3", \
												"%a2", "%a3", "%a4", "%a5", "%a6", "%a7", "%t3", "%t4", "%t5", "%t6" );
}

//--------------- ISAP tests End
