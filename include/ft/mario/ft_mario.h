#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Mario Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftMarioInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftMarioHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftMarioParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftMarioResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftMarioModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<281, 478> ftMarioAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<478, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftMarioMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftMarioCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 12, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftMarioCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftMarioLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<281, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftMarioStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_MARIO_ARTICLE_POOLS
#include <ft/mario/ft_mario_article_pools.h>
typedef ftMarioArticleManageModuleBuilder ftMarioGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x25AA8, Fighter_Mario> ftMarioGenerateArticleManageModuleBuilder;
#endif

class ftMarioBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftMarioInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftMarioHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftMarioParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftMarioResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftMarioAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftMarioModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftMarioMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftMarioCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftMarioCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftMarioLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftMarioStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftMarioGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnMarioFireball;
class wnMarioMantle;
class wnMarioPump;
class wnMarioHugeFlame;

class ftMario : public ftFighterBuilder<ftMarioBuildConfig> {
    ftData* m_data;
    u8 unk2DBD4;
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
public:
    ftMario(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onStart(int startKind);
    virtual void onDeadEnd();
    virtual void processUpdate();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnMarioFireball* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnMarioMantle* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnMarioPump* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnMarioHugeFlame* weapon, soModuleAccesser* acc);
};

// Dr. Mario: same class, created through its own class info
class ftMarioD : public ftMario {
public:
    ftMarioD(s32 entryId,
             Heaps::HeapType instHeap,
             Heaps::HeapType nwModelInstHeap,
             Heaps::HeapType nwMotionInstHeap) :
        ftMario(entryId, instHeap, nwModelInstHeap, nwMotionInstHeap) { }
};
