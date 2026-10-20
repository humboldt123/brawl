#pragma once

// Builder view of ftWolf for ft_wolf.cpp only. Other units (the reflector status and the posture-history array
// templates) include ft_wolf.h, whose Fighter-based declaration keeps the same layout; the builder headers cannot
// coexist with theirs.
#include <so/so_array.h>
#include <ft/ft_common_data_accesser.h>
#include <so/turn/so_turn_module.h>
#include <StaticAssert.h>
#include <types.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>

////////////////////////////////////////
// Wolf Build Configuration
////////////////////////////////////////

// Template arguments follow the module map's builder instance names; Wolf differs from Fox in animation
// command/motion counts (494), hit groups (12) and reflector groups (14).
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftWolfInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftWolfHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftWolfParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftWolfResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<10, 3, soModelModuleImpl> ftWolfModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<288, 494> ftWolfAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<494, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftWolfMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftWolfCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 14, 3, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftWolfCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftWolfLinkModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 12, 1, soCollisionHitModuleImpl, 0x3ff, true> ftWolfCollisionHitModuleBuildConfig;
typedef soStatusModuleBuildConfig<288, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftWolfStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools). Only the fighter unit builds the real pools; the other
// units of the REL see an opaque block of the same size.
#ifdef FT_WOLF_ARTICLE_POOLS
#include <ft/wolf/ft_wolf_article_pools.h>
typedef ftWolfArticleManageModuleBuilder ftWolfGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x14CDC, Fighter_Wolf> ftWolfGenerateArticleManageModuleBuilder;
#endif

class ftWolfBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftWolfInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftWolfHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftWolfParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftWolfResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftWolfAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftWolfModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftWolfMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftWolfCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftWolfCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftWolfLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftWolfCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftWolfStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftWolfGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};
#include <so/so_array.h>
#include <ft/ft_common_data_accesser.h>
#include <so/turn/so_turn_module.h>
#include <StaticAssert.h>
#include <types.h>

// Only the behavior-accessed tail is recovered.
// The constructor obtains this data from ftCommonDataAccesser::getData(Fighter_Wolf).
// HYPOTHESIS: the record grouping extends ftData; its turn record offset is verified.
struct ftWolfData : ftData {
    u8 unk58[0x94 - sizeof(ftData)];
    soTurnData reflectorTurnData;
};
static_assert(sizeof(ftWolfData) == 0x9c, "Reflector turn data offset is wrong!");

class wnWolfBlaster;
class wnWolfIllusion;
class wnFoxLandMaster;

// The constructor's 0x1CCDC allocation fixes the size; the tail is the status table, the common data pointer, the two
// reflector flags and the trail of the last positions (the Phantasm afterimage).
class ftWolf : public ftFighterBuilder<ftWolfBuildConfig> {
public:
    // One sample of the afterimage trail: the position and a rotation of the fighter. HYPOTHESIS: names.
    class PostureInfo {
    public:
        Vec3f m_pos;
        float m_rotX;
    };
private:
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x1CC74
public:
    ftWolfData* m_commonData; // +0x1CC84
    // Set by the reflector collision callbacks for one frame: the hit was too weak (unk1CC88) or was stopped (unk1CC89).
    u8 unk1CC88;
    u8 unk1CC89;
private:
    soArrayQueueImpl<soArrayVector<PostureInfo, 4> > m_postureTrail; // +0x1CC8C
public:
    ftWolf(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftWolf();
    virtual void processUpdate();
    virtual void onStart(int startKind);
    virtual void processFixPosition();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnWolfBlaster* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnWolfIllusion* weapon, soModuleAccesser* acc);
    static bool activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftFighterBuilder<ftWolfBuildConfig>) == 0x1CC74, "Wolf status table offset is wrong");
static_assert(sizeof(ftWolf) == 0x1ccdc, "Wolf allocation size is wrong!");
static_assert(sizeof(ftWolf::PostureInfo) == 16, "Posture info is the wrong size!");
