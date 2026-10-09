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

static math::MTX34 CalcRotate(Particle* pParticle, u8 axis) {
#pragma unused(axis)

    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    f32 sx, cx;
    math::SinCosRad(&sx, &cx, rot.x);

    f32 sy, cy;
    math::SinCosRad(&sy, &cy, rot.y);

    f32 sz, cz;
    math::SinCosRad(&sz, &cz, rot.z);

    f32 sx_sy = sx * sy;
    f32 cx_sy = cx * sy;

    // clang-format off
    return math::MTX34(
        cy * cz,  sx_sy * cz - cx * sz,  cx_sy * cz + sx * sz,  0.0f,
        cy * sz,  sx_sy * sz + cx * cz,  cx_sy * sz - sx * cz,  0.0f,
            -sy,               cy * sx,               cx * cy,  0.0f);
    // clang-format on
}

static math::MTX34 CalcLocalTransform(u8 flag, f32 px, f32 py, f32 sx, f32 sy,
                                      f32 stretch, const math::MTX34& rRot) {
#pragma unused(flag)

    f32 sx_px = sx * px;
    f32 sy_py = sy * py;

    // HYPOTHESIS: the stretch factor scales the Y column of the local frame.
    // clang-format off
    return math::MTX34(
        rRot._00 * sx,  rRot._01 * sy * stretch,  rRot._02 * sx,  -rRot._00 * sx_px - rRot._01 * sy_py + px,
        rRot._10 * sx,  rRot._11 * sy * stretch,  rRot._12 * sx,  -rRot._10 * sx_px - rRot._11 * sy_py + py,
        rRot._20 * sx,  rRot._21 * sy * stretch,  rRot._22 * sx,  -rRot._20 * sx_px - rRot._21 * sy_py);
    // clang-format on
}

void DrawDirectionalStrategy::DrawDirectional(const DrawInfo& rInfo,
                                              ParticleManager* pManager) {

    // clang-format off
    static const math::_VEC3 quadP[4] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };
    static const math::_VEC3 quadPX[4] = {
        0.0f, -1.0f,  1.0f,
        0.0f,  1.0f,  1.0f,
        0.0f,  1.0f, -1.0f,
        0.0f, -1.0f, -1.0f
    };
    // clang-format on

    InitGraphics(rInfo, pManager);

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    AheadContext ctx(*rInfo.GetViewMtx(), pManager);
    CalcAheadFunc pCalcAhead = GetCalcAheadFunc(pManager);

    math::MTX34 glbMtx;
    math::MTX34 posMtx;
    math::MTX34 emMtx;
    math::MTX34 invMtx;

    pManager->CalcGlobalMtx(&glbMtx);
    math::MTX34Mult(&posMtx, rInfo.GetViewMtx(), &glbMtx);
    GXLoadPosMtxImm(posMtx, GX_PNMTX0);

    pManager->mManagerEM->CalcGlobalMtx(&emMtx);
    math::MTX34Inv(&invMtx, &glbMtx);
    math::MTX34Mult(&emMtx, &invMtx, &emMtx);

    // HYPOTHESIS: the emitter's Y and Z columns, normalized, give the default
    // axis used to reset stale particles.
    math::VEC3 dfltY(emMtx._01, emMtx._11, emMtx._21);
    math::VEC3 dfltZ(emMtx._02, emMtx._12, emMtx._22);
    Normalize(&dfltY);
    Normalize(&dfltZ);

    // Particles whose stored axis is flagged (x > 1) are reset.
    Particle* pIt = GetOldestParticle(pManager);
    while (pIt != NULL && pIt->mPrevAxis.x > 1.0f) {
        pIt->mPrevAxis = dfltY;
        pIt = GetYoungerParticle(pManager, pIt);
    }

    f32 px = rSetting.pivotX / 100.0f;
    f32 py = rSetting.pivotY / 100.0f;

    GetFirstDrawParticleFunc pGetFirstFunc = GetGetFirstDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    bool first = true;

    for (pIt = pGetFirstFunc(pManager); pIt != NULL;
         pIt = pGetNextFunc(pManager, pIt)) {

        f32 sx = pIt->Draw_GetSizeX();
        if (sx < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        f32 sy = pIt->Draw_GetSizeY();
        if (sy < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        f32 stretch = 1.0f;
        if (rSetting.typeOption0 != 0) {
            math::VEC3 move;
            pIt->GetMoveDir(&move);
            stretch += 0.5f * math::FSqrt(math::VEC3Dot(&move, &move)) / sy;
        }

        SetupGP(pIt, rSetting, rInfo, first, false);
        first = false;

        math::MTX34 rotMtx = CalcRotate(pIt, rSetting.typeAxis);
        math::MTX34 locMtx = CalcLocalTransform(rSetting.typeOption1, px, py,
                                                sx, sy, stretch, rotMtx);

        math::VEC3 axisY;
        pCalcAhead(&axisY, &ctx, pIt);

        math::VEC3 cross;
        math::VEC3Cross(&cross, &axisY, &dfltZ);
        if (Normalize(&cross)) {
            pIt->mPrevAxis = cross;
        } else {
            pIt->mPrevAxis = dfltY;
        }

        math::VEC3 axisZ;
        math::VEC3Cross(&axisZ, &pIt->mPrevAxis, &axisY);
        Normalize(&axisZ);

        math::MTX34 frameMtx(
            pIt->mPrevAxis.x, axisY.x, axisZ.x, 0.0f,
            pIt->mPrevAxis.y, axisY.y, axisZ.y, 0.0f,
            pIt->mPrevAxis.z, axisY.z, axisZ.z, 0.0f);

        math::MTX34 outMtx;
        math::MTX34Mult(&outMtx, &frameMtx, &locMtx);
        math::MTX34Mult(&outMtx, &posMtx, &outMtx);

        DrawQuad(outMtx, quadP, mNumTexmap > 0);

        if (rSetting.typeOption == EmitterDrawSetting::TYPE_CMN_CROSS) {
            DrawQuad(outMtx, quadPX, mNumTexmap > 0);
        }
    }
}

void DrawDirectionalStrategy::DrawDirectionalBillboard(const DrawInfo& rInfo,
                                                       ParticleManager* pManager) {

    InitGraphics(rInfo, pManager);

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    AheadContext ctx(*rInfo.GetViewMtx(), pManager);
    CalcAheadFunc pCalcAhead = GetCalcAheadFunc(pManager);

    math::MTX34 posMtx;
    math::MTX34 glbMtx;

    pManager->CalcGlobalMtx(&glbMtx);
    math::MTX34Mult(&posMtx, rInfo.GetViewMtx(), &glbMtx);
    GXLoadPosMtxImm(mIdentityMtx, GX_PNMTX0);

    f32 vx = math::FSqrt(posMtx._00 * posMtx._00 + posMtx._10 * posMtx._10 +
                         posMtx._20 * posMtx._20);
    f32 vy = math::FSqrt(posMtx._01 * posMtx._01 + posMtx._11 * posMtx._11 +
                         posMtx._21 * posMtx._21);
    f32 vz = math::FSqrt(posMtx._02 * posMtx._02 + posMtx._12 * posMtx._12 +
                         posMtx._22 * posMtx._22);

    // Particles whose stored axis is flagged (x > 1) are reset, youngest first.
    Particle* pIt = GetYoungestParticle(pManager);
    while (pIt != NULL && pIt->mPrevAxis.x > 1.0f) {
        pIt->mPrevAxis = mZeroVec;
        pIt = GetElderParticle(pManager, pIt);
    }

    f32 px = rSetting.pivotX / 100.0f;
    f32 py = rSetting.pivotY / 100.0f;

    GetFirstDrawParticleFunc pGetFirstFunc = GetGetFirstDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    bool first = true;

    for (pIt = pGetFirstFunc(pManager); pIt != NULL;
         pIt = pGetNextFunc(pManager, pIt)) {

        f32 sx = pIt->Draw_GetSizeX();
        if (sx < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        f32 sy = pIt->Draw_GetSizeY();
        if (sy < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        f32 stretch = 1.0f;
        if (rSetting.typeOption0 != 0) {
            math::VEC3 move;
            pIt->GetMoveDir(&move);
            stretch += 0.5f * math::FSqrt(math::VEC3Dot(&move, &move)) / sy;
        }

        SetupGP(pIt, rSetting, rInfo, first, false);
        first = false;

        math::MTX34 rotMtx = CalcRotate(pIt, rSetting.typeAxis);
        math::MTX34 locMtx = CalcLocalTransform(rSetting.typeOption1, px, py,
                                                sx, sy, stretch, rotMtx);

        math::VEC3 perp;
        math::VEC3 axisY;
        math::VEC3 dir2;
        math::VEC3 axis;

        pCalcAhead(&axis, &ctx, pIt);
        math::VEC3TransformNormal(&axis, &posMtx, &axis);

        axisY = axis;
        Normalize(&axisY);

        perp = math::VEC3(axis.y, -axis.x, 0.0f);
        if (Normalize(&perp)) {
            // HYPOTHESIS: the cross product with a normalized perp is inlined here.
            dir2 = math::VEC3(-axis.x * axis.z, -axis.y * axis.z,
                              axis.x * axis.x + axis.y * axis.y);
        } else {
            math::VEC3Cross(&perp, &axis, &pIt->mPrevAxis);
            if (!Normalize(&perp)) {
                perp = mXUnitVec;
            }
            math::VEC3Cross(&dir2, &perp, &axis);
        }

        Normalize(&dir2);
        pIt->mPrevAxis = dir2;

        math::VEC3 pos;
        math::VEC3Transform(&pos, &posMtx, &pIt->mParameter.mPosition);

        math::MTX34 billboardMtx(
            vx * perp.x, vy * axisY.x, vz * dir2.x, pos.x,
            vx * perp.y, vy * axisY.y, vz * dir2.y, pos.y,
            vx * perp.z, vy * axisY.z, vz * dir2.z, pos.z);

        math::MTX34 mtx;
        math::MTX34Mult(&mtx, &billboardMtx, &locMtx);

        math::VEC3 center(mtx._03, mtx._13, mtx._23);

        math::VEC3 dA(mtx._00 + mtx._01, mtx._10 + mtx._11, mtx._20 + mtx._21);
        math::VEC3 dB(mtx._00 - mtx._01, mtx._10 - mtx._11, mtx._20 - mtx._21);
        DrawQuad(center, dA, dB, mNumTexmap != 0);

        if (rSetting.typeOption == EmitterDrawSetting::TYPE_CMN_CROSS) {
            math::VEC3 dC(mtx._01 + mtx._02, mtx._11 + mtx._12,
                          mtx._21 + mtx._22);
            math::VEC3 dD(mtx._01 - mtx._02, mtx._11 - mtx._12,
                          mtx._21 - mtx._22);
            DrawQuad(center, dC, dD, mNumTexmap != 0);
        }
    }
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

    case 6:
    default: {
        return CalcAhead_Speed;
    }
    }
}

} // namespace ef
} // namespace nw4r
