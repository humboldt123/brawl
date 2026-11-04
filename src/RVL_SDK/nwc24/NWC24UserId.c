// WiiConnect24 SDK: UserId.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve calls across SDK source boundaries; original inline helpers
// are marked inline explicitly.
#pragma auto_inline off

static u64 getUnScrambleId(u64 id);

// the user id has a checksum in its low bits: after the decode step the whole thing has to be a multiple
// of the generator polynomial 0x635
NWC24Err NWC24iCheckUserIdCrc(NWC24UserId id) {
    u64 v = getUnScrambleId(id);
    int i;

    for (i = 0; i < 43; i++) {
        if ((v >> (53 - (i + 1))) & 1) {
            v ^= (u64)0x635 << (42 - i);
        }
    }

    if (v != 0) {
        return NWC24_ERR_ID_CRC;
    }

    return NWC24_OK;
}

// Inverse of the WiiConnect24 ID nibble substitution. The byte permutation
// is reversed separately; the upper five bits bypass the substitution.
static const u8 IdInverseNibble[16] = {
    13, 5, 9, 7, 0, 15, 10, 2, 12, 3, 14, 1, 8, 6, 11, 4
};

static inline u8 UnscrambleByte(u8 value) {
    return (IdInverseNibble[value >> 4] << 4) | IdInverseNibble[value & 15];
}

// Undo the XOR, 53-bit rotation, byte permutation, nibble substitution and
// final rotation/XOR so the caller can validate the original CRC polynomial.
static u64 getUnScrambleId(u64 id) {
    u64 value = (id & 0x001FFFFFFFFFFFFFULL) ^ 0x00005E5E5E5E5E5EULL;
    u64 decoded;

    value = (value >> 1) | ((value & 1) << 52);
    decoded = value;
    decoded = (decoded & ~(0xFFULL << 16)) | ((u64)UnscrambleByte(value >> 0) << 16);
    decoded = (decoded & ~(0xFFULL << 0)) | ((u64)UnscrambleByte(value >> 8) << 0);
    decoded = (decoded & ~(0xFFULL << 8)) | ((u64)UnscrambleByte(value >> 40) << 8);
    decoded = (decoded & ~(0xFFULL << 40)) | ((u64)UnscrambleByte(value >> 24) << 40);
    decoded = (decoded & ~(0xFFULL << 32)) | ((u64)UnscrambleByte(value >> 16) << 32);
    decoded = (decoded & ~(0xFFULL << 24)) | ((u64)UnscrambleByte(value >> 32) << 24);
    return (((decoded << 10) | (decoded >> 43)) & 0x001FFFFFFFFFFFFFULL) ^ 0x0000B3B3B3B3B3B3ULL;
}
