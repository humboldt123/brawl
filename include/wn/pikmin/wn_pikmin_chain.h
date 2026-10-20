#pragma once

#include <wn/wn_module_accesser_builder.h>
#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/pikmin/wn_pikmin_chain_utility.h>
#include <wn/wn_weapon_builder.h>

// Olimar's Pikmin Chain (up special): a chain of Pikmin that is shot upward, reaches out like Link's clawshot and lets Olimar
// pull himself to the end. It is a lasso-style article (see the wnLasso status views), carrying up to a number of Pikmin kept in
// the work variables. HYPOTHESIS: all field names; offsets follow the native code.

// The parameter block of the chain. HYPOTHESIS: five floats (ids 4000-4004).
struct wnPikminChainParam {
    float unk0, unk4, unk8, unkC, unk10;
};
static_assert(sizeof(wnPikminChainParam) == 0x14, "Chain parameter block extent");

class wnPikminChainParamAccesser : public wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0> {
public:
    wnPikminChainParamAccesser(const wnPikminChainParam* param) : wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0>(0x90) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
    }
};
static_assert(sizeof(wnPikminChainParamAccesser) == 0x3C, "Chain parameter accesser extent");

// The static data block of the article: only the observed prefix is described.
struct wnPikminChainData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPikminChainParam* m_param; // +0x34
};

// The status process pointer vector uses the no-argument constructor, not the size overload.
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);

typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> wnPikminChainPhysicsModuleBuildConfig;

struct wnPikminChainModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<2, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnPikminChainPhysicsModuleBuildConfig PhysicsModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<13, 8, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<5, 5, 1>, 9, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 9, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnPikminChain : public wnWeaponBuilder<wnPikminChainModuleAccesserBuildConfig> {
public:
    const wnPikminChainParam* m_param;         // 0x1F00
    wnPikminChainParamAccesser m_paramAccesser; // 0x1F04

    wnPikminChain(s32 articleId, const wnArticleBuildInfo& info, const wnPikminChainData* data);
    virtual ~wnPikminChain() { }
    void activate(float lr, int founderTaskId, int kind, int team, bool unk, int pikminNum);
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void processFixCamera();
    virtual void renderDebug();
};
static_assert(sizeof(wnWeaponBuilder<wnPikminChainModuleAccesserBuildConfig>) == 0x1F00, "Chain builder extent");
static_assert(sizeof(wnPikminChain) == 0x1F40, "Chain article extent");
