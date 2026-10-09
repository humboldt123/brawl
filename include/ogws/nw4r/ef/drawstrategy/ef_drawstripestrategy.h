#ifndef NW4R_EF_DRAW_STRIPE_STRATEGY_H
#define NW4R_EF_DRAW_STRIPE_STRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawStripeStrategy : public DrawStrategyImpl {
public:
    struct Trigonometric {
        f32 mCos; // at 0x0
        f32 mSin; // at 0x4
    };

    struct VertexTube {
        math::VEC3 mCenter; // at 0x0
        math::VEC3 mX;      // at 0xC
        math::VEC3 mZ;      // at 0x18
        f32 mTexCoord;      // at 0x24
    };

    struct AheadContextStripe : public AheadContext {
        math::VEC3 mEmitterAxisX;      // at 0xBC
        math::VEC3 mScreenAxisZ;       // at 0xC8
        Trigonometric* mTrigonometric; // at 0xD4
    };

public:
    DrawStripeStrategy();

    virtual void Draw(const DrawInfo& rInfo,
                      ParticleManager* pManager); // at 0xC

    virtual CalcAheadFunc
    GetCalcAheadFunc(ParticleManager* pManager); // at 0x18

    u8 GetStripeTexmapType(const EmitterDrawSetting& rSetting) const;

    void DrawStripe(AheadContextStripe* pContext, int param,
                    const math::VEC3& rAxisA, const math::VEC3& rAxisB);

    void DrawParticle(AheadContextStripe* pContext, int param,
                      Particle* pParticle, CalcAheadFunc pCalcAhead,
                      const math::VEC3& rPos, const math::VEC3& rAxisA,
                      const math::VEC3& rAxisB, f32 pivot, f32 t);

    static void CalcAhead_Particle_Stripe(math::VEC3* pAxisY,
                                          AheadContextStripe* pContext,
                                          Particle* pParticle);
    static void CalcAhead_ParticleBoth_Stripe(math::VEC3* pAxisY,
                                              AheadContextStripe* pContext,
                                              Particle* pParticle);
    static void CalcAhead_ParticleBoth_Ring(math::VEC3* pAxisY,
                                            AheadContextStripe* pContext,
                                            Particle* pParticle);
    static void CalcAhead_ParticleBoth_Origin(math::VEC3* pAxisY,
                                              AheadContextStripe* pContext,
                                              Particle* pParticle);
};

} // namespace ef
} // namespace nw4r

#endif
