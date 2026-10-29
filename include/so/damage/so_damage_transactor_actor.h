#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/damage/so_damage.h>
#include <types.h>

class soModuleAccesser;

// Interface implemented by the per-StageObject-kind damage transactors
// (ftDamageTransactorImpl, itDamageTransactorImpl, soDamageTransactorNull ...).
// The vtable order below was recovered from the itDamageTransactorImpl vtable
// and the call sites in soDamageTransactorActor.
// HYPOTHESIS: parameter lists of the slots that soDamageTransactorActor does not
// call itself are guesses; only the slot order is confirmed.
class soDamageTransactor {
public:
    virtual ~soDamageTransactor() { }
    virtual int getDamageValueParam(soModuleAccesser* moduleAccesser) = 0;
    virtual bool onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) = 0;
    virtual int getDamageStatusKind(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isUseTurnDamage(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isUseTurn(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isApplyTurnDamage(soModuleAccesser* moduleAccesser) = 0;
    virtual int getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex) = 0;
    virtual float getHitStopMul(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isSlip(soModuleAccesser* moduleAccesser, float slipChance) = 0;
    virtual bool isSleepStatus(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isParalyzeDamage(soModuleAccesser* moduleAccesser) = 0;
    virtual void addSleepTime(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) = 0;
    virtual void onFlowerDamage(soModuleAccesser* moduleAccesser, soDamage* damage) = 0;
    virtual void onParalyzeDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) = 0;
    virtual void setFlagDownDamage3(soModuleAccesser* moduleAccesser, bool flag) = 0;
    virtual bool isCheckGroundDamage(soModuleAccesser* moduleAccesser) = 0;
    virtual void onGroundDamageAfter(soModuleAccesser* moduleAccesser) = 0;
    virtual bool onCompositionDamageSpeed(soModuleAccesser* moduleAccesser, soDamage* damage, Vec2f* speed, int level) = 0;
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual bool preProcessCheckDamage(soModuleAccesser* moduleAccesser, soDamage* damage);
    #else
    virtual bool preProcessCheckDamage(soModuleAccesser* moduleAccesser, soDamage* damage) { return false; }
    #endif
    virtual bool onDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) = 0;
    virtual void setupDamageStatusNormal(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog, int unk) = 0;
    virtual void setupDamageStatusTurn(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) = 0;
    virtual void setupSpeedDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) = 0;
    virtual void setupDamageStatusNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) = 0;
    virtual void setupDamageFlyRollStatus(float angle, float speed, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) = 0;
    virtual float getReactionSub(soModuleAccesser* moduleAccesser) = 0;
    virtual float getReactionMul(soModuleAccesser* moduleAccesser) = 0;
    #ifdef WN_NULL_DAMAGE_OUT_OF_LINE
    virtual float getWeightReactionMul(soModuleAccesser* moduleAccesser);
    #else
    virtual float getWeightReactionMul(soModuleAccesser* moduleAccesser) { return 1.0f; }
    #endif
    virtual float getDamageMul(soModuleAccesser* moduleAccesser) = 0;
    virtual void checkCheer(float reaction, float angle, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) = 0;
    virtual float getDamageForReaction(float damage, soModuleAccesser* moduleAccesser) = 0;
    virtual bool checkNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage) = 0;
    virtual int checkDownDamage(float reaction, float speed, soModuleAccesser* moduleAccesser) = 0;
#ifndef WN_DAMAGE_ACTOR_INTERFACE
#ifdef YK_STAGE_FULL // MATCH-ONLY: the stage RELs emit these three as plain inline functions of the base
    virtual bool isBindStatus(soModuleAccesser* moduleAccesser) { return false; }
    virtual bool isBuryStatus(soModuleAccesser* moduleAccesser) { return false; }
    virtual bool isSpeedDamage(soModuleAccesser* moduleAccesser) { return false; }
#else
    virtual bool isBindStatus(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isBuryStatus(soModuleAccesser* moduleAccesser) = 0;
    virtual bool isSpeedDamage(soModuleAccesser* moduleAccesser) = 0;
#endif
#endif
};
static_assert(sizeof(soDamageTransactor) == 4, "Class is wrong size!");

// SO_DAMAGE_TRANSACTOR_ACTOR_INLINE_TAIL (opt-in): the constant members are inline, as the sora_enemy REL's wnem damage
// transactor shows (it carries weak copies of them and no soDamageTransactorActor vtable). sora_melee's actor unit
// defines them out of line for now.
class soDamageTransactorActor : public soDamageTransactor {
public:
#ifndef SO_DAMAGE_TRANSACTOR_ACTOR_INLINE_TAIL
    virtual float getHitStopMul(soModuleAccesser* moduleAccesser);
#endif
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
    virtual void setupDamageStatusTurn(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void setupSpeedDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void setupDamageStatusNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog);
    virtual void setupDamageFlyRollStatus(float angle, float speed, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
#ifndef SO_DAMAGE_TRANSACTOR_ACTOR_INLINE_TAIL
    virtual float getReactionSub(soModuleAccesser* moduleAccesser);
    virtual float getReactionMul(soModuleAccesser* moduleAccesser);
    virtual float getDamageMul(soModuleAccesser* moduleAccesser);
    virtual void checkCheer(float reaction, float angle, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
    virtual float getDamageForReaction(float damage, soModuleAccesser* moduleAccesser);
    virtual bool checkNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage);
#endif
    virtual int checkDownDamage(float reaction, float speed, soModuleAccesser* moduleAccesser);
#ifdef SO_DAMAGE_TRANSACTOR_ACTOR_INLINE_TAIL
    // MATCH-ONLY order: MWCC emits these weak copies in reverse declaration order, and the wnem unit's copies run
    // checkNoReaction ... isBindStatus.
    virtual bool isBindStatus(soModuleAccesser* moduleAccesser) { return false; }
    virtual bool isBuryStatus(soModuleAccesser* moduleAccesser) { return false; }
    // HYPOTHESIS: the sora_enemy map names a weak copy soDamageTransactorActor::isSpeedDamage (returns true) in the
    // wnem damage transactor unit; sora_melee's actor unit has no copy of its own.
    virtual bool isSpeedDamage(soModuleAccesser* moduleAccesser) { return true; }
    virtual float getReactionSub(soModuleAccesser* moduleAccesser) { return 0.0f; }
    virtual float getReactionMul(soModuleAccesser* moduleAccesser) { return 1.0f; }
    virtual float getDamageMul(soModuleAccesser* moduleAccesser) { return 1.0f; }
    virtual void checkCheer(float reaction, float angle, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) { }
    virtual float getHitStopMul(soModuleAccesser* moduleAccesser) { return 1.0f; }
    virtual float getDamageForReaction(float damage, soModuleAccesser* moduleAccesser) { return damage; }
    virtual bool checkNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage) { return true; }
#else
    virtual bool isBindStatus(soModuleAccesser* moduleAccesser);
    virtual bool isBuryStatus(soModuleAccesser* moduleAccesser);
#endif

    bool onDamageSub(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog, bool* isNotFlinch);
};
