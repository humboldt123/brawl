#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Ganon Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftGanonInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftGanonHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftGanonParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftGanonResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftGanonModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<290, 488> ftGanonAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<488, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftGanonMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftGanonCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftGanonLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<290, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftGanonStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
// Ganon's own kinetic modes (ft_ganon_kinetic_transactor.cpp): FT_KINETIC_TRANSACTOR's members plus the two mode setups.
class ftGanonKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        return energy->updateEnergy(acc);
    }
};
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftGanonKineticTransactor> > ftGanonKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnGanonBeast / wnSimple pools). Only the fighter unit builds the real
// pools; the other units of the REL see an opaque block of the same size.
#ifdef FT_GANON_ARTICLE_POOLS
#include <ft/ganon/ft_ganon_article_pools.h>
typedef ftGanonArticleManageModuleBuilder ftGanonGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x4630, Fighter_Ganon> ftGanonGenerateArticleManageModuleBuilder;
#endif

class ftGanonBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftGanonInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftGanonHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftGanonParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftGanonResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftGanonAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftGanonModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftGanonMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftGanonCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftGanonLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftGanonStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftGanonKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftGanonGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnSimple;
class wnGanonBeast;

class ftGanon : public ftFighterBuilder<ftGanonBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    // The hooks the article mediator calls to start an article of the pool.
    static bool activeArticle(wnGanonBeast* weapon, soModuleAccesser* acc);
    static bool activeArticle(wnSimple* weapon, soModuleAccesser* acc);
    ftGanon(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftGanon();
    static float* getGanonKickParam();
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
};
static_assert(sizeof(ftGanon) == 0xC9A8, "Class is the wrong size!");
