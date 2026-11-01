#pragma once

#include <wn/wn_damage_transactor_actor.h>

extern "C" {
extern const float lbl_115_rodata_138;
extern const float lbl_115_rodata_13C;
}

class wnDiddyBarrelDamageTransactorImpl : public wnDamageTransactorActor {
public:
    // BYTES: preserve the out-of-line call from the module initializer.
    wnDiddyBarrelDamageTransactorImpl() __attribute__((never_inline)) { }
    virtual ~wnDiddyBarrelDamageTransactorImpl() __attribute__((never_inline)) { }
    // HYPOTHESIS: the native return register is pointer-sized; its pointed-to parameter type is unresolved.
    virtual int getDamageValueParam(soModuleAccesser*);
    virtual bool onDamageChangeStatusRequest(int, soModuleAccesser*, soDamageLog*);
    virtual int getDamageStatusKind(soModuleAccesser*);
    virtual bool isUseTurnDamage(soModuleAccesser*);
    virtual bool isUseTurn(soModuleAccesser*);
    virtual bool isApplyTurnDamage(soModuleAccesser*);
    virtual int getDamageHeight(soModuleAccesser*, u8);
    virtual float getHitStopMul(soModuleAccesser*);
    virtual bool isSlip(soModuleAccesser*, float);
    virtual bool isSleepStatus(soModuleAccesser*);
    virtual bool isParalyzeDamage(soModuleAccesser*);
    virtual void addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*);
    virtual void onFlowerDamage(soModuleAccesser*, soDamage*);
    virtual void onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*);
    virtual void setFlagDownDamage3(soModuleAccesser*, bool);
    virtual bool isCheckGroundDamage(soModuleAccesser*);
    virtual void onGroundDamageAfter(soModuleAccesser*);
    virtual bool onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int);
    virtual float getReactionSub(soModuleAccesser*) { return lbl_115_rodata_138; }
    virtual float getReactionMul(soModuleAccesser*) { return lbl_115_rodata_13C; }
    virtual float getDamageMul(soModuleAccesser*) { return lbl_115_rodata_13C; }
    virtual void onLeaveDamageStop(soModuleAccesser*);
};
static_assert(sizeof(wnDiddyBarrelDamageTransactorImpl) == 4, "Diddy damage transactor size");


extern wnDiddyBarrelDamageTransactorImpl g_wnDiddyBarrelDamageTransactorImpl;
template <typename T> class soSingletonHolder;
template <> class soSingletonHolder<wnDiddyBarrelDamageTransactorImpl> {
public:
    static wnDiddyBarrelDamageTransactorImpl* getInstance() {
        return &g_wnDiddyBarrelDamageTransactorImpl;
    }
};
