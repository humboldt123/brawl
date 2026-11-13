#pragma once

// Samus's grapple beam (article kind 0x84, a lasso article like the Zero Suit whip: its statuses are the shared wnLasso ones). The
// article is built in sora_melee; the fighter module only sees the opaque shell of ft/samus/ft_samus_article_pools.h.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
// BYTES: the sora_melee unit mode of the module accesser builder (member list macro, null builders, situation builder
// shadow) is shared with the Light Bow, see wn/zelda/wn_zelda_lighting_bow.h.
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_lasso_status_view.h>

struct wnArticleBuildInfo;

// HYPOTHESIS: the lasso status view of wn/wn_lasso_status_view.h with slot 0xB8 taking an int (the beam calls it with 1). A copy of
// the class so that this unit does not depend on a change of the shared header: its slotB8() is declared without parameters.
class wnSamusGBeamStatusView : public wnLassoViewPad8 {
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void slot10();
    virtual void slot14();
    virtual void unk18Attach(u32 archiveId, int, u32, u32, u32, int, int, int);
    virtual void slot1C();
    virtual void slot20();
    virtual void slot24();
    virtual void slot28();
    virtual void slot2C();
    virtual void unk30Int(int);
    virtual void slot34();
    virtual void slot38();
    virtual void slot3C();
    virtual void slot40();
    virtual void slot44();
    virtual void slot48();
    virtual void slot4C();
    virtual void slot50();
    virtual void slot54();
    virtual void slot58();
    virtual void slot5C();
    virtual void unk60Int(int);
    virtual void unk64Int(int);
    virtual void unk68VecPair(Vec3f* a, Vec3f* b);
    virtual void slot6C();
    virtual int unk70Int();
    virtual float unk74Float(int);
    virtual void slot78();
    virtual void slot7C();
    virtual void slot80();
    virtual void slot84();
    virtual int unk88Int();
    virtual void unk8CInt(int);
    virtual void slot90();
    virtual void slot94();
    virtual void slot98();
    virtual void slot9C();
    virtual void slotA0();
    virtual void unkA4Int(int);
    virtual int unkA8Int();
    virtual void slotAC();
    virtual void unkB0VecInt(Vec3f* pos, int, bool);
    virtual void unkB4Int(int);
    virtual void unkB8Int(int);
    virtual void unkBCInt(int);
    virtual void slotC0();
    virtual void slotC4();
    virtual int unkC8Int(int);
    virtual void unkCCVec(int, Vec3f* pos);
    virtual void slotD0();
    virtual int unkD4Int();
    virtual void slotD8();
    virtual void unkDCFloat(float);
    virtual int unkE0Int();
};

// HYPOTHESIS: all field names; the offsets follow the native code (parameter block read through +0x34 of the static data).
struct wnSamusGBeamParam {
    float unk0;
    s32 unk4;
    s32 unk8;
    float unkC;
    float unk10;
    float unk14; // read by activate
};

struct wnSamusGBeamData {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    const wnSamusGBeamParam* m_param; // +0x34
};

class wnSamusGBeamParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 2, 0, 0> {
public:
    wnSamusGBeamParamAccesser(const wnSamusGBeamParam* param) : wnExtendParamAccesser<3999, 4, 23999, 2, 0, 0>(0x84) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unkC;
        m_floats[2] = &param->unk10;
        m_floats[3] = &param->unk14;
        m_ints[0] = &param->unk4;
        m_ints[1] = &param->unk8;
    }
    virtual ~wnSamusGBeamParamAccesser();
};

// The four command threads of the lasso articles (see the whip): the stack capacity of every thread is 2.
template <int E0, int C0, int A0, int B0, int S0, int E1, int C1, int A1, int B1, int S1, int E2, int C2, int A2, int B2, int S2, int E3,
          int C3, int A3, int B3, int S3>
class wnAnimCmdModuleSubBuildConfigFullProperty {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, C0, A0, B0, 0, 0, S0> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, C1, A1, B1, 1, 0, S1> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, C2, A2, B2, 1, 0, S2> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, C3, A3, B3, 1, 0, S3> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 2> Unit4Config;
    enum { Unit0Enabled = E0, Unit1Enabled = E1, Unit2Enabled = E2, Unit3Enabled = E3, Unit4Enabled = 1 };
};

class wnSamusGBeamModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnTurnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnTurnInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<10, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 9, 0, 0, 2, 1, 10, 0, 0, 2, 1, 10, 0, 0, 2, 1, 10, 0, 0, 2>
        AnimCmdModuleSubBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    // HYPOTHESIS: the lasso articles keep their rope state in the physics module slot (see wn/wn_lasso_status_view.h).
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSamusGBeamModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusGBeamModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in the unit (emission order)
};

class wnSamusGBeam : public wnWeaponBuilder<wnSamusGBeamModuleAccesserBuildConfig> {
    const wnSamusGBeamParam* m_param;            // +0x1748
    wnSamusGBeamParamAccesser m_paramAccesser;   // +0x174C
public:
    wnSamusGBeam(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGBeamData* data);
    virtual ~wnSamusGBeam();
    // HYPOTHESIS: parameter names; attack is set by the fighter for the attack variants of the statuses (0x34, 0x36, 0x38).
    void activate(int founderTaskId, int etcResourceId, int team, float lr, Vec3f* __restrict pos, int attack);
};
static_assert(sizeof(wnSamusGBeam) == 0x178C, "Samus grapple beam article extent");
