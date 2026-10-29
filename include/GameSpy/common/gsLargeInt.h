#ifndef GAMESPY_COMMON_GSLARGEINT_H
#define GAMESPY_COMMON_GSLARGEINT_H

// GameSpy SDK large integers (gsLargeInt.c) and the RFC 3174 SHA-1 that the
// SDK ships next to them (sha1.c). The functions are still unnamed in our
// symbols; the SDK names are noted next to the placeholders.
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GS_LARGEINT_MAX_DIGITS 64

typedef u32 l_word;

typedef struct gsLargeInt_s {
    l_word mLength;                          // 0x0
    l_word mData[GS_LARGEINT_MAX_DIGITS];    // 0x4
} gsLargeInt_t;

__static_assert(sizeof(gsLargeInt_t) == 0x104, "gsLargeInt_t");

typedef struct SHA1Context {
    u32 Intermediate_Hash[5];   // 0x0
    u32 Length_Low;             // 0x14
    u32 Length_High;            // 0x18
    s16 Message_Block_Index;    // 0x1C
    u8 Message_Block[64];       // 0x1E
    int Computed;               // 0x60
    int Corrupted;              // 0x64
} SHA1Context;

__static_assert(sizeof(SHA1Context) == 0x68, "SHA1Context");

int fn_8035EB8C(gsLargeInt_t* lint, l_word length);  // gsiLargeIntResize
int fn_803612EC(gsLargeInt_t* lint);                 // gsLargeIntReverseBytes
int fn_80361878(SHA1Context* context, u8 Message_Digest[20]);  // SHA1Result
void fn_80362090(SHA1Context* context);              // SHA1PadMessage

#ifdef __cplusplus
}
#endif

#endif
