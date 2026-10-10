#pragma once
#include <wn/weapon.h>
#include <ft/builder/ft_module_accesser_builder.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/team/so_team_impl.h>

// Zero-capacity selections own no array; alignment before the next real member
// accounts for their four-byte occupied interval in the shared weapon builder.
template <class V, class T>
class soArraySelectHolder<0, V, soArrayNull<T> > {
    soSingletonHolder<soArrayNull<T> > m_holder;
public:
    soArraySelectHolder() { }
    soArraySelectHolder(s32, s32) { }
    explicit soArraySelectHolder(s32) { }
    soArray<T>* get() { return &getNullArray<T>(); }
    ~soArraySelectHolder() { }
};
template <class V, class T>
class soArraySelectHolder<0, V, soSingletonHolder<soArrayNull<T> > > {
    soSingletonHolder<soArrayNull<T> > m_holder;
public:
    soArraySelectHolder() { }
    soArraySelectHolder(s32, s32) { }
    explicit soArraySelectHolder(s32) { }
    soArray<T>* get() { return soSingletonHolder<soArrayNull<T> >::getInstance(); }
    ~soArraySelectHolder() { }
};

template <> class soEventUnitWithWorkArea<soStatusEventObserver,16> : public soEventUnitImpl<soStatusEventObserver> {
    soInstanceManagerFullPropertyVector<soStatusEventObserver*,16> m_observerList;
public:
    soEventUnitWithWorkArea(s16,s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soAnimCmdEventObserver,32> : public soEventUnitImpl<soAnimCmdEventObserver> {
    soInstanceManagerFullPropertyVector<soAnimCmdEventObserver*,32> m_observerList;
public:
    soEventUnitWithWorkArea(s16,s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soLinkEventObserver,2> : public soEventUnitImpl<soLinkEventObserver> {
    soInstanceManagerFullPropertyVector<soLinkEventObserver*,2> m_observerList;
public:
    soEventUnitWithWorkArea(s16,s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soArticleEventObserver,2> : public soEventUnitImpl<soArticleEventObserver> {
    soInstanceManagerFullPropertyVector<soArticleEventObserver*,2> m_observerList;
public:
    soEventUnitWithWorkArea(s16,s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};
template <> class soEventUnitWithWorkArea<soModelEventObserver,5> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*,5> m_observerList;
public:
    soEventUnitWithWorkArea(s16,s16);
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const;
};

typedef soInsideEventManageModuleBuildConfig<16,32,4,2,4,4,4,5,2,4,4,4,4,4,1,1,0,0,0,wnInsideEventManageModuleTypes> wnSimpleInsideEventManageModuleBuildConfig;
static_assert(sizeof(soInsideEventManageModuleBuilder<wnSimpleInsideEventManageModuleBuildConfig,wnInsideEventManageModuleTypes>) == 0x804, "WN events");

FT_DOL_ARRAY_VECTOR(soModelNodeSetUp,5);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soTransitionTerm>,4);
FT_DOL_ARRAY_VECTOR(soLinkConnection,5);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soAnimCmdControlUnit>,6);

// Six-unit weapon manager retains its trailing flag.
template <>
class soInstanceManagerFullPropertyVector<soAnimCmdControlUnit, 6> : public soInstanceManagerFullProperty<soAnimCmdControlUnit> {
    soArrayVector<soInstanceUnitFullProperty<soAnimCmdControlUnit>, 6> m_arrayVector; // 0x10
    bool m_unk1;
public:
    soInstanceManagerFullPropertyVector(bool p1);
    ~soInstanceManagerFullPropertyVector() { }
    virtual soAnimCmdControlUnit& at(s32 id);
    virtual soAnimCmdControlUnit& atIndex(s32 idx);
    virtual s32 getId(s32 idx);
    virtual u32 size() const;
    virtual bool isContain(s32 id) const;
    virtual void erase(s32 id);
    virtual void clear();
    virtual void set(const soAnimCmdControlUnit& elm, s32 id);
    virtual s32 add(soAnimCmdControlUnit& elm, s32 id, soAttributeFlag attr, s16 p4);
    virtual u32 capacity();
    virtual soAnimCmdControlUnit& atIndexFast(s32 idx);
    virtual soInstanceUnitFullProperty<soAnimCmdControlUnit>& atUnitIndexFast(s32 idx);
    virtual s32 getIndex(s32 id) const;
    virtual void getAttributeArray(soAttributeFlag targetAttr, soArray<soAnimCmdControlUnit*>& arr);
    virtual soAttributeFlag getAttribute(s32 id) const;
    virtual void getPriorityArray(soArray<soAnimCmdControlUnit*>& arr);
};

// Native motion construction registers one group of four transition terms.
typedef soTransitionModuleBuildConfig<soTypeList<soIntToType<4>,soTypeListNullType> > wnSimpleMotionTransitionBuildConfig;
// HYPOTHESIS: source name of this build-config adapter; the selected components
// and all occupied offsets are independently verified.
struct wnSimpleMotionBuildConfig {
    typedef soMotionModuleImpl ModuleType;
    typedef wnSimpleMotionTransitionBuildConfig TransitionBuildConfig;
    enum { MotionDataNum=1,PartialAnimNum=0,OtherAnimNum=1 };
};
template <>
class soMotionModuleBuilder<wnSimpleMotionBuildConfig> {
public:
    soTransitionModuleBuilder<wnSimpleMotionTransitionBuildConfig> m_transition;
    soArraySelectHolder<0,soArrayVector<soPartialAnim,0>,soSingletonHolder<soArrayNull<soPartialAnim> > > m_partial;
    soArraySelectHolder<1,soArrayVector<soOtherAnim,1>,soSingletonHolder<soArrayNull<soOtherAnim> > > m_other;
    soSingletonHolder<soArrayNull<soTransitionTermPack> > m_termPacks;
    soArraySelectHolder<0,soArrayVector<u32,0>,soSingletonHolder<soArrayNull<u32> > > m_u32s;
    soArrayContractibleTable<const soMotionData> m_motionData;
    // Disabled motion cache: no per-animation objects are owned.
    struct EmptyCache { } m_cache;
    soMotionModuleImpl m_module;
    soMotionModuleBuilder(soModuleAccesser*,void*);
    ~soMotionModuleBuilder() { }
};
static_assert(sizeof(soMotionModuleBuilder<wnSimpleMotionBuildConfig>) == 0x248, "WN motion extent");
static_assert(sizeof(soModelModuleBuilder<soModelModuleBuildConfig<5,0,soModelModuleImpl> >) == 0x1E0, "WN model extent");

// Empty interpolation storage selects the shared null array.
template <>
class soPostureModuleBuilder<soPostureModuleBuildConfig<0,soPostureModuleSimple> > :
    public soArraySelectHolder<0,soArrayVector<soInterpolation<Vec3f>,0>,soArrayNull<soInterpolation<Vec3f> > > {
public:
    soPostureModuleSimple m_module;
    soPostureModuleBuilder(soModuleAccesser*,soEventObserverRegistrationDesc*);
    ~soPostureModuleBuilder() { }
};
static_assert(sizeof(soPostureModuleBuilder<soPostureModuleBuildConfig<0,soPostureModuleSimple> >) == 0x48, "WN posture extent");

typedef soTeamModuleBuildConfig<soTeamImpl,soTeamModuleImpl> wnSimpleTeamBuildConfig;
template <>
class soTeamModuleBuilder<wnSimpleTeamBuildConfig> {
public:
    soTeamImpl m_primary;
    soTeamImpl m_secondary;
    soTeamModuleImpl m_module;
    soTeamModuleBuilder(s32,soModuleAccesser*);
    ~soTeamModuleBuilder() { }
};
static_assert(sizeof(soTeamModuleBuilder<wnSimpleTeamBuildConfig>) == 0x64, "WN team extent");

// HYPOTHESIS: source spelling of these disabled module selections.
// Separate empty members account for the native byte slots, not padding arrays.
struct wnDisabledModuleBuilder { };
// Storage and base identity verified through construction, teardown and RTTI.
// Remaining shake virtual methods are not yet declared here; this view is used
// only as owned storage with an imported destructor.
class soShakeModule {
public:
    virtual ~soShakeModule() { }
};
class soShakeModuleSimple : public soShakeModule {
    soModuleAccesser* m_accesser;
public:
    virtual ~soShakeModuleSimple();
};
template <> class soShakeModuleBuilder<soShakeModuleBuildConfig<0,soShakeModuleSimple> > : public soArraySelectHolder<0,soArrayVector<soShakeTerm,0>,soArrayNull<soShakeTerm> > {
public: soShakeModuleSimple m_module;
};
static_assert(sizeof(soShakeModuleBuilder<soShakeModuleBuildConfig<0,soShakeModuleSimple> >)==0xC,"WN shake extent");
// HYPOTHESIS: visibility config's node-selection parameter is still unresolved.
struct wnSimpleVisibilityBuilder {
    soVisibilityModuleSimple m_module;
};
static_assert(sizeof(wnSimpleVisibilityBuilder)==0x14,"WN visibility extent");
template <> class soSlowModuleBuilder<soSlowModuleBuildConfig<soSlowModuleSimple> > {
public: soSlowModuleSimple m_module;
};
static_assert(sizeof(soSlowModuleBuilder<soSlowModuleBuildConfig<soSlowModuleSimple> >)==0x18,"WN slow extent");
// HYPOTHESIS: use a named prototype until the full effect config schema is recovered.
struct wnSimpleEffectBuilder {
    soAnimCmdFirstMember<soArrayVector<soEffectContinual,1> > m_continual;
    soArraySelectHolder<1,soArrayVector<soEffectTime,1>,soArrayNull<soEffectTime> > m_times;
    soArraySelectHolder<0,soArrayVector<efScreenHandle,0>,soArrayNull<efScreenHandle> > m_screens;
    soArrayVector<u32,1> m_u32s;
    soEffectModuleImpl m_module;
};
static_assert(sizeof(wnSimpleEffectBuilder)==0x19C,"WN effect extent");
static_assert(sizeof(soGeneralWorkBuilder<soGeneralWorkBuildConfig<7,6,1> >)==0x5C,"WN work extent");
// HYPOTHESIS: the four thread-selection configs are still unnamed.
// Native construction enables only thread2, with command table capacity2.
struct wnSimpleAnimCmdSubBuilder {
    wnDisabledModuleBuilder m_unit0,m_unit1;
    soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<2,2,2,0,0,1,0,8> > m_unit2;
    wnDisabledModuleBuilder m_unit3;
    ~wnSimpleAnimCmdSubBuilder() { }
};
static_assert(sizeof(wnSimpleAnimCmdSubBuilder)==0x134,"WN ACMD subbuilder extent");
class wnSimpleModuleAccesserBuildConfig { };
template <> class soModuleAccesserBuilder<wnSimpleModuleAccesserBuildConfig> {
public:
    soInsideEventManageModuleBuilder<wnSimpleInsideEventManageModuleBuildConfig,wnInsideEventManageModuleTypes> m_event;
    soModuleAccesser m_accesser;
    soHeapModuleBuilder<soHeapModuleBuildConfig<soHeapModuleImpl> > m_heap;
    soParamCustomizeModuleBuilder<soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> > m_param;
    soResourceModuleBuilder<soResourceModuleBuildConfig<0,soResourceIdAccesserImpl,soResourceModuleImpl> > m_resource;
    soModelModuleBuilder<soModelModuleBuildConfig<5,0,soModelModuleImpl> > m_model;
    soMotionModuleBuilder<wnSimpleMotionBuildConfig> m_motion;
    soPostureModuleBuilder<soPostureModuleBuildConfig<0,soPostureModuleSimple> > m_posture;
    wnDisabledModuleBuilder m_ground, m_situation;
    soTeamModuleBuilder<wnSimpleTeamBuildConfig> m_team;
    wnDisabledModuleBuilder m_attack,m_hit,m_shield,m_reflector,m_catchCollision,m_search,m_damage,m_catch,m_capture,m_stop,m_turn;
    soShakeModuleBuilder<soShakeModuleBuildConfig<0,soShakeModuleSimple> > m_shake;
    wnDisabledModuleBuilder m_sound;
    soLinkModuleBuilder<soLinkModuleBuildConfigCap<5,soLinkModuleImpl> > m_link;
    // HYPOTHESIS: disabled selection type; extent and ownership are verified.
    wnDisabledModuleBuilder m_visibilitySelection;
    wnSimpleVisibilityBuilder m_visibility;
    wnDisabledModuleBuilder m_controller,m_camera;
    soWorkManageModuleBuilder<soWorkManageModuleBuildConfig<soWorkManageModuleImpl> > m_workManage;
    soAnimCmdModuleBuilder<soAnimCmdModuleBuildConfig<6,soAnimCmdModuleImpl> > m_animCmd;
    wnDisabledModuleBuilder m_status,m_kinetic;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<7,6,1> > m_generalWork;
    wnDisabledModuleBuilder m_article;
    wnSimpleEffectBuilder m_effect;
    wnDisabledModuleBuilder m_combo,m_area;
    soPhysicsModuleBuilder<soPhysicsModuleBuildConfigCap<0,soPhysicsModuleImpl> > m_physics;
    wnDisabledModuleBuilder m_slope;
    soShadowModuleBuilder<soShadowModuleBuildConfig<soShadowModuleImpl> > m_shadow;
    wnDisabledModuleBuilder m_item;
    soColorBlendModuleBuilder<soColorBlendModuleBuildConfig<1,1,soColorBlendModuleImpl> > m_color;
    wnDisabledModuleBuilder m_jostle,m_abnormal;
    soSlowModuleBuilder<soSlowModuleBuildConfig<soSlowModuleSimple> > m_slow;
    wnDisabledModuleBuilder m_glow;
    ~soModuleAccesserBuilder() { }
};
template <class BC> class wnModuleAccesserBuilder;
template <> class wnModuleAccesserBuilder<wnSimpleModuleAccesserBuildConfig> : public soModuleAccesserBuilder<wnSimpleModuleAccesserBuildConfig> {
public:
    wnSimpleAnimCmdSubBuilder m_animCmdSub;
    ~wnModuleAccesserBuilder() { }
};
template <class BC> class wnWeaponBuilder;
template <> class wnWeaponBuilder<wnSimpleModuleAccesserBuildConfig> : public Weapon {
public:
    wnModuleAccesserBuilder<wnSimpleModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: flag meanings unresolved. Native constructor writes +169A.
    u16 unk1698;
    bool unk169A;
    u8 unk169B;
    virtual ~wnWeaponBuilder() { }
};
static_assert(sizeof(wnWeaponBuilder<wnSimpleModuleAccesserBuildConfig>)==0x169C,"WN weapon builder extent");

static_assert(sizeof(soModuleAccesserBuilder<wnSimpleModuleAccesserBuildConfig>)==0x1498,"Simple weapon module storage");
