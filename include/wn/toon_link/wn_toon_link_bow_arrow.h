#pragma once

#include <wn/wn_weapon_builder.h>
#ifdef WN_TOON_LINK_BOW_ARROW_FULL_BUILDER
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 3);
#endif

// HYPOTHESIS: the article data type the class info fetches for kind 140; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnToonLinkBowArrowData;
struct wnArticleBuildInfo;

#ifdef WN_TOON_LINK_BOW_ARROW_FULL_BUILDER
// HYPOTHESIS: drafted from the Pit arrow and Fox illusion configurations; only the module builders named by the
// original's builder symbols (motion, situation, stop, visibility, status, general work, anim cmd) are fitted.
class wnToonLinkBowArrowModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<5, 3, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};

struct wnToonLinkBowArrowData {
    s32 m_resGroupNo; // HYPOTHESIS: as the other articles' data blocks; the rest is uninterpreted.
};

class wnToonLinkBowArrow : public wnWeaponBuilder<wnToonLinkBowArrowModuleAccesserBuildConfig> {
public:
    soKineticEnergyGroundMovement m_energyGround; // +0x1FA8: kinetic energy 2, the outside ground movement
    wnToonLinkBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowArrowData* data);
#ifdef FT_TOONLINK_BOW_ARROW_DTOR_EXTERNAL
    // The fighter holder calls this. This object does not emit the destructor.
    virtual ~wnToonLinkBowArrow();
#else
    virtual ~wnToonLinkBowArrow() { }
#endif
    void activate(int founderTaskId, int resourceId, int team, float lr);
    virtual void deactivate(bool force);
    const wnToonLinkBowArrowData* getData();
    void toFly(float lr, float unk2, float unk1);
    void setFlyPose();
    virtual void updateNodeSRT();
    virtual bool reflect();
};
static_assert(sizeof(wnToonLinkBowArrow) == 0x1FD8, "Toon Link bow arrow article extent");
#else
// Extent known from the class info createArray allocation (0x1FD8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnToonLinkBowArrow : public Weapon {
    u8 m_storage[0x1FD8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnToonLinkBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowArrowData* data);
    virtual ~wnToonLinkBowArrow();
    // HYPOTHESIS: parameter names; the same activation as Toon Link's arrow (founder, resource, team, facing).
    void activate(int founderTaskId, int resourceId, int team, float lr);
    virtual void deactivate(bool force);
    const wnToonLinkBowArrowData* getData();
    // HYPOTHESIS: parameter names; the two floats go to work floats 0x11000002 and 0x11000001.
    void toFly(float lr, float unk2, float unk1);
    void setFlyPose();
    virtual void updateNodeSRT();
    virtual bool reflect();
};
static_assert(sizeof(wnToonLinkBowArrow) == 0x1FD8, "Toon Link bow arrow article extent");
#endif
