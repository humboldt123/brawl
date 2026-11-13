#include <types.h>
#include <string.h>

// BYTES: original emits out-of-line local calls and stmw/lmw prologues
#pragma auto_inline off
#pragma use_lmw_stmw on

// HYPOTHESIS: bitstream writer state
typedef struct TMCC_Bitstream {
    u8* start;
    u8* cur;
    int unk8;
    u8 overflow;
    int capacity;
    u32 total;
} TMCC_Bitstream;

typedef struct TMCC_Encoder TMCC_Encoder;
typedef struct TMCC_UsrParam TMCC_UsrParam;
typedef struct TMCC_ImageInfo TMCC_ImageInfo;
typedef struct TMCC_McuBuf TMCC_McuBuf;
typedef int (*TMCC_CompFn)(int, int, TMCC_ImageInfo*);
typedef int (*TMCC_EdgeFn)(int, int, int, TMCC_ImageInfo*);
typedef void (*TMCC_PutFn)(TMCC_Bitstream*, int, int);
typedef int (*TMCC_CvtFn)(TMCC_McuBuf*, TMCC_UsrParam*, int, int);

// HYPOTHESIS: huffman bit-count / encoding table for a component
typedef struct TMCC_CompHuff {
    int dcMax;
    int unk4;
    int* dcBits;
    int acMax;
    int unk10;
    int* acBits;
} TMCC_CompHuff;

// HYPOTHESIS: component info inside image info
typedef struct TMCC_CompInfo {
    int unk0;
    int stride;
    int dcPred;
    int unkC;
    TMCC_CompHuff* unk10;
    int* unk14;
    int* unk18;
    TMCC_Bitstream* unk1C;
    u8 pad20[0x2C - 0x20];
    u8 unk2C;
    u8 unk2D;
    u8 pad2E[0x34 - 0x2E];
} TMCC_CompInfo;

// HYPOTHESIS: image conversion info at offset 0x1580 of TMCC_Encoder
struct TMCC_ImageInfo {
    u16 width;
    u16 height;
    u16 unk4;
    u16 unk6;
    u8 unk8;
    u8 unk9;
    u8 padA[0x14 - 0xA];
    u8 unk14;
    u8 unk15;
    u8 pad16[0x1C - 0x16];
    TMCC_CompInfo compY;
    TMCC_CompInfo compCb;
    int crUnk0;
    int crStride;
    int crDcPred;
    u8 padCr[0xA8 - 0x90];
    TMCC_UsrParam* usr;
    TMCC_CvtFn cvtFn;
    TMCC_CvtFn cvtEdgeFn;
};

// HYPOTHESIS: encoder state, only the fields these functions touch
struct TMCC_Encoder {
    u8 format;
    u8 pad1;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
    u16 unkA;
    u16 unkC;
    u8 unkE;
    u8 unkF;
    int unk10;
    int unk14;
    int restartCount;
    union {
        TMCC_Bitstream* stream;
        u8 unk1C;
    } u1C;
    union {
        TMCC_PutFn putFn;
        int unk20;
    } u20;
    TMCC_CompFn compFn;
    TMCC_EdgeFn edgeFn;
    const u8* huffSrc[8];
    u8 huffBuf[0x114c - 0x4C];
    struct {
        int unk0;
        void* unk4;
        void* unk8;
    } huffTbl[4];
    u8 pad117C[0x1580 - 0x117C];
    TMCC_ImageInfo img;
    TMCC_Bitstream outStream;
};

// HYPOTHESIS: user parameter block filled from the encoder state
struct TMCC_UsrParam {
    TMCC_Encoder* unk0;
    u16 unk4;
    u16 unk6;
    int unk8;
    u16 unkC;
    u16 unkE;
    int unk10;
    TMCC_PutFn unk14;
    int unk18;
    u16 unk1C;
    u16 unk1E;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    int unk24;
    int unk28;
    u8 unk2C;
    u8 pad2D;
    u8 unk2E;
    u8 pad2F;
    u16 unk30;
    u16 unk32;
    u16 unk34;
    u16 unk36;
    int unk38;
    int unk3C;
    int unk40;
    int unk44;
    int unk48;
    int unk4C;
    int unk50;
    int unk54;
    u8* unk58;
    u8* unk5C;
    u8* unk60;
    u16* unk64;
    u8 pad68[0x6FC - 0x68];
};

// HYPOTHESIS: init parameters
typedef struct TMCC_InitParam {
    TMCC_Encoder* enc;
    int unk4;
    u16 unk8;
    u16 unkA;
    u8 unkC;
    u8 padD;
    u16 unkE;
    u8 unk10;
    u8 pad11[3];
    int unk14;
    u8 pad18[0x9C - 0x18];
    int bufSize;
} TMCC_InitParam;

void TMCC_JPEGENC_set_usr_parameters(TMCC_Encoder*, TMCC_UsrParam*);
void TMCC_JPEGENC_PutStream(TMCC_Bitstream*, int, int);
void TMCC_JPEGENC_PutStreamWithCheck(TMCC_Bitstream*, int, int);
void TMCC_JPEGENC_PutMaker(TMCC_Bitstream*, u16);
int TMCC_JPEGENC_encoder_y411edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y420edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y422edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y411(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y420(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_encoder_y422(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y411edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y420edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y422edge(int, int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y411(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y420(int, int, TMCC_ImageInfo*);
int TMCC_JPEGENC_counter_y422(int, int, TMCC_ImageInfo*);

int fn_8026A62C(TMCC_InitParam*, TMCC_Encoder*);
void fn_8026C0AC(TMCC_Encoder*);
void fn_8026C45C(TMCC_Encoder*);
void fn_8026A7D0(TMCC_Encoder*);
int fn_8026A3B8(TMCC_Encoder*);
void fn_8026BBF0(s8*, int*, int);
void fn_8026BDE8(s8*, int*, int);
int fn_802698AC(int*, TMCC_CompInfo*);
int fn_8026CE80(int*, TMCC_CompInfo*);
int fn_80269C98(TMCC_Encoder*, TMCC_UsrParam*, int);
int fn_8026D53C(TMCC_Encoder*, TMCC_UsrParam*, int, int);
void fn_80269508(TMCC_Bitstream*);
void fn_8026D514(TMCC_Bitstream*, int*);

int fn_8026DCEC(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026DF60(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E104(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E2B4(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E4AC(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026E6A8(TMCC_McuBuf*, TMCC_UsrParam*, int, int);

int fn_8026E9B4(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026EBE4(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026ED68(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026EEF8(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026F0E0(TMCC_McuBuf*, TMCC_UsrParam*, int, int);
int fn_8026F2CC(TMCC_McuBuf*, TMCC_UsrParam*, int, int);

struct TMCC_McuBuf {
    int unk0;
    s8 data[384];
};

void TMCC_JPEGENC_Init_bitstream(TMCC_Bitstream* bs, u8* buf, int size) {
    bs->start = buf;
    bs->cur = buf;
    bs->unk8 = 8;
    bs->capacity = size;
    bs->total = 0;
    bs->overflow = 0;
}

void TMCC_JPEGENC_Get_bitstream(TMCC_Bitstream* bs, int* out) {
    *out = bs->total + (bs->cur - bs->start);
}

int TMCC_JPEGENC_CountMarker(TMCC_Encoder* enc) {
    enc->outStream.total = (((enc->outStream.total + 7) >> 3) + 2) << 3;
    return 0;
}

void TMCC_JPEGENC_PutRstMarkerRC(TMCC_Encoder* enc) {
    enc->outStream.total = (((enc->outStream.total + 7) >> 3) + 2) << 3;
    enc->restartCount++;
    enc->img.compY.dcPred = 0;
    enc->img.compCb.dcPred = 0;
    enc->img.crDcPred = 0;
}

void TMCC_JPEGENC_CheckPutStream(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = enc->u1C.stream;
    if ((u32)(bs->cur - bs->start) >= (u32)(bs->capacity - 0x100)) {
        enc->u20.putFn = TMCC_JPEGENC_PutStreamWithCheck;
    } else {
        enc->u20.putFn = TMCC_JPEGENC_PutStream;
    }
}

void TMCC_JPEGENC_init_comp_edge(TMCC_Encoder* enc, int mode) {
    switch (mode) {
    case 0:
        enc->edgeFn = TMCC_JPEGENC_encoder_y411edge;
        break;
    case 2:
        enc->edgeFn = TMCC_JPEGENC_encoder_y420edge;
        break;
    case 1:
        enc->edgeFn = TMCC_JPEGENC_encoder_y422edge;
        break;
    }
}

void TMCC_JPEGENC_init_comp(TMCC_Encoder* enc, int mode) {
    switch (mode) {
    case 0:
        enc->compFn = TMCC_JPEGENC_encoder_y411;
        break;
    case 2:
        enc->compFn = TMCC_JPEGENC_encoder_y420;
        break;
    case 1:
        enc->compFn = TMCC_JPEGENC_encoder_y422;
        break;
    }
}

void TMCC_JPEGENC_init_precomp_edge(TMCC_Encoder* enc, int mode) {
    switch (mode) {
    case 0:
        enc->edgeFn = TMCC_JPEGENC_counter_y411edge;
        break;
    case 2:
        enc->edgeFn = TMCC_JPEGENC_counter_y420edge;
        break;
    case 1:
        enc->edgeFn = TMCC_JPEGENC_counter_y422edge;
        break;
    }
}

void TMCC_JPEGENC_init_precomp(TMCC_Encoder* enc, int mode) {
    switch (enc->format) {
    case 0:
        enc->compFn = TMCC_JPEGENC_counter_y411;
        break;
    case 2:
        enc->compFn = TMCC_JPEGENC_counter_y420;
        break;
    case 1:
        enc->compFn = TMCC_JPEGENC_counter_y422;
        break;
    }
}

void TMCC_JPEGENC_enc_restart_interval(TMCC_Encoder* enc) {
    TMCC_JPEGENC_PutMaker(&enc->outStream, (enc->restartCount & 7) + 0xFFD0);
    enc->restartCount++;
    enc->img.compY.dcPred = 0;
    enc->img.compCb.dcPred = 0;
    enc->img.crDcPred = 0;
}

void TMCC_JPEGENC_PutWord(TMCC_Bitstream* bs, u16 word) {
    *bs->cur++ = word >> 8;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = word;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur = 0;
}

void TMCC_JPEGENC_PutString(TMCC_Bitstream* bs, const u8* str, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
            bs->overflow = 1;
            bs->total += bs->cur - bs->start;
            bs->cur = bs->start;
        }
        *bs->cur = *str++;
        bs->cur++;
    }
    *bs->cur = 0;
}

void TMCC_JPEGENC_set_usr_parameters(TMCC_Encoder* enc, TMCC_UsrParam* usr) {
    int edge;
    usr->unkC = 0;
    usr->unkE = 0;
    usr->unk10 = 0;
    usr->unk14 = enc->u20.putFn;
    usr->unk1C = enc->img.width;
    usr->unk1E = enc->img.height;
    usr->unk18 = usr->unk1C * usr->unk1E;
    usr->unk20 = enc->img.unk14;
    usr->unk21 = enc->img.unk15;
    usr->unk22 = enc->img.unk8;
    usr->unk23 = enc->img.unk9;
    if (enc->img.unk8 == 0) {
        edge = (usr->unk1C + 1) * usr->unk20;
    } else {
        edge = (usr->unk1C - 1) * usr->unk20;
    }
    usr->unk24 = edge;
    if (enc->img.unk9 == 0) {
        edge = (usr->unk1E + 1) * usr->unk21;
    } else {
        edge = (usr->unk1E - 1) * usr->unk21;
    }
    usr->unk28 = edge;
    usr->unk2C = 1;
    usr->unk2E = enc->format;
    usr->unk30 = enc->unk2;
    usr->unk32 = enc->unk4;
    usr->unk34 = enc->unk6;
    usr->unk36 = enc->unk8;
    usr->unk38 = usr->unk20 * usr->unk1C;
    usr->unk3C = usr->unk21 * usr->unk1E;
    usr->unk40 = enc->img.compY.unk2C * usr->unk1C;
    usr->unk44 = enc->img.compY.unk2D * usr->unk1E;
}

void fn_802692BC(TMCC_Bitstream* bs, u8 byte) {
    u8* p = bs->cur;
    *p = byte;
    p++;
    bs->cur = p;
    if ((u32)(p - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += p - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur = 0;
}

static const u8 sDefaultDcLumBits[0x14] = {
    0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultDcLumVals[0x0C] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
};
static const u8 sDefaultDcChrBits[0x14] = {
    0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultDcChrVals[0x0C] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
};
static const u8 sDefaultAcLumBits[0x14] = {
    0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultAcLumVals[0xA4] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA, 0x00, 0x00
};
static const u8 sDefaultAcChrBits[0x14] = {
    0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
    0x00, 0x00, 0x00, 0x00
};
static const u8 sDefaultAcChrVals[0xA4] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
    0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
    0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
    0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
    0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
    0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA, 0x00, 0x00
};

static const u8 sBitMasks[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

void fn_8026D514(TMCC_Bitstream* bs, int* out) {
    u32 bytes = bs->total >> 3;
    *out += bytes;
    bs->total -= bytes << 3;
}

int fn_8026BB2C(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = &enc->outStream;
    TMCC_JPEGENC_PutWord(bs, 0xFFDA);
    TMCC_JPEGENC_PutWord(bs, 12);
    fn_802692BC(bs, 3);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 1);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 2);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 0x3F);
    fn_802692BC(bs, 0);
    return 0;
}

void fn_80269508(TMCC_Bitstream* bs) {
    if (bs->unk8 != 8) {
        TMCC_JPEGENC_PutStreamWithCheck(bs, sBitMasks[bs->unk8], bs->unk8);
    }
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = 0xFF;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = 0xD9;
}

void TMCC_JPEGENC_PutMaker(TMCC_Bitstream* bs, u16 marker) {
    if (bs->unk8 != 8) {
        TMCC_JPEGENC_PutStreamWithCheck(bs, sBitMasks[bs->unk8], bs->unk8);
    }
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = marker >> 8;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur++ = marker;
    if ((u32)(bs->cur - bs->start) >= (u32)bs->capacity) {
        bs->overflow = 1;
        bs->total += bs->cur - bs->start;
        bs->cur = bs->start;
    }
    *bs->cur = 0;
}


void fn_8026BFFC(TMCC_Encoder* enc) {
    enc->huffSrc[0] = sDefaultDcLumBits;
    enc->huffSrc[1] = sDefaultDcLumVals;
    enc->huffSrc[2] = sDefaultDcChrBits;
    enc->huffSrc[3] = sDefaultDcChrVals;
    enc->huffSrc[4] = sDefaultAcLumBits;
    enc->huffSrc[5] = sDefaultAcLumVals;
    enc->huffSrc[6] = sDefaultAcChrBits;
    enc->huffSrc[7] = sDefaultAcChrVals;

    enc->huffTbl[0].unk0 = 0;
    enc->huffTbl[0].unk4 = &enc->huffBuf[0x0];
    enc->huffTbl[0].unk8 = &enc->huffBuf[0x40];

    enc->huffTbl[1].unk0 = 0;
    enc->huffTbl[1].unk4 = &enc->huffBuf[0x100];
    enc->huffTbl[1].unk8 = &enc->huffBuf[0x500];

    enc->huffTbl[2].unk0 = 0;
    enc->huffTbl[2].unk4 = &enc->huffBuf[0x80];
    enc->huffTbl[2].unk8 = &enc->huffBuf[0xC0];

    enc->huffTbl[3].unk0 = 0;
    enc->huffTbl[3].unk4 = &enc->huffBuf[0x900];
    enc->huffTbl[3].unk8 = &enc->huffBuf[0xD00];
}



int TMCC_JPEGENC_encoder_y422(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x80, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0xC0, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y422(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x80, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0xC0, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y422edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x80, block, 8);
    ret = fn_802698AC(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0xC0, block, 8);
    ret = fn_802698AC(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y422edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x80, block, 8);
    ret = fn_8026CE80(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0xC0, block, 8);
    ret = fn_8026CE80(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y411(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y411(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y420(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_802698AC(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y420(int x, int y, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    s8* yBuf;
    TMCC_CompInfo* comp;
    int ret;

    ret = img->cvtFn(&mcu, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcu.data;
    comp = &img->compY;
    fn_8026BBF0(yBuf, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = &img->compCb;
    fn_8026BDE8(mcu.data + 0x100, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }

    comp = (TMCC_CompInfo*)&img->crUnk0;
    fn_8026BDE8(mcu.data + 0x140, block, comp->stride);
    ret = fn_8026CE80(block, comp);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y411edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, 32);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_802698AC(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_802698AC(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y411edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 5, y << 3);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x10, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x18, block, 32);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_8026CE80(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_8026CE80(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_encoder_y420edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, 16);
    ret = fn_802698AC(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_802698AC(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_802698AC(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int TMCC_JPEGENC_counter_y420edge(int x, int y, int w, TMCC_ImageInfo* img) {
    int block[64];
    TMCC_McuBuf mcu;
    TMCC_McuBuf* mcuPtr = &mcu;
    s8* yBuf;
    TMCC_CompInfo* compY;
    int ret;

    ret = img->cvtEdgeFn(mcuPtr, img->usr, x << 4, y << 4);
    if (ret < 0) {
        return ret;
    }

    yBuf = mcuPtr->data;
    compY = &img->compY;
    fn_8026BBF0(yBuf, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 8, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x80, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BBF0(yBuf + 0x88, block, 16);
    ret = fn_8026CE80(block, compY);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x100, block, 8);
    ret = fn_8026CE80(block, &img->compCb);
    if (ret < 0) {
        return ret;
    }

    fn_8026BDE8(mcuPtr->data + 0x140, block, 8);
    ret = fn_8026CE80(block, (TMCC_CompInfo*)&img->crUnk0);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int fn_8026A28C(TMCC_Encoder* enc) {
    TMCC_Bitstream* bs = &enc->outStream;
    u16 h = enc->unk4;
    u16 w = enc->unk2;
    int v = 1 << enc->unkF;
    int u = 1 << enc->unkE;

    TMCC_JPEGENC_PutWord(bs, 0xFFC0);
    TMCC_JPEGENC_PutWord(bs, 17);
    fn_802692BC(bs, 8);
    TMCC_JPEGENC_PutWord(bs, h);
    TMCC_JPEGENC_PutWord(bs, w);
    fn_802692BC(bs, 3);
    fn_802692BC(bs, 0);
    TMCC_JPEGENC_PutStreamWithCheck(bs, u, 4);
    TMCC_JPEGENC_PutStreamWithCheck(bs, v, 4);
    fn_802692BC(bs, 0);
    fn_802692BC(bs, 1);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 1);
    fn_802692BC(bs, 2);
    fn_802692BC(bs, 0x11);
    fn_802692BC(bs, 1);
    if (fn_8026BB2C(enc) < 0) {
        return -2;
    }
    return 0;
}

int fn_8026C7D4(TMCC_UsrParam* usr, int unused, TMCC_InitParam* param) {
    TMCC_Encoder* enc = param->enc;
    int ret;
    if (enc == NULL) {
        return -1;
    }
    memset(usr, 0, sizeof(TMCC_UsrParam));
    memset(enc, 0, sizeof(TMCC_Encoder));
    usr->unk0 = enc;
    enc->img.usr = usr;
    ret = fn_8026A62C(param, enc);
    if (ret < 0) {
        return ret;
    }
    fn_8026BFFC(enc);
    fn_8026C0AC(enc);
    fn_8026C45C(enc);
    fn_8026A7D0(enc);
    TMCC_JPEGENC_Init_bitstream(&enc->outStream, 0, param->bufSize);
    TMCC_JPEGENC_init_precomp(enc, enc->format);
    TMCC_JPEGENC_init_precomp_edge(enc, enc->format);
    TMCC_JPEGENC_set_usr_parameters(enc, usr);
    switch (usr->unk8) {
    case 4:
        ret = fn_8026E8C0(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    case 6:
        ret = fn_8026DBF8(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    }
    if (enc->unk10 != 0) {
        usr->unk10 = 0x263;
    } else {
        usr->unk10 = 0x25D;
    }
    return usr->unk18;
}

int fn_8026B3D8(TMCC_UsrParam* usr, u8* outBuf, int unused, TMCC_InitParam* param) {
    TMCC_Encoder* enc = param->enc;
    int ret;
    if (enc == NULL) {
        return -1;
    }
    memset(usr, 0, sizeof(TMCC_UsrParam));
    memset(enc, 0, sizeof(TMCC_Encoder));
    usr->unk0 = enc;
    enc->img.usr = usr;
    ret = fn_8026A62C(param, enc);
    if (ret < 0) {
        return ret;
    }
    fn_8026BFFC(enc);
    fn_8026C0AC(enc);
    fn_8026C45C(enc);
    fn_8026A7D0(enc);
    TMCC_JPEGENC_Init_bitstream(&enc->outStream, outBuf, param->bufSize);
    TMCC_JPEGENC_init_comp(enc, enc->format);
    TMCC_JPEGENC_init_comp_edge(enc, enc->format);
    if (fn_8026A3B8(enc) < 0) {
        return -2;
    }
    TMCC_JPEGENC_set_usr_parameters(enc, usr);
    switch (usr->unk8) {
    case 4:
        ret = fn_8026E8C0(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    case 6:
        ret = fn_8026DBF8(&enc->img, usr);
        if (ret < 0) {
            return ret;
        }
        break;
    }
    TMCC_JPEGENC_Get_bitstream(&enc->outStream, &usr->unk10);
    return usr->unk18;
}












