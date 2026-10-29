#include <so/so_module_accesser.h>
#include <types.h>
#include <yk/yk_damage_effector_impl.h>

// HYPOTHESIS: map name soDamageEffectorImpl::reqCommonEffectParam (static); it stays unnamed in sora_melee.
extern "C" void fn_27_60CDC(float power, float reaction, float lr, float param0, float param1, float param2, float param3, float param4,
                            soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);

void ykDamageEffectorImpl::reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
    fn_27_60CDC(power, reaction, lr, 0.04f, 0.3f, 0.3f, 1.5f, 180.0f, moduleAccesser, attackData, collisionLog);
}

void ykDamageEffectorImpl::reqStop() { }

void ykDamageEffectorImpl::reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level) { }

void ykDamageEffectorImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) { }

void ykDamageEffectorImpl::reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData) { }

void ykDamageEffectorImpl::reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame) { }

ykDamageEffectorImpl::~ykDamageEffectorImpl() { }

// HYPOTHESIS: the shared yakumono effector instance (unnamed in symbols.txt).
ykDamageEffectorImpl g_ykDamageEffectorImpl;
