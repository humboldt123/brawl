#pragma once

// Lucario's Aura Sphere article (Qigong) article (article kind 0x64).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>


// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnLucarioQigongParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnLucarioQigongData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLucarioQigongParam* m_param; // +0x34
};

class wnLucarioQigongParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnLucarioQigongParamAccesser(const wnLucarioQigongParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x64) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnLucarioQigongParamAccesser() { }
};

class wnLucarioQigongModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfigNull MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 3, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<5, 1, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnLucarioQigong : public wnWeaponBuilder<wnLucarioQigongModuleAccesserBuildConfig> {
public:
    const wnLucarioQigongParam* m_param; // +0x1E0C
private:
    wnLucarioQigongParamAccesser m_paramAccesser; // +0x1E10
public:
    wnLucarioQigong(s32 articleId, const wnArticleBuildInfo& info, const wnLucarioQigongData* data);
    virtual ~wnLucarioQigong() { }
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
    void activate(int founderTaskId, int team, const Vec3f* position, float lr, float powerMul);
};
