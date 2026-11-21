#pragma once

#include <wn/wn_weapon_builder.h>
#include <mt/mt_vector.h>
#include <wn/wn_article_build_info.h>

#ifdef WN_WEAPON_BUILDER
// The unit that owns the constructor and builders instantiates the full builder (article kind 0x7C).
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/sonic/wn_sonic_super_sonic_kinetic_transactor.h>
#include <so/controller/so_controller_module_link_ref.h>
#ifdef WN_KINETIC_MEDIATOR_LATE // BYTES: link order, the mediator template follows the accesser and builder templates
#include <wn/wn_kinetic_mediator.h>
#endif

#ifndef WN_STATUS_PROC_VECTOR_4_DECLARED // the fighter unit declares it before the spring
WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(4);
#endif

// HYPOTHESIS: parameter names; ten floats and two ints, ids 4000.. and 24000..
struct wnSonicSuperSonicParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    s32 unk28;
    s32 unk2C;
};
static_assert(sizeof(wnSonicSuperSonicParam) == 0x30, "Super Sonic parameter block extent");

// HYPOTHESIS: layout of the untouched part of the static article data.
struct wnSonicSuperSonicData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSonicSuperSonicParam* m_param; // +0x34
    u32 m_hitDataStorage;                   // +0x38, start of the hit data set
    u32 unk3C;
    u32 unk40[2];                           // HYPOTHESIS: handed to the module build data
    s32 unk48;
    s32 unk4C;
};

class wnSonicSuperSonicParamAccesser : public wnExtendParamAccesser<3999, 10, 23999, 2, 0, 0> {
public:
    explicit wnSonicSuperSonicParamAccesser(const wnSonicSuperSonicParam* param) __attribute__((never_inline)) :
        wnExtendParamAccesser<3999, 10, 23999, 2, 0, 0>(0x7C) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->unk18;
        m_floats[7] = &param->unk1C;
        m_floats[8] = &param->unk20;
        m_floats[9] = &param->unk24;
        m_ints[0] = &param->unk28;
        m_ints[1] = &param->unk2C;
    }
    virtual ~wnSonicSuperSonicParamAccesser() { }
};
static_assert(sizeof(wnSonicSuperSonicParamAccesser) == 0x58, "Super Sonic parameter accesser extent");

// HYPOTHESIS: the fifth effect configuration value is the effect module's own constant (-1 in the common configuration);
// the builder is otherwise the common article effect builder.
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, 5, T> > :
    public soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> > {
    u8 unk38[4];
#ifdef FT_EFFECT_SCREEN_SINGLETON_HOLDER
    typedef soSingletonHolder<soArrayNull<efScreenHandle> > ScreenNullHolder;
#else
    typedef soArrayNull<efScreenHandle> ScreenNullHolder;
#endif
    soArraySelectHolder<1, soArrayVector<efScreenHandle, 1>, ScreenNullHolder> m_screens;
    soArrayVector<u32, 1> m_u32s;
    T m_effectModule;
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> >(1, 0),
        m_screens(1, 0), m_u32s(1, 0, 0),
        m_effectModule(acc, this->get(), nodeData, &m_u32s, soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData,
                       regDesc, commonData, 5, screenData, m_screens.get()) { }
    T* getModule() { return &m_effectModule; }
};

typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeListNullType> > wnSonicSuperSonicKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnSonicSuperSonicKineticTransactor, wnSonicSuperSonicKineticTransactor, wnKineticTransactor,
                                soKineticTransactHelper>
    wnSonicSuperSonicKineticTransactorInfo;

// HYPOTHESIS: configuration values other than those the module map names (motion, inside event, anim cmd, effect, color
// blend, controller, kinetic).
class wnSonicSuperSonicAnimCmdModuleSubBuildConfig :
    public wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 10, 0, 0, 1, 10, 0, 0, 1, 10, 0, 0> {
public:
    enum { Unit5Enabled = 1 };
};

struct wnSonicSuperSonicModuleAccesserBuildConfig : public wnCommonBuildConfig {
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnSonicSuperSonicKineticEnergyTypeList,
                                                             wnSonicSuperSonicKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<17, 33, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<10, soMotionModuleImpl, 0, 1,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 20, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef soCameraModuleBuildConfig<soCameraModuleImpl> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnSonicSuperSonicAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef wnEffectModuleBuildConfig<1, 1, 0, 1, 5, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<5, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// WN_SONIC_SUPER_SONIC_GENERIC_BUILDER (opt-in, the article unit): BYTES, the generic wnWeaponBuilder<BC> template
// (WN_WEAPON_NO_COMMON_ENERGIES, WN_WEAPON_BUILDER_CTOR_NO_FLAG); this explicit specialization hoists a .bss anchor.
#ifndef WN_SONIC_SUPER_SONIC_GENERIC_BUILDER
// The native builder constructor is out of line (the article constructor calls it).
template <>
class wnWeaponBuilder<wnSonicSuperSonicModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSonicSuperSonicModuleAccesserBuildConfig> m_modules;
    u32 unk2CD0; // HYPOTHESIS: unread tail word of the builder (the article starts at 0x2CD4)
public:
#ifdef WN_WEAPON_BUILDER_CTOR_EXTERN // the article unit defines the constructor
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd);
#else
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd)
        __attribute__((never_inline)) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
        Weapon::initialize();
    }
#endif
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
    }
};
#endif
static_assert(sizeof(wnWeaponBuilder<wnSonicSuperSonicModuleAccesserBuildConfig>) == 0x2CD4, "Sonic weapon builder size");

#else
struct wnSonicSuperSonicModuleAccesserBuildConfig;
struct wnSonicSuperSonicData;

// Original RTTI identifies Weapon as the builder's sole base. The constructor
// installs its modules within this object; their detailed fields remain opaque.
template <>
class wnWeaponBuilder<wnSonicSuperSonicModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x2CD4 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnSonicSuperSonicModuleAccesserBuildConfig>) == 0x2CD4, "Sonic weapon builder size");
#endif

// HYPOTHESIS: original effect-list type spellings; caller and loop establish
// node, position, scale, entry stride and list layout.
struct wnSonicSuperSonicEffectEntry {
    int nodeId;
    Vec3f pos;
    float scale;
};
struct wnSonicSuperSonicEffectList {
    wnSonicSuperSonicEffectEntry* entries;
    u32 size;
};
static_assert(sizeof(wnSonicSuperSonicEffectEntry) == 0x14, "Super Sonic effect entry size");
static_assert(sizeof(wnSonicSuperSonicEffectList) == 8, "Super Sonic effect list size");

class wnSonicSuperSonic : public wnWeaponBuilder<wnSonicSuperSonicModuleAccesserBuildConfig> {
#ifdef WN_WEAPON_BUILDER
    const wnSonicSuperSonicParam* m_param;          // +0x2CD4
    wnSonicSuperSonicParamAccesser m_paramAccesser; // +0x2CD8
#else
    u32 unk2CD4;
    u8 unk2CD8[0x58];
#endif
public:
    // Built by the article pool of the fighter (the constructor lives in the article unit).
    wnSonicSuperSonic(s32 articleId, const wnArticleBuildInfo& info, const wnSonicSuperSonicData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnSonicSuperSonic() { }
#else
    virtual ~wnSonicSuperSonic();
#endif
    // HYPOTHESIS: relative source ordering of lr; f1 and integer registers
    // follow the original caller independently of this ordering.
    void activate(int founderTaskId, int resourceId, int team, Vec2f* pos,
                  float lr, SituationKind situation, wnSonicSuperSonicEffectList* effects);
    virtual void processUpdate();
    virtual void updatePosture(bool);
    virtual void notifyEventCollisionAttack(float, const soCollisionLog*, soModuleAccesser*);
    virtual bool notifyEventCollisionAttackCheck(u32);
    virtual void notifyEventChangeStatus(int, int, soStatusData*, soModuleAccesser*);
    static int convertSonicNode(int nodeId);
};
// BRINGUP static_assert(sizeof(wnSonicSuperSonic) == 0x2D30, "Super Sonic article size");
