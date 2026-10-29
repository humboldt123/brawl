#ifndef GAMESPY_COMMON_MD5_H
#define GAMESPY_COMMON_MD5_H

// RSA reference MD5 as shipped in the GameSpy SDK (md5c.c). Our symbols do
// not name these functions yet; the SDK names are noted next to them.
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MD5_CTX {
    u32 state[4];        // 0x0  state (ABCD)
    u32 count[2];        // 0x10 number of bits, modulo 2^64 (lsb first)
    u8 buffer[64];       // 0x18 input buffer
} MD5_CTX;

__static_assert(sizeof(MD5_CTX) == 0x58, "MD5_CTX");

void fn_8035B34C(MD5_CTX* context);                                // MD5Init
void fn_8035B38C(MD5_CTX* context, const u8* input, u32 inputLen); // MD5Update
void fn_8035B5FC(u32 state[4], const u8 block[64]);                // MD5Transform
void fn_8035B478(u8 digest[16], MD5_CTX* context);                 // MD5Final

#ifdef __cplusplus
}
#endif

#endif
