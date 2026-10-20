#pragma once

// Lucas's Rope Snake (Himohebi, article kind 82): a lasso-style article shot out by his up special (grapple). It shares the
// lasso statuses of the sora_melee module with the Pikmin Chain and Link's clawshot. HYPOTHESIS: all field names; offsets follow
// the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the snake: three floats (ids 4000..). unk8 is also written to the work variables on a plain activation.
struct wnLucasHimohebiParam {
    float unk0, unk4, unk8;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnLucasHimohebiData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLucasHimohebiParam* m_param; // +0x34
};

class wnLucasHimohebiParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> {
public:
    wnLucasHimohebiParamAccesser(const wnLucasHimohebiParam* param) : wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>(82) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
    }
    virtual ~wnLucasHimohebiParamAccesser() { }
};

// The status process pointer vector uses the no-argument constructor, not the size overload.
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);

struct wnLucasHimohebiModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<7, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 9, 0, 0, 1, 7, 0, 0, 1, 7, 0, 0, 1, 7, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnLucasHimohebi : public wnWeaponBuilder<wnLucasHimohebiModuleAccesserBuildConfig> {
public:
    const wnLucasHimohebiParam* m_param;          // +0x20CC
    wnLucasHimohebiParamAccesser m_paramAccesser; // +0x20D0

    wnLucasHimohebi(s32 articleId, const wnArticleBuildInfo& info, const wnLucasHimohebiData* data);
    virtual ~wnLucasHimohebi() { }
    void activate(int founderTaskId, int kind, int team, float lr, int attach, int plain);
};
