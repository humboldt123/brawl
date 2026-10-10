#pragma once

// Builder view of ftYoshi for ft_yoshi.cpp only. The status units include ft_yoshi.h, whose Fighter-based
// declaration keeps the same layout; their status-process headers cannot coexist with the builder headers.
#include <ft/ft_fighter_builder.h>
#include <ft/yoshi/ft_yoshi_kinetic_transactor.h>
#include <sr/sr_common.h>

////////////////////////////////////////
// Yoshi Build Configuration
////////////////////////////////////////

// Template arguments follow the module map's builder instance names.
typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftYoshiInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftYoshiHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftYoshiParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftYoshiResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftYoshiModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<293, 488> ftYoshiAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<488, soMotionModuleImpl, 4, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftYoshiMotionModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftYoshiCollisionShieldModuleBuildConfig;
typedef soCollisionReflectorModuleBuildConfigGroups<3, 20, 2, soCollisionShieldEventPresenterReflector, soCollisionShieldModuleImpl> ftYoshiCollisionReflectorModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftYoshiLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<293, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftYoshiStatusModuleBuildConfig;

// Yoshi dispatches his own kinetic modes but updates energies through the shared fighter transactor.
typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl, ftKineticMediatorImplT<ftYoshiKineticTransactor, ftKineticTransactor> > ftYoshiKineticModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnYoshiStar pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x19E5C, Fighter_Yoshi> ftYoshiGenerateArticleManageModuleBuilder;

class ftYoshiBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftYoshiInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftYoshiHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftYoshiParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftYoshiResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftYoshiAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftYoshiModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftYoshiMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftYoshiCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftYoshiCollisionReflectorModuleBuildConfig CollisionReflectorModuleBuildConfig;
    typedef ftYoshiLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftYoshiStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftYoshiKineticModuleBuildConfig KineticModuleBuildConfig;
    typedef ftYoshiGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};
// The constructor's
// 0x222B0 allocation fixes the size; the members past the builder are not recovered yet.
class ftYoshi : public ftFighterBuilder<ftYoshiBuildConfig> {
    u8 unkTail[0x222b0 - sizeof(ftFighterBuilder<ftYoshiBuildConfig>)];
public:
    ftYoshi(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
    void createGuardColorAnim();
    void deleteGuardColorAnim();
    void updateGuardColorAnim();
};
static_assert(sizeof(ftYoshi) == 0x222b0, "Yoshi allocation size is wrong!");
