// BYTES: the soDamageEffectorImpl slots this effector inherits unchanged are emitted here as empty copies, in reverse
// vtable order, with the accesser-first reqQuake/reqCommonEffect signatures (as in the sora_enemy damage module).
#define EM_DAMAGE_EFFECTOR_IMPL_INLINE_SLOTS
#define SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
// BYTES: the destructor calls the out-of-line soDamageEffector destructor.
#define SO_DAMAGE_EFFECTOR_OUTLINE_DTOR
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_damage_effector_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// sora_melee: soDamageEffectorImpl::reqCommonEffectParam (map name, static); it stays unnamed there.
extern "C" void fn_27_60CDC(soModuleAccesser* moduleAccesser, float power, float reaction, float lr,
                            soCollisionAttackData* attackData, soCollisionLog* collisionLog, float param0, float param1,
                            float param2, float param3, float param4);

// The damage effector of a Pikmin: the common damage effect with the five effect parameters of the Pikmin constants
// 0xfc0..0xfc4 instead of the common fighter parameters.
class wnPikminPikminDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~wnPikminPikminDamageEffectorImpl() { }
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr,
                                 soCollisionAttackData* attackData, soCollisionLog* collisionLog);
    // HYPOTHESIS: the module map names this slot reqEffectScreen.
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser,
                                      soCollisionAttackData* attackData) { }
};

void wnPikminPikminDamageEffectorImpl::reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction,
                                                       float lr, soCollisionAttackData* attackData,
                                                       soCollisionLog* collisionLog) {
    // BYTES: declared ahead of the loads, which run in reverse argument order (as in wnDamageEffectorImpl).
    float param1;
    float param2;
    float param3;
    float param4;
    param4 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc4, 0);
    param3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0);
    param2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0);
    param1 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
    fn_27_60CDC(moduleAccesser, power, reaction, lr, attackData, collisionLog,
                soValueAccesser::getConstantFloat(moduleAccesser, 0xfc0, 0), param1, param2, param3, param4);
}

wnPikminPikminDamageEffectorImpl g_wnPikminPikminDamageEffectorImpl;
