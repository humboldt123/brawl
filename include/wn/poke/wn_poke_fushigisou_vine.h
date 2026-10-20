#pragma once
// wnPokeFushigisouVine article (article kind 0x4b).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);
// The parameter block of the article (2 float and 0 int parameters). HYPOTHESIS: meaning of the parameters.
struct wnPokeFushigisouVineParam {
    float unk0;
    float unk4;
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnPokeFushigisouVineData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPokeFushigisouVineParam* m_param; // +0x34
};
class wnPokeFushigisouVineParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 0, 0, 0> {
public:
    wnPokeFushigisouVineParamAccesser(const wnPokeFushigisouVineParam* param) : wnExtendParamAccesser<3999, 2, 23999, 0, 0, 0>(0x4b) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
    }
    virtual ~wnPokeFushigisouVineParamAccesser() { }
};
class wnPokeFushigisouVineModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 9, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};
class wnPokeFushigisouVine : public wnWeaponBuilder<wnPokeFushigisouVineModuleAccesserBuildConfig> {
public:
    const wnPokeFushigisouVineParam* m_param;
    wnPokeFushigisouVineParamAccesser m_paramAccesser;
    wnPokeFushigisouVine(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouVineData* data);
    virtual ~wnPokeFushigisouVine() { }
};
