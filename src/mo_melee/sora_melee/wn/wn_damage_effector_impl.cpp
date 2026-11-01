#include <wn/wn_damage_effector_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: map name soDamageEffectorImpl::reqCommonEffectParam (static); it stays unnamed in sora_melee.
extern "C" void fn_27_60CDC(float power, float reaction, float lr, float param0, float param1, float param2, float param3, float param4,
                            soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);

// HYPOTHESIS: five common fighter parameters handed to the common damage effect.
struct wnDamageEffectCommonParamView {
    u8 unk0[0x36C];
    float unk36C;
    float unk370;
    float unk374;
    float unk378;
    float unk37C;
};

static inline const wnDamageEffectCommonParamView* getDamageEffectCommonParam() {
    return reinterpret_cast<const wnDamageEffectCommonParamView*>(g_ftCommonDataAccesser.getParamCommon());
}

void wnDamageEffectorImpl::reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame) { }

void wnDamageEffectorImpl::reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
    // BYTES: declared ahead of the loads, which run in reverse argument order, to get the original float registers.
    float param1;
    float param2;
    float param3;
    float param4;
    param4 = getDamageEffectCommonParam()->unk37C;
    param3 = getDamageEffectCommonParam()->unk378;
    param2 = getDamageEffectCommonParam()->unk374;
    param1 = getDamageEffectCommonParam()->unk370;
    fn_27_60CDC(power, reaction, lr, getDamageEffectCommonParam()->unk36C, param1, param2, param3, param4,
                moduleAccesser, attackData, collisionLog);
}

void wnDamageEffectorImpl::reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData) { }

void wnDamageEffectorImpl::reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level) { }

void wnDamageEffectorImpl::reqStop() { }

void wnDamageEffectorImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) { }

wnDamageEffectorImpl::~wnDamageEffectorImpl() { }

// HYPOTHESIS: the instance labelled g_soDamageEffectorNull in symbols.txt (the effector the weapon builders share).
wnDamageEffectorImpl g_wnDamageEffectorImpl;
