#pragma once

// Popo's ice berg wind article (article kind 0x99).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>


// The parameter block of the article (1 float, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnPopoIceBergWindParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPopoIceBergWindData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPopoIceBergWindParam* m_param; // +0x34
};

class wnPopoIceBergWindParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnPopoIceBergWindParamAccesser(const wnPopoIceBergWindParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x99) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnPopoIceBergWindParamAccesser() { }
};

class wnPopoIceBergWindModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 3, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnPopoIceBergWind : public wnWeaponBuilder<wnPopoIceBergWindModuleAccesserBuildConfig> {
public:
    const wnPopoIceBergWindParam* m_param; // +0x1FEC
private:
    wnPopoIceBergWindParamAccesser m_paramAccesser; // +0x1FF0
public:
    wnPopoIceBergWind(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergWindData* data);
    virtual ~wnPopoIceBergWind() { }
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
