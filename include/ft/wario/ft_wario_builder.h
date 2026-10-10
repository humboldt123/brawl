#pragma once

// Builder view of ftWario for ft_wario.cpp only. The status units include ft_wario.h, whose Fighter-based
// declaration keeps the same layout; their status-process headers cannot coexist with the builder headers.
#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

class soModuleAccesser;

// Wario dispatches his own kinetic modes (changeKinetic/changeKineticSub close this translation unit) but updates
// energies through the shared fighter transactor.
class ftWarioKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
};

////////////////////////////////////////
// Wario Build Configuration
////////////////////////////////////////

// Template arguments follow the module map's builder instance names.
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftWarioInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftWarioHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftWarioParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftWarioResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImplVariable> ftWarioModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<305, 531> ftWarioAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<531, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftWarioMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftWarioCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftWarioCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<9, soLinkModuleImpl> ftWarioLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<305, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftWarioStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftWarioCollisionSearchModuleBuilder;
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftWarioKineticTransactor, ftKineticTransactor> > ftWarioKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnWarioBike pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x4BC8, Fighter_Wario> ftWarioGenerateArticleManageModuleBuilder;

class ftWarioBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftWarioInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftWarioHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftWarioParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftWarioResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftWarioAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftWarioModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftWarioMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftWarioCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftWarioCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftWarioLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftWarioStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftWarioCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftWarioKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftWarioGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// The constructor's 0xD324 allocation fixes the size; the members past the builder are not recovered yet.
class ftWario : public ftFighterBuilder<ftWarioBuildConfig> {
    u8 unkTail[0xD324 - sizeof(ftFighterBuilder<ftWarioBuildConfig>)];
public:
    ftWario(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
};
static_assert(sizeof(ftWario) == 0xD324, "Wario allocation size is wrong!");
