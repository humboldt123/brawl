#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Samus Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 10, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftSamusInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftSamusHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftSamusParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftSamusResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftSamusModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<301, 492> ftSamusAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<492, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftSamusMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftSamusCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftSamusLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<301, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftSamusStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_SAMUS_ARTICLE_POOLS
#include <ft/samus/ft_samus_article_pools.h>
typedef ftSamusArticleManageModuleBuilder ftSamusGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x1D7D4, Fighter_Samus> ftSamusGenerateArticleManageModuleBuilder;
#endif

class ftSamusBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftSamusInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftSamusHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftSamusParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftSamusResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftSamusAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftSamusModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftSamusMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftSamusCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftSamusLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftSamusStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftSamusGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnSamusCShot;
class wnSamusBomb;
class wnSamusMissile;
class wnSamusLaser;
class wnSamusLaser2;
class wnSamusGun;
class wnSamusSuperMissile;
class wnSamusGBeam;

class ftSamus : public ftFighterBuilder<ftSamusBuildConfig> {
    u8 unkTail[0x25BF8 - sizeof(ftFighterBuilder<ftSamusBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftSamus(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftSamus();
    virtual void processUpdate();
    virtual void onDeadEnd();
    virtual void onStart(int startKind);
    virtual u32 getChangeSucceedOption() { return 0x7fab; }
    virtual void setupChangeSucceedWork(void* work);
    virtual void change(void* work, int mode);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnSamusCShot* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnSamusBomb* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnSamusMissile* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnSamusSuperMissile* weapon, soModuleAccesser* acc);
    static bool activeArticle4(wnSamusLaser* weapon, soModuleAccesser* acc);
    static bool activeArticle5(wnSamusLaser2* weapon, soModuleAccesser* acc);
    static bool activeArticle6(wnSamusGun* weapon, soModuleAccesser* acc);
    static bool activeArticle7(wnSamusGBeam* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnSamusCShot* weapon, soModuleAccesser* acc);
    static bool shootArticle1(wnSamusBomb* weapon, soModuleAccesser* acc);
    static bool shootArticle2(wnSamusMissile* weapon, soModuleAccesser* acc);
    static bool shootArticle3(wnSamusSuperMissile* weapon, soModuleAccesser* acc);
    static bool shootArticle4(wnSamusLaser* weapon, soModuleAccesser* acc);
    static bool shootArticle5(wnSamusLaser2* weapon, soModuleAccesser* acc);
};
