#pragma once

#include <so/so_value_accesser.h>
#include <wn/wn_module_accesser_builder.h>

// HYPOTHESIS: the Mario fireball article data (kind 0, shared with the Luigi fireball kind 149): the class
// info createArray reads the resource group number (+0x00, cast to u8 for the module build data) and the
// parameter pointer (+0x34, stored at article +0x1F40); the middle stays unreconstructed.
struct wnMarioFireballData {
    s32 m_resGroupNo;     // 0x00
    u8 unk4[0x30];        // 0x04
    const void* m_param;  // 0x34 HYPOTHESIS: parameter block, type unreconstructed
};

// The module set the original unit instantiates (map 0x2da1f0-0x2dbba8): null situation, stop and
// visibility modules, a one-part fighter-category attack box, one motion slot, one status kind, link
// capacity 5 and four single-entry animation-command control units; the rest are the common defaults.
class wnMarioFireballModuleAccesserBuildConfig : public wnCommonBuildConfig {
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
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                     soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

// The bullet article. The shared class info placement-news it with the constructor below; defining it in
// the class body lets wnClassInfoImpl<0,...>::createArray inline it (the 0x3D8 createArray allocates the
// 0x1F44 extent and ends with the nine multiple-inheritance vptr writes).
class wnMarioFireball : public wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig> {
    const void* m_param;  // 0x1F40
public:
#ifdef WN_MARIO_FIREBALL_CTOR_OUTLINE
    // The Luigi fireball unit sees this constructor as a declaration only and calls the out-of-line
    // copy this unit emits (its createArray ends with the Luigi vptr writes after that call).
    wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data);
#else
    wnMarioFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data) :
        wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig>(
            "MarioFireball", articleId, info,
            wnModuleAccesserBuildData(0, data, info.getHeapModule()->getInstanceHeap(),
                info.getHeapModule()->getNWModelInstanceHeap(),
                info.getHeapModule()->getNWMotionInstanceHeap(),
                static_cast<u8>(data->m_resGroupNo), -1, &g_soParamAccesserNull, 0, 0, 0, 0, 0, 0, 0)),
        m_param(data->m_param) {
        Weapon::deactivate(false);
    }
#endif
    virtual ~wnMarioFireball() { }
};

static_assert(sizeof(soModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>) == 0x1848,
              "Mario module builder extent");
static_assert(sizeof(wnModuleAccesserBuilder<wnMarioFireballModuleAccesserBuildConfig>) == 0x1DF8,
              "Mario article modules extent");
static_assert(sizeof(wnWeaponBuilder<wnMarioFireballModuleAccesserBuildConfig>) == 0x1F40,
              "Mario weapon builder extent");
static_assert(sizeof(wnMarioFireball) == 0x1F44, "Mario fireball article extent");
