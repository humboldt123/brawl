#pragma once

#include <wn/wn_module_accesser_builder.h>
#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <so/controller/so_controller_module_link_ref.h>

// The Pikmin article of Olimar's final smash (End of Day): a Dolfin-type article that carries Olimar and the Pikmin through the
// stage. Its status flow is 1 (rise), 2 (final attack), 3 (descent). HYPOTHESIS: all field names; offsets follow the native code.

// The parameter block of the Dolfin article. HYPOTHESIS: eight floats and three ints (ids 4000-4010), laid out like the native block.
struct wnPikminDolfinParam {
    float unk0, unk4, unk8;
    s32 unkC, unk10;
    float unk14, unk18, unk1C, unk20;
    s32 unk24;
    float unk28;
};
static_assert(sizeof(wnPikminDolfinParam) == 0x2C, "Dolfin parameter block extent");

class wnPikminDolfinParamAccesser : public wnExtendParamAccesser<3999, 8, 23999, 3, 0, 0> {
public:
    wnPikminDolfinParamAccesser(const wnPikminDolfinParam* param) : wnExtendParamAccesser<3999, 8, 23999, 3, 0, 0>(0x81) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unk14;
        m_floats[4] = &param->unk18;
        m_floats[5] = &param->unk1C;
        m_floats[6] = &param->unk20;
        m_floats[7] = &param->unk28;
        m_ints[0] = &param->unkC;
        m_ints[1] = &param->unk10;
        m_ints[2] = &param->unk24;
    }
};
static_assert(sizeof(wnPikminDolfinParamAccesser) == 0x54, "Dolfin parameter accesser extent");

// sora_melee instances the article's builders call (declared only).
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 3);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
// The status process pointer vector uses the no-argument constructor, not the size overload.
WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(5);

// The static data block of the article: only the observed prefix is described.
struct wnPikminDolfinData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPikminDolfinParam* m_param; // +0x34
    s32 unk38;
    s32 unk3C;
    u32 m_hitDataStorage;               // +0x40, start of an opaque collision hit set
};

// Kinetic transactor of the Dolfin article. HYPOTHESIS: like the other article transactors, the original passes an empty
// one-byte object as the implicit this of the changeKineticSub* helpers; `pools` is the opaque energy-pool storage (the
// normal energy sits at +0x10).
class wnPikminDolfinKineticTransactor {
    u8 unk0;

public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);

    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub1(void* pools, soModuleAccesser* moduleAccesser);
};

// One normal energy; the mediator changes its kind through the Dolfin transactor and adds speed through the shared one.
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
                   soTypeListNullType> wnPikminDolfinKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnPikminDolfinKineticTransactor, wnKineticTransactor, wnKineticTransactor, soKineticTransactHelper>
    wnPikminDolfinKineticTransactorInfo;

struct wnPikminDolfinModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPikminDolfinKineticEnergyTypeList, wnPikminDolfinKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<8, soMotionModuleImpl, 0, 2, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 3, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 2> VisibilityModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef soCameraModuleBuildConfig<soCameraModuleImpl> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<5, soGeneralWorkBuildConfig<2, 2, 1>, 5, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 5, 0, 0, 1, 8, 0, 0, 1, 8, 0, 0, 1, 8, 0, 0> AnimCmdModuleSubBuildConfig;
};
// The builder of this article carries no appended common energies: its only energy is the normal one of the kinetic mediator.
template <>
class wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnPikminDolfinModuleAccesserBuildConfig> m_modules; // +0xCC
    // HYPOTHESIS: the native module storage ends one word later than the generic builder (padding of its last animation command
    // unit); the word is kept here.
    u32 unk23C0;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), true), m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
    }
};
static_assert(sizeof(wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig>) == 0x23C4, "Dolfin builder extent");

class wnPikminDolfin : public wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig> {
public:
    const wnPikminDolfinParam* m_param;           // 0x23C4
    wnPikminDolfinParamAccesser m_paramAccesser;  // 0x23C8
    Vec3f m_startPos;                              // 0x241C: position saved when the article rises

    wnPikminDolfin(s32 articleId, const wnArticleBuildInfo& info, const wnPikminDolfinData* data);
    virtual ~wnPikminDolfin() { }
    void activate(int founderTaskId, int kind, int team, Vec3f* pos);
    virtual void onDeactivate();
    virtual void updateNodeSRT();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void processFixPosition();
    virtual bool notifyEventCollisionAttackCheck(u32 attackKind);
};
static_assert(sizeof(wnPikminDolfin) == 0x2428, "Dolfin article extent");
