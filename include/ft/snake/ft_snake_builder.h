#pragma once

// Builder view of ftSnake for ft_snake.cpp only. The status units include ft_snake.h, whose Fighter-based
// declaration keeps the same layout; their status-process headers cannot coexist with the builder headers.
#include <ft/ft_fighter_builder.h>
#include <so/so_photo_call_back.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Snake Build Configuration
////////////////////////////////////////

// Template arguments follow the module map's builder instance names.
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftSnakeInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftSnakeHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftSnakeParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftSnakeResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftSnakeModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<309, 510> ftSnakeAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<510, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftSnakeMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftSnakeCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftSnakeCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftSnakeLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<309, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftSnakeStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftSnakeCollisionSearchModuleBuilder;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (the article pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_SNAKE_ARTICLE_POOLS
#include <ft/snake/ft_snake_article_pools.h>
typedef ftSnakeArticleManageModuleBuilder ftSnakeGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x249E4, Fighter_Snake> ftSnakeGenerateArticleManageModuleBuilder;
#endif

class ftSnakeBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftSnakeInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftSnakeHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftSnakeParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftSnakeResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftSnakeAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftSnakeModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftSnakeMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftSnakeCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftSnakeCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftSnakeLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftSnakeStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftSnakeCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftSnakeGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnSnakeMine;
class wnSnakeTrenchMortar;
class wnSnakeRpg7;
class wnSnakeCypher;
class wnSnakeC4;
class wnSnakeC4Switch;
class wnSnakeNikita;
class wnSnakeLadder;
class wnSnakeRgb6;
class wnSnakeRgb6Sight;
class wnSnakeRgb6Clip;
class wnSnakeRgb6Bullet;
class soArticle;

class ftSnake : public ftFighterBuilder<ftSnakeBuildConfig>, public soPhotoCallBack {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftSnake(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onActivate();
    virtual void onStart(int startKind);
    virtual void analyzeSeal(void* seal);
    virtual void onEndFinal();
    virtual void playEatSE();
    virtual void notifyEventOnDamage(soDamage* damage, bool flag, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void updateNodeSRT();
    virtual void processFixCamera();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyArticleEventEject(int unk1, int unk2, int* unk3);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual void photoMoved();
    virtual void photoExit();
    bool ckeckChangeEarCloseStatus(Vec3f& position);
    static void syncFinalCursorRate(Vec2f* cursor, soModuleAccesser* accesser);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnSnakeMine* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnSnakeTrenchMortar* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnSnakeRpg7* weapon, soModuleAccesser* acc);
    static bool activeArticle3(wnSnakeCypher* weapon, soModuleAccesser* acc);
    static bool activeArticle4(wnSnakeC4* weapon, soModuleAccesser* acc);
    static bool activeArticle5(wnSnakeC4Switch* weapon, soModuleAccesser* acc);
    static bool activeArticle6(wnSnakeNikita* weapon, soModuleAccesser* acc);
    static bool activeArticle7(wnSnakeLadder* weapon, soModuleAccesser* acc);
    static bool activeArticle8(wnSnakeRgb6* weapon, soModuleAccesser* acc);
    static bool activeArticle9(wnSnakeRgb6Sight* weapon, soModuleAccesser* acc);
    static bool activeArticle10(wnSnakeRgb6Clip* weapon, soModuleAccesser* acc);
    static bool activeArticle11(wnSnakeRgb6Bullet* weapon, soModuleAccesser* acc);
    static bool activeArticle12(wnSimple* weapon, soModuleAccesser* acc);
    static bool shootArticle(wnSnakeCypher* weapon, soModuleAccesser* acc);
    static bool shootItem(soArticle* article, bool* flag, soModuleAccesser* acc);
    static bool shootItem1(soArticle* article, bool* flag, soModuleAccesser* acc);
};
static_assert(sizeof(ftSnake) == 0x2D040, "Snake allocation size is wrong!");
