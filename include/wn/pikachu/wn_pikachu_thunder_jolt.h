#pragma once

#include <so/so_value_accesser.h>
#include <wn/wn_module_accesser_builder.h>
// After the weapon builder scaffolding (its fighter chain defines the helpers the builder headers use).
#include <ft/builder/ft_module_accesser_builder.h>

// HYPOTHESIS: the Pikachu thunder jolt article data (kind 9): the class info createArray reads the
// resource group number (+0x00, cast to u8 for the module build data) and the parameter pointer
// (+0x34, stored at article +0x1F90); the middle stays unreconstructed.
struct wnPikachuThunderJoltData {
    s32 m_resGroupNo;     // 0x00
    u8 unk4[0x30];        // 0x04
    const void* m_param;  // 0x34 HYPOTHESIS: parameter block, type unreconstructed
};

// The module set the original unit instantiates: the Mario fireball set with two changes - the
// standalone general-work builder uses <6,1,1> (the fireball keeps the shared <7,6,1> default) and
// the generate-article-manage module is the soArticleMedia one instead of the null builder. The
// net effect is a builder 0x50 bytes larger than the Mario fireball's at every member after them.
class wnPikachuThunderJoltModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    // The capacity-carrying config class: link capacity 5 (the one-argument config would fix it at 7
    // and change the extent).
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    // The smaller general-work selection this weapon uses; it is what the standalone work builder
    // member instantiates (the status module keeps its own <2,2,1> selection).
    typedef soGeneralWorkBuildConfig<6, 1, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                     soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0>
        AnimCmdModuleSubBuildConfig;
    // soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfig<soArticleMedia> >:
    // its constructor and destructor bodies live in this unit's assembly, so the class stays behind
    // the opaque storage of the right size (module last 0x3C bytes); the Id keeps the out-of-line
    // constructor/destructor symbols apart from the other opaque instantiations.
    typedef ftOpaqueGenerateArticleManageModuleBuilder<0x6C, 9> GenerateArticleManageModuleBuilder;
};

// The jolt article. The shared class info placement-news it with the constructor below (the 0x3D8
// createArray allocates the 0x1F94 extent and ends with the nine multiple-inheritance vptr writes).
class wnPikachuThunderJolt : public wnWeaponBuilder<wnPikachuThunderJoltModuleAccesserBuildConfig> {
    const void* m_param;  // 0x1F90
public:
    wnPikachuThunderJolt(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderJoltData* data) :
        wnWeaponBuilder<wnPikachuThunderJoltModuleAccesserBuildConfig>(
            "PikachuThunderJolt", articleId, info,
            wnModuleAccesserBuildData(9, data, info.getHeapModule()->getInstanceHeap(),
                info.getHeapModule()->getNWModelInstanceHeap(),
                info.getHeapModule()->getNWMotionInstanceHeap(),
                static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
        m_param(data->m_param) {
        Weapon::deactivate(false);
    }
    virtual ~wnPikachuThunderJolt() { }
};

static_assert(sizeof(soModuleAccesserBuilder<wnPikachuThunderJoltModuleAccesserBuildConfig>) == 0x1898,
              "jolt module builder extent");
static_assert(sizeof(wnModuleAccesserBuilder<wnPikachuThunderJoltModuleAccesserBuildConfig>) == 0x1E48,
              "jolt article modules extent");
static_assert(sizeof(wnWeaponBuilder<wnPikachuThunderJoltModuleAccesserBuildConfig>) == 0x1F90,
              "jolt weapon builder extent");
static_assert(sizeof(wnPikachuThunderJolt) == 0x1F94, "jolt article extent");
