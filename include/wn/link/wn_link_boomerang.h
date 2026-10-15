#pragma once

// Link's boomerang (article kind 3): thrown by the Gale Boomerang special, it is carried around by two energies (the normal
// movement and a rotation) and turns back after a while.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 2);
WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(4);

// The parameter block of the boomerang (14 float and 7 int parameters, ids 4000.. and 24000..).
// HYPOTHESIS: meanings; the layout follows the pointer table the constructor builds.
struct wnLinkBoomerangParam {
    float unk0;
    s32 unk4;
    s32 unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    s32 unk34;
    s32 unk38;
    float unk3C;
    float unk40;
    float unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnLinkBoomerangData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLinkBoomerangParam* m_param; // +0x34
};

class wnLinkBoomerangParamAccesser : public wnExtendParamAccesser<3999, 14, 23999, 7, 0, 0> {
public:
    wnLinkBoomerangParamAccesser(const wnLinkBoomerangParam* param) : wnExtendParamAccesser<3999, 14, 23999, 7, 0, 0>(3) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unkC;
        m_floats[2] = &param->unk10;
        m_floats[3] = &param->unk14;
        m_floats[4] = &param->unk18;
        m_floats[5] = &param->unk1C;
        m_floats[6] = &param->unk20;
        m_floats[7] = &param->unk24;
        m_floats[8] = &param->unk28;
        m_floats[9] = &param->unk2C;
        m_floats[10] = &param->unk30;
        m_floats[11] = &param->unk3C;
        m_floats[12] = &param->unk40;
        m_floats[13] = &param->unk44;
        m_ints[0] = &param->unk4;
        m_ints[1] = &param->unk8;
        m_ints[2] = &param->unk34;
        m_ints[3] = &param->unk38;
        m_ints[4] = &param->unk48;
        m_ints[5] = &param->unk4C;
        m_ints[6] = &param->unk50;
    }
    virtual ~wnLinkBoomerangParamAccesser() { }
};

class wnLinkBoomerangModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soPostureModuleBuildConfig<2, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<6, 2, 1> GeneralWorkBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnLinkBoomerang : public wnWeaponBuilder<wnLinkBoomerangModuleAccesserBuildConfig> {
public:
    const wnLinkBoomerangParam* m_param;           // +0x20E4
private:
    wnLinkBoomerangParamAccesser m_paramAccesser;  // +0x20E8
public:
    wnLinkBoomerang(s32 articleId, const wnArticleBuildInfo& info, const wnLinkBoomerangData* data);
    virtual ~wnLinkBoomerang() { }
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
    // HYPOTHESIS: parameter names beyond the founder, resource and team.
    void activate(int founderTaskId, int resourceId, int team);
    // Throws the boomerang from the hand node of the thrower (lr = facing, angle in radians, kind 1 = the held throw).
    void shoot(float lr, float angle, u32 kind);
};
