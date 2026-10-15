#pragma once

// Captain Falcon's Final Smash background article (article kind 0x8F).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>


// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnCaptainFinalBgParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnCaptainFinalBgData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnCaptainFinalBgParam* m_param; // +0x34
};

class wnCaptainFinalBgParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnCaptainFinalBgParamAccesser(const wnCaptainFinalBgParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x8F) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnCaptainFinalBgParamAccesser() { }
};

class wnCaptainFinalBgModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnCaptainFinalBg : public wnWeaponBuilder<wnCaptainFinalBgModuleAccesserBuildConfig> {
public:
    const wnCaptainFinalBgParam* m_param; // +0x1EF8
private:
    wnCaptainFinalBgParamAccesser m_paramAccesser; // +0x1EFC
public:
    wnCaptainFinalBg(s32 articleId, const wnArticleBuildInfo& info, const wnCaptainFinalBgData* data);
    virtual ~wnCaptainFinalBg() { }
    void activate(int founderTaskId, int resourceId, int team);
};
