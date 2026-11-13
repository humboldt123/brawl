#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Sheik Build Configuration (Zelda's, with the counts of the module map's template arguments)
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftSheikInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftSheikHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftSheikParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftSheikResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftSheikModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<287, 486> ftSheikAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<486, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftSheikMotionModuleBuildConfig;
typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 5, 10, soCollisionAttackModuleImpl, 5, true, true> ftSheikCollisionAttackModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftSheikCollisionShieldModuleBuildConfig;
// The module map names it soCollisionReflectorModuleBuildConfig<0, 20, 2, soCollisionShieldModuleImpl>.
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftSheikCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftSheikLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<287, soGeneralWorkBuildConfig<34, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftSheikStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftSheikCollisionSearchModuleBuilder;

// Sheik's own kinetic modes (ft_sheik_kinetic_transactor.cpp); energy updates use the common transactor.
#include <ft/zelda/ft_sheik_kinetic_transactor.h>
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftSheikKineticTransactor, ftKineticTransactor> > ftSheikKineticModuleBuildConfig;

// The article pools. Only the fighter unit builds them.
#ifdef FT_SHEIK_ARTICLE_POOLS
#include <ft/zelda/ft_sheik_article_pools.h>
typedef ftSheikArticleManageModuleBuilder ftSheikGenerateArticleManageModuleBuilder;
#endif

class ftSheikBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftSheikInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftSheikHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftSheikParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftSheikResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftSheikAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftSheikModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftSheikMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftSheikCollisionAttackModuleBuildConfig CollisionAttackModuleBuildConfig;
    typedef ftSheikCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftSheikCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftSheikLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftSheikStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftSheikCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftSheikKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftSheikGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnSheikIronWire;
class wnSheikFusin;
class wnZeldaLightingBow;
class wnZeldaLightingBowArrow;

class ftSheik : public ftFighterBuilder<ftSheikBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
public:
    ftData* m_data;

    ftSheik(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    void** getExtendParam();
    virtual void onStart(int startKind);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void changeSucceedCore(void* work, int status);
    virtual void change(void* work, int startKind);
    virtual void setupChangeSucceedWork(void* work);
    virtual u32 getChangeSucceedOption() { return 0x7ffe; }
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnSheikIronWire* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnSheikFusin* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnZeldaLightingBow* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnZeldaLightingBowArrow* weapon, soModuleAccesser* acc);
};
