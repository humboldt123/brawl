// gx lighting: light objects live in plain memory until they are loaded into the transform unit, the
// channel setup registers are only cached here and get flushed by the state update before drawing.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

// the sdk calls the double versions of these
extern double cos(double x);
extern double sqrt(double x);

#define XF_LIGHT_BASE 0x600
#define XF_LIGHT_SIZE 16

void GXInitLightAttn(GXLightObj *lt_obj, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1, f32 k2) {
    GX_SETUP_LIGHT(lt, lt_obj);

    lt->a[0] = a0;
    lt->a[1] = a1;
    lt->a[2] = a2;
    lt->k[0] = k0;
    lt->k[1] = k1;
    lt->k[2] = k2;
}

void GXInitLightAttnA(GXLightObj *lt_obj, f32 a0, f32 a1, f32 a2) {
    GX_SETUP_LIGHT(lt, lt_obj);

    lt->a[0] = a0;
    lt->a[1] = a1;
    lt->a[2] = a2;
}

void GXInitLightAttnK(GXLightObj *lt_obj, f32 k0, f32 k1, f32 k2) {
    GX_SETUP_LIGHT(lt, lt_obj);

    lt->k[0] = k0;
    lt->k[1] = k1;
    lt->k[2] = k2;
}

void GXInitLightSpot(GXLightObj *lt_obj, f32 cutoff, GXSpotFn spot_func) {
    f32 theta, cr;
    f32 a, b, c, d;
    GX_SETUP_LIGHT(lt, lt_obj);

    if (cutoff <= 0.0f || cutoff > 90.0f) {
        spot_func = GX_SP_OFF;
    }

    theta = (3.1415927f * cutoff) / 180.0f;
    cr = cos(theta);

    switch (spot_func) {
    case GX_SP_FLAT:
        a = -1000.0f * cr;
        b = 1000.0f;
        c = 0.0f;
        break;
    case GX_SP_COS:
        b = 1.0f / (1.0f - cr);
        a = -cr * b;
        c = 0.0f;
        break;
    case GX_SP_COS2:
        c = 1.0f / (1.0f - cr);
        a = 0.0f;
        b = -cr * c;
        break;
    case GX_SP_SHARP:
        d = 1.0f / ((1.0f - cr) * (1.0f - cr));
        a = (cr * (cr - 2.0f)) * d;
        b = 2.0f * d;
        c = -d;
        break;
    case GX_SP_RING1:
        d = 1.0f / ((1.0f - cr) * (1.0f - cr));
        c = -4.0f * d;
        a = c * cr;
        b = 4.0f * (1.0f + cr) * d;
        break;
    case GX_SP_RING2:
        d = 1.0f / ((1.0f - cr) * (1.0f - cr));
        a = 1.0f - 2.0f * cr * cr * d;
        b = 4.0f * cr * d;
        c = -2.0f * d;
        break;
    case GX_SP_OFF:
    default:
        a = 1.0f;
        b = 0.0f;
        c = 0.0f;
        break;
    }

    lt->a[0] = a;
    lt->a[1] = b;
    lt->a[2] = c;
}

void GXInitLightDistAttn(GXLightObj *lt_obj, f32 ref_dist, f32 ref_br, GXDistAttnFn dist_func) {
    f32 k0, k1, k2;
    GX_SETUP_LIGHT(lt, lt_obj);

    if (ref_dist < 0.0f) {
        dist_func = GX_DA_OFF;
    }

    if (ref_br <= 0.0f || ref_br >= 1.0f) {
        dist_func = GX_DA_OFF;
    }

    switch (dist_func) {
    case GX_DA_GENTLE:
        k0 = 1.0f;
        k1 = (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.0f;
        break;
    case GX_DA_MEDIUM:
        k0 = 1.0f;
        k1 = 0.5f * (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.5f * (1.0f - ref_br) / (ref_br * ref_dist * ref_dist);
        break;
    case GX_DA_STEEP:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = (1.0f - ref_br) / (ref_br * ref_dist * ref_dist);
        break;
    case GX_DA_OFF:
    default:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = 0.0f;
        break;
    }

    lt->k[0] = k0;
    lt->k[1] = k1;
    lt->k[2] = k2;
}

void GXInitLightPos(GXLightObj *lt_obj, f32 x, f32 y, f32 z) {
    GX_SETUP_LIGHT(lt, lt_obj);

    lt->lpos[0] = x;
    lt->lpos[1] = y;
    lt->lpos[2] = z;
}

void GXGetLightPos(const GXLightObj *lt_obj, f32 *x, f32 *y, f32 *z) {
    GX_SETUP_LIGHT(lt, lt_obj);

    *x = lt->lpos[0];
    *y = lt->lpos[1];
    *z = lt->lpos[2];
}

void GXInitLightDir(GXLightObj *lt_obj, f32 nx, f32 ny, f32 nz) {
    GX_SETUP_LIGHT(lt, lt_obj);

    lt->ldir[0] = -nx;
    lt->ldir[1] = -ny;
    lt->ldir[2] = -nz;
}

void GXGetLightDir(const GXLightObj *lt_obj, f32 *nx, f32 *ny, f32 *nz) {
    GX_SETUP_LIGHT(lt, lt_obj);

    *nx = -lt->ldir[0];
    *ny = -lt->ldir[1];
    *nz = -lt->ldir[2];
}

void GXInitSpecularDir(GXLightObj *lt_obj, f32 nx, f32 ny, f32 nz) {
    f32 mag, vx, vy, vz;
    GX_SETUP_LIGHT(lt, lt_obj);

    vx = -nx;
    vy = -ny;
    vz = -nz + 1.0f;
    mag = (vx * vx) + (vy * vy) + (vz * vz);
    if (mag != 0.0f) {
        mag = 1.0f / (f32)sqrt(mag);
    }

    lt->ldir[0] = vx * mag;
    lt->ldir[1] = vy * mag;
    lt->ldir[2] = vz * mag;
    lt->lpos[0] = -999999999999999999.0f * nx;
    lt->lpos[1] = -999999999999999999.0f * ny;
    lt->lpos[2] = -999999999999999999.0f * nz;
}

void GXInitLightColor(GXLightObj *lt_obj, GXColor color) {
    GX_SETUP_LIGHT(lt, lt_obj);

    lt->Color = *(u32 *)&color;
}

void GXGetLightColor(const GXLightObj *lt_obj, GXColor *color) {
    GX_SETUP_LIGHT(lt, lt_obj);

    *(u32 *)color = lt->Color;
}

// BYTES: the original copies with paired singles and stores the three reserved words as zero
static inline void WriteLightObj_i(register volatile void *dst, register const GXLightObjInt *src) {
    register u32 color;
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm volatile {
        lwz color, src->Color
        xor r6, r6, r6
        psq_l ps_0, GXLightObjInt.a(src), 0, 0
        psq_l ps_1, GXLightObjInt.a[2](src), 0, 0
        psq_l ps_2, GXLightObjInt.k[1](src), 0, 0
        psq_l ps_3, GXLightObjInt.lpos(src), 0, 0
        psq_l ps_4, GXLightObjInt.lpos[2](src), 0, 0
        psq_l ps_5, GXLightObjInt.ldir[1](src), 0, 0

        stwu r6, -0x8000(dst)
        stw r6, 0(dst)
        stw r6, 0(dst)

        stw color, 0(dst)
        psq_st ps_0, 0(dst), 0, 0
        psq_st ps_1, 0(dst), 0, 0
        psq_st ps_2, 0(dst), 0, 0
        psq_st ps_3, 0(dst), 0, 0
        psq_st ps_4, 0(dst), 0, 0
        psq_st ps_5, 0(dst), 0, 0
    }
}

void GXLoadLightObjImm(const GXLightObj *lt_obj, GXLightID light) {
    u32 addr;
    u32 idx;
    volatile u8 *fifo = (volatile u8 *)0xCC010000;
    GX_SETUP_LIGHT(lt, lt_obj);

    idx = 31 - __cntlzw(light);
    idx &= 7;
    addr = idx * XF_LIGHT_SIZE + XF_LIGHT_BASE;

    *(volatile u8 *)(fifo - 0x8000) = 0x10;
    *(volatile u32 *)(fifo - 0x8000) = addr | ((XF_LIGHT_SIZE - 1) << 16);
    WriteLightObj_i(fifo, lt);

    gx->bpSentNot = GX_TRUE;
}

void GXSetChanAmbColor(GXChannelID chan, GXColor amb_color) {
    u32 reg;
    u32 colorID;

    switch (chan) {
    case GX_COLOR0:
        reg = gx->ambColor[0];
        reg = __rlwimi(reg, *(u32 *)&amb_color, 0, 0, 23);
        colorID = 0;
        break;
    case GX_COLOR1:
        reg = gx->ambColor[1];
        reg = __rlwimi(reg, *(u32 *)&amb_color, 0, 0, 23);
        colorID = 1;
        break;
    case GX_ALPHA0:
        reg = gx->ambColor[0];
        FAST_FLAG_SET(reg, amb_color.a, 0, 8);
        colorID = 0;
        break;
    case GX_ALPHA1:
        reg = gx->ambColor[1];
        FAST_FLAG_SET(reg, amb_color.a, 0, 8);
        colorID = 1;
        break;
    case GX_COLOR0A0:
        reg = *(u32 *)&amb_color;
        colorID = 0;
        break;
    case GX_COLOR1A1:
        reg = *(u32 *)&amb_color;
        colorID = 1;
        break;
    default:
        return;
    }

    gx->dirtyState |= 0x0100 << colorID;
    gx->ambColor[colorID] = reg;
}

void GXSetChanMatColor(GXChannelID chan, GXColor mat_color) {
    u32 reg;
    u32 colorID;

    switch (chan) {
    case GX_COLOR0:
        reg = gx->matColor[0];
        reg = __rlwimi(reg, *(u32 *)&mat_color, 0, 0, 23);
        colorID = 0;
        break;
    case GX_COLOR1:
        reg = gx->matColor[1];
        reg = __rlwimi(reg, *(u32 *)&mat_color, 0, 0, 23);
        colorID = 1;
        break;
    case GX_ALPHA0:
        reg = gx->matColor[0];
        FAST_FLAG_SET(reg, mat_color.a, 0, 8);
        colorID = 0;
        break;
    case GX_ALPHA1:
        reg = gx->matColor[1];
        FAST_FLAG_SET(reg, mat_color.a, 0, 8);
        colorID = 1;
        break;
    case GX_COLOR0A0:
        reg = *(u32 *)&mat_color;
        colorID = 0;
        break;
    case GX_COLOR1A1:
        reg = *(u32 *)&mat_color;
        colorID = 1;
        break;
    default:
        return;
    }

    gx->dirtyState |= 0x0400 << colorID;
    gx->matColor[colorID] = reg;
}

void GXSetNumChans(u8 nChans) {
    SC_GEN_MODE_SET_NCOL(gx->genMode, nChans);
    gx->dirtyState |= 0x01000004;
}

void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc amb_src, GXColorSrc mat_src, u32 light_mask,
                   GXDiffuseFn diff_fn, GXAttnFn attn_fn) {
    u32 reg;
    u32 idx = chan & 3;

    reg = 0;
    FAST_FLAG_SET(reg, enable, 1, 1);
    FAST_FLAG_SET(reg, mat_src, 0, 1);
    FAST_FLAG_SET(reg, amb_src, 6, 1);
    FAST_FLAG_SET(reg, attn_fn == GX_AF_SPEC ? GX_DF_NONE : diff_fn, 7, 2);
    FAST_FLAG_SET(reg, attn_fn != GX_AF_NONE, 9, 1);
    FAST_FLAG_SET(reg, attn_fn != GX_AF_SPEC, 10, 1);
    FAST_FLAG_SET(reg, light_mask, 2, 4);
    FAST_FLAG_SET(reg, light_mask >> 4, 11, 4);

    gx->chanCtrl[idx] = reg;
    gx->dirtyState |= 0x1000 << idx;

    if (chan == GX_COLOR0A0) {
        gx->chanCtrl[2] = reg;
        gx->dirtyState |= 0x5000;
    } else if (chan == GX_COLOR1A1) {
        gx->chanCtrl[3] = reg;
        gx->dirtyState |= 0xA000;
    }
}
