#pragma once
// wnMarioPumpWater article (article kind 0x48).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(3);
// The parameter block of the article (6 float and 2 int parameters). HYPOTHESIS: meaning of the parameters.
struct wnMarioPumpWaterParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    s32 unk10;
    float unk14;
    float unk18;
    s32 unk1C;
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnMarioPumpWaterData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnMarioPumpWaterParam* m_param; // +0x34
};
class wnMarioPumpWaterParamAccesser : public wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0> {
public:
    wnMarioPumpWaterParamAccesser(const wnMarioPumpWaterParam* param) : wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0>(0x48) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk14;
        m_floats[5] = &param->unk18;
        m_ints[0] = &param->unk10;
        m_ints[1] = &param->unk1C;
    }
    virtual ~wnMarioPumpWaterParamAccesser() { }
};
class wnMarioPumpWaterModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};
class wnMarioPumpWater : public wnWeaponBuilder<wnMarioPumpWaterModuleAccesserBuildConfig> {
public:
    const wnMarioPumpWaterParam* m_param;
    wnMarioPumpWaterParamAccesser m_paramAccesser;
    wnMarioPumpWater(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpWaterData* data);
    virtual ~wnMarioPumpWater() { }
};
