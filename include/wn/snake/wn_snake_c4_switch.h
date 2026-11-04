#pragma once

// Snake's C4 detonator (article kind 0x19): the remote switch Snake holds up to set off his C4 charges. It has no
// motion, collision, kinetic or animation-command modules of its own, only one status.

#include <wn/wn_weapon_builder.h>


// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSnakeC4SwitchData {
    s32 m_resGroupNo;
};

class wnSnakeC4SwitchModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfigNull MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfigNull AnimCmdModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// The builder of this article carries no appended common energies (WN_WEAPON_NO_COMMON_ENERGIES in the unit), and its
// status builder ends without the tail word (WN_STATUS_NO_TAIL).
class wnSnakeC4Switch : public wnWeaponBuilder<wnSnakeC4SwitchModuleAccesserBuildConfig> {
public:
    wnSnakeC4Switch(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4SwitchData* data);
    virtual ~wnSnakeC4Switch() { }
    void activate(int founderTaskId, int etcResourceId, int team);
};

static_assert(sizeof(wnSnakeC4Switch) == 0x1364, "C4 switch article layout");
