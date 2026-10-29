#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_log.h>
#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;

// Vtable order recovered from the soDamageEffectorImpl vtable (slot -> map name) and the
// call sites in soDamageTransactorActor (argument lists).
// HYPOTHESIS: the parameter lists are inferred from the call sites; the slot at +0x28 is
// pure virtual in soDamageEffectorImpl and its name here is a guess.
// The accesser comes first in reqCommonEffect and reqQuake: the sora_enemy REL's wnemDamageEffectorImpl saves it before
// the float parameters, and its call of the common effect helper passes it first (MWCC keeps parameter order). The
// register assignment is the same either way, so this is opt-in until the other effectors and their callers are checked.
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
#define SO_DAMAGE_EFFECTOR_ACCESSER_FIRST_DECLARED 1
#endif
// SO_DAMAGE_EFFECTOR_PRIVATE_NULL_BASE (opt-in): no original type information lists soNull among the bases (the
// sora_enemy wnem effector's base list is soNullable, soDamageEffector, soDamageEffectorImpl); MWCC leaves private bases
// out of the list, and the layout (the effector's own vptr at +0, soNullable at +4) is the same either way.
#ifdef SO_DAMAGE_EFFECTOR_PRIVATE_NULL_BASE
class soDamageEffector : private soNull, public soNullable {
#else
class soDamageEffector : public soNull, public soNullable {
#endif
public:
// SO_DAMAGE_EFFECTOR_OUTLINE_DTOR (opt-in): the same constructor and out-of-line destructor as the weapon builder's, for
// a unit that is no weapon builder (the sora_enemy damage module, which embeds an effector).
#if defined(WN_WEAPON_BUILDER) || defined(SO_DAMAGE_EFFECTOR_OUTLINE_DTOR)
    soDamageEffector() : soNullable(false) { }
#endif
#if defined(WN_WEAPON_BUILDER) || defined(SO_DAMAGE_EFFECTOR_OUTLINE_DTOR)
    // MATCH-ONLY: not inlined into the derived destructor (weak copy in the article units).
    virtual ~soDamageEffector() __attribute__((never_inline)) { }
#else
    virtual ~soDamageEffector() { }
#endif
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#else
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#endif
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    virtual void reqInvincibleEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser* moduleAccesser, Vec2f* normal);
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
    virtual void reqQuake(soModuleAccesser* moduleAccesser, float frameReaction, int level);
#else
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
#endif
    virtual void reqStop();
    virtual void reqTipEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
};
