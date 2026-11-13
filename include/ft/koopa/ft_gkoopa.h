#pragma once

#include <ft/ft_common_data_accesser.h>
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

// The contact logs of the area builder are a sora_melee array (its constructor is called out of line).
FT_DOL_ARRAY_VECTOR(soAreaContactLog, 4);

////////////////////////////////////////
// Giga Bowser Build Configuration (Bowser's, with the counts of the module map's template arguments)
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftGKoopaInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftGKoopaHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftGKoopaParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftGKoopaResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftGKoopaModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<292, 484> ftGKoopaAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<484, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftGKoopaMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftGKoopaCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftGKoopaLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<292, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftGKoopaStatusModuleBuildConfig;

// The breath pool. Only the fighter unit builds it.
#ifdef FT_GKOOPA_ARTICLE_POOLS
#include <ft/koopa/ft_gkoopa_article_pools.h>
typedef ftGKoopaArticleManageModuleBuilder ftGKoopaGenerateArticleManageModuleBuilder;
#endif

// The module map names the area builder's config soAreaModuleBuildConfig<1, 4, soAreaEnviromentElementCheckerImpl,
// soAreaModuleImpl>: one area instance and four contact logs with the plain area module (the layout of the weapon articles'
// area builders, include/ft/builder/ft_builder_misc.h), built from the u8 area category of the fighter build data.
// HYPOTHESIS: the config name; the original template takes the counts and both classes as arguments.
template <s32 NumInstances, s32 NumLogs>
class ftGKoopaAreaModuleBuildConfig {
public:
    typedef soAreaModuleImpl ModuleType;
};

template <s32 NumInstances, s32 NumLogs>
class soAreaModuleBuilder<ftGKoopaAreaModuleBuildConfig<NumInstances, NumLogs> > {
    u32 m_unk0;                                                   // +0
    soAreaModuleImpl m_module;                                    // +4
    soAreaEnviromentElementCheckerImpl m_checker;                 // +0x50
    soArrayVector<soAreaContactLog, NumLogs> m_contactLogs;       // +0x6C
    soArrayVector<soAreaInstance, NumInstances> m_instances;      // +0xD8
    u32 m_pad;
public:
    soAreaModuleBuilder(soModuleAccesser* acc, u8 areaCategory, soEventObserverRegistrationDesc* regDesc) :
        m_module(acc, areaCategory, &m_instances, &m_contactLogs, &m_checker, &getNullArray<soAreaWind>(), regDesc, -1),
        m_checker(), m_contactLogs(0), m_instances() { }
    void* getModule() { return &m_module; }
};

class ftGKoopaBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftGKoopaInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftGKoopaHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftGKoopaParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftGKoopaResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftGKoopaAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftGKoopaModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftGKoopaMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftGKoopaCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftGKoopaLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftGKoopaStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftGKoopaGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef ftGKoopaAreaModuleBuildConfig<1, 4> AreaModuleBuildConfig;
};

class wnGKoopaBreath;
struct soLinkTouchItemEventArgs;

class ftGKoopa : public ftFighterBuilder<ftGKoopaBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusDataTable;
    ftData* m_data;
public:
    ftGKoopa(s32 entryId,
             Heaps::HeapType instHeap,
             Heaps::HeapType nwModelInstHeap,
             Heaps::HeapType nwMotionInstHeap);
    virtual void onStart(int startKind);
    virtual void onDeadEnd();
    virtual void toKnockOut();
    virtual void processUpdate();
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual void touchItem(soLinkTouchItemEventArgs* args);
    virtual void change(void* work, int startKind);
    virtual void setupChangeSucceedWork(void* work);
    virtual bool isHeartSwapEnableCondition() { return false; }
    virtual bool setMetal(bool setStatus, float health, int unk3) { return false; }
    virtual u32 getChangeSucceedOption() { return 0x7fa3; }
    // The hook the article mediator calls to start the breath (HYPOTHESIS: the name follows the module map).
    static bool activeArticle(wnGKoopaBreath* weapon, soModuleAccesser* acc);
};
