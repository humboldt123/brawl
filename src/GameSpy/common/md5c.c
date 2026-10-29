#include <types.h>
#include <string.h>
#include <GameSpy/common/md5.h>

// Function bodies below are derived from https://github.com/jj-link/brawl and follow
// the RSA reference implementation that the GameSpy SDK ships as md5c.c.

// MD5 padding (0x80 followed by zeros)
extern u8 lbl_8048E7B8[64];

void fn_8035B34C(MD5_CTX* context) {
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
}

void fn_8035B38C(MD5_CTX* context, const u8* input, u32 inputLen) {
    u32 i, index, partLen;

    index = (u32)((context->count[0] >> 3) & 0x3F);
    if ((context->count[0] += ((u32)inputLen << 3)) < ((u32)inputLen << 3)) {
        context->count[1]++;
    }
    context->count[1] += ((u32)inputLen >> 29);
    partLen = 64 - index;
    if (inputLen >= partLen) {
        memcpy(&context->buffer[index], input, partLen);
        fn_8035B5FC(context->state, context->buffer);
        for (i = partLen; i + 63 < inputLen; i += 64) {
            fn_8035B5FC(context->state, &input[i]);
        }
        index = 0;
    } else {
        i = 0;
    }
    memcpy(&context->buffer[index], &input[i], inputLen - i);
}

// Encodes input (u32) into output (u8), little-endian.
static inline void Encode(u8* output, const u32* input, u32 len) {
    u32 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (u8)(input[i] & 0xff);
        output[j + 1] = (u8)((input[i] >> 8) & 0xff);
        output[j + 2] = (u8)((input[i] >> 16) & 0xff);
        output[j + 3] = (u8)((input[i] >> 24) & 0xff);
    }
}

void fn_8035B478(u8 digest[16], MD5_CTX* context) {
    u8 bits[8];
    u32 index, padLen;

    Encode(bits, context->count, 8);
    index = (u32)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    fn_8035B38C(context, lbl_8048E7B8, padLen);
    fn_8035B38C(context, bits, 8);
    Encode(digest, context->state, 16);
    memset(context, 0, sizeof(*context));
}
