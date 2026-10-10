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

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnWolf article pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x14CDC, Fighter_Wolf> ftWolfGenerateArticleManageModuleBuilder;

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

class ftWolf : public ftFighterBuilder<ftWolfBuildConfig> {
    u8 unkTail[0x1cc84 - sizeof(ftFighterBuilder<ftWolfBuildConfig>)];
public:
    // HYPOTHESIS: field grouping follows copies; field meanings remain unknown.
    class PostureInfo {
    public:
        struct { u32 unk0, unk4; } unk0;
        u32 unk8;
        float unkc;
    };
    ftWolfData* m_commonData;
private:
    // Set by reflector collision callbacks and consumed by the reflector check.
    u8 unk1cc88;
    u8 unk1cc89;
    u8 unk1cc8a[2];
    // Points to the embedded soArrayVector<PostureInfo, 4> at +0x1cc90.
    soArray<PostureInfo>* m_postureHistory;
    u8 unk1cc90[0x1ccdc - 0x1cc90];
public:
    ftWolf(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftWolf();
    virtual void onStart(int startKind);
    virtual void processUpdate();
};
static_assert(sizeof(ftWolf) == 0x1ccdc, "Wolf allocation size is wrong!");
static_assert(sizeof(ftWolf::PostureInfo) == 16, "Posture info is the wrong size!");
