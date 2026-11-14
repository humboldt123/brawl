#include <types.h>
#include <string.h>

// The original calls local helpers out of line (e.g. move_ptr from rewind_ptr).
#pragma auto_inline off

// Function bodies below are taken from https://github.com/jj-link/brawl.

void TMCC_JPEGDEC_IdctBlock1x1(int *arg0, signed char *arg1)
{
    int flag = 0;
    int v = (*arg0 >> 11) + 0x80;

    if (v < 0x100 && v > -1) {
        flag = 1;
    }

    *arg1 = flag ? v : (v < 0 ? 0 : 255);
}

void TMCC_JPEGDEC_IdctBlock1x1_Col(s32 *arg0, s8 *arg1) {
    s32 value;
    s32 valid;

    valid = 0;
    value = *arg0 >> 11;
    if (value < 128 && value > -129) {
        valid = 1;
    }
    *arg1 = valid ? value : (value > 0 ? 127 : -128);
}

// HYPOTHESIS: stream buffer state used by the buffer_system functions.
typedef struct TMCC_JPEGDEC_Stream {
    s32 unk0;
    s32 unk4;
    u8* start;
    u8* cur;
    u8* end;
    u8* limit;
} TMCC_JPEGDEC_Stream;

s32 TMCC_JPEGDEC_get_position(TMCC_JPEGDEC_Stream* stream) {
    return stream->cur - stream->start;
}

s32 TMCC_JPEGDEC_chk_possible_size(TMCC_JPEGDEC_Stream* stream) {
    return stream->end - stream->cur;
}

s32 TMCC_JPEGDEC_set_thumbnaildata(TMCC_JPEGDEC_Stream* stream, u8* data, s32 size) {
    stream->cur = data;
    stream->start = data;
    stream->end = data + size;
    stream->limit = data + size - 10;
    return 0;
}

s32 TMCC_JPEGDEC_load_buff(TMCC_JPEGDEC_Stream* stream) {
    u32 word;
    s32 shift;
    u8 byte;
    u8* cur;
    cur = stream->cur;

    if (cur < stream->limit) {
        word = stream->unk0;
        shift = stream->unk4;
        do {
            byte = *cur++;
            word = (word << 8) + byte;
            shift += 8;
            if (byte == 0xFF) {
                cur++;
            }
        } while (shift <= 0x18);
        stream->unk0 = word;
        stream->unk4 = shift;
        stream->cur = cur;
    } else {
        do {
            if (stream->end <= stream->cur) {
                break;
            }
            cur = stream->cur;
            byte = *cur++;
            stream->cur = cur;
            stream->unk0 = (stream->unk0 << 8) + byte;
            stream->unk4 += 8;
            if (byte == 0xFF) {
                stream->cur = cur + 1;
            }
        } while (stream->unk4 <= 0x18);
    }
    return 0;
}

s32 TMCC_JPEGDEC_init_buff(TMCC_JPEGDEC_Stream* stream) {
    stream->unk0 = 0;
    stream->unk4 = 0;
    return TMCC_JPEGDEC_load_buff(stream);
}

s32 TMCC_JPEGDEC_init_stream(TMCC_JPEGDEC_Stream* stream, u8* data, s32 size) {
    if (data == 0) {
        return -1;
    }
    if (size == 0) {
        return -1;
    }
    if ((u32)data & 0x1F) {
        return -1;
    }
    stream->start = data;
    stream->cur = data;
    stream->end = data + size;
    stream->limit = data + size - 10;
    return 0;
}

s32 TMCC_JPEGDEC_get_byte(u8* out, TMCC_JPEGDEC_Stream* stream) {
    u8* cur = stream->cur;
    u8* end;
    if (cur >= stream->end) {
        return -0x90;
    }
    *out = *cur++;
    end = stream->end;
    stream->cur = cur;
    return cur >= end ? -0x90 : 0;
}

s32 TMCC_JPEGDEC_get_sbyte(u8* out, u32 count, TMCC_JPEGDEC_Stream* stream) {
    u32 i;
    s32 result;
    u8* end;
    u8* cur;
    u8 value;
    for (i = 0; i < count; i++) {
        cur = stream->cur;
        end = stream->end;
        if (cur >= end) {
            result = -0x90;
        } else {
            stream->cur = cur + 1;
            value = *cur;
            result = (cur + 1 >= end) ? -0x90 : 0;
        }
        *out++ = value;
        if (result < 0) {
            return result;
        }
    }
    return 0;
}

s32 TMCC_JPEGDEC_move_ptr(s32 offset, TMCC_JPEGDEC_Stream* stream) {
    s32 back;
    if (offset >= 0) {
        if (stream->end - stream->cur >= offset) {
            stream->cur = stream->cur + offset;
            if (stream->end <= stream->cur) {
                return -0x90;
            }
        } else {
            return -0x90;
        }
    } else {
        back = -offset;
        if (stream->cur - stream->start >= back) {
            stream->cur = stream->cur - back;
        } else {
            return -0x90;
        }
    }
    return 0;
}

// HYPOTHESIS: decoder context; only the fields the code touches are named.
typedef struct TMCC_JPEGDEC_Context {
    union {
        u8 pad0[0x17DC];
        struct {
            TMCC_JPEGDEC_Stream stream;
            u8 scanCompIdx[4];
            s32 dcPred[4];
            u8 pad2C[0x10];
            u16 unk3C;
            u16 unk3E;
            u32 unk40;
            u8 pad44[0x400];
            u8 zzTbl[64];
        };
        struct {
            u8 pad0_44[0x44];
            s32 qtable[5][64];
            u8 pad544[0x1290];
            u8 qtableFlag[8];
        };
        struct {
            u8 pad0_440[0x440];
            u8* dcTable;
            u8* dcSizes;
            u8* dcCodes;
            u8 pad44C[4];
            u8* acTable;
            u8* acSizes;
            u8* acCodes;
            u8 pad45C[4];
            u8 dcTable0[0x400];
            u8 dcTable1[0x400];
            u8 acTable0[0x400];
            u8 acTable1[0x400];
            u8 dcSizes0[0x44];
            u8 dcSizes1[0x44];
            u8 acSizes0[0x44];
            u8 acSizes1[0x44];
            u8 dcCodes0[0x10];
            u8 dcCodes1[0x10];
            u8 acCodes0[0x10];
            u8 pad15A0[0xF0];
            u8 acCodes1[0x14C];
        };
    };
    u16 unk17DC;
    u16 unk17DE;
    s32 unk17E0;
    s32 unk17E4;
    u8 pad17E8;
    u8 unk17E9;
    u8 unk17EA;
    u8 pad17EB;
    u16 unk17EC;
    u16 unk17EE;
    u8 pad17F0[0x16];
    u16 unk1806;
    u8 unk1808;
} TMCC_JPEGDEC_Context;

s32 TMCC_JPEGDEC_scanstart(TMCC_JPEGDEC_Context* context);
s32 TMCC_JPEGDEC_imagestart(TMCC_JPEGDEC_Context* context);

s32 TMCC_JPEGDEC_Decompscan(TMCC_JPEGDEC_Context* context) {
    s32 result = TMCC_JPEGDEC_scanstart(context);
    if (result < 0) {
        return result;
    }
    if (context->unk1808 == 1) {
        return 0;
    }
    return result;
}

s32 TMCC_JPEGDEC_HeaderAnalyze(TMCC_JPEGDEC_Context* context) {
    s32 result = TMCC_JPEGDEC_imagestart(context);
    if (result < 0) {
        return result;
    }
    if (context->unk17DC == 0) {
        result = -0x50;
    } else {
        context->unk17E0 = context->unk17E9 * context->unk17EC;
        context->unk17E4 = context->unk17EA * context->unk17EE;
    }
    return result;
}

void TMCC_JPEGDEC_set_entropytbl(TMCC_JPEGDEC_Context* context, s32 dcSelect, s32 acSelect) {
    switch (dcSelect) {
    case 0:
        context->dcTable = context->dcTable0;
        context->dcCodes = context->dcCodes0;
        context->dcSizes = context->dcSizes0;
        break;
    case 1:
        context->dcTable = context->dcTable1;
        context->dcCodes = context->dcCodes1;
        context->dcSizes = context->dcSizes1;
        break;
    }
    switch (acSelect) {
    case 0:
        context->acTable = context->acTable0;
        context->acCodes = context->acCodes0;
        context->acSizes = context->acSizes0;
        break;
    case 1:
        context->acTable = context->acTable1;
        context->acCodes = context->acCodes1;
        context->acSizes = context->acSizes1;
        break;
    }
}

// HYPOTHESIS: image state used by the RGB565 tile functions; only touched fields are named.
typedef struct TMCC_JPEGDEC_Image {
    u8 pad0[0x30];
    u16 validWidth;
    u16 validHeight;
    u8 pad34[0x14];
    u32 width;
    u32 height;
    u8 pad50[0x14];
    u16* pixels;
} TMCC_JPEGDEC_Image;

void TMCC_JPEGDEC_fillerBlankTileRGB565(TMCC_JPEGDEC_Image* image) {
    u16* pixels = image->pixels;
    u32 tilesPerRow = image->width >> 2;
    u32 x;
    u32 y;
    for (y = 0; y < image->validHeight; y++) {
        for (x = image->validWidth; x < image->width; x++) {
            pixels[(((y >> 2) * tilesPerRow + (x >> 2)) << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
        }
    }
    for (y = image->validHeight; y < image->height; y++) {
        for (x = 0; x < image->width; x++) {
            pixels[(((y >> 2) * tilesPerRow + (x >> 2)) << 4) + ((y & 3) << 2) + (x & 3)] = 0xFFFF;
        }
    }
}

void TMCC_JPEGDEC_IdctBlock2x2(s32* block, u8* out, s32 stride) {
    s32 flag = 0;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 sum0;
    s32 diff0;
    s32 sum1;
    s32 diff1;

    a = block[0] + 0x40000;
    b = block[1];
    sum0 = a + b;
    diff0 = a - b;
    c = block[8];
    d = block[9];
    sum1 = c + d;
    diff1 = c - d;

    v = (sum0 + sum1) >> 11;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[0] = flag ? v : (v < 0 ? 0 : 255);

    v = (sum0 - sum1) >> 11;
    flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[stride] = flag ? v : (v < 0 ? 0 : 255);

    v = (diff0 + diff1) >> 11;
    flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[1] = flag ? v : (v < 0 ? 0 : 255);

    v = (diff0 - diff1) >> 11;
    flag = 0;
    if (v < 0x100 && v > -1) {
        flag = 1;
    }
    out[stride + 1] = flag ? v : (v < 0 ? 0 : 255);
}

void TMCC_JPEGDEC_IdctBlock2x2_Col(s32* block, s8* out) {
    s32 flag = 0;
    s32 v;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 sum0;
    s32 diff0;
    s32 sum1;
    s32 diff1;

    a = block[0];
    b = block[1];
    c = block[8];
    d = block[9];
    sum0 = a + b;
    diff0 = a - b;
    sum1 = c + d;
    diff1 = c - d;

    v = (sum0 + sum1) >> 11;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[0] = flag ? v : (v > 0 ? 127 : -128);

    v = (sum0 - sum1) >> 11;
    flag = 0;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[8] = flag ? v : (v > 0 ? 127 : -128);

    v = (diff0 + diff1) >> 11;
    flag = 0;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[1] = flag ? v : (v > 0 ? 127 : -128);

    v = (diff0 - diff1) >> 11;
    flag = 0;
    if (v < 128 && v > -129) {
        flag = 1;
    }
    out[9] = flag ? v : (v > 0 ? 127 : -128);
}

s32 TMCC_JPEGDEC_move_ptr(s32 offset, TMCC_JPEGDEC_Stream* stream);

s32 TMCC_JPEGDEC_rewind_ptr(TMCC_JPEGDEC_Stream* stream) {
    u32 word = stream->unk0;
    s32 shift = stream->unk4;
    s32 result;

    shift -= 8;
    while (shift >= 0) {
        result = TMCC_JPEGDEC_move_ptr(-1, stream);
        if (result < 0) {
            return result;
        }
        if ((word & 0xFF) == 0xFF) {
            result = TMCC_JPEGDEC_move_ptr(-1, stream);
            if (result < 0) {
                return result;
            }
        }
        word >>= 8;
        shift -= 8;
    }
    stream->unk4 = 0;
    stream->unk0 = 0;
    return 0;
}

// HYPOTHESIS: the context starts with the stream; unk19D0 points at a status block.
typedef struct TMCC_JPEGDEC_Status {
    u8 pad0[4];
    s32 error;
    u8 pad8[0x25];
    u8 mode;
} TMCC_JPEGDEC_Status;

typedef struct TMCC_JPEGDEC_Ctx2 {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x17F0];
    u8 unk1808;
    u8 pad1809[0x1C7];
    TMCC_JPEGDEC_Status* status;
} TMCC_JPEGDEC_Ctx2;

s32 TMCC_JPEGDEC_get_wbyte(u16* out, TMCC_JPEGDEC_Stream* stream);

s32 TMCC_JPEGDEC_imageend(TMCC_JPEGDEC_Ctx2* context) {
    u16 marker;
    s32 result;

    if (context->status->mode == 1) {
        return 0;
    }
    if (context->unk1808 == 0) {
        result = TMCC_JPEGDEC_rewind_ptr(&context->stream);
        if (result < 0) {
            return result;
        }
        result = TMCC_JPEGDEC_get_wbyte(&marker, &context->stream);
        if (result < 0) {
            if (result != -0x90) {
                return result;
            }
        }
        if (marker != 0xFFD9 && marker != 0xFFDC) {
            context->status->error = -0x21;
            return 0;
        }
    }
    return 0;
}

// HYPOTHESIS: EXIF thumbnail check; layouts inferred from the offsets used.
typedef struct TMCC_JPEGDEC_ExifCtx TMCC_JPEGDEC_ExifCtx;

typedef struct TMCC_JPEGDEC_ExifInfo {
    TMCC_JPEGDEC_ExifCtx* ctx;
    u8 kind;
    u8 pad5[3];
    u32 limit;
    u8 pad0C[0x10];
    u8 unk1C;
    u8 unk1D;
} TMCC_JPEGDEC_ExifInfo;

typedef struct TMCC_JPEGDEC_ExifStatus {
    TMCC_JPEGDEC_ExifCtx* ctx;
    u8 pad4[4];
    s32 kind;
    u8 pad0C[4];
    s32 unk10;
    u8 pad14[4];
    s32 unk18;
    u8 pad1C[0x10];
    u8 unk2C;
    u8 unk2D;
    u8 pad2E[0x3A];
    u8 unk68[0x654];
    u16 unk6BC;
    u8 pad6BE[0x16];
    u32 offset;
    u32 length;
    u32 base;
    u8 pad6E0[0x1C];
} TMCC_JPEGDEC_ExifStatus;

struct TMCC_JPEGDEC_ExifCtx {
    TMCC_JPEGDEC_Stream stream;
    u8 pad18[0x19B8];
    TMCC_JPEGDEC_ExifStatus* status;
};

s32 TMCC_JPEGDEC_ThumbnailCheck(TMCC_JPEGDEC_ExifInfo* info, TMCC_JPEGDEC_ExifCtx* context, u32 arg2, u32 arg3) {
    TMCC_JPEGDEC_ExifStatus* status = context->status;
    u32 end;
    u32 size;
    s32 result;

    if (status->offset == 0) {
        return -0xA0;
    }
    if (status->length == 0) {
        return -0xA0;
    }
    if (status->unk6BC != 6) {
        return -0xA0;
    }
    end = status->base - arg3 + (status->length + status->offset);
    if (end > info->limit) {
        return -0xA0;
    }
    size = status->length - (end - arg2);
    if (end <= arg2) {
        size = status->length;
    }
    if (info->unk1C != 1) {
        return -1;
    }
    if (info->kind != 4 && info->kind != 6) {
        return -4;
    }
    if (info->unk1D != 1) {
        return -1;
    }
    status->unk2D = 0;
    status->unk2C = info->unk1D;
    status->kind = info->kind;
    result = TMCC_JPEGDEC_set_thumbnaildata(&context->stream, (u8*)(status->base + status->offset), size);
    return result < 0 ? result : 0;
}

s32 TMCC_JPEGDEC_get_wbyte(u16* out, TMCC_JPEGDEC_Stream* stream) {
    u8* next;
    u8* end;
    u8* cur;
    u8 byte;
    u32 acc;
    s32 result;

    end = stream->end;
    cur = stream->cur;
    if (cur >= end) {
        result = -0x90;
    } else {
        next = cur + 1;
        stream->cur = next;
        byte = *cur;
        result = (next >= end) ? -0x90 : 0;
        cur = next;
    }
    acc = byte << 8;
    if (result < 0) {
        return result;
    }
    if (cur >= end) {
        result = -0x90;
    } else {
        u8* p = stream->cur;
        end = stream->end;
        next = p + 1;
        stream->cur = next;
        byte = *p;
        result = (next >= end) ? -0x90 : 0;
    }
    *out = acc + byte;
    return result < 0 ? result : 0;
}

// HYPOTHESIS: one Huffman code-length slot, two big-endian halfwords.
typedef struct TMCC_JPEGDEC_HuffPair {
    u16 maxcode;
    u16 offset;
} TMCC_JPEGDEC_HuffPair;

s32 TMCC_JPEGDEC_vl_decode_rc(u8* table, u8* vals, TMCC_JPEGDEC_Stream* stream) {
    TMCC_JPEGDEC_HuffPair* entry;
    TMCC_JPEGDEC_HuffPair pair;
    u32 word;
    u16 maxcode;
    u32 bits;
    u32 code;
    u32 len;
    u32 bit;
    u32 index;
    s32 result;

    if (stream->unk4 <= 0x11) {
        result = TMCC_JPEGDEC_load_buff(stream);
        if (result < 0) {
            return result;
        }
    }
    entry = (TMCC_JPEGDEC_HuffPair*)(table + 0x24);
    bits = (u32)stream->unk4 - 9;
    code = (*(u32*)&stream->unk0 >> bits) & 0x1FF;
    stream->unk4 = (s32)bits;
    len = 9;
    goto check;
    for (;;) {
        len += 1;
        entry += 1;
        if (len > 0x10) {
            return -0x64;
        }
        bits = (u32)stream->unk4 - 1;
        bit = *(u32*)&stream->unk0 >> bits;
        bit &= 1;
        code <<= 1;
        code |= bit;
        stream->unk4 = (s32)bits;
    check:
        pair = *entry;
        word = *(u32*)&pair;
        maxcode = *(u16*)&word;
        if (code <= maxcode) {
            index = code - pair.maxcode;
            index = index + pair.offset;
            return vals[index];
        }
    }
}

s32 TMCC_JPEGDEC_get_byte(u8* out, TMCC_JPEGDEC_Stream* stream);
s32 TMCC_JPEGDEC_init_buff(TMCC_JPEGDEC_Stream* stream);
s32 fn_802635B4(void* unused, TMCC_JPEGDEC_Context* ctx);

typedef struct TMCC_JPEGDEC_ScaleTable {
    s32 vals[64];
} TMCC_JPEGDEC_ScaleTable;

extern const TMCC_JPEGDEC_ScaleTable lbl_8040DBE0;
extern const u8 lbl_8040DD10[64];

// HYPOTHESIS: Huffman & quant table area at ctx + 0x44
typedef struct TMCC_JPEGDEC_Tables {
    s32 qvals[4][64];
    u8 pad400[0x60];
    u8 dcLut[2][0x400];
    u8 acLut[2][0x400];
    TMCC_JPEGDEC_HuffPair dcPairs[2][17];
    TMCC_JPEGDEC_HuffPair acPairs[2][17];
    u8 dcVals[2][0x10];
    u8 acVals[2][0x100];
    u8 qFlags[4];
    u8 dcFlags[2];
    u8 acFlags[2];
} TMCC_JPEGDEC_Tables;

typedef struct TMCC_JPEGDEC_HuffOut {
    u8* table;
    u8* sizes;
    u8* codes;
    u8 count;
} TMCC_JPEGDEC_HuffOut;

void fn_80260DC4(TMCC_JPEGDEC_HuffOut* out, s32 dcAc, s32 id, TMCC_JPEGDEC_Tables* tbl) {
    if (dcAc == 0) {
        switch (id) {
        case 0:
            out->table = tbl->dcLut[0];
            out->codes = tbl->dcVals[0];
            out->sizes = (u8*)tbl->dcPairs[0];
            tbl->dcFlags[0] = 1;
            memset(tbl->dcLut[0], 0, 0x400);
            memset(tbl->dcVals[0], 0, 0x10);
            memset(tbl->dcPairs[0], 0, 0x44);
            break;
        case 1:
            out->table = tbl->dcLut[1];
            out->codes = tbl->dcVals[1];
            out->sizes = (u8*)tbl->dcPairs[1];
            tbl->dcFlags[1] = 1;
            memset(tbl->dcLut[1], 0, 0x400);
            memset(tbl->dcVals[1], 0, 0x10);
            memset(tbl->dcPairs[1], 0, 0x44);
            break;
        }
    } else {
        switch (id) {
        case 0:
            out->table = tbl->acLut[0];
            out->codes = tbl->acVals[0];
            out->sizes = (u8*)tbl->acPairs[0];
            tbl->acFlags[0] = 1;
            memset(tbl->acLut[0], 0, 0x400);
            memset(tbl->acVals[0], 0, 0x100);
            memset(tbl->acPairs[0], 0, 0x44);
            break;
        case 1:
            out->table = tbl->acLut[1];
            out->codes = tbl->acVals[1];
            out->sizes = (u8*)tbl->acPairs[1];
            tbl->acFlags[1] = 1;
            memset(tbl->acLut[1], 0, 0x400);
            memset(tbl->acVals[1], 0, 0x100);
            memset(tbl->acPairs[1], 0, 0x44);
            break;
        }
    }
}

typedef struct TMCC_JPEGDEC_DqtTable {
    s32 vals[5][64];
    u8 pad544[0x1290];
    u8 flags[8];
} TMCC_JPEGDEC_DqtTable;

#pragma use_lmw_stmw on

s32 fn_80263794(TMCC_JPEGDEC_Context* ctx) {
    TMCC_JPEGDEC_ScaleTable scale = lbl_8040DBE0;
    TMCC_JPEGDEC_DqtTable* dqt = (TMCC_JPEGDEC_DqtTable*)ctx->qtable;
    u16 len;
    u8 id;
    u8 val;
    s32 i;
    s32 result;

    result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    len -= 2;

    do {
        len -= 0x41;
        result = TMCC_JPEGDEC_get_byte(&id, &ctx->stream);
        if (result < 0) {
            return result;
        }
        if (id > 4) {
            return -0x41;
        }
        dqt->flags[id] = 1;
        for (i = 0; i < 64; i++) {
            s32 zz = lbl_8040DD10[i];
            if (zz > 0x3F) {
                return -0x41;
            }
            result = TMCC_JPEGDEC_get_byte(&val, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if ((dqt->vals[id][zz] = val * scale.vals[zz]) == 0) {
                return -0x41;
            }
        }
    } while (len != 0);

    return 0;
}

#pragma use_lmw_stmw off

// HYPOTHESIS: scan geometry sub-struct at ctx + 0x17DC
typedef struct TMCC_JPEGDEC_ScanSub {
    u16 width;
    u16 height;
    s32 unk4;
    s32 unk8;
    u8 unkC;
    u8 mcuW;
    u8 mcuH;
    u8 padF;
    u16 mcusPerRow;
    u16 mcusPerCol;
    u32 totalMcus;
    u8 remX;
    u8 remY;
    u8 numComps;
    u8 scanNumComps;
    u8 compBlocks[4];
    u8 compH[4];
    u8 compV[4];
    u8 maxH;
    u8 maxV;
} TMCC_JPEGDEC_ScanSub;

s32 fn_80262F7C(TMCC_JPEGDEC_Context* ctx) {
    s32 i;
    TMCC_JPEGDEC_ScanSub* sub = (TMCC_JPEGDEC_ScanSub*)((u8*)ctx + 0x17DC);

    sub->unkC |= 0x100;
    if (sub->scanNumComps == 1) {
        u8 idx = ctx->scanCompIdx[0];
        sub->mcuW = (sub->maxH << 3) / sub->compH[idx];
        sub->mcuH = (sub->maxV << 3) / sub->compV[idx];
        sub->mcusPerRow = sub->width / sub->mcuW;
        sub->mcusPerCol = sub->height / sub->mcuH;
    } else {
        sub->mcuW = sub->maxH << 3;
        sub->mcuH = sub->maxV << 3;
        sub->mcusPerRow = sub->width / sub->mcuW;
        sub->mcusPerCol = sub->height / sub->mcuH;
    }

    sub->remX = sub->width % sub->mcuW;
    sub->remY = sub->height % sub->mcuH;
    sub->mcusPerRow += (sub->remX != 0);
    sub->mcusPerCol += (sub->remY != 0);
    sub->totalMcus = sub->mcusPerRow * sub->mcusPerCol;

    for (i = 0; i < sub->scanNumComps; i++) {
        u8 idx = ctx->scanCompIdx[i];
        sub->compBlocks[i] = (sub->scanNumComps == 1) ? 1 : (sub->compH[idx] * sub->compV[idx]);
    }
    for (; i < 4; i++) {
        sub->compBlocks[i] = 0;
    }

    return 0;
}

typedef struct TMCC_JPEGDEC_SosTable {
    u8 scanCompIdx[4];
    u8 dcSel[4];
    u8 acSel[4];
    u8 padC[8];
    u8 compId[4];
    u8 compQIdx[4];
    u8 compDcSel[4];
    u8 compAcSel[4];
} TMCC_JPEGDEC_SosTable;

#pragma use_lmw_stmw on

s32 fn_80263C58(TMCC_JPEGDEC_Context* ctx) {
    s32 i;
    TMCC_JPEGDEC_SosTable* sos = (TMCC_JPEGDEC_SosTable*)((u8*)ctx + 0x18);
    TMCC_JPEGDEC_DqtTable* dqt = (TMCC_JPEGDEC_DqtTable*)ctx->qtable;
    u16 len;
    u8 byte;
    u8* compPtr;
    s32 result;

    result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (len < 2) {
        return -0x51;
    }

    result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if ((*((u8*)ctx + 0x17F7) = byte) > 4 || *((u8*)ctx + 0x17F7) != *((u8*)ctx + 0x17F6)) {
        return -0x51;
    }

    for (i = 0; i < *((u8*)ctx + 0x17F7); i++) {
        s32 j;
        s32 dcSel;
        s32 acSel;
        s32 numComps;
        s32 id;

        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        numComps = *((u8*)ctx + 0x17F6);
        for (j = 0; j < numComps; j++) {
            id = byte;
            if (id == (s32)sos->compId[j]) {
                sos->scanCompIdx[i] = j;
                compPtr = &sos->scanCompIdx[i];
                goto found;
            }
        }
        return -0x51;

    found:
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            return result;
        }
        dcSel = (s32)byte >> 4;
        acSel = byte & 0xF;
        if (dcSel > 1 || acSel > 1) {
            return -0x51;
        }

        sos->compDcSel[*compPtr] = dcSel;
        sos->compAcSel[*compPtr] = acSel;
        if (dqt->flags[4 + dcSel] != 1) {
            return -0x40;
        }
        if (dqt->flags[6 + acSel] != 1) {
            return -0x40;
        }
        if (dqt->flags[compPtr[0x18]] != 1) {
            return -0x41;
        }
    }

    result = TMCC_JPEGDEC_move_ptr(3, &ctx->stream);
    return result < 0 ? result : 0;
}

#pragma use_lmw_stmw off

static inline s32 skip_app_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 len;
    s32 result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (len < 2) {
        return -0x45;
    }
    len -= 2;
    result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
    result = result < 0 ? result : 0;
    return result < 0 ? result : 0;
}

static inline s32 parse_dri_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 val;
    s32 result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (val != 4) {
        return -0x42;
    }
    result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk1806 = val;
    return 0;
}

static inline s32 parse_dnl_inline(TMCC_JPEGDEC_Context* ctx) {
    u16 val;
    s32 result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (val != 4) {
        return -0x43;
    }
    result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk17DE = val;
    return 0;
}

static inline s32 skip_com_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 len;
    s32 result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (len < 2) {
        return -0x44;
    }
    len -= 2;
    result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
    result = result < 0 ? result : 0;
    return result < 0 ? result : 0;
}

#pragma use_lmw_stmw on

s32 fn_802632F4(u16* out, TMCC_JPEGDEC_Context* ctx) {
    s32 done = 0;
    u16 initMarker;
    u16 marker;
    s32 result;

    initMarker = *out;

    do {
        result = TMCC_JPEGDEC_get_wbyte(&marker, &ctx->stream);
        if (result < 0) {
            return result;
        }
        while (marker == 0xFFFF) {
            u8 b;
            result = TMCC_JPEGDEC_get_byte(&b, &ctx->stream);
            if (result < 0) {
                return result;
            }
            marker = b | 0xFF00;
        }

        if (marker >= 0xFFE0 && marker <= 0xFFEF) {
            result = skip_app_marker(ctx);
        } else {
            switch (marker) {
            case 0xFFC4:
                result = fn_802635B4((void*)initMarker, ctx);
                break;
            case 0xFFDB:
                result = fn_80263794(ctx);
                break;
            case 0xFFDD:
                result = parse_dri_marker(ctx);
                break;
            case 0xFFDC:
                result = parse_dnl_inline(ctx);
                break;
            case 0xFFFE:
                result = skip_com_marker(ctx);
                break;
            case 0xFFC0:
                done = 1;
                break;
            case 0xFFC2:
                done = 1;
                break;
            case 0xFFDA:
                done = 1;
                break;
            case 0xFFD9:
                ctx->unk1808 = 1;
                done = 1;
                break;
            default:
                result = -0x2F;
                break;
            }
        }
        if (result < 0) {
            done = 1;
        }
    } while (!done);

    *out = marker;
    return result;
}

#pragma use_lmw_stmw off

static inline s32 parse_dnl_marker(TMCC_JPEGDEC_Context* ctx) {
    u16 val;
    s32 result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (val != 4) {
        return -0x43;
    }
    result = TMCC_JPEGDEC_get_wbyte(&val, &ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk17DE = val;
    return 0;
}

static inline s32 find_dnl(TMCC_JPEGDEC_Context* ctx) {
    u8 byte;
    u32 word = 0;
    s32 result;
    TMCC_JPEGDEC_Stream saved = ctx->stream;

    while ((u16)word != 0xFFDC) {
        result = TMCC_JPEGDEC_get_byte(&byte, &ctx->stream);
        if (result < 0) {
            break;
        }
        word = ((word & 0xFF) << 8) | byte;
    }

    if ((u16)word == 0xFFDC) {
        result = parse_dnl_marker(ctx);
        if (result < 0) {
            return result;
        }
    }

    ctx->stream = saved;
    return 0;
}

s32 TMCC_JPEGDEC_scanstart(TMCC_JPEGDEC_Context* ctx) {
    u16 marker;
    s32 result;

    marker = 0;
    result = fn_802632F4(&marker, ctx);
    if (result < 0) {
        return result;
    }
    if (marker != 0xFFDA) {
        return -0x22;
    }

    result = fn_80263C58(ctx);
    if (result < 0) {
        return result;
    }

    result = find_dnl(ctx);
    if (result < 0) {
        return result;
    }

    if (ctx->unk17DE == 0) {
        return -0x50;
    }

    result = fn_80262F7C(ctx);
    if (result < 0) {
        return result;
    }

    ctx->dcPred[0] = 0;
    ctx->dcPred[1] = 0;
    ctx->dcPred[2] = 0;
    ctx->dcPred[3] = 0;
    ctx->unk3C = 0;
    result = TMCC_JPEGDEC_init_buff(&ctx->stream);
    if (result < 0) {
        return result;
    }
    ctx->unk3E = 0;
    ctx->unk40 = 0;
    return 0;
}

typedef struct TMCC_JPEGDEC_Unaligned16 {
    u8 b0;
    u8 b1;
} TMCC_JPEGDEC_Unaligned16;

s32 fn_802644D4(u8* data, u16 size, void* info);
s32 fn_8026260C(TMCC_JPEGDEC_ExifCtx* ctx);
s32 fn_80267980(TMCC_JPEGDEC_ExifCtx* ctx);
s32 fn_80265EB0(TMCC_JPEGDEC_ExifCtx* ctx);

#pragma use_lmw_stmw on

s32 fn_80264074(s32* outOffset, s32* outSize, u8* buf, TMCC_JPEGDEC_ExifInfo* info) {
    TMCC_JPEGDEC_ExifCtx* ctx = info->ctx;
    s32 result;
    u16 len;
    u16 marker;
    u16 soi;
    u8 sig[4];

    if (ctx == 0) {
        return -1;
    }

    memset(ctx, 0, sizeof(TMCC_JPEGDEC_ExifCtx));
    result = TMCC_JPEGDEC_init_stream(&ctx->stream, buf, info->limit);
    if (result < 0) {
        return result;
    }

    result = TMCC_JPEGDEC_get_wbyte(&soi, &ctx->stream);
    if (result < 0) {
        return result;
    }
    if (soi != 0xFFD8) {
        return -0x20;
    }

    do {
        result = TMCC_JPEGDEC_get_wbyte(&marker, &ctx->stream);
        if (result < 0) {
            return result;
        }
        if (marker == 0xFFE1) {
            result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if (len < 2) {
                return -0x45;
            }
            result = TMCC_JPEGDEC_get_sbyte(sig, 4, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if (sig[0] == 'E' && sig[1] == 'x' && sig[2] == 'i' && sig[3] == 'f') {
                *outOffset = TMCC_JPEGDEC_get_position(&ctx->stream) - 8;
                *outSize = len + 2;
                return 0;
            }
            return -0x45;
        } else {
            result = TMCC_JPEGDEC_get_wbyte(&len, &ctx->stream);
            if (result < 0) {
                return result;
            }
            if (len < 2) {
                return -0x45;
            }
            len -= 2;
            if (marker >= 0xFFE0 && marker <= 0xFFEF) {
                result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
                if (result < 0) {
                    return result;
                }
            } else {
                switch (marker) {
                case 0xFFC0:
                case 0xFFC2:
                case 0xFFC4:
                case 0xFFDB:
                case 0xFFDC:
                case 0xFFDD:
                case 0xFFFE:
                    result = TMCC_JPEGDEC_move_ptr(len, &ctx->stream);
                    if (result < 0) {
                        return result;
                    }
                    break;
                case 0xFFDA:
                    return -2;
                case 0xFFD9:
                    return -2;
                default:
                    return -0x2F;
                }
            }
        }
    } while (TMCC_JPEGDEC_chk_possible_size(&ctx->stream) > 4);

    return -2;
}

s32 fn_802642F0(TMCC_JPEGDEC_ExifStatus* status, u8* data, TMCC_JPEGDEC_ExifInfo* info) {
    TMCC_JPEGDEC_ExifCtx* ctx = info->ctx;
    s32 result;
    u32 raw;
    u16 marker;
    u16 fullLen;
    u16 len;

    if (ctx == 0) {
        return -1;
    }

    memset(status, 0, sizeof(TMCC_JPEGDEC_ExifStatus));
    memset(ctx, 0, sizeof(TMCC_JPEGDEC_ExifCtx));
    ctx->status = status;
    status->ctx = ctx;

    raw = ((TMCC_JPEGDEC_Unaligned16*)data)->b0 | (((TMCC_JPEGDEC_Unaligned16*)data)->b1 << 8);
    marker = ((raw >> 8) & 0xFF) | ((raw & 0xFF) << 8);
    if (marker != 0xFFE1) {
        return -0x45;
    }

    raw = ((TMCC_JPEGDEC_Unaligned16*)(data + 2))->b0 | (((TMCC_JPEGDEC_Unaligned16*)(data + 2))->b1 << 8);
    len = ((raw >> 8) & 0xFF) | ((raw & 0xFF) << 8);
    if (len + 2 > info->limit) {
        return -0x91;
    }
    if (len < 2) {
        return -0x45;
    }

    fullLen = len + 2;
    result = fn_802644D4(data + 10, (u16)(len - 8), status->unk68);
    if (result < 0) {
        return result;
    }

    if (info->unk1C == 0) {
        return 0;
    }
    if (info->unk1C == 1) {
        result = TMCC_JPEGDEC_ThumbnailCheck(info, ctx, fullLen, (u32)data);
        if (result < 0) {
            return result;
        }
    } else {
        return -1;
    }

    result = TMCC_JPEGDEC_HeaderAnalyze((TMCC_JPEGDEC_Context*)ctx);
    if (result < 0) {
        goto err;
    }
    result = TMCC_JPEGDEC_Decompscan((TMCC_JPEGDEC_Context*)ctx);
    if (result < 0) {
        goto err;
    }
    if (((TMCC_JPEGDEC_Context*)ctx)->unk1808 != 0) {
        goto err;
    }
    result = fn_8026260C(ctx);
    if (result < 0) {
        goto err;
    }

    switch (status->kind) {
    case 4:
        result = fn_80267980(ctx);
        if (result < 0) {
            goto err;
        }
        break;
    case 5:
        break;
    case 6:
        result = fn_80265EB0(ctx);
        if (result < 0) {
            goto err;
        }
        break;
    default:
        break;
    }

    status->unk10 = TMCC_JPEGDEC_get_position(&ctx->stream);
    return status->unk18;

err:
    return result < 0 ? result : -2;
}

#pragma use_lmw_stmw off
