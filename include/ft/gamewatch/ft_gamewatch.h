#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <so/anim/so_anim_obj.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// GameWatch Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftGameWatchInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftGameWatchHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftGameWatchParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftGameWatchResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftGameWatchModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<313, 514> ftGameWatchAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<514, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftGameWatchMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftGameWatchCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftGameWatchCollisionReflectorBaseModuleBuildConfig;
// HYPOTHESIS: the absorber builder (part kind 4) follows the reflector builder
typedef soCollisionShieldModuleBuildConfigGroups<4, 1, 1, soCollisionShieldEventPresenterAbsorber, soCollisionShieldModuleImpl> ftGameWatchCollisionAbsorberModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigWithAbsorber<ftGameWatchCollisionReflectorBaseModuleBuildConfig, ftGameWatchCollisionAbsorberModuleBuildConfig> ftGameWatchCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftGameWatchLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<313, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftGameWatchStatusModuleBuildConfig;

// Mr. Game & Watch's own kinetic modes 0x64..0x72 (ft_gamewatch_kinetic_transactor.cpp).
class ftGameWatchKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
#define FT_GAMEWATCH_KINETIC_SUB(Name) static void Name(bool*, void*, Vec2f* speed, soModuleAccesser* acc);
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub1)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub2)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub3)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub4)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub5)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub6)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub7)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub8)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub9)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub10)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub11)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub12)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub13)
    FT_GAMEWATCH_KINETIC_SUB(changeKineticSub14)
#undef FT_GAMEWATCH_KINETIC_SUB
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
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftGameWatchKineticTransactor, ftKineticTransactor> > ftGameWatchKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_GAMEWATCH_ARTICLE_POOLS
#include <ft/gamewatch/ft_gamewatch_article_pools.h>
typedef ftGameWatchArticleManageModuleBuilder ftGameWatchGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x1771C, Fighter_GameWatch> ftGameWatchGenerateArticleManageModuleBuilder;
#endif

class ftGameWatchBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftGameWatchInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftGameWatchHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftGameWatchParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftGameWatchResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftGameWatchAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftGameWatchModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftGameWatchMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftGameWatchCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftGameWatchCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftGameWatchLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftGameWatchStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftGameWatchKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftGameWatchGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnGameWatchSausage;
class wnGameWatchRescue;
class wnGameWatchParachute;
class wnGameWatchPanel;
class wnGameWatchOil;
class wnGameWatchOctopus;
class wnGameWatchNormalWeapon;

class ftGameWatch : public ftFighterBuilder<ftGameWatchBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable; // 0x1FDA8, 0x10 bytes
    ftData* m_data;
    soAnimObj m_animObj;
public:
    ftGameWatch(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
    virtual void processHit();
    virtual void onDeactivate();
    void* getExtendParam();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnGameWatchSausage* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnGameWatchRescue* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnGameWatchParachute* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnGameWatchPanel* weapon, soModuleAccesser* acc);
    static bool activeArticle4(wnGameWatchOil* weapon, soModuleAccesser* acc);
    static bool activeArticle5(wnGameWatchOctopus* weapon, soModuleAccesser* acc);
    static bool activeArticle6(wnGameWatchNormalWeapon* weapon, soModuleAccesser* acc);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser);
    virtual void analyzeSeal(void* seal);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftGameWatch) == 0x1FDE4, "Class is the wrong size!");
