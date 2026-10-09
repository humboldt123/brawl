#include <nw4r/ef.h>
#include <nw4r/math.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ef {

DrawDirectionalStrategy::DrawDirectionalStrategy() {}

static void DrawQuad(const math::MTX34& rMtx, const math::_VEC3* pPosArray,
                     bool texCoord) {

    math::VEC3 p0, p1, p2, p3;

    math::VEC3Transform(&p0, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[0]));
    math::VEC3Transform(&p1, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[1]));
    math::VEC3Transform(&p2, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[2]));
    math::VEC3Transform(&p3, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[3]));

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition(p0);
        if (texCoord) {
            GXTexCoord1x8(0);
        }

        GXPosition(p1);
        if (texCoord) {
            GXTexCoord1x8(1);
        }

        GXPosition(p2);
        if (texCoord) {
            GXTexCoord1x8(2);
        }

        GXPosition(p3);
        if (texCoord) {
            GXTexCoord1x8(3);
        }
    }
    GXEnd();
}

static void DrawQuad(const math::VEC3& rP, const math::VEC3& rD1,
                     const math::VEC3& rD2, int flags) {

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition(rP - rD1);
        if (flags != 0) {
            GXTexCoord1x8(0);
        }

        GXPosition(rP - rD2);
        if (flags != 0) {
            GXTexCoord1x8(1);
        }

        GXPosition(rP + rD1);
        if (flags != 0) {
            GXTexCoord1x8(2);
        }

        GXPosition(rP + rD2);
        if (flags != 0) {
            GXTexCoord1x8(3);
        }
    }
    GXEnd();
}

void DrawDirectionalStrategy::Draw(const DrawInfo& rInfo,
                                   ParticleManager* pManager) {

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    InitTexture(rSetting);
    InitTev(rSetting, rInfo);
    InitColor(pManager, rSetting, rInfo);

    GXEnableTexOffsets(GX_TEXCOORD0, TRUE, TRUE);

    // MATCH-ONLY: function-local table; a file-scope name would show up as a named reloc.
    static u8 texcoord[] = {0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01};
    GXSetArray(GX_VA_TEX0, texcoord, 2);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);

    if (mNumTexmap > 0) {
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    }

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);

    GXSetCurrentMtx(GX_PNMTX0);

    if ((pManager->mResource->GetEmitterDrawSetting()->typeOption2 & 3) != 1) {
        DrawDirectional(rInfo, pManager);
    } else {
        DrawDirectionalBillboard(rInfo, pManager);
    }
}

void DrawDirectionalStrategy::InitGraphics(const DrawInfo& rInfo,
                                           ParticleManager* pManager) {

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    InitTexture(rSetting);
    InitTev(rSetting, rInfo);
    InitColor(pManager, rSetting, rInfo);

    GXEnableTexOffsets(GX_TEXCOORD0, TRUE, TRUE);

    // MATCH-ONLY: function-local table; a file-scope name would show up as a named reloc.
    static u8 texcoord[] = {0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01};
    GXSetArray(GX_VA_TEX0, texcoord, 2);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);

    if (mNumTexmap > 0) {
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    }

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);

    GXSetCurrentMtx(GX_PNMTX0);
}

DrawStrategyImpl::CalcAheadFunc
DrawDirectionalStrategy::GetCalcAheadFunc(ParticleManager* pManager) {

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    switch (rSetting.typeDir) {
    case EmitterDrawSetting::AHEAD_BB_SPEED: {
        return CalcAhead_Speed;
    }

    case EmitterDrawSetting::AHEAD_BB_EMITTER_CENTER: {
        return CalcAhead_EmitterCenter;
    }

    case EmitterDrawSetting::AHEAD_BB_EMITTER_DESIGN: {
        return CalcAhead_EmitterDesign;
    }

    case EmitterDrawSetting::AHEAD_BB_PARTICLE: {
        return CalcAhead_Particle;
    }

    case EmitterDrawSetting::DIR_NO_DESIGN: {
        return CalcAhead_NoDesign;
    }

    case EmitterDrawSetting::AHEAD_BB_PARTICLE_BOTH: {
        return CalcAhead_ParticleBoth;
    }

    case 6: {
        return CalcAhead_Speed;
    }

    default: {
        return CalcAhead_Speed;
    }
    }
}

} // namespace ef
} // namespace nw4r
