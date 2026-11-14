#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Captain Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftCaptainInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftCaptainHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftCaptainParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftCaptainResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftCaptainModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<290, 491> ftCaptainAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<491, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftCaptainMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftCaptainCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<12, soLinkModuleImpl> ftCaptainLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<290, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftCaptainStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnCaptainBlueFalcon / wnCaptainFinalBg pools). Only the fighter unit builds
// the real pools; the other units of the REL see an opaque block of the same size.
#ifdef FT_CAPTAIN_ARTICLE_POOLS
#include <ft/captain/ft_captain_article_pools.h>
typedef ftCaptainArticleManageModuleBuilder ftCaptainGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x4118, Fighter_Captain> ftCaptainGenerateArticleManageModuleBuilder;
#endif

// Captain Falcon's own kinetic modes 0x64..0x6D (ft_captain_kinetic_transactor.cpp), called by the fighter's mediator.
class ftCaptainKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* acc);
    static void postUpdateEnergy(void* pools, soModuleAccesser* acc);
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

typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftCaptainKineticTransactor> > ftCaptainKineticModuleBuildConfig;

class ftCaptainBuildConfig : public ftCommonBuildConfig {
public:
    enum { UniqueStatusCount = 16 };
    typedef ftCaptainInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftCaptainHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftCaptainKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftCaptainParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftCaptainResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftCaptainAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftCaptainModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftCaptainMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftCaptainCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftCaptainLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftCaptainStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftCaptainGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnCaptainBlueFalcon;
class wnCaptainFinalBg;

class ftCaptain : public ftFighterBuilder<ftCaptainBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftCaptain(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftCaptain();
    static float* getFalconKickParam();
    virtual void processUpdate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnCaptainBlueFalcon* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnCaptainFinalBg* weapon, soModuleAccesser* acc);
    virtual void analyzeSeal(void* seal);
};
static_assert(sizeof(ftCaptain) == 0xC5C8, "Class is the wrong size!");
