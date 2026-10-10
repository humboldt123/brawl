#pragma once

// Builder view of ftSnake for ft_snake.cpp only. The status units include ft_snake.h, whose Fighter-based
// declaration keeps the same layout; their status-process headers cannot coexist with the builder headers.
#include <ft/ft_fighter_builder.h>
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

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (wnSnake weapon pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0x249E4, Fighter_Snake> ftSnakeGenerateArticleManageModuleBuilder;

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

// The constructor's 0x2D040 allocation fixes the size; the members past the builder are not recovered yet.
class ftSnake : public ftFighterBuilder<ftSnakeBuildConfig> {
    u8 unkTail[0x2D040 - sizeof(ftFighterBuilder<ftSnakeBuildConfig>)];
public:
    ftSnake(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
};
static_assert(sizeof(ftSnake) == 0x2D040, "Snake allocation size is wrong!");
