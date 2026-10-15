#pragma once

// The beam of the Waddle Doo of Dedede's Final Smash (article kind 0x34).

#include <wn/wn_weapon_builder.h>

// The parameter block of the beam. HYPOTHESIS: meaning of the fields.
struct wnDededeWaddledooBeamParam {
    float unk0; // speed
    s32 unk4;   // lifetime
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnDededeWaddledooBeamData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnDededeWaddledooBeamParam* m_param; // +0x34
};

class wnDededeWaddledooBeamModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnDededeWaddledooBeam : public wnWeaponBuilder<wnDededeWaddledooBeamModuleAccesserBuildConfig> {
    const wnDededeWaddledooBeamParam* m_param; // +0x1FB4
public:
    wnDededeWaddledooBeam(s32 articleId, const wnArticleBuildInfo& info, const wnDededeWaddledooBeamData* data);
    virtual ~wnDededeWaddledooBeam() { }
    void activate(int founderTaskId, int unused, int team, const Vec3f* position, float lr, float angle);
};
