#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <wn/wn_damage_effector_impl.h>

// The damage effector of the Subspace enemies' weapons (wnem articles): one shared instance that the wnem article
// builders hand their damage modules. It overrides the same slots as the fighters' wnDamageEffectorImpl, but reacts to
// a hit: the shake module shakes the weapon, the anim cmd module plays a damage effect script, and a hard hit in the
// air quakes the camera.
class wnemDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~wnemDamageEffectorImpl();
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
#if SO_DAMAGE_EFFECTOR_ACCESSER_FIRST_DECLARED
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#else
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#endif
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    // HYPOTHESIS: the module map names this slot reqEffectScreen (as for wnDamageEffectorImpl).
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
#if SO_DAMAGE_EFFECTOR_ACCESSER_FIRST_DECLARED
    virtual void reqQuake(soModuleAccesser* moduleAccesser, float frameReaction, int level);
#else
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
#endif
    virtual void reqStop();
};
static_assert(sizeof(wnemDamageEffectorImpl) == 0xC, "Class is wrong size!");

// The shared instance (handed to the damage modules of the wnem articles).
extern wnemDamageEffectorImpl g_wnemDamageEffectorImpl;
