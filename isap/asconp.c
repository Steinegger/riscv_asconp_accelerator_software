#include "asconp.h"

// Round constants, bit-interleaved
u32 rc_o[12] = {0xc, 0xc, 0x9, 0x9, 0xc, 0xc, 0x9, 0x9, 0x6, 0x6, 0x3, 0x3};
u32 rc_e[12] = {0xc, 0x9, 0xc, 0x9, 0x6, 0x3, 0x6, 0x3, 0xc, 0x9, 0xc, 0x9};

/* ---------------------------------------------------------------- */

u64 U64BIG(u64 x)
{
    return ((((x)&0x00000000000000FFULL) << 56) | (((x)&0x000000000000FF00ULL) << 40) |
            (((x)&0x0000000000FF0000ULL) << 24) | (((x)&0x00000000FF000000ULL) << 8) |
            (((x)&0x000000FF00000000ULL) >> 8) | (((x)&0x0000FF0000000000ULL) >> 24) |
            (((x)&0x00FF000000000000ULL) >> 40) | (((x)&0xFF00000000000000ULL) >> 56));
}

/* ---------------------------------------------------------------- */

// Credit to Henry S. Warren, Hacker's Delight, Addison-Wesley, 2002
void to_bit_interleaving(u32_2 *out, u64 in)
{
    u32 hi = (in) >> 32;
    u32 lo = (u32)(in);
    u32 r0, r1;
    r0 = (lo ^ (lo >> 1)) & 0x22222222, lo ^= r0 ^ (r0 << 1);
    r0 = (lo ^ (lo >> 2)) & 0x0C0C0C0C, lo ^= r0 ^ (r0 << 2);
    r0 = (lo ^ (lo >> 4)) & 0x00F000F0, lo ^= r0 ^ (r0 << 4);
    r0 = (lo ^ (lo >> 8)) & 0x0000FF00, lo ^= r0 ^ (r0 << 8);
    r1 = (hi ^ (hi >> 1)) & 0x22222222, hi ^= r1 ^ (r1 << 1);
    r1 = (hi ^ (hi >> 2)) & 0x0C0C0C0C, hi ^= r1 ^ (r1 << 2);
    r1 = (hi ^ (hi >> 4)) & 0x00F000F0, hi ^= r1 ^ (r1 << 4);
    r1 = (hi ^ (hi >> 8)) & 0x0000FF00, hi ^= r1 ^ (r1 << 8);
    (*out).e = (lo & 0x0000FFFF) | (hi << 16);
    (*out).o = (lo >> 16) | (hi & 0xFFFF0000);
}

/* ---------------------------------------------------------------- */

// Credit to Henry S. Warren, Hacker's Delight, Addison-Wesley, 2002
void from_bit_interleaving(u64 *out, u32_2 in)
{
    u32 lo = ((in).e & 0x0000FFFF) | ((in).o << 16);
    u32 hi = ((in).e >> 16) | ((in).o & 0xFFFF0000);
    u32 r0, r1;
    r0 = (lo ^ (lo >> 8)) & 0x0000FF00, lo ^= r0 ^ (r0 << 8);
    r0 = (lo ^ (lo >> 4)) & 0x00F000F0, lo ^= r0 ^ (r0 << 4);
    r0 = (lo ^ (lo >> 2)) & 0x0C0C0C0C, lo ^= r0 ^ (r0 << 2);
    r0 = (lo ^ (lo >> 1)) & 0x22222222, lo ^= r0 ^ (r0 << 1);
    r1 = (hi ^ (hi >> 8)) & 0x0000FF00, hi ^= r1 ^ (r1 << 8);
    r1 = (hi ^ (hi >> 4)) & 0x00F000F0, hi ^= r1 ^ (r1 << 4);
    r1 = (hi ^ (hi >> 2)) & 0x0C0C0C0C, hi ^= r1 ^ (r1 << 2);
    r1 = (hi ^ (hi >> 1)) & 0x22222222, hi ^= r1 ^ (r1 << 1);
    *out = (u64)hi << 32 | lo;
}

