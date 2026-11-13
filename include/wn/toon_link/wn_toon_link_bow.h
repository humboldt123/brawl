#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 139; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnToonLinkBowData;
struct wnArticleBuildInfo;

#ifdef WN_TOON_LINK_BOW_FULL_BUILDER // only the fighter TU owns the bow's builder (its weak teardown is emitted there)
// The bow follows Toon Link's hand like Pit's bow: no movement energies, no shadow. Module set read from the teardown
// ft_toonlink owns (it shares the hookshot's reflect, effect, sound, stop and event-manage builder teardowns).
// The posture keeps one interpolation slot (the teardown destroys one soInterpolation<Vec3f> vector).
class wnToonLinkBowModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    // Six motion data entries: the constructor sizes the motion table with 6 (the hookshot has 1, so the two articles do not
    // share one motion builder and each destructor expands its own teardown).
    typedef soMotionModuleBuildConfig<6, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<1, soPostureModuleSimple> PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfigNull AnimCmdModuleBuildConfig;
    typedef soStatusModuleBuildConfigNull StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
};

// No common energies, as Pit's bow.
template <>
class wnWeaponBuilder<wnToonLinkBowModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnToonLinkBowModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in Pit's bow builder.
    u32 m_modulesPad;
public:
    virtual ~wnWeaponBuilder() { }
};

class wnToonLinkBow : public wnWeaponBuilder<wnToonLinkBowModuleAccesserBuildConfig> {
public:
    wnToonLinkBow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowData* data);
    virtual ~wnToonLinkBow() { }
    // HYPOTHESIS: parameter names; ftToonLinkTransactor::activeArticle passes the founder task and the resource id.
    void activate(int founderTaskId, int resourceId);
};
#else
// Extent known from the class info createArray allocation (0x14E4); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnToonLinkBow : public Weapon {
    u8 m_storage[0x14E4 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnToonLinkBow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowData* data);
    virtual ~wnToonLinkBow();
    // HYPOTHESIS: parameter names; ftToonLinkTransactor::activeArticle passes the founder task and the resource id.
    void activate(int founderTaskId, int resourceId);
};
#endif
static_assert(sizeof(wnToonLinkBow) == 0x14E4, "ToonLinkBow article extent");
