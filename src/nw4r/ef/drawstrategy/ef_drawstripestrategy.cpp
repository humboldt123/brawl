// nw4r ef_drawstripestrategy.cpp (main.dol 0x8017B400-0x8017E24C). Partially reconstructed.
#include <nw4r/ef.h>

namespace nw4r {
namespace ef {

DrawStripeStrategy::DrawStripeStrategy() {}

u8 DrawStripeStrategy::GetStripeTexmapType(
    const EmitterDrawSetting& rSetting) const {
    return rSetting.typeOption2 & 0xC0;
}

void DrawStripeStrategy::CalcAhead_Particle_Stripe(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    if (pYounger != NULL) {
        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pYounger->mParameter.mPosition);
    } else {
        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pContext->mCommon.mEmitterCenter);
    }

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

DrawStrategyImpl::CalcAheadFunc
DrawStripeStrategy::GetCalcAheadFunc(ParticleManager* pManager) {

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
        return reinterpret_cast<CalcAheadFunc>(CalcAhead_Particle_Stripe);
    }

    case EmitterDrawSetting::AHEAD_BB_PARTICLE_BOTH: {
        return CalcAhead_ParticleBoth;
    }

    case EmitterDrawSetting::AHEAD_CMN_NODESIGN: {
        switch (rSetting.typeOption2 & 7) {
        case 1: {
            return reinterpret_cast<CalcAheadFunc>(CalcAhead_ParticleBoth_Ring);
        }

        case 2: {
            return reinterpret_cast<CalcAheadFunc>(CalcAhead_ParticleBoth_Origin);
        }

        default: {
            return reinterpret_cast<CalcAheadFunc>(CalcAhead_ParticleBoth_Stripe);
        }
        }
    }

    case 6:
    default: {
        return CalcAhead_Speed;
    }
    }
}

void DrawStripeStrategy::CalcAhead_ParticleBoth_Stripe(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

    if (pElder != NULL) {
        math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&elderPos)) {
            elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3 youngerPos(0.0f, 0.0f, 0.0f);

    if (pYounger != NULL) {
        math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&youngerPos)) {
            youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawStripeStrategy::CalcAhead_ParticleBoth_Ring(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;

    Particle* pElder = GetElderDrawParticle(pManager, pParticle);
    if (pElder == NULL) {
        pElder = GetYoungestDrawParticle(pManager);
    }

    Particle* pYounger = GetYoungerDrawParticle(pManager, pParticle);
    if (pYounger == NULL) {
        pYounger = GetOldestDrawParticle(pManager);
    }

    math::VEC3 elderPos;
    math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&elderPos)) {
        elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3 youngerPos;
    math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&youngerPos)) {
        youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawStripeStrategy::CalcAhead_ParticleBoth_Origin(
    math::VEC3* pAxisY, AheadContextStripe* pContext, Particle* pParticle) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;

    Particle* pElder = GetElderDrawParticle(pManager, pParticle);
    Particle* pYounger = GetYoungerDrawParticle(pManager, pParticle);

    math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

    if (pElder != NULL) {
        math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
        Normalize(&elderPos);
    }

    math::VEC3 youngerPos;

    if (pYounger != NULL) {
        math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
    } else {
        math::VEC3Sub(&youngerPos, &pContext->mCommon.mEmitterCenter,
                      &pParticle->mParameter.mPosition);
    }
    Normalize(&youngerPos);

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawStripeStrategy::DrawStripe(AheadContextStripe* pContext, int param,
                                    const math::VEC3& rAxisA,
                                    const math::VEC3& rAxisB) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadFunc pCalcAhead = GetCalcAheadFunc(pManager);

    u16 drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;

    GetFirstDrawParticleFunc pGetFirst = GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNext = GetGetNextDrawParticleFunc(drawOrder);

    int count = GetNumDrawParticle(pManager);

    u8 assist = rSetting.typeOption2 & 7;
    bool isCross = (assist == EmitterDrawSetting::ASSIST_ST_CROSS);
    bool isBillboard = (assist == EmitterDrawSetting::ASSIST_ST_BILLBOARD);

    if (isCross || isBillboard) {
        count++;
    }

    f32 pivot = rSetting.pivotX * 0.01f;

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, count * 2);

    f32 step = 1.0f;
    if (GetStripeTexmapType(rSetting) != 0x40) {
        step = 1.0f / (f32)(count - 1);
    }

    int index;
    int inc;
    if (drawOrder == 0) {
        index = 0;
        inc = 1;
    } else {
        index = count - 1;
        inc = -1;
    }

    if (isBillboard && drawOrder == 0) {
        DrawParticle(pContext, param, NULL, pCalcAhead,
                     pContext->mCommon.mEmitterCenter, rAxisA, rAxisB, pivot,
                     step * index);
        index += inc;
    }

    for (Particle* pIt = pGetFirst(pManager); pIt != NULL;
         pIt = pGetNext(pManager, pIt)) {

        DrawParticle(pContext, param, pIt, pCalcAhead,
                     pIt->mParameter.mPosition, rAxisA, rAxisB, pivot,
                     step * index);
        index += inc;
    }

    if (isCross) {
        Particle* pFirst = pGetFirst(pManager);
        DrawParticle(pContext, param, pFirst, pCalcAhead,
                     pFirst->mParameter.mPosition, rAxisA, rAxisB, pivot,
                     step * index);
    }

    if (isBillboard && drawOrder != 0) {
        DrawParticle(pContext, param, NULL, pCalcAhead,
                     pContext->mCommon.mEmitterCenter, rAxisA, rAxisB, pivot,
                     step * index);
    }

    GXEnd();
}

} // namespace ef
} // namespace nw4r
