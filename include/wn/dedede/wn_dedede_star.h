#pragma once

// The star of Dedede (article kind 0x31): a small star thrown from his hammer (the Jet Hammer's/Waddle Dee shots).

#include <wn/wn_weapon_builder.h>

// The parameter block of the star. HYPOTHESIS: meaning of the field.
struct wnDededeStarParam {
    s32 unk0; // lifetime
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnDededeStarData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnDededeStarParam* m_param; // +0x34
};

// The shared null modules of the star: no resource, ground, hit, damage, shadow, color blend or glow module.
extern char lbl_27_bss_2EC[]; // resource

class wnDededeStarModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnDededeStar : public wnWeaponBuilder<wnDededeStarModuleAccesserBuildConfig> {
    const wnDededeStarParam* m_param; // +0x1ECC
public:
    wnDededeStar(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarData* data);
    virtual ~wnDededeStar() { }
    void activate(int founderTaskId, int unused, int team, const Vec3f* position, bool flip, float dir, float angle);
};
