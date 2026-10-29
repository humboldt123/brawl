#pragma once

#include <wn/wn_damage_transactor_actor.h>

extern "C" {
extern char lbl_117_data_C488[];
}

class wnDededeFollowerDamageTransactorImpl : public wnDamageTransactorActor {
public:
    virtual ~wnDededeFollowerDamageTransactorImpl() __attribute__((never_inline)) { }
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
    virtual float getReactionSub(soModuleAccesser*) { return 0.0f; }
    virtual float getReactionMul(soModuleAccesser*) { return 1.0f; }
    virtual float getDamageMul(soModuleAccesser*) { return 1.0f; }
    virtual void onLeaveDamageStop(soModuleAccesser*);
};
static_assert(sizeof(wnDededeFollowerDamageTransactorImpl) == 4, "Follower damage transactor size");

extern wnDededeFollowerDamageTransactorImpl g_wnDededeFollowerDamageTransactorImpl;
