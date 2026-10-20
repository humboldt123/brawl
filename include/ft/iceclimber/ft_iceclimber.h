#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Popo Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPopoInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPopoHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPopoParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftPopoResourceIdAccesserImpl, soResourceModuleImpl> ftPopoResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPopoModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<289, 498> ftPopoAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<498, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPopoMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPopoCollisionShieldModuleBuildConfig;
typedef soGeneralWorkBuildConfig<66, 34, 3> ftPopoGeneralWorkBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftPopoLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<289, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPopoStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
FT_KINETIC_TRANSACTOR(ftPopoKineticTransactor);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftPopoKineticTransactor> > ftPopoKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_POPO_ARTICLE_POOLS
#include <ft/iceclimber/ft_iceclimber_article_pools.h>
typedef ftPopoArticleManageModuleBuilder ftPopoGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x17ACC, Fighter_Popo> ftPopoGenerateArticleManageModuleBuilder;
#endif

class ftPopoBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPopoInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPopoHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPopoParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPopoResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPopoAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPopoModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPopoMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPopoCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPopoGeneralWorkBuildConfig GeneralWorkBuildConfig;
    typedef ftPopoLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPopoStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPopoKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftPopoGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnPopoBlizzard;
class wnPopoIceShot;
class wnPopoIceBerg;
class wnPopoRubber;
class wnPopoIceBergWind;

class ftPopo : public ftFighterBuilder<ftPopoBuildConfig> {
    u8 unkTail[0x1FE38 - sizeof(ftFighterBuilder<ftPopoBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
    bool m_isPartner; // HYPOTHESIS: set for the second fighter of the pair (fighter number 1), whose input and owner are the sub ones
    u8 unkTail2[0x1FE50 - 0x1FE4D];
public:
    ftPopo(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void processUpdate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void postCreate();
    virtual bool notifyUseItem(BaseItem* item, u32 nodeIndex, int* unk);
    virtual void setCursor(bool enable);
    virtual void setNameCursor(bool enable);
    virtual void setLoupe(bool enable);
    virtual void setLoupeDamage(bool enable);
    virtual void onActivate();
    virtual ftOwner* getOwner();
    virtual Fighter* getPartner();
    virtual Input* getInput();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual void onDeadEnd();
    virtual void onStart(int startKind);
    virtual void toKnockOut();
    virtual void analyzeSeal(void* seal);
    virtual bool checkTransitionStatus(u32 statusKind);
    virtual void adjustParentGroundCollision(int unk1, float* unk2);
    static bool isModelSwapColor(int color);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnPopoBlizzard* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnPopoIceBerg* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnPopoRubber* weapon, soModuleAccesser* acc);
};
