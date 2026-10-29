#pragma once

// The damage module of the enemies (mo_enemy/sora_enemy/damage/em_damage_module_impl.cpp): soDamageModuleImpl with the
// effector (a soDamageEffectorImpl) and a second anim cmd observer built into the same object, and the whole enemy damage
// reaction (the transactor's job for fighters) in the module itself. Its transactor slot holds a soDamageTransactorNull.

#include <StaticAssert.h>
#include <so/damage/so_damage_module_impl.h>
#include <types.h>
#include <wn/wn_damage_effector_impl.h>

class Enemy;
class emKineticModuleImpl;
class soGroundModule;
class soPostureModule;
class soModuleAccesser;
class soCollisionLog;

// Who made the last hit (emDamageModuleImpl::m_attackerKind).
enum emDamageAttackerKind {
    EmDamageAttackerKind_Fighter = 0,
    EmDamageAttackerKind_Enemy = 1,
    EmDamageAttackerKind_None = 5, // no hit recorded
};

// The original's RTTI lists soDamageModuleImpl at 0, the damage effector at 0xAC and a second soAnimCmdEventObserver at
// 0xB8 (the first one is the base of soDamageModuleImpl), in that order.
class emDamageModuleImpl : public soDamageModuleImpl, public soDamageEffectorImpl, public soAnimCmdEventObserver {
public:
    // Who hit the enemy last (updateDamageAttackInfo): the Fighter's entry id or the Enemy's create id, whether it was
    // a fighter or an enemy, its kind (soGetSubKind) and the position of the hit.
    // HYPOTHESIS: names.
    int m_attackerId;           // 0xC4
    int m_attackerKind;         // 0xC8
    int m_attackerSubKind;      // 0xCC
    Vec3f m_attackerPos;        // 0xD0
    // HYPOTHESIS: 1 while the last hit left the enemy no reaction (set by notifyEventCollisionHit from the hit's
    // power and the hit part's multiplier, consumed by onDamage), -1 when there is none.
    int m_damageReactState;     // 0xDC

    emDamageModuleImpl(soModuleAccesser* acc, soArray<soDamage>* damages);
    virtual ~emDamageModuleImpl();

    virtual void activate(float damage);
    virtual void deactivate();
    virtual void storeDamage(soDamage* damage, float addedDamage);
    virtual bool isCaptureCut(void* p);
    virtual bool onDamage(u32 damageIndex);
    virtual float getReactionMul(soCollisionAttackData* attackData, u32 damageIndex, int hitIndex);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int hitPart,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void notifyEventCollisionHit2nd(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog,
                                            u32 groupIndex, float posX, float collisionLr, soModuleAccesser* moduleAccesser,
                                            bool unk);
    // The effector slots of the second base.
#if SO_DAMAGE_EFFECTOR_ACCESSER_FIRST_DECLARED
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr,
                                 soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#else
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser,
                                 soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#endif
    // HYPOTHESIS: the module map names this slot reqEffectScreen (it is empty here).
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser,
                                      soCollisionAttackData* attackData);

    // Module map names. HYPOTHESIS: addDamageHp subtracts *damage from the strength and returns true when that defeated
    // the enemy.
    bool addDamageHp(float* damage, int p2);
    // HYPOTHESIS: the last parameter is a bool (the Paratroopa's call reuses the register of its first 0, as for a bool).
    float setupDamageStatusNormalSubDamageKinetic(emKineticModuleImpl* kinetic, soPostureModule* posture, float speed,
                                                  Vec2f* dir, bool isKnockout, int attackVector, float lr, bool spin);
    // The position the damage of the enemy is taken at (Enemy::getDamageOffsetFromNode).
    Vec3f getDamagePos();

private:
    void initDamageAttackInfo();
    void updateDamageAttackInfo(soCollisionLog* collisionLog);
    int getDamageLevel(float frameReaction);
    bool isDamageVectorMeteor(int attackVector);
    bool checkDamageMeteor(int attackVector, int* meteorFrame);
    float getDamageAngle(float reaction, int attackVector, int situation, bool isKnockout, bool isFlinchless, Vec2f speed,
                         float lr);
    void reqEffect(soDamage* damage, int level, bool unk1, bool unk2);
    int getQuakeKind(int level, float frameReaction, bool launched);
    void setupDamageStatusNormalSubRotDamage(emKineticModuleImpl* kinetic, float lr, float rotSpeed, int energyA,
                                             int energyB);
    void setupDamageStatusNormalSubReflect(soGroundModule* ground, Vec2f* speed);
    void setupDamageStatusNormal(soDamage* damage, bool unk);
    void setupDamageStatusTurnRotEnergy(Enemy* enemy, emKineticModuleImpl* kinetic, soPostureModule* posture, int front,
                                        int frames, int* outFrames);
    void setupDamageStatusTurn(soDamage* damage);
    void setupDamageStatusInvalidityReaction(soDamage* damage);
    void setupDamageStatusNoReaction(soDamage* damage);
    float getNoReactionValue();
    bool addDamageFigure(float threshold, float hpRate);
    bool onDamageSubCheckStatusTransition();
};
static_assert(sizeof(emDamageModuleImpl) == 0xE0, "Class is wrong size!");
