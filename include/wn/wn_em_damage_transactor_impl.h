#pragma once

#include <StaticAssert.h>
#include <so/damage/so_damage_transactor_actor.h>
#include <types.h>

// The damage transactor of the Subspace enemies' weapons (wnem articles): one shared instance that the wnem article
// builders hand their damage modules. A weapon takes no damage reaction of its own: a hit that carries damage or
// reaction requests the normal damage setup, which only plays the effects (the uniq effect and the shake); every
// status, turn and sleep query answers "no".
// RTTI: wnemDamageTransactorImpl : soDamageTransactorActor : soDamageTransactor.
class wnemDamageTransactorImpl : public soDamageTransactorActor {
public:
    // MATCH-ONLY: the static initializer calls an out-of-line (weak) copy of the constructor.
    wnemDamageTransactorImpl() __attribute__((never_inline)) { }
    // Inline: its weak copy follows the actor's inline members, as in the original.
    virtual ~wnemDamageTransactorImpl() { }
    // HYPOTHESIS: the native return register is pointer-sized; here it is always 0 (no damage value table).
    virtual int getDamageValueParam(soModuleAccesser* moduleAccesser);
    virtual bool onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
    virtual int getDamageStatusKind(soModuleAccesser* moduleAccesser);
    virtual bool isUseTurnDamage(soModuleAccesser* moduleAccesser);
    virtual bool isUseTurn(soModuleAccesser* moduleAccesser);
    virtual bool isApplyTurnDamage(soModuleAccesser* moduleAccesser);
    virtual int getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex);
    virtual bool isSlip(soModuleAccesser* moduleAccesser, float slipChance);
    virtual bool isSleepStatus(soModuleAccesser* moduleAccesser);
    virtual bool isParalyzeDamage(soModuleAccesser* moduleAccesser);
    virtual void addSleepTime(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void onFlowerDamage(soModuleAccesser* moduleAccesser, soDamage* damage);
    virtual void onParalyzeDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void setFlagDownDamage3(soModuleAccesser* moduleAccesser, bool flag);
    virtual bool isCheckGroundDamage(soModuleAccesser* moduleAccesser);
    virtual void onGroundDamageAfter(soModuleAccesser* moduleAccesser);
    virtual bool onCompositionDamageSpeed(soModuleAccesser* moduleAccesser, soDamage* damage, Vec2f* speed, int level);
    virtual bool onDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void setupDamageStatusNormal(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog, int unk);
#ifndef SO_DAMAGE_TRANSACTOR_ACTOR_INLINE_TAIL
    // Without the inline actor tail (so/damage/so_damage_transactor_actor.h) the weak copy is declared here.
    virtual bool isSpeedDamage(soModuleAccesser* moduleAccesser) { return true; }
#endif
    // The article damage actors' damage-stop callback (see wn/wn_damage_transactor_actor.h); nothing to do here.
    virtual void onLeaveDamageStop(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(wnemDamageTransactorImpl) == 4, "Class is wrong size!");

// The shared instance (handed to the damage modules of the wnem articles).
extern wnemDamageTransactorImpl g_wnemDamageTransactorImpl;
