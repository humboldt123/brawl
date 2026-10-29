// The weapon-builder view of soDamageEffector: its destructor stays out of line (the REL's weak copy that ours calls).
#define WN_WEAPON_BUILDER
// The sora_enemy calls of interpretCmd1 and resetInterpreter2 pass the frame last (see so/anim/so_anim_cmd_module_impl.h),
// and this effector takes the accesser first in reqCommonEffect and reqQuake (see so/damage/so_damage_effector_impl.h).
#define SO_ANIM_CMD_INTERPRET_CMD1_FRAME_LAST
#define SO_ANIM_CMD_RESET_INTERPRETER2_FRAME_LAST
#define SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
// Keeps soNull out of the type information's base lists, as in the original.
#define SO_DAMAGE_EFFECTOR_PRIVATE_NULL_BASE
#include <wn/wn_em_damage_effector_impl.h>
#include <ft/ft_common_data_accesser.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_module_hyp.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: map name soDamageEffectorImpl::reqCommonEffectParam (static); it stays unnamed in sora_melee.
// The parameter order is the one this unprescheduled REL caller shows (MWCC evaluates simple arguments in parameter
// order: the accesser before the floats, the four parameters after the attack data and the log); the register
// assignment is the same as wn/ft_damage_effector_impl.cpp's declaration.
extern "C" void fn_27_60CDC(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, float param0,
                            soCollisionAttackData* attackData, soCollisionLog* collisionLog, float param1, float param2,
                            float param3, float param4);

// HYPOTHESIS: five common fighter parameters handed to the common damage effect (as in wn_damage_effector_impl.cpp).
struct wnemDamageEffectCommonParamView {
    u8 unk0[0x36C];
    float unk36C;
    float unk370;
    float unk374;
    float unk378;
    float unk37C;
};

static inline const wnemDamageEffectCommonParamView* getDamageEffectCommonParam() {
    return reinterpret_cast<const wnemDamageEffectCommonParamView*>(g_ftCommonDataAccesser.getParamCommon());
}

// MATCH-ONLY: the original reads the attack data bitfields as whole words (lwz) with unsigned extraction.
static inline u32 getAttackDataWord(soCollisionAttackData* attackData, int offset) {
    return *(u32*)((u8*)attackData + offset);
}

// HYPOTHESIS: the anim cmd group that plays the weapon's damage effect scripts.
static const int DamageEffectAnimCmdGroup = 6;

// Restarts the damage effect group on the given script.
static inline void resetDamageEffectScript(soModuleAccesser* moduleAccesser, u32 script) {
#if SO_ANIM_CMD_RESET_INTERPRETER2_FRAME_LAST_DECLARED
    moduleAccesser->getAnimCmdModule().resetInterpreter2(DamageEffectAnimCmdGroup, moduleAccesser, script, 0.0f);
#else
    // TODO(nonmatching): reqUniqEffect 78.0%, (a) the original loads the frame after the integer arguments
    // (frame-last declaration, $B/E2/shared_patch_animcmd.diff); exact with the patch.
    moduleAccesser->getAnimCmdModule().resetInterpreter2(0.0f, DamageEffectAnimCmdGroup, moduleAccesser, script);
#endif
}

void wnemDamageEffectorImpl::reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame) {
    // HYPOTHESIS: situation 0 is the ground.
    getShakeModule(moduleAccesser).req(0, hitStopFrame, situation == 0, normal);
}

#if SO_DAMAGE_EFFECTOR_ACCESSER_FIRST_DECLARED
void wnemDamageEffectorImpl::reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
#else
// TODO(nonmatching): 96.9%, (a) the original saves the accesser before the float parameters (accesser-first
// declaration, $B/E3/shared_patch.diff); calls, arguments, constants and branches agree. Exact with the patch.
void wnemDamageEffectorImpl::reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
#endif
    // MATCH-ONLY: declared ahead of the loads, which run in reverse argument order, to get the original float registers.
    float param1;
    float param2;
    float param3;
    float param4;
    param4 = getDamageEffectCommonParam()->unk37C;
    param3 = getDamageEffectCommonParam()->unk378;
    param2 = getDamageEffectCommonParam()->unk374;
    param1 = getDamageEffectCommonParam()->unk370;
    float param0 = getDamageEffectCommonParam()->unk36C;
    fn_27_60CDC(moduleAccesser, power, reaction, lr, param0, attackData, collisionLog, param1, param2, param3, param4);
}

// Picks the damage effect script by the attack's element (the level offsets the script within an element, as the
// fighters' effect ids do in ftDamageEffectorImpl::reqUniqEffect) and runs it.
void wnemDamageEffectorImpl::reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData) {
    switch (getAttackDataWord(attackData, 0x30) & 0x1F) {
        case soCollisionAttackData::Attribute_Fire:
            resetDamageEffectScript(moduleAccesser, level + 3);
            break;
        case soCollisionAttackData::Attribute_Electric:
            resetDamageEffectScript(moduleAccesser, level + 7);
            break;
        case soCollisionAttackData::Attribute_Purple:
            resetDamageEffectScript(moduleAccesser, level + 11);
            break;
        case soCollisionAttackData::Attribute_Aura:
            resetDamageEffectScript(moduleAccesser, level + 15);
            break;
        default:
            resetDamageEffectScript(moduleAccesser, 0);
            break;
    }
    moduleAccesser->getAnimCmdModule().interpretCmd1(DamageEffectAnimCmdGroup, moduleAccesser, 0.0f);
}

#if SO_DAMAGE_EFFECTOR_ACCESSER_FIRST_DECLARED
void wnemDamageEffectorImpl::reqQuake(soModuleAccesser* moduleAccesser, float frameReaction, int level) {
#else
// TODO(nonmatching): 95.7%, (a) the accesser is saved after the frame here (accesser-first declaration,
// $B/E3/shared_patch.diff); everything else agrees. Exact with the patch.
void wnemDamageEffectorImpl::reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level) {
#endif
    if (level == 3 && moduleAccesser->getSituationModule().getKind() != Situation_Ground) {
        soCameraModule::QuakeKind kind;
        if (frameReaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0x7F4, 0)) {
            kind = soCameraModule::Quake_XL;
        } else {
            kind = (soCameraModule::QuakeKind)(frameReaction >= soValueAccesser::getConstantFloat(moduleAccesser, 0x7F3, 0));
        }
        moduleAccesser->getCameraModule().reqQuake(kind, moduleAccesser);
    }
}

void wnemDamageEffectorImpl::reqStop() { }

void wnemDamageEffectorImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) { }

wnemDamageEffectorImpl::~wnemDamageEffectorImpl() { }

wnemDamageEffectorImpl g_wnemDamageEffectorImpl;
