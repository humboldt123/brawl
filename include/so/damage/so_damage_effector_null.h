#pragma once

// soDamageEffectorNull: the damage effector of stage objects that show no damage reaction (weapons). Every request is
// ignored; the virtuals are inline (weak copies where the vtable is needed).

#include <so/damage/so_damage_effector_impl.h>

class soDamageEffectorNull : public soDamageEffector {
public:
    virtual ~soDamageEffectorNull() { }
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqShake(soModuleAccesser*, int, Vec2f*, soCollisionAttackData*, int);
    #else
    virtual void reqShake(soModuleAccesser*, int, Vec2f*, soCollisionAttackData*, int) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqCommonEffect(float, float, float, soModuleAccesser*, soCollisionAttackData*, soCollisionLog*);
    #else
    virtual void reqCommonEffect(float, float, float, soModuleAccesser*, soCollisionAttackData*, soCollisionLog*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqUniqEffect(soModuleAccesser*, int, soCollisionAttackData*);
    #else
    virtual void reqUniqEffect(soModuleAccesser*, int, soCollisionAttackData*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqInvincibleEffect(soModuleAccesser*, soCollisionLog*);
    #else
    virtual void reqInvincibleEffect(soModuleAccesser*, soCollisionLog*) { }
    #endif
    // HYPOTHESIS: the module maps name this slot reqEffectScreen (all implementations).
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqDamageEffectParam(float, float, soModuleAccesser*, soCollisionAttackData*);
    #else
    virtual void reqDamageEffectParam(float, float, soModuleAccesser*, soCollisionAttackData*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser*, Vec2f*);
    #else
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser*, Vec2f*) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqQuake(float, soModuleAccesser*, int);
    #else
    virtual void reqQuake(float, soModuleAccesser*, int) { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqStop();
    #else
    virtual void reqStop() { }
    #endif
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual void reqTipEffect(soModuleAccesser*, soCollisionLog*);
    #else
    virtual void reqTipEffect(soModuleAccesser*, soCollisionLog*) { }
    #endif
};

// Opt-in (WN_DAMAGE_EFFECTOR_NULL_EXTERN): the shared instance (a sora_melee object) that some article
// builders pass to their damage module. Other units instantiate the generic holder and emit their own copy.
#ifdef WN_DAMAGE_EFFECTOR_NULL_EXTERN
extern soDamageEffectorNull g_soDamageEffectorNull;
template <typename T> class soSingletonHolder;
template <> class soSingletonHolder<soDamageEffectorNull> {
public:
    static soDamageEffectorNull* getInstance() { return &g_soDamageEffectorNull; }
};
#endif
