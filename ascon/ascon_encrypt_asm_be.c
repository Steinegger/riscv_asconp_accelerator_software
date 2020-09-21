#include "api.h"
#include "benchmark_config.h"
#include "permutations.h"
#include "util.h"

#if (IMPLEMENTATION_CHOICE == CRYPTO_AEAD_ENCRYPT_ASM_BIG_ENDIAN || PERFORMANCE_TEST == 1)
int crypto_aead_encrypt_asm_big_endian(unsigned char* c, io_length* clen,
                        const unsigned char* m, io_length mlen,
                        const unsigned char* ad, io_length adlen,
                        const unsigned char* nsec, const unsigned char* npub,
                        const unsigned char* k) {
  u32* mlen_32  = (u32*) &mlen;
  u32* adlen_32 = (u32*) &adlen;
  u32* clen_32  = (u32*) clen;
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
                        "lw t0, %[npub_p];"
                        SWAPPED_WORD_TO_REG("a4", "t3", 0, "t0")
                        SWAPPED_WORD_TO_REG("a4", "t4", 4, "t0")
                        SWAPPED_WORD_TO_REG("a4", "t5", 8, "t0")
                        SWAPPED_WORD_TO_REG("a4", "t6", 12, "t0")
                        // load key to x1, x2
                        // load key pointer
                        "lw t0, %[key_p];"
                        SWAPPED_WORD_TO_REG("a4", "s2", 0, "t0")
                        SWAPPED_WORD_TO_REG("a4", "s3", 4, "t0")
                        SWAPPED_WORD_TO_REG("a4", "s4", 8, "t0")
                        SWAPPED_WORD_TO_REG("a4", "s5", 12, "t0")
                        "mv a4, s2;"
                        "mv a5, s3;"
                        "mv a6, s4;"
                        "mv a7, s5;"
                        // P12 permutation
                        "nop;"
                        P12_BE
                        // xor key with remaining states
                        "xor t3, t3, s2;"
                        "xor t4, t4, s3;"
                        "xor t5, t5, s4;"
                        "xor t6, t6, s5;"
                        // process associated data
                        /* "lw t1, %[adlen_h];" */ // 32-bit system cannot address more blocks in memory
                        "lw t1, %[adlen_l];"
                        "beqz t1, end_ad;"
                        // load ad starting pointer
                        "lw t0, %[ad_p];"
                        // compute endpointer for comparision: start_ad_p + adlen - rate
                        "add t2, t0, t1;"
                        "addi t2, t2, -%[rate];"
                        "bgtu t0, t2, ad_last_block;"
#if (RISCY_HIGH_PERFORMANCE==1)
                        // hardwarelooped version
                        "srli t1, t1, 0x3;"
                        HWLOOPI(AD_ENC_LOOP_LEN_BE, REG_T1)
                        // loading 64 ad bits
                "loop_ad_block_loop_start:"
                        SWAPPED_WORD_TO_REG("s6", "t1", 0, "t0")
                        SWAPPED_WORD_TO_REG("s6", "s1", 4, "t0")
                        "xor a2, a2, t1;"
                        "xor a3, a3, s1;"
                        // P6 permutation
                        "addi t0, t0, %[rate];"
                        P6_BE
#else
                        // loading 64 ad bits
                "loop_ad_block_loop_start:"
                        SWAPPED_WORD_TO_REG("s6", "t1", 0, "t0")
                        SWAPPED_WORD_TO_REG("s6", "s1", 4, "t0")
                        "xor a2, a2, t1;"
                        "xor a3, a3, s1;"
                        // P6 permutation
                        "addi t0, t0, %[rate];"
                        P6_BE
                        "bleu t0, t2, loop_ad_block_loop_start;"
#endif
                "ad_last_block: "
                        // // rate - adlen -> number of additional shifts
                        // "sub t2, t2, t1;"
                        // t0 ad_p
                        // t2 t2
                        // usable s10, s1, s6,...
                        // calculate what is left (difference between pointers)
                        "addi t2, t2, %[rate];"
                        "sub t1, t2, t0;"

                        "li s7, 4;" // magic value 4 byte in a word
                        "mv s1, x0;" // clear s1 aka loaded bytes
                "ad_next_halfline:"
                        "mv s10, x0;"
                "ad_next_byte:"
                        "beqz t1, ad_finalize;"
                        SHIFT_BYTE_TO_REG("s6", "s10", 0, "t0")
                        "addi t1, t1, -1;" // decrement adlen
                        "addi s1, s1, 1;" // increment loaded
                        "addi t0, t0, 1;" // increment ad pointer
                        "bltu s1, s7, ad_next_byte;"
                        "bgtu s1, s7, ad_next_byte;"
                        "xor a2, a2, s10;"
                        "j ad_next_halfline;"
                "ad_finalize: "
                        "slli s10, s10, 8;"
                        "ori s10, s10, 0x80;"
                        // determine necessary shifts
                        "not t1, s1;" // copy negated loaded to adlen
                        "andi t1, t1, 0x3;"
                        "slli t1, t1, 3;"
                        "sll s10, s10, t1;"
                        // determine which register to xor to
                        "bgeu s1, s7, ad_final_store_to_lower;"
                        "xor a2, a2, s10;"
                        "j ad_final_permutation;"
                "ad_final_store_to_lower:"
                        "xor a3, a3, s10;"
                "ad_final_permutation:"
                        // P6 permutation
                        "nop;"
                        P6_BE
                "end_ad:"
                        "xori t6, t6, 1;"

                        // process plaintext
                        /* "lw t1, %[mlen_h];" */ // 32-bit system cannot address more blocks in memory
                        "lw t1, %[mlen_l];"
                        "lw s8, %[c_p];" // load ciphertext pointer before branch to write tag
                        // following block can be used to accelerate empty messages
                        // "bnez t1, m_initialize;" // no message means just padding
                        // // only pad and jump to tag
                        // "li s10, 0x80;"
                        // "slli s10, s10, 24;"
                        // "xor a2, a2, s10;"
                        // "j end_m;"
                        // "m_initialize:"
                        // load m starting pointer
                        "lw t0, %[m_p];"

                        // compute endpointer for comparision: start_ad_p + adlen - rate
                        "add t2, t0, t1;"
                        "addi t2, t2, -%[rate];"
                        "bgtu t0, t2, m_last_block;"

#if (RISCY_HIGH_PERFORMANCE==1)
                        // hardwarelooped version
                        "srli t1, t1, 0x3;"
                        HWLOOPI(M_ENC_LOOP_LEN_BE, REG_T1)
                "loop_m_block_loop_start:"
                        // loading 64 message bits
                        SWAPPED_WORD_TO_REG_INCREMENT(REG_S6, REG_T1, REG_T0)
                        SWAPPED_WORD_TO_REG_INCREMENT(REG_S6, REG_S1, REG_T0)
                        "xor a2, a2, t1;"
                        "xor a3, a3, s1;"
                        // Writing 64 ciphertext bits
                        REG_TO_SWAPPED_WORD_INCREMENT(REG_S6, REG_A2, REG_S8)
                        REG_TO_SWAPPED_WORD_INCREMENT(REG_S6, REG_A3, REG_S8)
                        // P6 permutation
                        P6_BE
#else
                "loop_m_block_loop_start:"
                        // loading 64 message bits
                        SWAPPED_WORD_TO_REG("s6", "t1", 0, "t0")
                        SWAPPED_WORD_TO_REG("s6", "s1", 4, "t0")
                        "xor a2, a2, t1;"
                        "xor a3, a3, s1;"
                        // Writing 64 ciphertext bits
                        REG_TO_SWAPPED_WORD("s6", "a2", 0, "s8")
                        REG_TO_SWAPPED_WORD("s6", "a3", 4, "s8")
                        // P6 permutation
                        P6_BE
                        "addi t0, t0, %[rate];"
                        "addi s8, s8, %[rate];"
                        "bleu t0, t2, loop_m_block_loop_start;"
#endif
                "m_last_block: "
                        // rate - mlen -> number of mditional shifts
                        // "sub t2, t2, t1;"
                        // t0 m_p
                        // t2 rate
                        // usable s10, s1, s6,...
                        // calculate what is left (difference between pointers)
                        "addi t2, t2, %[rate];"
                        "sub t1, t2, t0;"

                        "li s7, 4;" // magic value 4 byte in a word
                        "mv s10, x0;" // clear s10 aka loaded bytes
                "m_next_halfline:"
                        "mv s1, x0;"
                "m_next_byte:"
                        "beqz t1, m_finalize;"
                        SHIFT_BYTE_TO_REG("s6", "s1", 0, "t0")
                        "add t1, t1, -1;" // decrement mlen
                        "add s10, s10, 1;" // increment loaded
                        "add t0, t0, 1;" // increment m pointer
                        "bltu s10, s7, m_next_byte;"
                        "bgtu s10, s7, m_next_byte;"
                        "xor a2, a2, s1;"
                        "j m_next_halfline;"
                "m_finalize: "
                        "slli s1, s1, 8;"
                        "ori s1, s1, 0x80;"
                        // determine necessary shifts
                        "not t1, s10;" // copy negated loaded to adlen
                        "andi t1, t1, 0x3;"
                        "slli t1, t1, 3;"
                        "sll s1, s1, t1;"
                        // determine which register to xor to
                        "bgeu s10, s7, m_final_store_to_lower;"
                        "xor a2, a2, s1;"
                        "j m_finalize_message;"
                "m_final_store_to_lower:"
                        "xor a3, a3, s1;"
                "m_finalize_message:"
                        REG_TO_SWAPPED_WORD("s6", "a2", 0, "s8")
                        REG_TO_SWAPPED_WORD("s6", "a3", 4, "s8")
                        "add s8, s8, s10;"
                "end_m:"
                        "xor a4, a4, s2;"
                        "xor a5, a5, s3;"
                        "xor a6, a6, s4;"
                        "xor a7, a7, s5;"

                        // P12 permutation
                        "nop;"
                        P12_BE

                        "xor t3, t3, s2;"
                        "xor t4, t4, s3;"
                        "xor t5, t5, s4;"
                        "xor t6, t6, s5;"

                        REG_TO_SWAPPED_WORD("s6", "t3", 0, "s8")
                        REG_TO_SWAPPED_WORD("s6", "t4", 4, "s8")
                        REG_TO_SWAPPED_WORD("s6", "t5", 8, "s8")
                        REG_TO_SWAPPED_WORD("s6", "t6", 12, "s8")

                        // // finalization
                        "lw t1, %[mlen_l];"
                        "li t2, %[crypto_abytes];"
                        "add t2, t1, t2;"
                        "sw t2, %[clen_l];"

                        : \
                        // [clen_h]          "=m"  ( ( (u32*) (clen) )[1] ), // 32-bit system cannot address more blocks in memory
                        [clen_l]          "=m"  ( *clen_32 )
                        :
                        [key_p]            "m"  ( k ),
                        [npub_p]           "m"  ( npub ),
                        [m_p]              "m"  ( m ),
                        [ad_p]             "m"  ( ad ),
                        [c_p]              "m"  ( c ),
                        // [adlen_h]          "m"  ( ( (u32*) &(adlen) )[1] ),  // 32-bit system cannot address more blocks in memory
                        [adlen_l]          "m"  ( *adlen_32 ),
                        // [mlen_h]           "m"  ( ( (u32*) &(mlen ) )[1] ),  // 32-bit system cannot address more blocks in memory
                        [mlen_l]           "m"  ( *mlen_32 ),
                        [rate]             "sn" ( RATE ),
                        [iv_h]             "sn" ( (u32) ((IV >> 32 ) & 0xFFFFFFFF ) ),
                        [iv_l]             "sn" ( (u32)  (IV         & 0xFFFFFFFF ) ),
                        [crypto_abytes]    "sn" (CRYPTO_ABYTES)
                        : "%t0", "%t1", "%t2", "%s1", "%s2", "%s3", "%s4", "%s5",  "%s6", "%s7", "%s8", "%s10", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7", "%t3", "%t4", "%t5", "%t6" );
  return 0;
}
#endif