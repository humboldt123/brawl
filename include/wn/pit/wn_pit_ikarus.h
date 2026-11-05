#pragma once

#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

class wnPitIkarusModuleAccesserBuildConfig;
class soModuleAccesser;
struct wnArticleBuildInfo;
// HYPOTHESIS: the article data record the class info passes to the constructor (layout unreconstructed).
struct wnPitIkarusData;

// HYPOTHESIS: the transactor of the Ikarus article; the per-mode changeKineticSub helpers run on an empty one-byte object.
class wnPitIkarusKineticTransactor {
    u8 unk0;
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) const;
    void changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser) const;
};

#ifdef WN_PIT_IKARUS_FULL_BUILDER
#include <wn/wn_module_accesser_builder.h>
// BYTES: the builder holds the full 3f energy (size 0x48); the unit must see it complete before anything else includes it.
#ifndef SO_KINETIC_ENERGY_3F_FULL
#define SO_KINETIC_ENERGY_3F_FULL
#endif
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <so/controller/so_controller_module_link_ref.h>

// Four energies: a free 3f energy (mode 0x20), a rotation energy (mode 0x21), a normal and a gravity energy.
// Certain from the map names: the soTypeList / soInstancePoolInfo / soKineticEnergyInitInfo<0..3, ...> chain.
// HYPOTHESIS: the second InitInfo argument of each (cut off in the map names).
typedef soTypeList<soInstancePoolInfo<soKineticEnergy3f, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyRotNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 1> >,
        soTypeListNullType> > > > wnPitIkarusKineticEnergyTypeList;
// HYPOTHESIS: transactor info arguments follow the Dolfin / Diddy barrel pattern.
typedef soKineticTransactorInfo<wnPitIkarusKineticTransactor, wnKineticTransactor, wnKineticTransactor, soKineticTransactHelper>
    wnPitIkarusKineticTransactorInfo;

// Module typedefs read off the weak builder destructor names (kinetic generic impl + mediator, status <5, work <2,2,1>, 5>,
// damage impl with singleton holders, collision hit <..., 1, 1, Impl, 10..>, motion <3, Impl, 0, 0>, anim command sub
// <1,5,0,0, 1,3,0,0, 1,3,0,0, 1,3,0,0>). The remaining arguments (model, link, visibility, attack, inside-event) are
// HYPOTHESIS, copied from the sibling pooled articles.
class wnPitIkarusModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPitIkarusKineticEnergyTypeList, wnPitIkarusKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<soTypeList<soIntToType<4>, soTypeListNullType> >,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>,
                                      soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<5, soGeneralWorkBuildConfig<2, 2, 1>, 5,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 5, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};

// Ikarus keeps its energies in its own kinetic mediator (no appended common energies), unlike the other pooled articles,
// so the builder is spelled out here instead of selecting WN_WEAPON_NO_COMMON_ENERGIES for the whole unit (as the pump).
template <>
class wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnPitIkarusModuleAccesserBuildConfig> m_modules; // +0xCC
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the no-common-energies builder.
    u32 m_modulesPad;
public:
    // Defined out of line in wn_pit_ikarus.cpp (a global function placed after the Ikarus constructor, as Nikita's).
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd);
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

#else
template <>
class wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x22C8 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
};
#endif

#ifdef WN_PIT_IKARUS_FULL_BUILDER
// Field meanings unknown; offsets from the parameter accesser constructor (ints at 0x0/0x10/0x14, floats elsewhere).
struct wnPitIkarusParam {
    s32 unk0;
    float unk4;
    float unk8;
    float unkC;
    s32 unk10;
    s32 unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
};

struct wnPitIkarusData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPitIkarusParam* m_param; // +0x34
    u8 unk38[1];                     // +0x38, the attack collision set the constructor adds
};
#endif

class wnPitIkarusParamAccesser : public wnExtendParamAccesser<3999, 11, 23999, 3, 0, 0> {
public:
#ifdef WN_PIT_IKARUS_FULL_BUILDER
    explicit wnPitIkarusParamAccesser(const wnPitIkarusParam* param) __attribute__((never_inline)) :
        wnExtendParamAccesser<3999, 11, 23999, 3, 0, 0>(0x7d) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_floats[2] = &param->unkC;
        m_floats[3] = &param->unk18;
        m_floats[4] = &param->unk1C;
        m_floats[5] = &param->unk20;
        m_floats[6] = &param->unk24;
        m_floats[7] = &param->unk28;
        m_floats[8] = &param->unk2C;
        m_floats[9] = &param->unk30;
        m_floats[10] = &param->unk34;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk10;
        m_ints[2] = &param->unk14;
    }
#endif
    virtual ~wnPitIkarusParamAccesser() { }
};

class wnPitIkarus : public wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>, public soDamageEventObserver {
    const void* unk22D4; // the wnPitIkarusParam record (data->m_param)
    wnPitIkarusParamAccesser unk22D8;
public:
#ifdef WN_PIT_IKARUS_FULL_BUILDER
    wnPitIkarus(s32 articleId, const wnArticleBuildInfo& info, const wnPitIkarusData* data);
#endif
    // Weak in the native Ikarus unit (__dt__11wnPitIkarusFv), so defined in the class.
    virtual ~wnPitIkarus() { }
    void activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
};

static_assert(sizeof(wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>) == 0x22C8, "Ikarus builder size");
static_assert(sizeof(wnPitIkarusParamAccesser) == 0x60, "Ikarus parameter accesser size");
static_assert(sizeof(wnPitIkarus) == 0x2338, "Ikarus size");
