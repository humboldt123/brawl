#pragma once

#include <wn/wn_weapon_builder.h>
#include <so/controller/so_controller_module_link_ref.h>

struct wnArticleBuildInfo;

// HYPOTHESIS: parameter names beyond their uses in activate (launch angle in degrees, launch speed, gravity and the
// speed limits of the normal and gravity energies); twelve floats, ids 4000...
struct wnLucasPKFlashParam {
    float unk0;
    float unk4;
    float unk8;
    float m_launchAngle; // +0xC
    float m_launchSpeed; // +0x10
    float unk14;
    float m_gravity;     // +0x18
    float m_speedLimitX; // +0x1C
    float m_speedLimitY; // +0x20
    float unk24;
    float unk28;
    float unk2C;
};

#ifdef WN_WEAPON_BUILDER
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_kinetic_energy_gravity.h>

// HYPOTHESIS: layout of the untouched part of the static article data.
struct wnLucasPKFlashData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLucasPKFlashParam* m_param; // +0x34
};

class wnLucasPKFlashParamAccesser : public wnExtendParamAccesser<3999, 12, 23999, 0, 0, 0> {
public:
    wnLucasPKFlashParamAccesser(const wnLucasPKFlashParam* param) : wnExtendParamAccesser<3999, 12, 23999, 0, 0, 0>(77) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->m_launchAngle;
        m_floats[4] = &param->m_launchSpeed;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->m_gravity;
        m_floats[7] = &param->m_speedLimitX;
        m_floats[8] = &param->m_speedLimitY;
        m_floats[9] = &param->unk24;
        m_floats[10] = &param->unk28;
        m_floats[11] = &param->unk2C;
    }
    virtual ~wnLucasPKFlashParamAccesser() { }
};
static_assert(sizeof(wnLucasPKFlashParamAccesser) == 0x58, "PK Flash parameter accesser extent");

// HYPOTHESIS: the inside event, model, attack and link configurations (the status, motion, controller and animation
// command builders are named in the module map).
class wnLucasPKFlashModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
        wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<4, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0,
        soCollisionAttackModuleImpl, 1, false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 3, 1>, 4,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnLucasPKFlash : public wnWeaponBuilder<wnLucasPKFlashModuleAccesserBuildConfig> {
    const wnLucasPKFlashParam* m_param;          // +0x1FE4
    wnLucasPKFlashParamAccesser m_paramAccesser; // +0x1FE8
    wnKineticEnergyGravity m_energyGravity;     // +0x2040
#else
// HYPOTHESIS: the article data type the class info fetches for kind 77; only passed as a pointer here.
struct wnLucasPKFlashData;
class wnLucasPKFlash : public Weapon {
    u8 m_storage[0x2054 - sizeof(Weapon)];
#endif
public:
    wnLucasPKFlash(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKFlashData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnLucasPKFlash() { }
#else
    virtual ~wnLucasPKFlash();
#endif
    // HYPOTHESIS: parameter names.
    void activate(int founderTaskId, int team, const Vec3f* pos, float lr);
    virtual void onDeactivate();
    void shoot();
};
static_assert(sizeof(wnLucasPKFlash) == 0x2054, "Lucas PK Flash article extent");
