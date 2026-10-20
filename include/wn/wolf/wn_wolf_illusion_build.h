#pragma once
// wnWolfIllusion article (article kind 0x28).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(3);
// The parameter block of the article (0 float and 2 int parameters). HYPOTHESIS: meaning of the parameters.
struct wnWolfIllusionParam {
    s32 unk0;
    s32 unk4;
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnWolfIllusionData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnWolfIllusionParam* m_param; // +0x34
};
class wnWolfIllusionParamAccesser : public wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0> {
public:
    wnWolfIllusionParamAccesser(const wnWolfIllusionParam* param) : wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0>(0x28) {
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnWolfIllusionParamAccesser() { }
};
class wnWolfIllusionModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};
class wnWolfIllusion : public wnWeaponBuilder<wnWolfIllusionModuleAccesserBuildConfig> {
public:
    const wnWolfIllusionParam* m_param;
    wnWolfIllusionParamAccesser m_paramAccesser;
    wnWolfIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnWolfIllusionData* data);
    virtual ~wnWolfIllusion() { }
};
static_assert(sizeof(wnWolfIllusion) == 0x1DE4, "wnWolfIllusion extent");
