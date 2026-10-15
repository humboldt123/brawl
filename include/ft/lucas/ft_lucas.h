#pragma once

#include <ft/ft_fighter_builder.h>
#include <ft/ft_common_data_accesser.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Lucas Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftLucasInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftLucasHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftLucasParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftLucasResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftLucasModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<290, 491> ftLucasAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<491, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftLucasMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftLucasCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 14, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftLucasCollisionReflectorBaseModuleBuildConfig;
// HYPOTHESIS: the absorber builder (part kind 4) follows the reflector builder
typedef soCollisionShieldModuleBuildConfigGroups<4, 1, 1, soCollisionShieldEventPresenterAbsorber, soCollisionShieldModuleImpl> ftLucasCollisionAbsorberModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigWithAbsorber<ftLucasCollisionReflectorBaseModuleBuildConfig, ftLucasCollisionAbsorberModuleBuildConfig> ftLucasCollisionReflectorModuleBuildConfig;
typedef soStatusModuleBuildConfig<290, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftLucasStatusModuleBuildConfig;

// HYPOTHESIS: this fighter has its own kinetic transactor for changeKinetic (updateEnergy stays in ftKineticTransactor)
FT_KINETIC_TRANSACTOR(ftLucasKineticTransactor);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftLucasKineticTransactor, ftKineticTransactor> > ftLucasKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_LUCAS_ARTICLE_POOLS
#include <ft/lucas/ft_lucas_article_pools.h>
typedef ftLucasArticleManageModuleBuilder ftLucasGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x40814, Fighter_Lucas> ftLucasGenerateArticleManageModuleBuilder;
#endif

class wnLucasPKFire;
class wnLucasPKThunder;
class wnLucasPKStarStorm;
class wnLucasHimohebi;

class ftLucasBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftLucasInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftLucasHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftLucasParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftLucasResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftLucasAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftLucasModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftLucasMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftLucasCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftLucasCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftLucasStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftLucasKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftLucasGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class ftLucas : public ftFighterBuilder<ftLucasBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x48CB0
    ftData* m_commonData; // +0x48CC0
    u8 unk48CC4[4];
public:
    ftLucas(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    void** getExtendParam();
    virtual void onStart(int param);
    virtual void toDead(Dead::Reason deadReason);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY);
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY);
    virtual bool notifyEventCollisionReflectorCheck();
    virtual bool notifyEventCollisionAbsorberCheck();
    // The hooks the article mediator calls to start an article of the pool (they return false when the article is not
    // handed out). HYPOTHESIS: names follow the module map.
    static bool activeArticle(wnLucasPKFire* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnLucasPKThunder* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnLucasPKStarStorm* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnLucasHimohebi* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftFighterBuilder<ftLucasBuildConfig>) == 0x48CB0, "Lucas status table offset is wrong");
static_assert(sizeof(ftLucas) == 0x48CC8, "Lucas fighter size is wrong");
