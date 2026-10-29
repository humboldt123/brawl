#include <mv/mv_THPDraw.h>
#include <revolution/GX.h>
#include <revolution/MTX.h>
#include <types.h>

// Not declared by the OpenRVL GX headers.
extern "C" void GXSetDispCopyGamma(u32 gamma);
extern "C" void GXSetTevColorS10(GXTevRegID id, GXColorS10 color);
#define GX_GM_1_0 0

// The OpenRVL headers number GXTevMode and the GXTevKAlphaSel K*_R entries differently from the SDK; these are
// the SDK values the game passes (GX_REPLACE = 3, GX_TEV_KASEL_K0_R = 28, GX_TEV_KASEL_K1_R = 29).
static const GXTevMode SDK_GX_REPLACE = (GXTevMode)3;
static const GXTevKAlphaSel SDK_GX_TEV_KASEL_K0_R = (GXTevKAlphaSel)28;
static const GXTevKAlphaSel SDK_GX_TEV_KASEL_K1_R = (GXTevKAlphaSel)29;

// Restores the GX state THPGXYuv2RgbSetup changed for ordinary textured drawing.
void THPGXRestore() {
    GXSetZMode(TRUE, GX_ALWAYS, FALSE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_SET);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, SDK_GX_REPLACE);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
}

// Sets up an orthographic view of width x height and a four-stage TEV that turns the three I8 planes
// (Y in GX_TEXMAP0, U in GX_TEXMAP1, V in GX_TEXMAP2) into RGB.
void THPGXYuv2RgbSetup(int width, int height) {
    Mtx44 proj;
    Mtx mtx;

    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    C_MTXOrtho(proj, 0.0f, height, 0.0f, width, 0.0f, -1.0f);
    GXSetProjection(proj, GX_ORTHOGRAPHIC);
    GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
    GXSetScissor(0, 0, width, height);

    PSMTXIdentity(mtx);
    GXLoadPosMtxImm(mtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

    GXSetZMode(TRUE, GX_ALWAYS, FALSE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetColorUpdate(TRUE);
    GXSetAlphaUpdate(FALSE);
    GXSetDispCopyGamma(GX_GM_1_0);

    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX_IDENT, FALSE, GX_DUALMTX_IDENT);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX_IDENT, FALSE, GX_DUALMTX_IDENT);

    GXInvalidateTexAll();

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST, GX_U16, 0);

    GXSetNumTevStages(4);

    // Stage 0: U (half-resolution, texcoord 1) weighted by the K0 constant.
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, FALSE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, FALSE, GX_TEVPREV);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, SDK_GX_TEV_KASEL_K0_R);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);

    // Stage 1: V weighted by the K1 constant.
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, FALSE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_APREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, FALSE, GX_TEVPREV);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
    GXSetTevKAlphaSel(GX_TEVSTAGE1, SDK_GX_TEV_KASEL_K1_R);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);

    // Stage 2: Y added.
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);

    // Stage 3: the alpha accumulator scaled by K2 into the green channel.
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_APREV, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

    // YUV to RGB offsets and weights.
    GXColorS10 c0 = { -90, 0, -114, 135 };
    GXSetTevColorS10(GX_TEVREG0, c0);
    GXColor k0 = { 0, 0, 226, 88 };
    GXSetTevKColor(GX_KCOLOR0, k0);
    GXColor k1 = { 179, 0, 0, 182 };
    GXSetTevKColor(GX_KCOLOR1, k1);
    GXColor k2 = { 255, 0, 255, 128 };
    GXSetTevKColor(GX_KCOLOR2, k2);

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}

// Draws one frame: the Y plane is textureWidth x textureHeight, the U and V planes half that, on a quad of
// polygonWidth x polygonHeight at (x, y).
void THPGXYuv2RgbDraw(u8* yData, u8* uData, u8* vData, s16 x, s16 y, s16 textureWidth, s16 textureHeight,
                      s16 polygonWidth, s16 polygonHeight) {
    GXTexObj texObjY;
    GXTexObj texObjU;
    GXTexObj texObjV;

    GXInitTexObj(&texObjY, yData, textureWidth, textureHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, FALSE);
    GXInitTexObjLOD(&texObjY, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, FALSE, FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObjY, GX_TEXMAP0);

    GXInitTexObj(&texObjU, uData, textureWidth >> 1, textureHeight >> 1, GX_TF_I8, GX_CLAMP, GX_CLAMP, FALSE);
    GXInitTexObjLOD(&texObjU, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, FALSE, FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObjU, GX_TEXMAP1);

    GXInitTexObj(&texObjV, vData, textureWidth >> 1, textureHeight >> 1, GX_TF_I8, GX_CLAMP, GX_CLAMP, FALSE);
    GXInitTexObjLOD(&texObjV, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, FALSE, FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObjV, GX_TEXMAP2);

    GXBegin(GX_QUADS, GX_VTXFMT7, 4);
    GXPosition3s16(x, y, 0);
    GXTexCoord2u16(0, 0);
    GXPosition3s16(x + polygonWidth, y, 0);
    GXTexCoord2u16(1, 0);
    GXPosition3s16(x + polygonWidth, y + polygonHeight, 0);
    GXTexCoord2u16(1, 1);
    GXPosition3s16(x, y + polygonHeight, 0);
    GXTexCoord2u16(0, 1);
    GXEnd();
}
