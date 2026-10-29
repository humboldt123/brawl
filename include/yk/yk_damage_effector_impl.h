#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <wn/wn_damage_effector_impl.h>

class ykDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~ykDamageEffectorImpl();
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    // HYPOTHESIS: the module map names this slot reqEffectScreen (the base declares it as reqDamageEffectParam).
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
    virtual void reqStop();
};
static_assert(sizeof(ykDamageEffectorImpl) == 0xC, "Class is wrong size!");
