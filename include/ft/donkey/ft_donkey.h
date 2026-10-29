#pragma once

#include <ft/ft_common_data_accesser.h>
#include <so/item/so_item_manage_module_impl.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Donkey Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftDonkeyInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftDonkeyHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftDonkeyParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftDonkeyResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftDonkeyModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<309, 516> ftDonkeyAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<516, soMotionModuleImpl, 4, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftDonkeyMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftDonkeyCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftDonkeyLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<309, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftDonkeyStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (barrel conga and sound effect pools). Only the fighter unit builds the real
// pools; the other units of the REL see an opaque block of the same size.
#ifdef FT_DONKEY_ARTICLE_POOLS
#include <ft/donkey/ft_donkey_article_pools.h>
typedef ftDonkeyArticleManageModuleBuilder ftDonkeyGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0xA0B0, Fighter_Donkey> ftDonkeyGenerateArticleManageModuleBuilder;
#endif

class ftDonkeyBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftDonkeyInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftDonkeyHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftDonkeyParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftDonkeyResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftDonkeyAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftDonkeyModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftDonkeyMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftDonkeyCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftDonkeyLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftDonkeyStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftDonkeyGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnDonkeyBarrelConga;
class wnDonkeySoundEffect;

class ftDonkey : public ftFighterBuilder<ftDonkeyBuildConfig> {
    u8 unkTail[0x12588 - sizeof(ftFighterBuilder<ftDonkeyBuildConfig>)];
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftDonkey(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    virtual void onStart(int startKind);
    virtual bool notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int, int);
    // The hooks the article mediator calls to start an article of the pool (HYPOTHESIS: names follow the module map).
    static bool activeArticle(wnDonkeyBarrelConga* weapon, soModuleAccesser* acc);
    static bool activeArticle1(wnDonkeySoundEffect* weapon, soModuleAccesser* acc);
};
