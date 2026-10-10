#pragma once

#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// Sonic Build Configuration
////////////////////////////////////////

// Template arguments follow the module map's builder instance names.
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftSonicInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftSonicHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftSonicParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftSonicResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftSonicModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<295, 497> ftSonicAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<497, soMotionModuleImpl, 2, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftSonicMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftSonicCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftSonicCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<7, soLinkModuleImpl> ftSonicLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<295, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftSonicStatusModuleBuildConfig;
typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > ftSonicCollisionSearchModuleBuilder;

// HYPOTHESIS: this fighter has its own kinetic transactor (changeKinetic is in a later translation unit)
FT_KINETIC_TRANSACTOR(ftSonicKineticTransactor);
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftSonicKineticTransactor> > ftSonicKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnSonicGimmickJump pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x4FFC, Fighter_Sonic> ftSonicGenerateArticleManageModuleBuilder;

class ftSonicBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftSonicInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftSonicHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftSonicParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftSonicResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftSonicAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftSonicModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftSonicMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftSonicCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftSonicCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftSonicLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftSonicStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftSonicCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef ftSonicKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftSonicGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// The constructor's 0xD5D8 allocation fixes the size; the members past the builder are not recovered yet.
class ftSonic : public ftFighterBuilder<ftSonicBuildConfig> {
    u8 unkTail[0xD5D8 - sizeof(ftFighterBuilder<ftSonicBuildConfig>)];
public:
    ftSonic(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
};
static_assert(sizeof(ftSonic) == 0xD5D8, "Sonic allocation size is wrong!");
