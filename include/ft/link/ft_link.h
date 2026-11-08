#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ft/link/ft_link_data.h>

class wnLinkBoomerang;
class wnLinkBowArrow;
class wnSimple;
class wnLinkHookshot;

// The activator of the arrow lives in the transactor of sora_melee (a singleton).
// HYPOTHESIS: the arguments of activeArticle2 (the article, the fighter, the effect resource id of the fighter and the facing).
class ftLinkTransactor {
    u32 m_unk4;
public:
    ftLinkTransactor();
    virtual ~ftLinkTransactor();
    bool activeArticle2(wnLinkBowArrow* weapon, soModuleAccesser* acc, u32 etcResId, float lr);
};

////////////////////////////////////////
// Link Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftLinkInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftLinkHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftLinkParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftLinkResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftLinkModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<284, 484> ftLinkAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<484, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftLinkMotionModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftLinkLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<284, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftLinkStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftLinkCollisionSearchModuleBuilder;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
// Same shape as FT_KINETIC_TRANSACTOR plus the three mode handlers (0x64..0x66); modes 0x67..0x6d do nothing here.
class ftLinkKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    void changeKineticSub(void* pools, Vec2f* speed, soModuleAccesser* acc);
    void changeKineticSub1(void* pools, Vec2f* speed, soModuleAccesser* acc);
    void changeKineticSub2(void* pools, Vec2f* speed, soModuleAccesser* acc);
    void changeKineticSub3(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub4(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub5(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub6(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub7(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub8(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub9(void*, Vec2f*, soModuleAccesser*) { }
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
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftLinkKineticTransactor> > ftLinkKineticModuleBuildConfig;

// soGenerateArticleManageModuleBuilder (bow, arrows, boomerang, hookshot, two simple articles).
// Only the fighter unit builds the real pools; the other units of the REL see an opaque block of the same size.
#ifdef FT_LINK_ARTICLE_POOLS
#include <ft/link/ft_link_article_pools.h>
typedef ftLinkArticleManageModuleBuilder ftLinkGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x11CA8, Fighter_Link> ftLinkGenerateArticleManageModuleBuilder;
#endif


class ftLinkBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftLinkInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftLinkHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftLinkParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftLinkResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftLinkAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftLinkModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftLinkMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftLinkLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftLinkStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftLinkCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftLinkKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftLinkGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// HYPOTHESIS: the sword and shield fighter. m_shieldRaised tells whether the shield collision (the Hylian Shield group
// registered in the constructor) is switched on; it follows the work flag 0x12000041.
class ftLink : public ftFighterBuilder<ftLinkBuildConfig> {
    u8 m_shieldRaised;
    ftData* m_data;
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
public:
    ftLink(s32 entryId,
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
    static bool activeArticle(wnLinkBoomerang* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnLinkBoomerang* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnLinkBowArrow* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnLinkHookshot* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnSimple* weapon, soModuleAccesser* acc);
    static float getBoomerangAngle(soModuleAccesser* acc);
};
static_assert(sizeof(ftLink) == 0x1A360, "ftLink size mismatch");
