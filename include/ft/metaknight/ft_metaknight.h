#pragma once

#include <ft/ft_fighter_builder.h>
#include <ft/ft_common_data_accesser.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Metaknight Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftMetaknightInsideEventManageModuleBuildConfig;
typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 5, 6, soCollisionAttackModuleImpl, 1, true, true> ftMetaknightCollisionAttackModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftMetaknightHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftMetaknightParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftMetaknightResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftMetaknightModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<290, 492> ftMetaknightAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<492, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftMetaknightMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftMetaknightCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<12, soLinkModuleImpl> ftMetaknightLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<290, soGeneralWorkBuildConfig<28, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftMetaknightStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnMetaknightMantle pool). Only the fighter unit builds the real pool; the
// other units of the REL see an opaque block of the same size.
#ifdef FT_METAKNIGHT_ARTICLE_POOLS
#include <ft/metaknight/ft_metaknight_article_pools.h>
typedef ftMetaknightArticleManageModuleBuilder ftMetaknightGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x2178, Fighter_MetaKnight> ftMetaknightGenerateArticleManageModuleBuilder;
#endif

// MATCH-ONLY: Meta Knight's effect module builder keeps two screen handles; the symbol names are the same as in the other fighters
// (the build config has the same name), so the difference is expressed as a specialization local to this fighter.
// HYPOTHESIS: the second handle is for the screen effect of the final smash.
template <>
class soEffectModuleBuildConfig<soEffectModuleImpl> {
public:
    enum { ScreenCapacity = 2 };
    typedef soEffectModuleImpl ModuleType;
};

class ftMetaknightBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftMetaknightInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftMetaknightCollisionAttackModuleBuildConfig CollisionAttackModuleBuildConfig;
    typedef ftMetaknightHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftMetaknightParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftMetaknightResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftMetaknightAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftMetaknightModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftMetaknightMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftMetaknightCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftMetaknightLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftMetaknightStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftMetaknightGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnMetaknightMantle;

class ftMetaknight : public ftFighterBuilder<ftMetaknightBuildConfig> {
    u8 unkTail[0xA5FC - sizeof(ftFighterBuilder<ftMetaknightBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable; // +0xA5FC
    ftData* m_data;                                                  // +0xA60C
public:
    ftMetaknight(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual ~ftMetaknight();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // The hook the article mediator calls to start the article of the pool (HYPOTHESIS: the name follows the module map).
    static bool activeArticle(wnMetaknightMantle* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftMetaknight) == 0xA610, "Class is the wrong size!");
