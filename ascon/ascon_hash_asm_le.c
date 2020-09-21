#include "api.h"
#include "benchmark_config.h"
#include "permutations.h"
#include "util.h"

#if (IMPLEMENTATION_CHOICE == CRYPTO_HASH_ASM_LITTLE_ENDIAN || PERFORMANCE_TEST == 1)
int crypto_hash_asm_little_endian(unsigned char *out, const unsigned char *in,
                io_length inlen) {

  u32* inlen_32  = (u32*) &inlen;
  __asm__ __volatile__ (
                        // a2,a3,a4,a5,a6,a7,t3,t4,t5,t6 are used for ASCON state
                        // a2 = x0_h      a3 = x0_l      |   x0 =  [ a2  ,  a3 ]
                        // a4 = x1_h      a5 = x1_l      |   x1 =  [ a4  ,  a5 ]
                        // a6 = x2_h      a7 = x2_l      |   x2 =  [ a6  ,  a7 ]
                        // t3 = x3_h      t4 = x3_l      |   x3 =  [ t3  ,  t4 ]
                        // t5 = x4_h      t6 = x4_l      |   x4 =  [ t5  ,  t6 ]
                        // const u64 K0 = BYTES_TO_U64(k, 8);

                        // Load IV to x0
                        "li a2, %[iv_h];"
                        "li a3, %[iv_l];"
                        // Load nonce to x3, x4
                        // load nonce pointer
                        "li a4, 0;"
                        "li a5, 0;"
                        "li a6, 0;"
                        "li a7, 0;"
                        "li t3, 0;"
                        "li t4, 0;"
                        "li t5, 0;"
                        "li t6, 0;"
                        "nop;"
                        // P12 permutation
                        P12_LE

                        // process message
                        /* "lw t1, %[adlen_h];" */ // 32-bit system cannot address more blocks in memory
                        "lw t1, %[inlen_l];"

                        // load in starting pointer
                        "lw t0, %[in_p];"
                        // compute endpointer for comparision: start_ad_p + adlen - rate
                        "add t2, t0, t1;"
                        "addi t2, t2, -%[rate];"
                        "bgtu t0, t2, digest_last_block_le;"
#if (RISCY_HIGH_PERFORMANCE==1)
                      // hardwarelooped version
                      "srli t1, t1, 0x3;"
                      HWLOOPI(M_HASH_LOOP_LEN_LE, REG_T1)
                      // loading 64 in bits
                "digest_block_start_loop_le:"
                        "lw t1, 0(t0);"
                        "lw s1, 4(t0);"
                        "xor a2, a2, t1;"
                        "xor a3, a3, s1;"
                        "addi t0, t0, %[rate];" // add for next full block
                        // P12 permutation
                        P12_LE
#else
                    // loading 64 in bits
                "digest_block_start_loop_le:"
                        "lw t1, 0(t0);"
                        "lw s1, 4(t0);"
                        "xor a2, a2, t1;"
                        "xor a3, a3, s1;"
                        "addi t0, t0, %[rate];" // add for next full block
                        // P12 permutation
                        P12_LE
                        "bleu t0, t2, digest_block_start_loop_le;"
#endif
                "digest_last_block_le: "
                        // // rate - adlen -> number of additional shifts
                        // "sub t2, t2, t1;"
                        // t0 ad_p
                        // t2 t2
                        // usable s10, s1, s6,...
                        // calculate what is left (difference between pointers)
                        "addi t2, t2, %[rate];"

                        "li s7, 32;" // magic value 4 byte/ 32 bit in a word
                        "mv s1, x0;" // clear s1 aka loaded bytes
                        "beq t0, t2, digest_finalize_le;"
                "digest_next_byte_le:"
                        "lbu s10, 0(t0);"
                        "and s6, s1, (0x3 << 3);" // index in halflane mask
                        "sll s10, s10, s6;"
                        "addi s1, s1, 8;" // increment loaded
                        "addi t0, t0, 1;" // increment ad pointer
                        "bgtu s1, s7, digest_byte_second_reg_le;"  // xor to second register
                        "xor a2, a2, s10;"
                        "bne t0, t2, digest_next_byte_le;"
                        "j digest_finalize_le;"
                "digest_byte_second_reg_le:"
                        "xor a3, a3, s10;"
                        "bne t0, t2, digest_next_byte_le;"
                "digest_finalize_le: "
                        "li s10, 0x80;"
                        // determine necessary shifts
                        "andi t1, s1, (0x3 << 3);"
                        "sll s10, s10, t1;"
                        // determine which register to xor to
                        "bgeu s1, s7, digest_final_store_to_lower_le;"
                        "xor a2, a2, s10;"
                        "j digest_final_permutation_le;"
                "digest_final_store_to_lower_le:"
                        "xor a3, a3, s10;"
                "digest_final_permutation_le:"
                        "nop;"
                        // P12 permutation
                        P12_LE
                "end_digest_le:"
                        // output hash
                        /* "lw t1, %[mlen_h];" */ // 32-bit system cannot address more blocks in memory
                        "li t1, %[crypto_bytes];"
                        "lw s8, %[hash_p];" // load ciphertext pointer before branch to write tag
                        // compute endpointer for comparision: start_out_p + crypto_bytes - rate
                        "add t2, s8, t1;"
                        "addi t2, t2, -%[rate];"

                        "bgtu s8, t2, hash_output_last_block_le;"
                "hash_block_start_loop_le:"
                        // loading 64 message bits
                        "sw a2, 0(s8);"
                        "sw a3, 4(s8);"
                        "addi s8, s8, %[rate];"
                        P12_LE
                        "blt s8, t2, hash_block_start_loop_le;"
                "hash_output_last_block_le: "
                        // rate - mlen -> number of mditional shifts
                        // "sub t2, t2, t1;"
                        // t0 m_p
                        // t1 mlen
                        // t2 endpointer
                        // usable s10, s1, s6,...
                        // calculate what is left (difference between pointers)
                        "sw a2,  0(s8);"
                        "sw a3,  4(s8);"
                        :
                        :
                        [in_p]             "m"  ( in ),
                        [hash_p]           "m"  ( out ),
                        // [inlen_h]          "m"  ( ( (u32*) &(inlen) )[1] ),  // 32-bit system cannot address more blocks in memory
                        [inlen_l]          "m"  ( *inlen_32 ),

                        [rate]             "sn" ( RATE ),
                        [iv_h]             "sn" ( (u32) SWAP_ENDIANESS(((IV_HASH >> 32 ) & 0xFFFFFFFF )) ),
                        [iv_l]             "sn" ( (u32) SWAP_ENDIANESS( (IV_HASH         & 0xFFFFFFFF )) ),
                        [crypto_bytes]     "sn" (CRYPTO_BYTES)
                        : "%t0", "%t1", "%t2", "%s1", "%s6", "%s7", "%s8", "s10", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7", "%t3", "%t4", "%t5", "%t6" );
  return 0;
}

#endif
