#pragma once

// Luigi's Final Smash article, the Negative Zone (article kind 0x74).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the article.
// HYPOTHESIS: meaning of the parameters. The float is at +0, two groups of indefinite data (+4 and +0x18) follow,
// and the two ints are at +0x50 and +0x54.
struct wnLuigiNegativeZoneParam {
    float unk0;
    u8 unk4[0x14];
    u8 unk18[0x38];
    s32 unk50;
    s32 unk54;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnLuigiNegativeZoneData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLuigiNegativeZoneParam* m_param; // +0x34
};

class wnLuigiNegativeZoneParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 2, 43999, 2> {
public:
    wnLuigiNegativeZoneParamAccesser(const wnLuigiNegativeZoneParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 2, 43999, 2>(0x74) {
        m_floats[0] = &param->unk0;
        m_ints[0] = &param->unk50;
        m_ints[1] = &param->unk54;
        m_indefinites[0] = (void*)param->unk4;
        m_indefinites[1] = (void*)param->unk18;
    }
    virtual ~wnLuigiNegativeZoneParamAccesser() { }
};

class wnLuigiNegativeZoneModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soAreaModuleBuildConfigWeapon<1, 7> AreaModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnLuigiNegativeZone : public wnWeaponBuilder<wnLuigiNegativeZoneModuleAccesserBuildConfig> {
public:
    const wnLuigiNegativeZoneParam* m_param; // +0x2050
private:
    wnLuigiNegativeZoneParamAccesser m_paramAccesser; // +0x2054
public:
    wnLuigiNegativeZone(s32 articleId, const wnArticleBuildInfo& info, const wnLuigiNegativeZoneData* data);
    virtual ~wnLuigiNegativeZone() { }
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    void activate(int founderTaskId, int resourceId, int team, float lr);
    void toFly(Vec3f* position);
};
