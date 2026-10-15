#pragma once

// Lucas's PK Starstorm (article kind 81): a star that falls in from the upper corner of the screen along a diagonal, speeding up as it
// falls. HYPOTHESIS: all field names; offsets follow the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the star: two floats (ids 4000..) and one int (id 24000).
struct wnLucasPKStarStormParam {
    float m_speed;   // initial speed along the fall direction
    float m_accel;   // acceleration along the fall direction
    s32 unk8;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnLucasPKStarStormData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLucasPKStarStormParam* m_param; // +0x34
};

class wnLucasPKStarStormParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    wnLucasPKStarStormParamAccesser(const wnLucasPKStarStormParam* param) : wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0>(81) {
        m_floats[0] = &param->m_speed;
        m_floats[1] = &param->m_accel;
        m_ints[0] = &param->unk8;
    }
    virtual ~wnLucasPKStarStormParamAccesser() { }
};

class wnLucasPKStarStormModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 24, 4, 2, 4, 0, 4, 4, 2, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnTurnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnTurnInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<2, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnLucasPKStarStorm : public wnWeaponBuilder<wnLucasPKStarStormModuleAccesserBuildConfig> {
    const wnLucasPKStarStormParam* m_param;          // +0x1BD8
    wnLucasPKStarStormParamAccesser m_paramAccesser; // +0x1BDC
public:
    wnLucasPKStarStorm(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKStarStormData* data);
    virtual ~wnLucasPKStarStorm() { }
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
