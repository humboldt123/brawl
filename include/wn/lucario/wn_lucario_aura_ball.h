#pragma once

#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;
class soModuleAccesser;

// The parameter record reached through the article data +0x34: two ints (+0x0, +0x10) and sixteen floats.
// HYPOTHESIS: meaning of the parameters.
struct wnLucarioAuraBallParam {
    s32 unk0;
    float unk4;
    float unk8;
    float unkC;
    s32 unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
    float unk40;
    float unk44;
};

#ifdef WN_WEAPON_BUILDER
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: layout of the untouched part of the static article data.
struct wnLucarioAuraBallData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnLucarioAuraBallParam* m_param; // +0x34
};

class wnLucarioAuraBallParamAccesser : public wnExtendParamAccesser<3999, 16, 23999, 2, 0, 0> {
public:
    wnLucarioAuraBallParamAccesser(const wnLucarioAuraBallParam* param) :
        wnExtendParamAccesser<3999, 16, 23999, 2, 0, 0>(0x63) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_floats[2] = &param->unkC;
        m_floats[3] = &param->unk14;
        m_floats[4] = &param->unk18;
        m_floats[5] = &param->unk1C;
        m_floats[6] = &param->unk20;
        m_floats[7] = &param->unk24;
        m_floats[8] = &param->unk28;
        m_floats[9] = &param->unk2C;
        m_floats[10] = &param->unk30;
        m_floats[11] = &param->unk34;
        m_floats[12] = &param->unk38;
        m_floats[13] = &param->unk3C;
        m_floats[14] = &param->unk40;
        m_floats[15] = &param->unk44;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk10;
    }
    virtual ~wnLucarioAuraBallParamAccesser() { }
};
static_assert(sizeof(wnLucarioAuraBallParamAccesser) == 0x70, "Aura ball parameter accessor extent");

class wnLucarioAuraBallModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
        wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<10, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0,
        soCollisionAttackModuleImpl, 1, false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<10, 11, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 10, 0, 0, 1, 10, 0, 0, 1, 10, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnLucarioAuraBall : public wnWeaponBuilder<wnLucarioAuraBallModuleAccesserBuildConfig> {
    const wnLucarioAuraBallParam* m_param;          // +0x1FDC
    wnLucarioAuraBallParamAccesser m_paramAccesser; // +0x1FE0
#else
// Call-side view keeps the native extent without instantiating the builder.
struct wnLucarioAuraBallData;
class wnLucarioAuraBall : public Weapon {
    u8 m_storage[0x2050 - sizeof(Weapon)];
#endif
public:
    wnLucarioAuraBall(s32 articleId, const wnArticleBuildInfo& info, const wnLucarioAuraBallData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnLucarioAuraBall() { }
#else
    virtual ~wnLucarioAuraBall();
#endif
    // HYPOTHESIS: the order of the integer and float parameters (the native code only fixes each kind's order).
    void activate(int founderTaskId, int team, float lr, int chargeFrame, float chargeScale, int fighterKind);
    virtual void onDeactivate();
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    static float getChargeRate(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: arguments are the fighter's facing, a work float and the fighter's scale.
    void shoot(float lr, float unk, float scale);
    int getChargeLevel();
    int getChargeFrame();
    static void setEffectScale(soModuleAccesser* moduleAccesser);
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnLucarioAuraBall) == 0x2050, "Lucario aura ball article extent");
