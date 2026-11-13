#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ft/toonlink/ft_toonlink_data.h>

class wnToonLinkBoomerang;
class wnToonLinkBowArrow;
class wnToonLinkBow;
class wnSimple;
class wnToonLinkHookShot;

// The activator of the arrow lives in the transactor of sora_melee (a singleton).
// HYPOTHESIS: the arguments of activeArticle2 (the article, the fighter, the effect resource id of the fighter and the facing).
class ftToonLinkTransactor {
    u32 m_unk4;
public:
    ftToonLinkTransactor();
    virtual ~ftToonLinkTransactor();
    // The bow activator and the arrow shooter (map: ftToonLinkTransactor__activeArticle1, __shootArticle1).
    // HYPOTHESIS: parameter types, from the callers in ft_toonlink.cpp.
    bool activeArticle1(wnToonLinkBow* weapon, soModuleAccesser* acc, u32 etcResId);
    bool activeArticle2(wnToonLinkBowArrow* weapon, soModuleAccesser* acc, u32 etcResId, float lr);
    bool shootArticle1(wnToonLinkBowArrow* weapon, soModuleAccesser* acc);
};

////////////////////////////////////////
// ToonLink Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftToonLinkInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftToonLinkHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftToonLinkParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftToonLinkResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftToonLinkModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<284, 484> ftToonLinkAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<484, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftToonLinkMotionModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftToonLinkLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<284, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftToonLinkStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftToonLinkCollisionSearchModuleBuilder;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
// Same shape as FT_KINETIC_TRANSACTOR plus the three mode handlers (0x64..0x66); modes 0x67 and 0x68 do nothing here.
class ftToonLinkKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    void changeKineticSub(void* pools, Vec2f* speed, soModuleAccesser* acc);
    void changeKineticSub1(void* pools, Vec2f* speed, soModuleAccesser* acc);
    void changeKineticSub2(void* pools, Vec2f* speed, soModuleAccesser* acc);
    void changeKineticSub3(void*, Vec2f*, soModuleAccesser*) { }
    void changeKineticSub4(void*, Vec2f*, soModuleAccesser*) { }
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
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftToonLinkKineticTransactor> > ftToonLinkKineticModuleBuildConfig;

// soGenerateArticleManageModuleBuilder (bow, arrows, boomerang, hookshot, two simple articles).
// Only the fighter unit builds the real pools; the other units of the REL see an opaque block of the same size.
#ifdef FT_TOONLINK_ARTICLE_POOLS
#include <ft/toonlink/ft_toonlink_article_pools.h>
typedef ftToonLinkArticleManageModuleBuilder ftToonLinkGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x11CE4, Fighter_ToonLink> ftToonLinkGenerateArticleManageModuleBuilder;
#endif


class ftToonLinkBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftToonLinkInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftToonLinkHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftToonLinkParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftToonLinkResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftToonLinkAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftToonLinkModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftToonLinkMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftToonLinkLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftToonLinkStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftToonLinkCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftToonLinkKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftToonLinkGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// HYPOTHESIS: the sword and shield fighter. m_shieldRaised tells whether the shield collision (the Hylian Shield group
// registered in the constructor) is switched on; it follows the work flag 0x12000041.
class ftToonLink : public ftFighterBuilder<ftToonLinkBuildConfig> {
    u8 m_shieldRaised;
    ftData* m_data;
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
public:
    ftToonLink(s32 entryId,
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
    static bool activeArticle(wnToonLinkBoomerang* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnToonLinkBoomerang* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnToonLinkBowArrow* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnToonLinkHookShot* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnSimple* weapon, soModuleAccesser* acc);
    static float getBoomerangAngle(soModuleAccesser* acc);
};
static_assert(sizeof(ftToonLink) == 0x1A39C, "ftToonLink size mismatch");
