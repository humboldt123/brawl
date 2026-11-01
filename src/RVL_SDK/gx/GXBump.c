// gx indirect texturing setup: the tev indirect stage, the 2x3 indirect matrices, scale/order, plus the
// couple of tiny bp mask helpers at the end.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

// the direct variant calls this for real, so it must not be inlined
#pragma push
#pragma auto_inline off
void GXSetTevIndirect(GXTevStageID tevStage, GXIndTexStageID texStage, GXIndTexFormat texFmt, GXIndTexBiasSel biasSel, GXIndTexMtxID mtxId,
                      GXIndTexWrap wrapS, GXIndTexWrap wrapT, GXBool addPrev, GXBool utcLod, GXIndTexAlphaSel alphaSel) {
    u32 reg = 0;

    // HYPOTHESIS: field layout of the 0x10+n bp register, worked out from the rlwimi masks
    SET_REG_FIELD(reg, 2, 0, texStage);
    SET_REG_FIELD(reg, 2, 2, texFmt);
    SET_REG_FIELD(reg, 3, 4, biasSel);
    SET_REG_FIELD(reg, 2, 7, alphaSel);
    SET_REG_FIELD(reg, 4, 9, mtxId);
    SET_REG_FIELD(reg, 3, 13, wrapS);
    SET_REG_FIELD(reg, 3, 16, wrapT);
    SET_REG_FIELD(reg, 1, 19, utcLod);
    SET_REG_FIELD(reg, 1, 20, addPrev);
    SET_REG_FIELD(reg, 8, 24, tevStage + 0x10);

    GX_WRITE_RA_REG(reg);
    gx->bpSentNot = GX_FALSE;
}
#pragma pop

void GXSetIndTexMtx(GXIndTexMtxID id, const f32 offset[2][3], s8 scaleExp) {
    u32 index;
    u32 reg;

    switch (id) {
    case GX_ITM_0:
    case GX_ITM_1:
    case GX_ITM_2:
        index = id - GX_ITM_0;
        break;
    case GX_ITM_S0:
    case GX_ITM_S1:
    case GX_ITM_S2:
        index = id - GX_ITM_S0;
        break;
    case GX_ITM_T0:
    case GX_ITM_T1:
    case GX_ITM_T2:
        index = id - GX_ITM_T0;
        break;
    default:
        index = 0;
        break;
    }

    scaleExp += 17;

    reg = 0;
    SET_REG_FIELD(reg, 11, 0, (s32)(1024.0f * offset[0][0]));
    SET_REG_FIELD(reg, 11, 11, (s32)(1024.0f * offset[1][0]));
    SET_REG_FIELD(reg, 2, 22, scaleExp);
    SET_REG_FIELD(reg, 8, 24, index * 3 + 6);
    GX_WRITE_RA_REG(reg);

    reg = 0;
    SET_REG_FIELD(reg, 11, 0, (s32)(1024.0f * offset[0][1]));
    SET_REG_FIELD(reg, 11, 11, (s32)(1024.0f * offset[1][1]));
    reg = __rlwimi(reg, scaleExp, 20, 8, 9);
    SET_REG_FIELD(reg, 8, 24, index * 3 + 7);
    GX_WRITE_RA_REG(reg);

    reg = 0;
    SET_REG_FIELD(reg, 11, 0, (s32)(1024.0f * offset[0][2]));
    SET_REG_FIELD(reg, 11, 11, (s32)(1024.0f * offset[1][2]));
    reg = __rlwimi(reg, scaleExp, 18, 8, 9);
    SET_REG_FIELD(reg, 8, 24, index * 3 + 8);
    GX_WRITE_RA_REG(reg);

    gx->bpSentNot = GX_FALSE;
}

void GXSetIndTexCoordScale(GXIndTexStageID stage, GXIndTexScale scaleS, GXIndTexScale scaleT) {
    switch (stage) {
    case GX_INDTEXSTAGE0:
        SET_REG_FIELD(gx->IndTexScale0, 4, 0, scaleS);
        SET_REG_FIELD(gx->IndTexScale0, 4, 4, scaleT);
        SET_REG_FIELD(gx->IndTexScale0, 8, 24, 0x25);
        GX_WRITE_RA_REG(gx->IndTexScale0);
        break;
    case GX_INDTEXSTAGE1:
        SET_REG_FIELD(gx->IndTexScale0, 4, 8, scaleS);
        SET_REG_FIELD(gx->IndTexScale0, 4, 12, scaleT);
        SET_REG_FIELD(gx->IndTexScale0, 8, 24, 0x25);
        GX_WRITE_RA_REG(gx->IndTexScale0);
        break;
    case GX_INDTEXSTAGE2:
        SET_REG_FIELD(gx->IndTexScale1, 4, 0, scaleS);
        SET_REG_FIELD(gx->IndTexScale1, 4, 4, scaleT);
        SET_REG_FIELD(gx->IndTexScale1, 8, 24, 0x26);
        GX_WRITE_RA_REG(gx->IndTexScale1);
        break;
    case GX_INDTEXSTAGE3:
        SET_REG_FIELD(gx->IndTexScale1, 4, 8, scaleS);
        SET_REG_FIELD(gx->IndTexScale1, 4, 12, scaleT);
        SET_REG_FIELD(gx->IndTexScale1, 8, 24, 0x26);
        GX_WRITE_RA_REG(gx->IndTexScale1);
        break;
    }

    gx->bpSentNot = GX_FALSE;
}

void GXSetIndTexOrder(GXIndTexStageID stage, GXTexCoordID coord, GXTexMapID map) {
    if (map == GX_TEXMAP_NULL) {
        map = GX_TEXMAP0;
    }

    if (coord == GX_TEXCOORD_NULL) {
        coord = GX_TEXCOORD0;
    }

    switch (stage) {
    case GX_INDTEXSTAGE0:
        SET_REG_FIELD(gx->iref, 3, 0, map);
        SET_REG_FIELD(gx->iref, 3, 3, coord);
        break;
    case GX_INDTEXSTAGE1:
        SET_REG_FIELD(gx->iref, 3, 6, map);
        SET_REG_FIELD(gx->iref, 3, 9, coord);
        break;
    case GX_INDTEXSTAGE2:
        SET_REG_FIELD(gx->iref, 3, 12, map);
        SET_REG_FIELD(gx->iref, 3, 15, coord);
        break;
    case GX_INDTEXSTAGE3:
        SET_REG_FIELD(gx->iref, 3, 18, map);
        SET_REG_FIELD(gx->iref, 3, 21, coord);
        break;
    }

    GX_WRITE_RA_REG(gx->iref);
    gx->dirtyState |= 3;
    gx->bpSentNot = GX_FALSE;
}

void GXSetNumIndStages(u8 nIndStages) {
    SET_REG_FIELD(gx->genMode, 3, 16, nIndStages);
    gx->dirtyState |= 6;
}

void GXSetTevDirect(GXTevStageID tevStage) {
    GXSetTevIndirect(tevStage, GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
}

// HYPOTHESIS: empty, the body is probably debug-build only
void __GXUpdateBPMask(void) {}

void __GXSetIndirectMask(u32 mask) {
    SET_REG_FIELD(gx->bpMask, 8, 0, mask);
    GX_WRITE_RA_REG(gx->bpMask);
    gx->bpSentNot = GX_FALSE;
}

void __GXFlushTextureState(void) {
    GX_WRITE_RA_REG(gx->bpMask);
    gx->bpSentNot = GX_FALSE;
}
