#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 34; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnFoxLandMasterShellData;
struct wnArticleBuildInfo;

#if defined(WN_FOX_LAND_MASTER_FIGHTER_TU) && defined(WN_WEAPON_BUILDER) && (defined(FT_FOX_ARTICLE_POOLS) || defined(FT_WOLF_ARTICLE_POOLS) || defined(FT_FALCO_ARTICLE_POOLS))
// The fighter unit expands the shell's teardown inside the Landmaster's weak destructor (the shell sits in the
// Landmaster's article mediator). Layout from that destructor's member offsets: the Blaster's status builder and command
// threads (their teardowns are the Blaster's out-of-line copies), four model event observers, one collision attack
// part, and the common kinetic, effect, reflect and slow modules with the two common energies.
#include <wn/wn_extend_param_accesser.h>
// The kinetic builder starts a word after the (Blaster-shared, 0x1BC) status builder: the accesser builder's empty
// status/kinetic slot (SO_STATUS_KINETIC_SLOT, set by the fighter unit) pads to it.
class wnFoxLandMasterShellModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soKineticModuleBuildConfigMediator<wnKineticModuleImpl, soKineticMediatorNull> KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

// HYPOTHESIS: one parameter, as the Blaster's accesser (the extent is 0x2C).
class wnFoxLandMasterShellParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    virtual ~wnFoxLandMasterShellParamAccesser() { }
};

class wnFoxLandMasterShell : public wnWeaponBuilder<wnFoxLandMasterShellModuleAccesserBuildConfig> {
    const void* m_param; // HYPOTHESIS: the shell's parameter block (type unreconstructed)
    wnFoxLandMasterShellParamAccesser m_paramAccesser;
public:
    wnFoxLandMasterShell(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterShellData* data);
    virtual ~wnFoxLandMasterShell() { }
};
#else
// Extent known from the class info createArray allocation (0x1F28); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnFoxLandMasterShell : public Weapon {
    u8 m_storage[0x1F28 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnFoxLandMasterShell(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterShellData* data);
    virtual ~wnFoxLandMasterShell();
};
#endif
static_assert(sizeof(wnFoxLandMasterShell) == 0x1F28, "Fox Land Master shell article extent");
