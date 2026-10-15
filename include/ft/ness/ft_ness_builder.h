#pragma once

// Builder declaration for ft_ness.cpp; status units use the Fighter-based ft_ness.h.

#include <ft/ft_fighter_builder.h>
#include <ft/ft_common_data_accesser.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Ness Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftNessInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftNessHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftNessParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftNessResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftNessModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<289, 490> ftNessAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<490, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftNessMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftNessCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 12, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftNessCollisionReflectorBaseModuleBuildConfig;
// HYPOTHESIS: the absorber builder (part kind 4) follows the reflector builder
typedef soCollisionShieldModuleBuildConfigGroups<4, 1, 1, soCollisionShieldEventPresenterAbsorber, soCollisionShieldModuleImpl> ftNessCollisionAbsorberModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigWithAbsorber<ftNessCollisionReflectorBaseModuleBuildConfig, ftNessCollisionAbsorberModuleBuildConfig> ftNessCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<8, soLinkModuleImpl> ftNessLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<289, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftNessStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor for changeKinetic (updateEnergy stays in ftKineticTransactor)
FT_KINETIC_TRANSACTOR(ftNessKineticTransactor);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftNessKineticTransactor, ftKineticTransactor> > ftNessKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_NESS_ARTICLE_POOLS
#include <ft/ness/ft_ness_article_pools.h>
typedef ftNessArticleManageModuleBuilder ftNessGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x42A0C, Fighter_Ness> ftNessGenerateArticleManageModuleBuilder;
#endif

class wnNessPKFire;
class wnNessPKThunder;
class wnNessPKStarStorm;
class wnNessYoyo;
class wnNessYoyoHead;

class ftNessBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftNessInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftNessHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftNessParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftNessResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftNessAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftNessModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftNessMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftNessCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftNessCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftNessLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftNessStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftNessKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftNessGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class ftNess : public ftFighterBuilder<ftNessBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x4AE10
    ftData* m_data; // +0x4AE20: getExtendParam reads the instance data pointer here.
    u8 unk4AE24; // HYPOTHESIS: flag cleared by processUpdate
    u8 unk4AE25; // HYPOTHESIS: flag cleared by processUpdate
    u8 unkTail2[0x4AE28 - 0x4AE26];
public:
    ftNess(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);

    virtual void processUpdate();
    virtual void processFixPosition();
    void** getExtendParam();
    virtual void onStart(int param);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void toDead(Dead::Reason deadReason);
    virtual void notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY);
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY);
    virtual bool notifyEventCollisionReflectorCheck();
    virtual bool notifyEventCollisionAbsorberCheck();
    // The hooks the article mediator calls to start an article of the pool (they return false when the article is not
    // handed out). HYPOTHESIS: names follow the module map.
    static bool activeArticle(wnNessPKFire* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnNessPKThunder* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnNessPKStarStorm* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnNessYoyo* weapon, soModuleAccesser* acc);
    static bool activeArticle4(wnNessYoyoHead* weapon, soModuleAccesser* acc);
};

static_assert(sizeof(ftFighterBuilder<ftNessBuildConfig>) == 0x4AE10, "Ness status table offset is wrong");
static_assert(sizeof(ftNess) == 0x4AE28, "Ness allocation size is wrong!");
