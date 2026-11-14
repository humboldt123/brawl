#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Zero Suit Samus Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftSZerosuitInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftSZerosuitHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftSZerosuitParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftSZerosuitResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftSZerosuitModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<289, 491> ftSZerosuitAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<491, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftSZerosuitMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftSZerosuitCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftSZerosuitLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<289, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftSZerosuitStatusModuleBuildConfig;
typedef soAreaModuleBuildConfigInstances<10, ftAreaModuleImpl> ftSZerosuitAreaModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_SZEROSUIT_ARTICLE_POOLS
#include <ft/szerosuit/ft_szerosuit_article_pools.h>
typedef ftSZerosuitArticleManageModuleBuilder ftSZerosuitGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x118EC, Fighter_SZeroSuit> ftSZerosuitGenerateArticleManageModuleBuilder;
#endif

class ftSZerosuitBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftSZerosuitInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftSZerosuitHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftSZerosuitParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftSZerosuitResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftSZerosuitAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftSZerosuitModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftSZerosuitMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftSZerosuitCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftSZerosuitLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftSZerosuitStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftSZerosuitAreaModuleBuildConfig AreaModuleBuildConfig;
    typedef ftSZerosuitGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// The owned status table and the data pointer follow the builder.
static_assert(sizeof(ftFighterBuilder<ftSZerosuitBuildConfig>) == 0x19C9C, "Zero Suit Samus builder extent");
class wnSZerosuitParalyzer;
class wnSZerosuitParalyzerBullet;
class wnSZerosuitWhip;
class wnSZerosuitArmor;

// The activators of the paralyzer and its bullet live in the transactor of sora_melee (a singleton).
class ftSZerosuitTransactor {
    u32 m_unk4;
public:
    ftSZerosuitTransactor();
    virtual ~ftSZerosuitTransactor();
    bool activeArticle(wnSZerosuitWhip* weapon, soModuleAccesser* acc);
#ifdef FT_SZEROSUIT_TRANSACTOR_3ARG
    bool activeArticle1(wnSZerosuitParalyzer* weapon, soModuleAccesser* acc, u32 etcResId);
#else
    bool activeArticle1(wnSZerosuitParalyzer* weapon, soModuleAccesser* acc);
#endif
    bool activeArticle2(wnSZerosuitParalyzerBullet* weapon, soModuleAccesser* acc);
};

class ftSZerosuit : public ftFighterBuilder<ftSZerosuitBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable; // +0x19C9C
    ftData* m_data; // +0x19CAC
public:
    ftSZerosuit(s32 entryId,
                Heaps::HeapType instHeap,
                Heaps::HeapType nwModelInstHeap,
                Heaps::HeapType nwMotionInstHeap);
    virtual ~ftSZerosuit() { }
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onStart(int startKind);
    virtual u32 getChangeSucceedOption() { return 0x7fbb; }
    virtual void setupChangeSucceedWork(void* work);
    virtual void change(void* work, int mode);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    // The hooks the article mediator calls to start/shoot an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnSZerosuitWhip* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnSZerosuitArmor* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnSZerosuitArmor* weapon, soModuleAccesser* acc);
};
