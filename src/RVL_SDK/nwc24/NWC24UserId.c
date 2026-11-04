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
static inline u8 GetIdByte(u64 id, unsigned int pos) {
    return (id >> (pos * 8)) & 0xFF;
}

static u64 getUnScrambleId(u64 id) {
    u64 value = (id & 0x001FFFFFFFFFFFFFULL) ^ 0x00005E5E5E5E5E5EULL;
    u64 permuted;

    value = ((value >> 1) | ((value & 1) << 52));
    // Preserve the input bytes while permuting, then substitute in place.
    permuted = value;
    permuted = (permuted & ~(0xFFULL << 0)) | ((u64)(GetIdByte(value, 1)) << 0);
    permuted = (permuted & ~(0xFFULL << 8)) | ((u64)(GetIdByte(value, 5)) << 8);
    permuted = (permuted & ~(0xFFULL << 16)) | ((u64)(GetIdByte(value, 0)) << 16);
    permuted = (permuted & ~(0xFFULL << 24)) | ((u64)(GetIdByte(value, 4)) << 24);
    permuted = (permuted & ~(0xFFULL << 32)) | ((u64)(GetIdByte(value, 2)) << 32);
    permuted = (permuted & ~(0xFFULL << 40)) | ((u64)(GetIdByte(value, 3)) << 40);

    permuted = (permuted & ~(0xFFULL << 0)) | ((u64)(UnscrambleByte(GetIdByte(permuted, 0))) << 0);
    permuted = (permuted & ~(0xFFULL << 8)) | ((u64)(UnscrambleByte(GetIdByte(permuted, 1))) << 8);
    permuted = (permuted & ~(0xFFULL << 16)) | ((u64)(UnscrambleByte(GetIdByte(permuted, 2))) << 16);
    permuted = (permuted & ~(0xFFULL << 24)) | ((u64)(UnscrambleByte(GetIdByte(permuted, 3))) << 24);
    permuted = (permuted & ~(0xFFULL << 32)) | ((u64)(UnscrambleByte(GetIdByte(permuted, 4))) << 32);
    permuted = (permuted & ~(0xFFULL << 40)) | ((u64)(UnscrambleByte(GetIdByte(permuted, 5))) << 40);
    return (((permuted & 0x000007FFFFFFFFFFULL) << 10) | (permuted >> 43)) ^ 0x0000B3B3B3B3B3B3ULL;
}
