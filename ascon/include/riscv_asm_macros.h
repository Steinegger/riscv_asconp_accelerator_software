#ifndef RISCV_ASM_MACROS_H_
#define RISCV_ASM_MACROS_H_

#include "util.h"

#define REG_T0  5
#define REG_T1  6
#define REG_T2  7
#define REG_S1  9
#define REG_A2 12
#define REG_A3 13
#define REG_S6 22
#define REG_S7 23
#define REG_S8 24

#define ASCON_BExN(RC, ROUNDS) \
  ASCON(RC, ROUNDS, 0x0)

#define ASCON_BE(RC) \
  ASCON_BExN(RC, 1)

#define ASCON_LExN(RC, ROUNDS) \
  ASCON(RC, ROUNDS, 0x1)

#define ASCON_LE(RC) \
  ASCON_LExN(RC, 1)

#define ITYPE(imm, rs1, funct3, rd, opcode) \
  (((imm & 0xFFF) << 20) |  ((rs1 & 0x1F) << 15) | ((funct3 & 0x7) << 12) | ((rd & 0x1F) << 7) | ((opcode & 0x7F) << 0))

#define STYPE(imm, rs2, rs1, funct3, opcode) \
  (((imm & 0xFE0) << 25) |  ((rs2 & 0x1F) << 20) |((rs1 & 0x1F) << 15) | ((funct3 & 0x7) << 12) | ((imm & 0x1F) << 7) | ((opcode & 0x7F) << 0))

#define HWLOOP(uimmL, rs1, funct3, L, opcode) \
  (((((uimmL << 2) >> 1) & 0xFFF) << 20) |  ((rs1 & 0x1F) << 15) | ((funct3 & 0x7) << 12) | ((0x0 & 0xF) << 8) | ((L & 0x1) << 7) | ((opcode & 0x7F) << 0))

#define ASCON(RC, ROUNDS, LITTLE_ENDIAN) \
  ".word " VALUE(ITYPE(( (LITTLE_ENDIAN << 11) | ( ((ROUNDS - 1) & 0x7) << 8) | (RC & 0xFF)), 0, 0x3, 0, 0xB)) ";"

#define ITYPE_INST(imm, rs1, funct3, rd, opcode) \
    ".word " VALUE(ITYPE(imm, rs1, funct3, rd, opcode)) ";"

#define STYPE_INST(imm, rs2, rs1, funct3, opcode) \
    ".word " VALUE(STYPE(imm, rs2, rs1, funct3, opcode)) ";"

#define LOADI_INCR(DST_REG, INCREMENT, ADDR_REG) \
  ITYPE_INST(INCREMENT, ADDR_REG, 0x2, DST_REG, 0xB)

#define LOADBUI_INCR(DST_REG, INCREMENT, ADDR_REG) \
  ITYPE_INST(INCREMENT, ADDR_REG, 0x4, DST_REG, 0xB)

#define STOREI_INCR(SRC_REG, INCREMENT, ADDR_REG) \
  STYPE_INST(INCREMENT, SRC_REG, ADDR_REG, 0x2, 0x2B)

#define STOREBI_INCR(SRC_REG, INCREMENT, ADDR_REG) \
  STYPE_INST(INCREMENT, SRC_REG, ADDR_REG, 0x0, 0x2B)

#define HWLOOPI(LOOP_LENGHT_INSTR, LOOP_ITERATIONS) \
    ".word " VALUE(HWLOOP(LOOP_LENGHT_INSTR, LOOP_ITERATIONS, 0x4, 0, 0x7B)) ";"


#define RATE (64 / 8)
#define PA_ROUNDS 12
#define PB_ROUNDS 6
#define IV                                                        \
  ((u64)(8 * (CRYPTO_KEYBYTES)) << 56 | (u64)(8 * (RATE)) << 48 | \
   (u64)(PA_ROUNDS) << 40 | (u64)(PB_ROUNDS) << 32)

#define IV_HASH                                      \
 ((u64)(8 * (RATE)) << 48 | (u64)(PA_ROUNDS) << 40 | \
    (u64)(8 * (CRYPTO_BYTES)) << 0)

#define SHIFT_BYTE_TO_REG(tmp_reg,  dest_reg, offset, addr_reg) \
  "lbu "   tmp_reg ", " STR(offset) "(" addr_reg ");" \
  "slli " dest_reg "," dest_reg ", 8;" \
  "or "   dest_reg ", " dest_reg ", " tmp_reg ";"

// load and convert endianess // 10 instructions
#define SWAPPED_WORD_TO_REG(tmp_reg,  dest_reg, additional_offset, addr_reg) \
  "lbu "  dest_reg ", " STR(0 + additional_offset) "(" addr_reg ");" \
  SHIFT_BYTE_TO_REG(tmp_reg,  dest_reg, (1 + additional_offset), addr_reg) \
  SHIFT_BYTE_TO_REG(tmp_reg,  dest_reg, (2 + additional_offset), addr_reg) \
  SHIFT_BYTE_TO_REG(tmp_reg,  dest_reg, (3 + additional_offset), addr_reg)

// store register and convert endianess // 7 instructions
#define REG_TO_SWAPPED_WORD(tmp_reg, src_reg, additional_offset, addr_reg) \
  STORE_AND_SHIFT_BYTE(tmp_reg, src_reg, (3 + additional_offset), addr_reg) \
  STORE_AND_SHIFT_BYTE(tmp_reg, tmp_reg, (2 + additional_offset), addr_reg) \
  STORE_AND_SHIFT_BYTE(tmp_reg, tmp_reg, (1 + additional_offset), addr_reg) \
  "sb " tmp_reg "," STR(0 + additional_offset) "(" addr_reg ");"

#define STORE_AND_SHIFT_BYTE(shift_dst_reg, src_reg, offset, addr_reg) \
  "sb   " src_reg ", " STR(offset) "(" addr_reg ");" \
  "srli " shift_dst_reg ", " src_reg     ", 8;"

//----------------------------
// ri5cy specific macro to do load and store increments
#define SHIFT_BYTE_TO_REG_INCREMENT(tmp_reg,  dest_reg, addr_reg) \
  LOADBUI_INCR(tmp_reg, 0x1, addr_reg) \
  "slli x" STR(dest_reg) ", x" STR(dest_reg) ", 8;" \
  "or x"   STR(dest_reg) ", x" STR(dest_reg) ", x" STR(tmp_reg) ";"

// load and convert endianess // 10 instructions
#define SWAPPED_WORD_TO_REG_INCREMENT(tmp_reg,  dest_reg, addr_reg) \
  LOADBUI_INCR(dest_reg, 0x1, addr_reg) \
  SHIFT_BYTE_TO_REG_INCREMENT(tmp_reg, dest_reg, addr_reg) \
  SHIFT_BYTE_TO_REG_INCREMENT(tmp_reg, dest_reg, addr_reg) \
  SHIFT_BYTE_TO_REG_INCREMENT(tmp_reg, dest_reg, addr_reg)


// store register and convert endianess // 7 instructions
#define REG_TO_SWAPPED_WORD_INCREMENT(tmp_reg, src_reg, addr_reg) \
  STORE_AND_SHIFT_BYTE_INCREMENT(tmp_reg, src_reg, 3, addr_reg) \
  STORE_AND_SHIFT_BYTE_INCREMENT(tmp_reg, tmp_reg, 2, addr_reg) \
  STORE_AND_SHIFT_BYTE_INCREMENT(tmp_reg, tmp_reg, 1, addr_reg) \
  STOREBI_INCR(tmp_reg, 4, addr_reg)

#define STORE_AND_SHIFT_BYTE_INCREMENT(shift_dst_reg, src_reg, offset, addr_reg) \
  "sb   x" STR(src_reg) ", " STR(offset) "(x" STR(addr_reg) ");" \
  "srli x" STR(shift_dst_reg) ", x" STR(src_reg)     ", 8;"

//----------------------------

#define REG_TO_WORD(tmp_reg, src_reg, additional_offset, addr_reg) \
  STORE_AND_SHIFT_BYTE(tmp_reg, src_reg, (0 + additional_offset), addr_reg) \
  STORE_AND_SHIFT_BYTE(tmp_reg, tmp_reg, (1 + additional_offset), addr_reg) \
  STORE_AND_SHIFT_BYTE(tmp_reg, tmp_reg, (2 + additional_offset), addr_reg) \
  "sb " tmp_reg "," STR(3 + additional_offset) "(" addr_reg ");"


#endif // RISCV_ASM_MACROS_H_
