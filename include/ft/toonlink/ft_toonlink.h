#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// ToonLink Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftToonLinkInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftToonLinkHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftToonLinkParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftToonLinkResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftToonLinkModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<284, 484> ftToonLinkAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<484, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftToonLinkMotionModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftToonLinkLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<284, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftToonLinkStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftToonLinkCollisionSearchModuleBuilder;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
FT_KINETIC_TRANSACTOR(ftToonLinkKineticTransactor);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftToonLinkKineticTransactor> > ftToonLinkKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x11CE4, Fighter_ToonLink> ftToonLinkGenerateArticleManageModuleBuilder;

class ftToonLinkBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftToonLinkInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftToonLinkHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftToonLinkParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftToonLinkResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftToonLinkAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftToonLinkModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftToonLinkMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftToonLinkLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftToonLinkStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftToonLinkCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftToonLinkKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftToonLinkGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnToonLinkBoomerang;
class wnToonLinkBowArrow;
class wnToonLinkHookShot;
class wnSimple;

// The activator of the arrow lives in the transactor of sora_melee (a singleton), as for Link.
// HYPOTHESIS: the arguments of activeArticle2 (the article, the fighter, the effect resource id of the fighter and the facing).
class ftToonLinkTransactor {
    u32 m_unk4;
public:
    ftToonLinkTransactor();
    virtual ~ftToonLinkTransactor();
    bool activeArticle2(wnToonLinkBowArrow* weapon, soModuleAccesser* acc, u32 etcResId, float lr);
};

// HYPOTHESIS: the sword and shield fighter, like Link. m_shieldRaised tells whether the shield collision (the shield group
// registered in the constructor) is switched on; it follows the work flag 0x12000041.
class ftToonLink : public ftFighterBuilder<ftToonLinkBuildConfig> {
    u8 m_shieldRaised;
    ftData* m_data;
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
public:
    ftToonLink(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onActivate();
    virtual void onStart(int startKind);
    virtual void processUpdate();
    virtual void processFixPosition();
    virtual void notifyEventChangeCollisionHit(int index, soModuleAccesser* moduleAccesser);
    virtual void onHitReflector(soModuleAccesser* moduleAccesser, bool hit);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionShieldCheck();
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();

    // The article starters the status processes (and the article builder) call.
    static bool activeArticle(wnToonLinkBoomerang* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnToonLinkBoomerang* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnToonLinkBowArrow* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnToonLinkHookShot* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnSimple* weapon, soModuleAccesser* acc);
    static float getBoomerangAngle(soModuleAccesser* acc);
};
static_assert(sizeof(ftToonLink) == 0x1A39C, "ftToonLink size mismatch");
