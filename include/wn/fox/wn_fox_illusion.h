#pragma once

#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>
#include <wn/fox/wn_fox_illusion_util.h>

// Native constructor calls the zero-argument soStatusUniqProcess* vector constructor.
template <> class soArrayVector<soStatusUniqProcess*, 3> : public soArrayVectorAbstract<soStatusUniqProcess*> {
    s32 m_topIndex : sizeof(bit_width<3>) + 1;
    s32 m_lastIndex : sizeof(bit_width<3>) + 1;
    s32 m_size : sizeof(bit_width<3>) + 1;
    u32 m_isFull : 1;
    soStatusUniqProcess* m_elements[3];
public:
    soArrayVector();
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soStatusUniqProcess*& atFastAbstractSub(s32 index) const;
    virtual soStatusUniqProcess*& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};
template <> class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 3>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > {
public:
    soArrayVector<soStatusUniqProcess*, 3> m_array;
    soArraySelectHolder() : m_array() { }
    ~soArraySelectHolder() { }
    soArrayVector<soStatusUniqProcess*, 3>* get() { return &m_array; }
};

struct wnFoxIllusionParam { s32 unk0; s32 unk4; };
struct wnFoxIllusionData {
    s32 m_resGroupNo; // Native constructor reads the low byte only.
    u8 unk4[0x30]; // HYPOTHESIS: uninterpreted bytes before the pointer at +0x34.
    const wnFoxIllusionParam* m_param; // Native load from data+0x34.
};
class wnFoxIllusionParamAccesser : public wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0> {
public:
    explicit wnFoxIllusionParamAccesser(const wnFoxIllusionParam* param)
        : wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0>(32) {
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnFoxIllusionParamAccesser() { }
};
// Native stores these two addresses at accessor+0x20/+0x24; do not name the integers.

// Article capacities shared with the paired Falco config where constructor evidence agrees.
struct wnFoxIllusionKineticModuleBuildConfigNull { }; // HYPOTHESIS: local config spelling.
class wnFoxIllusionModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16,32,4,2,4,4,4,4,2,4,4,4,4,4,1,1,0,0,0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5,0,soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3,soMotionModuleImpl,0,0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0,soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter,1,0,soCollisionAttackModuleImpl,1,false,true>
        CollisionAttackModuleBuildConfig; // attack builder dtor fn_97_5648 is <0,1,0,...>.
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soCollisionReflectorModuleBuildConfigNull CollisionReflectorModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5,soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3,soGeneralWorkBuildConfig<2,2,1>,3,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1,3,0,0,1,3,0,0,1,3,0,0,1,3,0,0>
        AnimCmdModuleSubBuildConfig;
    typedef wnFoxIllusionKineticModuleBuildConfigNull KineticModuleBuildConfig;
};

// Fox selects the kinetic singleton without a storage slot between status and work.
// The other member selections retain the common article builder order.
extern char lbl_27_bss_530[];
template <>
class soModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig> : public utUnCopyable {
    typedef wnFoxIllusionModuleAccesserBuildConfig BC;
public:
    soInsideEventManageModuleBuilder<BC::InsideEventManageModuleBuildConfig, BC::InsideEventManageModuleTypes> unk194;
    soModuleAccesser m_moduleAccsr;
    soHeapModuleBuilder<BC::HeapModuleBuildConfig> m_heapModuleBuilder;
    soParamCustomizeModuleBuilder<BC::ParamCustomizeModuleBuildConfig> m_paramCustomizeModuleBuilder;
    soResourceModuleBuilder<BC::ResourceModuleBuildConfig> m_resourceModuleBuilder;
    soModelModuleBuilder<BC::ModelModuleBuildConfig> m_modelModuleBuilder;
    soMotionModuleBuilder<BC::MotionModuleBuildConfig> m_motionBuilder;
    soPostureModuleBuilder<BC::PostureModuleBuildConfig> m_postureModuleBuilder;
    soGroundModuleBuilder<BC::GroundModuleBuildConfig> m_groundModuleBuilder;
    soSituationModuleBuilder<BC::SituationModuleBuildConfig> m_situationModuleBuilder;
    soTeamModuleBuilder<BC::TeamModuleBuildConfig> m_teamBuilder;
    soCollisionAttackModuleBuilder<BC::CollisionAttackModuleBuildConfig> m_attackModuleBuilder;
    soCollisionHitModuleBuilder<BC::CollisionHitModuleBuildConfig> m_hitModuleBuilder;
    soCollisionShieldModuleBuilder<BC::CollisionShieldModuleBuildConfig> m_shieldModuleBuilder;
    soCollisionReflectorModuleBuilder<BC::CollisionReflectorModuleBuildConfig> m_reflectorModuleBuilder;
    ftNullCollisionAbsorberBuilder m_absorberBuilder; // the four bytes after the reflector builder
    soCollisionCatchModuleBuilder<BC::CollisionCatchModuleBuildConfig> m_collisionCatchModuleBuilder;
    BC::CollisionSearchModuleBuilder m_searchBuilder;
    soDamageModuleBuilder<BC::DamageModuleBuildConfig> m_damageModuleBuilder;
    soCatchModuleBuilder<BC::CatchModuleBuildConfig> m_catchModuleBuilder;
    soCaptureModuleBuilder<BC::CaptureModuleBuildConfig> m_captureModuleBuilder;
    soStopModuleBuilder<BC::StopModuleBuildConfig> m_stopModuleBuilder;
    soTurnModuleBuilder<BC::TurnModuleBuildConfig> m_turnModuleBuilder;
    soShakeModuleBuilder<BC::ShakeModuleBuildConfig> m_shakeModuleBuilder;
    soSoundModuleBuilder<BC::SoundModuleBuildConfig> m_soundModuleBuilder;
    soLinkModuleBuilder<BC::LinkModuleBuildConfig> m_linkModuleBuilder;
    struct EmptyVisibilitySelection { } m_visibilitySelection;
    soVisibilityModuleBuilder<BC::VisibilityModuleBuildConfig> m_visibilityModuleBuilder;
    soControllerModuleBuilder<BC::ControllerModuleBuildConfig> m_controllerModuleBuilder;
    soCameraModuleBuilder<BC::CameraModuleBuildConfig> m_cameraModuleBuilder;
    soWorkManageModuleBuilder<BC::WorkManageModuleBuildConfig> m_workManageModuleBuilder;
    soAnimCmdModuleBuilder<BC::AnimCmdModuleBuildConfig> m_animCmdBuilder;
    soStatusModuleBuilder<BC::StatusModuleBuildConfig> m_statusBuilder;
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder;
    BC::GenerateArticleManageModuleBuilder m_generateArticleBuilder;
    soEffectModuleBuilder<BC::EffectModuleBuildConfig> m_effectModuleBuilder;
    soComboModuleBuilder<BC::ComboModuleBuildConfig> m_comboModuleBuilder;
    soAreaModuleBuilder<BC::AreaModuleBuildConfig> m_areaBuilder;
    soPhysicsModuleBuilder<BC::PhysicsModuleBuildConfig> m_physicsModuleBuilder;
    soSlopeModuleBuilder<BC::SlopeModuleBuildConfig> m_slopeModuleBuilder;
    soShadowModuleBuilder<BC::ShadowModuleBuildConfig> m_shadowModuleBuilder;
    soItemManageModuleBuilder<BC::ItemManageModuleBuildConfig> m_itemManageModuleBuilder;
    soColorBlendModuleBuilder<BC::ColorBlendModuleBuildConfig> m_colorBlendModuleBuilder;
    soJostleModuleBuilder<BC::JostleModuleBuildConfig> m_jostleModuleBuilder;
    soAbnormalModuleBuilder<BC::AbnormalModuleBuildConfig> m_abnormalModuleBuilder;
    soSlowModuleBuilder<BC::SlowModuleBuildConfig> m_slowModuleBuilder;
    soReflectModuleBuilder<BC::ReflectModuleBuildConfig> m_reflectModuleBuilder;
    soGlowModuleBuilder<BC::GlowModuleBuildConfig> m_glowModuleBuilder;

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder() { }
    soModuleAccesser* getModuleAccesser() { return &m_moduleAccsr; }
};
static_assert(offsetof(soModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig>, m_statusBuilder) == 0x1334,
              "Fox status builder offset");
static_assert(offsetof(soModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig>, m_generalWorkBuilder) == 0x14FC,
              "Fox work follows status without kinetic storage");

// This full specialization is essential: generic wnWeaponBuilder owns two energy members and registers both.
template <> class wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig> m_modules; // starts at native +0xCC.
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info,
                    const wnModuleAccesserBuildData& bd)
        : Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true),
          m_modules(this, bd) {
        Weapon::initialize();
        // Native constructor has no energy constructors and no kinetic addEnergy calls.
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
    }
};

class wnFoxIllusion : public wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig> {
    // HYPOTHESIS: ownership of this unwritten interval may belong to an empty builder selection.
    u8 unk1DAC[4];
    const wnFoxIllusionParam* m_param; // native +0x1DB0.
    wnFoxIllusionParamAccesser m_paramAccesser; // native +0x1DB4.
public:
    wnFoxIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFoxIllusionData* data);
    virtual ~wnFoxIllusion() { }
    void activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3);
    virtual void processUpdate();
    virtual bool notifyEventCollisionAttackCheck(u32 unk0);
};
