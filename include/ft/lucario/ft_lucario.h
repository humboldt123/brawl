#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Lucario-only builder variants
////////////////////////////////////////

#include <ft/lucario/ft_lucario_param_customize_module.h>

////////////////////////////////////////
// Lucario Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftLucarioInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftLucarioHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftLucarioParamCustomizeModule> ftLucarioParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftLucarioResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftLucarioModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<293, 491> ftLucarioAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<491, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftLucarioMotionModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftLucarioLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<293, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftLucarioStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (aura ball and Aura Sphere pools). Only the fighter unit builds the real
// pools; the other units of the REL see an opaque block of the same size.
#ifdef FT_LUCARIO_ARTICLE_POOLS
#include <ft/lucario/ft_lucario_article_pools.h>
typedef ftLucarioArticleManageModuleBuilder ftLucarioGenerateArticleManageModuleBuilder;
#else
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x801C, Fighter_Lucario> ftLucarioGenerateArticleManageModuleBuilder;
#endif

class ftLucarioBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftLucarioInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftLucarioHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftLucarioParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftLucarioResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftLucarioAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftLucarioModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftLucarioMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftLucarioLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftLucarioStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftLucarioGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

class wnLucarioQigong;

class ftLucario : public ftFighterBuilder<ftLucarioBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftLucario(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    // The hook the article mediator calls to start the Aura Sphere (HYPOTHESIS: the name follows the module map).
    static bool activeArticle(wnLucarioQigong* weapon, soModuleAccesser* acc);
    static void setMachValidity(soModuleAccesser* moduleAccesser, bool valid);
    void effect_suspend();
    void effect_resume();
    float calcAuraScale();
    void setAuraScale(float scale);
    virtual void notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser);
};
