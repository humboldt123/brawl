#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 90; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnPitBowData;
struct wnArticleBuildInfo;

#if defined(WN_PIT_IKARUS_FULL_BUILDER) || defined(WN_PIT_BOW_FULL_BUILDER)
// The bow follows Pit's hand (as Mario's cape): no movement energies, one other-animation slot, a shadow and a
// color blend. Module offsets checked against the destructor ft_pit owns.
class wnPitBowModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<0, soPostureModuleSimple> PostureModuleBuildConfig;
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
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
};

// No common energies, as the cape's builder.
template <>
class wnWeaponBuilder<wnPitBowModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnPitBowModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the no-common-energies builder.
    u32 m_modulesPad;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), weaponFlag), m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnPitBow : public wnWeaponBuilder<wnPitBowModuleAccesserBuildConfig> {
public:
    wnPitBow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowData* data);
    virtual ~wnPitBow() { }
    // HYPOTHESIS: parameter names and types (see the shell below).
    void activate(int founderTaskId, int resourceId, int texResId, bool unk1, bool unk2);
};
#else
// Extent known from the class info createArray allocation (0x1534, as in include/ft/pit/ft_pit_article_pools.h);
// the internal layout and the builder chain stay blob-provided until the constructor is reconstructed.
class wnPitBow : public Weapon {
    u8 m_storage[0x1534 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnPitBow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowData* data);
    virtual ~wnPitBow();
    // HYPOTHESIS: parameter names and types; ftPitTransactor::activeArticle passes the founder task, the model
    // and texture resource ids and its two flags.
    void activate(int founderTaskId, int resourceId, int texResId, bool unk1, bool unk2);
};
#endif
static_assert(sizeof(wnPitBow) == 0x1534, "Pit bow article extent");
