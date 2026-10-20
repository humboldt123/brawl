#pragma once

// Generic weapon ("article") builder: wnWeaponBuilder<BC> owns a wnModuleAccesserBuilder<BC> (the shared
// soModuleAccesserBuilder<BC> module storage plus the anim cmd thread units of an article) and the two common kinetic
// energies. Each article supplies a build config BC (see wnCommonBuildConfig for the null defaults); its translation unit
// builds everything inline in the article constructor.
//
// Include this header first in an article unit, after defining WN_WEAPON_BUILDER and FT_REL_LINK_EXTERN (the sora_melee
// constructors of the event units and instance managers are called, not instantiated).

#ifndef WN_WEAPON_BUILDER
#error define WN_WEAPON_BUILDER before including wn_module_accesser_builder.h
#endif

// The fighter builder scaffolding brings the shared module builders in their required order.
#include <ft/ft_fighter_builder.h>
#include <wn/weapon.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/team/so_team_impl.h>
#include <so/damage/so_damage_transactor_null.h>
#include <so/damage/so_damage_effector_null.h>
#include <so/kinetic/so_kinetic_mediator_null.h>
#include <so/article/so_article_mediator_null.h>
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <wn/wn_article_build_info.h>
#include <wn/wn_module_accesser_build_data.h>

////////////////////////////////////////
// sora_melee instances used by the article builders (declared only: the REL calls them)
////////////////////////////////////////

FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 5);
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 2);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soTransitionTerm>, 4);
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 2);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 2);
FT_DOL_ARRAY_VECTOR(soLinkConnection, 5);
#ifndef WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 2);
#endif
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soAnimCmdControlUnit>, 6);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soKineticEnergy*>, 8);

// An article whose status unique process vector is built by the sora_melee constructor without a size argument: declares the
// vector of N entries and the status holder that default-constructs it (the singleton null-array flavour).
#define WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(N)                                                                              \
    template <>                                                                                                            \
    class soArrayVector<soStatusUniqProcess*, N> : public soArrayVectorAbstract<soStatusUniqProcess*> {                    \
        s32 m_topIndex : sizeof(bit_width<N>) + 1;                                                                        \
        s32 m_lastIndex : sizeof(bit_width<N>) + 1;                                                                       \
        s32 m_size : sizeof(bit_width<N>) + 1;                                                                            \
        u32 m_isFull : 1;                                                                                                  \
        soStatusUniqProcess* m_elements[N];                                                                                \
    public:                                                                                                                \
        soArrayVector();                                                                                                   \
        virtual s32 size() const;                                                                                          \
        virtual ~soArrayVector();                                                                                          \
        virtual s32 capacity() const;                                                                                      \
        virtual bool isFull() const;                                                                                       \
        virtual soStatusUniqProcess*& atFastAbstractSub(s32 index) const;                                                  \
        virtual soStatusUniqProcess*& getArrayValueConst(s32 index);                                                       \
        virtual s32 getTopIndex() const;                                                                                   \
        virtual s32 getLastIndex() const;                                                                                  \
        virtual void setSize(s32 size);                                                                                    \
        virtual void setTopIndex(s32 topIndex);                                                                            \
        virtual void setLastIndex(s32 lastIndex);                                                                          \
        virtual void onFull();                                                                                             \
        virtual void offFull();                                                                                            \
    };                                                                                                                     \
    template <>                                                                                                            \
    class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, N>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > { \
    public:                                                                                                                \
        soArrayVector<soStatusUniqProcess*, N> m_array;                                                                    \
        soArraySelectHolder() : m_array() { }                                                                              \
        ~soArraySelectHolder() { }                                                                                         \
        soArrayVector<soStatusUniqProcess*, N>* get() { return &m_array; }                                                 \
    }

// Same, for the vector whose sora_melee constructor is called with a second, unused argument (the caller loads 0 into r4
// before the call; the constructor ignores it): the status vector of the Jet Hammer.
#define WN_STATUS_PROC_VECTOR_ARG_CTOR(N)                                                                              \
    template <>                                                                                                            \
    class soArrayVector<soStatusUniqProcess*, N> : public soArrayVectorAbstract<soStatusUniqProcess*> {                    \
        s32 m_topIndex : sizeof(bit_width<N>) + 1;                                                                        \
        s32 m_lastIndex : sizeof(bit_width<N>) + 1;                                                                       \
        s32 m_size : sizeof(bit_width<N>) + 1;                                                                            \
        u32 m_isFull : 1;                                                                                                  \
        soStatusUniqProcess* m_elements[N];                                                                                \
    public:                                                                                                                \
        soArrayVector(s32);                                                                                                \
        virtual s32 size() const;                                                                                          \
        virtual ~soArrayVector();                                                                                          \
        virtual s32 capacity() const;                                                                                      \
        virtual bool isFull() const;                                                                                       \
        virtual soStatusUniqProcess*& atFastAbstractSub(s32 index) const;                                                  \
        virtual soStatusUniqProcess*& getArrayValueConst(s32 index);                                                       \
        virtual s32 getTopIndex() const;                                                                                   \
        virtual s32 getLastIndex() const;                                                                                  \
        virtual void setSize(s32 size);                                                                                    \
        virtual void setTopIndex(s32 topIndex);                                                                            \
        virtual void setLastIndex(s32 lastIndex);                                                                          \
        virtual void onFull();                                                                                             \
        virtual void offFull();                                                                                            \
    };                                                                                                                     \
    template <>                                                                                                            \
    class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, N>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > { \
    public:                                                                                                                \
        soArrayVector<soStatusUniqProcess*, N> m_array;                                                                    \
        soArraySelectHolder() : m_array(0) { }                                                                              \
        ~soArraySelectHolder() { }                                                                                         \
        soArrayVector<soStatusUniqProcess*, N>* get() { return &m_array; }                                                 \
    }

#ifdef WN_SHARED_ARRAYS_1
// These article builders call sora_melee-owned constructors and virtual methods.
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 1);
// The status pointer vector uses the no-argument constructor, not the size overload.
template <>
class soArrayVector<soStatusUniqProcess*, 1> : public soArrayVectorAbstract<soStatusUniqProcess*> {
    s32 m_topIndex : sizeof(bit_width<1>) + 1;
    s32 m_lastIndex : sizeof(bit_width<1>) + 1;
    s32 m_size : sizeof(bit_width<1>) + 1;
    u32 m_isFull : 1;
    soStatusUniqProcess* m_elements[1];
public:
    soArrayVector(s32);
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
// Its owning status holder passes the same ignored argument.
template <>
class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soArrayNull<soStatusUniqProcess*> > {
public:
    soArrayVector<soStatusUniqProcess*, 1> m_array;
    soArraySelectHolder() : m_array(0) { }
    ~soArraySelectHolder() { }
    soArrayVector<soStatusUniqProcess*, 1>* get() { return &m_array; }
};
#ifdef WN_ARTICLE_NATIVE_BUILDERS
// These article builders use the singleton null-array type in their status holder.
template <>
class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > {
public:
    soArrayVector<soStatusUniqProcess*, 1> m_array;
    soArraySelectHolder() : m_array(0) { }
    ~soArraySelectHolder() { }
    soArrayVector<soStatusUniqProcess*, 1>* get() { return &m_array; }
};
#endif
static_assert(sizeof(soArrayVector<soCollisionAttackPart, 1>) == 0x9C, "Oil attack part vector size");
static_assert(sizeof(soArrayVector<soStatusUniqProcess*, 1>) == 0x10, "Oil status pointer vector size");
#endif

#ifdef WN_OCTOPUS_ARRAYS
// Octopus imports these constructors and virtual methods from sora_melee.
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 8);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 4);
template <>
class soArrayVector<soStatusUniqProcess*, 4> : public soArrayVectorAbstract<soStatusUniqProcess*> {
    s32 m_topIndex : sizeof(bit_width<4>) + 1;
    s32 m_lastIndex : sizeof(bit_width<4>) + 1;
    s32 m_size : sizeof(bit_width<4>) + 1;
    u32 m_isFull : 1;
    soStatusUniqProcess* m_elements[4];
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
template <>
class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 4>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > {
public:
    soArrayVector<soStatusUniqProcess*, 4> m_array;
    soArraySelectHolder() : m_array() { }
    ~soArraySelectHolder() { }
    soArrayVector<soStatusUniqProcess*, 4>* get() { return &m_array; }
};
static_assert(sizeof(soArrayVector<soStatusUniqProcess*, 4>) == 0x1C, "Octopus status pointer vector extent");
#endif

// The null arrays (soArrayNull<T>) are sora_melee instances: with WN_WEAPON_BUILDER so/so_array.h declares their
// constructor and destructor only.

// The zero-capacity absolute attack array: constructed from a default element (the sora_melee constructor ignores it).
template <>
class soArrayNull<soCollisionAttackAbsolute> : public soArray<soCollisionAttackAbsolute> {
public:
    soArrayNull(s32 size, const soCollisionAttackAbsolute& element);
    virtual bool isNull() const;
    virtual soCollisionAttackAbsolute& at(s32 index);
    virtual const soCollisionAttackAbsolute& at(s32 index) const;
    virtual s32 size() const;
    virtual ~soArrayNull();
    virtual void shift();
    virtual void pop();
    virtual void clear();
    virtual void unshift(const soCollisionAttackAbsolute&);
    virtual void push(const soCollisionAttackAbsolute&);
    virtual void insert(s32, const soCollisionAttackAbsolute&);
    virtual void erase(s32);
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual void set(s32 startingIndex, const soCollisionAttackAbsolute& element, s32 numIndicesToSet);
};

// Instance managers constructed in sora_melee.
#define WN_DOL_INSTANCE_MANAGER(T, C)                                                                  \
    template <>                                                                                        \
    class soInstanceManagerFullPropertyVector<T, C> : public soInstanceManagerFullProperty<T> {        \
        soArrayVector<soInstanceUnitFullProperty<T>, C> m_arrayVector;                                 \
        bool m_unk1;                                                                                   \
    public:                                                                                            \
        soInstanceManagerFullPropertyVector(bool p1);                                                  \
        ~soInstanceManagerFullPropertyVector() { }                                                     \
        virtual T& at(s32 id);                                                                         \
        virtual T& atIndex(s32 idx);                                                                   \
        virtual s32 getId(s32 idx);                                                                    \
        virtual u32 size() const;                                                                      \
        virtual bool isContain(s32 id) const;                                                          \
        virtual void erase(s32 id);                                                                    \
        virtual void clear();                                                                          \
        virtual void set(const T& elm, s32 id);                                                        \
        virtual s32 add(T& elm, s32 id, soAttributeFlag attr, s16 p4);                                 \
        virtual u32 capacity();                                                                        \
        virtual T& atIndexFast(s32 idx);                                                               \
        virtual soInstanceUnitFullProperty<T>& atUnitIndexFast(s32 idx);                               \
        virtual s32 getIndex(s32 id) const;                                                            \
        virtual void getAttributeArray(soAttributeFlag targetAttr, soArray<T*>& arr);                  \
        virtual soAttributeFlag getAttribute(s32 id) const;                                            \
        virtual void getPriorityArray(soArray<T*>& arr);                                               \
    }

WN_DOL_INSTANCE_MANAGER(soAnimCmdControlUnit, 6);
typedef soKineticEnergy* wnKineticEnergyPtr;
WN_DOL_INSTANCE_MANAGER(wnKineticEnergyPtr, 8);
WN_DOL_INSTANCE_MANAGER(wnKineticEnergyPtr, 2);

// sora_melee modules of the articles (constructor and destructor in sora_melee; the REL only needs size and calls).
FT_DOL_POLY_BEGIN(soReflectModuleImpl, 0x40);
    soReflectModuleImpl(soModuleAccesser* acc, int kind);
FT_DOL_POLY_END;
FT_DOL_POLY_BEGIN(soGlowModuleSimple, 0x1C);
    soGlowModuleSimple(soModuleAccesser* acc);
FT_DOL_POLY_END;
FT_DOL_POLY_BEGIN(soShakeModuleSimple, 0x8);
    soShakeModuleSimple(soModuleAccesser* acc, soArray<soShakeTerm>* terms, void* shakeData);
FT_DOL_POLY_END;
// The weapon kinetic module (soKineticModuleImpl plus a status observer; constructed in sora_melee).
#ifdef WN_ARTICLE_NATIVE_BUILDERS
#include <wn/wn_kinetic_module_impl.h>
#else
FT_DOL_POLY_BEGIN(wnKineticModuleImpl, 0x30);
    wnKineticModuleImpl(soModuleAccesser* acc, soInstanceManagerFullProperty<soKineticEnergy*>* energies, soKineticMediator* mediator);
FT_DOL_POLY_END;
#endif

// Null module singletons of sora_melee (the null builders hand these to the module accesser).
extern char lbl_27_bss_3BC[];  // collision catch
extern char lbl_27_bss_3F4[];  // catch
extern char lbl_27_bss_404[];  // capture
extern char lbl_27_bss_424[];  // turn
extern char lbl_27_bss_47C[];  // controller
extern char lbl_27_bss_48C[];  // camera
extern char lbl_27_bss_170[];  // general term decide
extern char lbl_27_bss_188[];  // switch decide
extern char lbl_27_bss_31C[];  // motion
extern char lbl_27_bss_D7C[];  // null transition module
extern char lbl_27_bss_584[];  // combo
extern char lbl_27_bss_598[];  // area
extern char lbl_27_bss_5EC[];  // physics
extern char lbl_27_bss_604[];  // slope
extern char lbl_27_bss_614[];  // shadow
extern char lbl_27_bss_628[];  // item manage
extern char lbl_27_bss_640[];  // color blend
extern char lbl_27_bss_654[];  // jostle
extern char lbl_27_bss_66C[];  // abnormal
extern char lbl_27_bss_9BC[];  // null transition term pack array
extern char lbl_27_bss_DBC[];  // null motion anim obj cache module

////////////////////////////////////////
// Inside event manage module of an article: the gimmick, turn and catch collision units are disabled (their slots are
// empty and get no manage id).
////////////////////////////////////////

template <typename B>
class soInsideEventManageModuleBuilder<B, wnInsideEventManageModuleTypes> {
    typedef wnInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(m_module.getManageId(), B::StatusEvtId),
        m_animCmd(m_module.getManageId(), B::AnimCmdEvtId),
        m_damage(m_module.getManageId(), B::DamageEvtId),
        m_link(m_module.getManageId(), B::LinkEvtId),
        m_situation(m_module.getManageId(), B::SituationEvtId),
        m_collisionHit(m_module.getManageId(), B::CollisionHitEvtId),
        m_collisionAttack(m_module.getManageId(), B::CollisionAttackEvtId),
        m_model(m_module.getManageId(), B::ModelEvtId),
        m_article(m_module.getManageId(), B::ArticleEvtId),
        m_capture(m_module.getManageId(), B::CaptureEvtId),
        m_collisionShield(m_module.getManageId(), B::CollisionShieldEvtId),
        m_collisionReflector(m_module.getManageId(), B::CollisionReflectorEvtId),
        m_collisionAbsorber(m_module.getManageId(), B::CollisionAbsorberEvtId),
        m_collisionSearch(m_module.getManageId(), B::CollisionSearchEvtId),
        m_itemManage(m_module.getManageId(), B::ItemManageEvtId),
        m_motion(m_module.getManageId(), B::MotionEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};

// Articles with a turn event unit (Ness's PK Starstorm): the units without capacity are not built at all (no manage id is asked
// for them). HYPOTHESIS: the source spelling of the selection and of the units that are absent.
struct wnTurnInsideEventManageModuleTypes {
    enum {
        hasStatus = true,
        hasAnimCmd = true,
        hasDamage = true,
        hasLink = true,
        hasSituation = true,
        hasCollisionHit = true,
        hasCollisionAttack = true,
        hasModel = true,
        hasArticle = true,
        hasCapture = true,
        hasCollisionShield = true,
        hasCollisionReflector = true,
        hasCollisionAbsorber = true,
        hasCollisionSearch = true,
        hasItemManage = true,
        hasMotion = true,
        hasGimmick = true,
        hasTurn = true,
        hasCollisionCatch = true,
    };
};

template <typename B>
class soInsideEventManageModuleBuilder<B, wnTurnInsideEventManageModuleTypes> {
    typedef wnTurnInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(m_module.getManageId(), B::StatusEvtId),
        m_animCmd(m_module.getManageId(), B::AnimCmdEvtId),
        m_damage(m_module.getManageId(), B::DamageEvtId),
        m_link(m_module.getManageId(), B::LinkEvtId),
        m_situation(m_module.getManageId(), B::SituationEvtId),
        m_collisionAttack(m_module.getManageId(), B::CollisionAttackEvtId),
        m_model(m_module.getManageId(), B::ModelEvtId),
        m_article(m_module.getManageId(), B::ArticleEvtId),
        m_collisionShield(m_module.getManageId(), B::CollisionShieldEvtId),
        m_turn(m_module.getManageId(), B::TurnEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};

////////////////////////////////////////
// Null module builders: an empty slot (one byte in the builder) whose module is the shared null module. Their constructors
// take the same arguments as the real builders, so the build data queries still happen.
////////////////////////////////////////

// HYPOTHESIS: source names of the null build configs.
struct soCollisionShieldModuleBuildConfigNull { };
struct soCollisionReflectorModuleBuildConfigNull { };
struct soCollisionCatchModuleBuildConfigNull { };
struct soGroundModuleBuildConfigNull { }; // HYPOTHESIS: source spelling of the null configuration.
struct soCollisionHitModuleBuildConfigNull { }; // HYPOTHESIS: source spelling of the null configuration.
struct soDamageModuleBuildConfigNull { }; // HYPOTHESIS: source spelling of the null configuration.
struct soCatchModuleBuildConfigNull { };
struct soCaptureModuleBuildConfigNull { };
struct soTurnModuleBuildConfigNull { };
struct soControllerModuleBuildConfigNull { };
struct soCameraModuleBuildConfigNull { };
// Articles without animation (the motion module is the shared null one). HYPOTHESIS: source spelling of the null configuration.
struct soMotionModuleBuildConfigNull { };
struct soComboModuleBuildConfigNull { };
struct soAreaModuleBuildConfigNull { };
struct soPhysicsModuleBuildConfigNull { };
struct soSlopeModuleBuildConfigNull { };
struct soShadowModuleBuildConfigNull { };
struct soItemManageModuleBuildConfigNull { };
struct soColorBlendModuleBuildConfigNull { };
struct soJostleModuleBuildConfigNull { };
struct soAbnormalModuleBuildConfigNull { };
struct soSituationModuleBuildConfigNull { }; // HYPOTHESIS: source spelling of the null configuration.
struct soStopModuleBuildConfigNull { };       // HYPOTHESIS: null stop module (fireball family, map 0x2da26c).
struct soVisibilityModuleBuildConfigNull { }; // HYPOTHESIS: source spelling of the null configuration.

template <>
class soCollisionShieldModuleBuilder<soCollisionShieldModuleBuildConfigNull> {
public:
    soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category) { }
    soCollisionShieldModule* getModule() { return (soCollisionShieldModule*)g_soCollisionAbsorberModuleNull; }
};
// As in the fighter builders the absorber selection is folded into the reflector slot: two empty selections.
// HYPOTHESIS: the source has a separate (empty) absorber builder member; the extent is verified by the next member.
struct soCollisionAbsorberModuleBuilderNull { };
template <>
class soCollisionReflectorModuleBuilder<soCollisionReflectorModuleBuildConfigNull> {
    struct Empty { } m_reflector;
public:
    soCollisionReflectorModuleBuilder(soModuleAccesser*, int, gfTask::Category) { }
    soCollisionShieldModule* getModule() { return (soCollisionShieldModule*)g_soCollisionAbsorberModuleNull; }
};
template <>
struct ftReflectorBase<soCollisionReflectorModuleBuildConfigNull> {
    static soCollisionShieldModule* getAbsorber(soCollisionReflectorModuleBuilder<soCollisionReflectorModuleBuildConfigNull>*) {
        return (soCollisionShieldModule*)g_soCollisionAbsorberModuleNull;
    }
};
template <>
class soCollisionCatchModuleBuilder<soCollisionCatchModuleBuildConfigNull> {
public:
    soCollisionCatchModuleBuilder(soModuleAccesser*, int, gfTask::Category, soEventObserverRegistrationDesc*) { }
    void* getModule() { return lbl_27_bss_3BC; }
};
// These shared instances are selected directly by the article enumeration.
extern char lbl_27_bss_344[];
extern char lbl_27_bss_398[];
template <>
class soGroundModuleBuilder<soGroundModuleBuildConfigNull> {
public:
    soGroundModuleBuilder(soModuleAccesser*, soGroundConditionChecker*) { }
    soGroundModule* getModule() { return (soGroundModule*)lbl_27_bss_344; }
};
template <>
class soCollisionHitModuleBuilder<soCollisionHitModuleBuildConfigNull> {
public:
    soCollisionHitModuleBuilder(soModuleAccesser*, int, gfTask::Category, soEventObserverRegistrationDesc*) { }
    soCollisionHitModule* getModule() { return (soCollisionHitModule*)lbl_27_bss_398; }
};

// Oil's module enumeration selects the shared null damage instance.
extern char lbl_27_bss_3DC[];
template <>
class soDamageModuleBuilder<soDamageModuleBuildConfigNull> {
public:
    soDamageModuleBuilder(soModuleAccesser*, soEventObserverRegistrationDesc*) { }
    soDamageModule* getModule() { return (soDamageModule*)lbl_27_bss_3DC; }
};

template <>
class soCatchModuleBuilder<soCatchModuleBuildConfigNull> {
public:
    soCatchModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return lbl_27_bss_3F4; }
};
template <>
class soCaptureModuleBuilder<soCaptureModuleBuildConfigNull> {
public:
    soCaptureModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return lbl_27_bss_404; }
};
template <>
class soTurnModuleBuilder<soTurnModuleBuildConfigNull> {
public:
    soTurnModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return lbl_27_bss_424; }
};
template <>
class soControllerModuleBuilder<soControllerModuleBuildConfigNull> {
public:
    soControllerModuleBuilder(soModuleAccesser*, s16) { }
    soControllerModule* getModule() { return (soControllerModule*)lbl_27_bss_47C; }
};
// Controller of an article that reads its owner's controller (the Pikmin articles): no controller or clatter slots of its own.
// HYPOTHESIS: the source spelling is soControllerModuleBuildConfig<Module, Controllers, Clatters> (map name); the fighters'
// one-argument placeholder owns that template name.
template <typename T, s32 Controllers, s32 Clatters>
class wnControllerModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename T>
class soControllerModuleBuilder<wnControllerModuleBuildConfig<T, 0, 0> > {
    soSingletonHolder<soArrayNull<soControllerImpl> > m_controllers;
    soSingletonHolder<soArrayNull<soControllerClatter> > m_clatters;
    T m_controllerModule;
public:
    soControllerModuleBuilder(soModuleAccesser* acc, s16 manageId) :
        m_controllerModule(acc, manageId, soSingletonHolder<soArrayNull<soControllerImpl> >::getInstance(),
                           soSingletonHolder<soArrayNull<soControllerClatter> >::getInstance()) { }
    T* getModule() { return &m_controllerModule; }
};
template <>
class soCameraModuleBuilder<soCameraModuleBuildConfigNull> {
public:
    soCameraModuleBuilder(soModuleAccesser*, soSet<soCameraRange>*, soSet<soCameraClipSphere>*, soEventObserverRegistrationDesc*) { }
    soCameraModule* getModule() { return (soCameraModule*)lbl_27_bss_48C; }
};
template <>
class soMotionModuleBuilder<soMotionModuleBuildConfigNull> {
    u8 unk0[4]; // HYPOTHESIS: the null selection owns a few bytes (the next builder starts a word later)
public:
    soMotionModuleBuilder(soModuleAccesser*, void*) { }
    soMotionModule* getModule() { return (soMotionModule*)lbl_27_bss_31C; }
};
template <>
class soComboModuleBuilder<soComboModuleBuildConfigNull> {
public:
    soComboModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return lbl_27_bss_584; }
};
template <>
class soAreaModuleBuilder<soAreaModuleBuildConfigNull> {
    // HYPOTHESIS: the null territory and target search builders (which use the shared null modules) follow the area builder;
    // only their storage (one byte each) is visible in the layout.
    u8 unkTerritory;
    u8 unkTargetSearch;
public:
    soAreaModuleBuilder(soModuleAccesser*, u8, soEventObserverRegistrationDesc*) { }
    void* getModule() { return lbl_27_bss_598; }
};
template <>
class soPhysicsModuleBuilder<soPhysicsModuleBuildConfigNull> {
public:
    soPhysicsModuleBuilder(soModuleAccesser*, void*) { }
    void* getModule() { return lbl_27_bss_5EC; }
};
template <>
class soSlopeModuleBuilder<soSlopeModuleBuildConfigNull> {
public:
    soSlopeModuleBuilder(soModuleAccesser*, float) { }
    void* getModule() { return lbl_27_bss_604; }
};
template <>
class soShadowModuleBuilder<soShadowModuleBuildConfigNull> {
public:
    soShadowModuleBuilder(soModuleAccesser*) { }
    soShadowModule* getModule() { return (soShadowModule*)lbl_27_bss_614; }
};
template <>
class soItemManageModuleBuilder<soItemManageModuleBuildConfigNull> {
public:
    soItemManageModuleBuilder(soModuleAccesser*, void*) { }
    soItemManageModule* getModule() { return (soItemManageModule*)lbl_27_bss_628; }
};
template <>
class soColorBlendModuleBuilder<soColorBlendModuleBuildConfigNull> {
public:
    soColorBlendModuleBuilder(soModuleAccesser*) { }
    soColorBlendModule* getModule() { return (soColorBlendModule*)lbl_27_bss_640; }
};
template <>
class soJostleModuleBuilder<soJostleModuleBuildConfigNull> {
public:
    soJostleModuleBuilder(soModuleAccesser*, void*) { }
    void* getModule() { return lbl_27_bss_654; }
};
extern char lbl_27_bss_35C[]; // situation
extern char lbl_27_bss_46C[]; // visibility
template <>
class soSituationModuleBuilder<soSituationModuleBuildConfigNull> {
public:
    soSituationModuleBuilder(s16, soModuleAccesser*, soEventObserverRegistrationDesc*) { }
    soSituationModule* getModule() { return (soSituationModule*)lbl_27_bss_35C; }
};
template <>
class soVisibilityModuleBuilder<soVisibilityModuleBuildConfigNull> {
    u8 unk0[4]; // HYPOTHESIS: the null selection owns a few bytes (the next builder starts a word later)
public:
    soVisibilityModuleBuilder(soModuleAccesser*, soVisibilityData*) { }
    soVisibilityModule* getModule() { return (soVisibilityModule*)lbl_27_bss_46C; }
};
extern char lbl_27_bss_414[]; // stop
template <>
class soStopModuleBuilder<soStopModuleBuildConfigNull> {
public:
    soStopModuleBuilder(soModuleAccesser*) { }
    soStopModule* getModule() { return (soStopModule*)lbl_27_bss_414; }
};
template <>
class soAbnormalModuleBuilder<soAbnormalModuleBuildConfigNull> {
public:
    soAbnormalModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return lbl_27_bss_66C; }
};

////////////////////////////////////////
// Module builders of the articles
////////////////////////////////////////

// Parameter customization: the plain module (no per-fighter customization).
template <>
class soParamCustomizeModuleBuilder<soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> > {
    soParamCustomizeModuleImpl m_paramCustomizeModule;
public:
    soParamCustomizeModuleBuilder(soModuleAccesser*) { }
};

// A motion module without transitions uses the shared null transition module of sora_melee.
struct soTransitionModuleBuildConfigNull { };
template <>
class soTransitionModuleBuilder<soTransitionModuleBuildConfigNull> {
public:
    soTransitionModuleBuilder() { }
    soTransitionModule* getModule() { return (soTransitionModule*)lbl_27_bss_D7C; }
};

// Motion without partial / other animations or an anim obj cache: the null arrays come from singletons, the null cache
// module is sora_melee's.
class soMotionAnimObjCacheModuleNull;

template <u32 DataNum, typename T, typename TransitionConfig>
class soMotionModuleBuilder<soMotionModuleBuildConfig<DataNum, T, 0, 0, TransitionConfig,
                                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > > {
    soTransitionModuleBuilder<TransitionConfig> m_transitionBuilder;  // +0
    soSingletonHolder<soArrayNull<soPartialAnim> > m_partialAnims;
    soSingletonHolder<soArrayNull<soOtherAnim> > m_otherAnims;
    soSingletonHolder<soArrayNull<soTransitionTermPack> > m_termPacks;
    soSingletonHolder<soArrayNull<u32> > m_u32s;
    soArrayContractibleTable<soMotionDataConst> m_motionData;         // +0x88
    soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> m_cache;
    T m_module;                                                       // +0x9C
public:
    soMotionModuleBuilder(soModuleAccesser* acc, void* motionData) :
        m_transitionBuilder(),
        m_motionData((const soMotionData*)motionData, DataNum),
        m_module(acc, &m_motionData, m_transitionBuilder.getModule(), soSingletonHolder<soArrayNull<soPartialAnim> >::getInstance(),
                 soSingletonHolder<soArrayNull<soOtherAnim> >::getInstance(), (soArray<soTransitionTermPack>*)lbl_27_bss_9BC,
                 soSingletonHolder<soArrayNull<u32> >::getInstance(),
                 static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), false,
                 (soMotionAnimObjCacheModule*)lbl_27_bss_DBC) {
        acc->getModelModule().isNull();
    }
    ~soMotionModuleBuilder() { }
    soMotionModule* getModule() { return &m_module; }
};

// Articles with one other-animation slot still use the shared null cache.
template <u32 DataNum, typename T, typename TransitionConfig>
class soMotionModuleBuilder<soMotionModuleBuildConfig<DataNum, T, 0, 1, TransitionConfig,
                                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > > {
    soTransitionModuleBuilder<TransitionConfig> m_transitionBuilder;
    soSingletonHolder<soArrayNull<soPartialAnim> > m_partialAnims;
    soArraySelectHolder<1, soArrayVector<soOtherAnim, 1>, soSingletonHolder<soArrayNull<soOtherAnim> > > m_otherAnims;
    soSingletonHolder<soArrayNull<soTransitionTermPack> > m_termPacks;
    soSingletonHolder<soArrayNull<u32> > m_u32s;
    soArrayContractibleTable<soMotionDataConst> m_motionData;
    soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> m_cache;
    T m_module;
public:
    soMotionModuleBuilder(soModuleAccesser* acc, void* motionData) :
        m_transitionBuilder(), m_otherAnims(1, 0),
        m_motionData((const soMotionData*)motionData, DataNum),
        m_module(acc, &m_motionData, m_transitionBuilder.getModule(),
                 soSingletonHolder<soArrayNull<soPartialAnim> >::getInstance(), m_otherAnims.get(),
                 (soArray<soTransitionTermPack>*)lbl_27_bss_9BC,
                 soSingletonHolder<soArrayNull<u32> >::getInstance(),
                 static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), false,
                 (soMotionAnimObjCacheModule*)lbl_27_bss_DBC) {
        acc->getModelModule().isNull();
    }
    ~soMotionModuleBuilder() { }
    soMotionModule* getModule() { return &m_module; }
};

// Articles with two other-animation slots use the shared null cache as well.
template <u32 DataNum, typename T, typename TransitionConfig>
class soMotionModuleBuilder<soMotionModuleBuildConfig<DataNum, T, 0, 2, TransitionConfig,
                                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > > {
    soTransitionModuleBuilder<TransitionConfig> m_transitionBuilder;
    soSingletonHolder<soArrayNull<soPartialAnim> > m_partialAnims;
    soArraySelectHolder<1, soArrayVector<soOtherAnim, 2>, soSingletonHolder<soArrayNull<soOtherAnim> > > m_otherAnims;
    soSingletonHolder<soArrayNull<soTransitionTermPack> > m_termPacks;
    soSingletonHolder<soArrayNull<u32> > m_u32s;
    soArrayContractibleTable<soMotionDataConst> m_motionData;
    soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> m_cache;
    T m_module;
public:
    soMotionModuleBuilder(soModuleAccesser* acc, void* motionData) :
        m_transitionBuilder(), m_otherAnims(2, 0),
        m_motionData((const soMotionData*)motionData, DataNum),
        m_module(acc, &m_motionData, m_transitionBuilder.getModule(),
                 soSingletonHolder<soArrayNull<soPartialAnim> >::getInstance(), m_otherAnims.get(),
                 (soArray<soTransitionTermPack>*)lbl_27_bss_9BC,
                 soSingletonHolder<soArrayNull<u32> >::getInstance(),
                 static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), false,
                 (soMotionAnimObjCacheModule*)lbl_27_bss_DBC) {
        acc->getModelModule().isNull();
    }
    ~soMotionModuleBuilder() { }
    soMotionModule* getModule() { return &m_module; }
};

// Model without virtual nodes: see so/so_module_accesser_builder.h.


// Team of an article: the team number from the build data, both team views are plain soTeamImpl.
template <>
class soTeamModuleBuilder<soTeamModuleBuildConfig<soTeamImpl, soTeamModuleImpl> > {
    soTeamImpl m_team;
    soTeamImpl m_teamIndirect;
    soTeamModuleImpl m_module;
public:
    soTeamModuleBuilder(s32 team, soModuleAccesser* acc) :
        m_team(team), m_teamIndirect(team), m_module(&m_team, &m_teamIndirect, &m_team, acc, g_soTeamModuleNullArg) { }
    ~soTeamModuleBuilder() { }
    void* getModule() { return &m_module; }
};

// Attack collision without absolute attacks: the absolute array is the null array.
template <soCollision::Category Cat, u32 P, class M, u32 G, bool b1, bool b2>
class soCollisionAttackModuleBuilder<soCollisionAttackModuleBuildConfig<Cat, P, 0, M, G, b1, b2> > {
    soArrayVector<soCollisionAttackPart, P> m_attackPartArrayVector;
    soArrayVector<soCollisionGroup, G> m_collisionGroupArrayVector;
    soArrayNull<soCollisionAttackAbsolute> m_attackAbsoluteArray;
    M m_attackModule;
public:
    soCollisionAttackModuleBuilder(soModuleAccesser* moduleAccesser, int taskId, u8 taskCategory,
                                   soEventObserverRegistrationDesc* registrationDesc) :
        m_attackPartArrayVector(P, soCollisionAttackPart(Cat, b1), 0),
        m_collisionGroupArrayVector(G, 0),
        m_attackAbsoluteArray(0, soCollisionAttackAbsolute()),
        m_attackModule(moduleAccesser, taskId, (gfTask::Category)taskCategory, &m_attackPartArrayVector, &m_collisionGroupArrayVector,
                       &m_attackAbsoluteArray, registrationDesc, b2) { }
    soCollisionAttackModule* getModule() { return &m_attackModule; }
};

// Damage of an article: null effector and transactor singletons.
// HYPOTHESIS: the source config is soDamageModuleBuildConfig<Module, 1, soSingletonHolder<Transactor>, ...> (map name); that
// template name is taken by the fighters' placeholder, so the article form is spelled separately.
template <typename T, s32 N, typename TransactorHolder, typename EffectorHolder>
class wnDamageModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename T, s32 N, typename TransactorHolder, typename EffectorHolder>
class soDamageModuleBuilder<wnDamageModuleBuildConfig<T, N, TransactorHolder, EffectorHolder> > :
    public soArraySelectHolder<1, soArrayVector<soDamage, 1>, soArrayNull<soDamage> > {
    T m_damageModule;
public:
    soDamageModuleBuilder(soModuleAccesser* acc, soEventObserverRegistrationDesc* regDesc) :
        soArraySelectHolder<1, soArrayVector<soDamage, 1>, soArrayNull<soDamage> >(1, 0),
        m_damageModule(acc, this->get(), EffectorHolder::getInstance(), TransactorHolder::getInstance(), regDesc) { }
    T* getModule() { return &m_damageModule; }
};

// Shake without terms.
template <typename T>
class soShakeModuleBuilder<soShakeModuleBuildConfig<0, T> > {
    soSingletonHolder<soArrayNull<soShakeTerm> > m_terms;
    T m_shakeModule;
public:
    soShakeModuleBuilder(soModuleAccesser* acc, void* shakeData) :
        m_shakeModule(acc, soSingletonHolder<soArrayNull<soShakeTerm> >::getInstance(), shakeData) { }
    T* getModule() { return &m_shakeModule; }
};

// Sound with the single-generator accesser.
// HYPOTHESIS: source form soSoundModuleBuildConfig<0, soSound3dGeneratorAccesserImpl, soSoundModuleImpl> (map name); the
// fighters' placeholder owns that template name.
template <s32 N, typename G, typename T>
class wnSoundModuleBuildConfig {
public:
    typedef G GeneratorType;
    typedef T ModuleType;
};

template <s32 N, typename G, typename T>
class soSoundModuleBuilder<wnSoundModuleBuildConfig<N, G, T> > {
    G m_generator;
    T m_soundModule;
public:
    soSoundModuleBuilder(soModuleAccesser* acc, soSoundIdExchanger* exchanger, soEventObserverRegistrationDesc* regDesc) :
        m_generator(), m_soundModule(acc, &m_generator, exchanger, true, false, regDesc) { }
    T* getModule() { return &m_soundModule; }
};

// Visibility with the simple module.
template <s32 N>
class soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, N> > {
    soVisibilityModuleSimple m_module;
public:
    soVisibilityModuleBuilder(soModuleAccesser* acc, soVisibilityData* data) : m_module(acc, data, N) { }
    soVisibilityModuleSimple* getModule() { return &m_module; }
};

// Status of an article.
// HYPOTHESIS: the source spelling is soStatusModuleBuildConfig<Kinds, GeneralWork, DataCount, Transition> (map name); that
// template name is taken by the fighters' five-argument placeholder.
template <s32 StatusKinds, typename GeneralWorkConfig, s32 StatusDataCount, typename TransitionConfig>
class wnStatusModuleBuildConfig {
public:
    enum { StatusKindCap = StatusKinds, DataCount = StatusDataCount };
    typedef GeneralWorkConfig GeneralWorkBuildConfig;
    typedef TransitionConfig TransitionBuildConfig;
};

// The article selection constructs its module with an unassigned blend index.
// Kinetic module of an article: the weapon kinetic module, eight energies, the given mediator.
template <typename M>
class soKineticModuleBuilder<soKineticModuleBuildConfigMediator<wnKineticModuleImpl, M> > {
    wnKineticModuleImpl m_module;                                  // +0x0
    soInstanceManagerFullPropertyVector<soKineticEnergy*, 8> m_manager; // +0x30
    M m_mediator;                                                  // +0xB0
public:
    soKineticModuleBuilder(soModuleAccesser* acc) :
        m_module(acc, &m_manager, &m_mediator), m_manager(false), m_mediator() { }
    soKineticModule* getModule() { return (soKineticModule*)&m_module; }
};

// Kinetic module of an article with another energy capacity (two for Ness's/Lucas's PK Starstorm, twelve for
// Kirby's Nabe).
// HYPOTHESIS: source spelling; the builder map name is the same as the eight-energy one.
template <typename Impl, typename M, int Capacity>
class wnKineticModuleBuildConfigSized { };

template <typename M, int Capacity>
class soKineticModuleBuilder<wnKineticModuleBuildConfigSized<wnKineticModuleImpl, M, Capacity> > {
    wnKineticModuleImpl m_module;                                              // +0x0
    soInstanceManagerFullPropertyVector<soKineticEnergy*, Capacity> m_manager; // +0x30
    M m_mediator;
public:
    soKineticModuleBuilder(soModuleAccesser* acc) :
        m_module(acc, &m_manager, &m_mediator), m_manager(false), m_mediator() { }
    soKineticModule* getModule() { return (soKineticModule*)&m_module; }
};

// Effect of an article.
// HYPOTHESIS: source spelling soEffectModuleBuildConfig<Continual, Screen, Time, U32, ScreenNum, Module> (map name); the
// fighters' placeholder owns that template name.
template <s32 Continual, s32 Screen, s32 Time, s32 U32, s32 Num, typename T>
class wnEffectModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, -1, T> > :
    public soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> > {
    // HYPOTHESIS: the disabled time selection occupies this slot (its array is the null singleton).
    u8 unk38[4];
    soArraySelectHolder<1, soArrayVector<efScreenHandle, 1>, soArrayNull<efScreenHandle> > m_screens; // +0x3C
    soArrayVector<u32, 1> m_u32s;                                                                     // +0x48
    T m_effectModule;                                                                                 // +0x58
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> >(1, 0),
        m_screens(1, 0), m_u32s(1, 0, 0),
        m_effectModule(acc, this->get(), nodeData, &m_u32s, soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData,
                       regDesc, commonData, -1, screenData, m_screens.get()) { }
    T* getModule() { return &m_effectModule; }
};

// The Oil article selects the shared null Glow instance in its enumeration.
// HYPOTHESIS: the null build-config spelling is not preserved in module RTTI.
struct soGlowModuleBuildConfigNull { };
extern char lbl_27_bss_6E8[];
template <>
class soGlowModuleBuilder<soGlowModuleBuildConfigNull> {
public:
    soGlowModuleBuilder(soModuleAccesser*) { }
    void* getModule() { return lbl_27_bss_6E8; }
};

// Slow / reflect / glow of an article.
template <>
class soSlowModuleBuilder<soSlowModuleBuildConfig<soSlowModuleSimple> > {
    soSlowModuleSimple m_module;
public:
    soSlowModuleBuilder(soModuleAccesser* acc) : m_module(acc) { }
    soSlowModuleSimple* getModule() { return &m_module; }
};

template <s32 Kind, typename T>
class soReflectModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <s32 Kind, typename T>
class soReflectModuleBuilder<soReflectModuleBuildConfig<Kind, T> > {
    T m_module;
public:
    soReflectModuleBuilder(soModuleAccesser* acc) : m_module(acc, Kind) { }
    T* getModule() { return &m_module; }
};

template <>
class soGlowModuleBuilder<soGlowModuleBuildConfig<soGlowModuleSimple> > {
    soGlowModuleSimple m_module;
public:
    soGlowModuleBuilder(soModuleAccesser* acc) : m_module(acc) { }
    soGlowModuleSimple* getModule() { return &m_module; }
};

////////////////////////////////////////
// Anim cmd threads of an article
////////////////////////////////////////

// Disabled thread: no storage of its own, the command data queries still happen.
template <class BC>
class soAnimCmdControlUnitBuilderNull {
public:
    soAnimCmdControlUnitBuilderNull(soModuleAccesser*, void*, void*, void*) { }
};

template <bool Enabled, class BC>
struct wnAnimCmdControlUnitSelect {
    typedef soAnimCmdControlUnitBuilder<BC> Type;
};
template <class BC>
struct wnAnimCmdControlUnitSelect<false, BC> {
    typedef soAnimCmdControlUnitBuilderNull<BC> Type;
};

// Four command threads (each: enabled, table capacity, two unknown values) and the uniq interpreter thread 4.
// HYPOTHESIS: meaning of the per-thread values beyond the command table capacity.
template <int E0, int C0, int A0, int B0, int E1, int C1, int A1, int B1, int E2, int C2, int A2, int B2, int E3, int C3, int A3, int B3>
class wnAnimCmdModuleSubBuildConfigDefault {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, C0, A0, B0, 0, 0, 8> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, C1, A1, B1, 1, 0, 8> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, C2, A2, B2, 1, 0, 8> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, C3, A3, B3, 1, 0, 8> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 8> Unit4Config;
    enum { Unit0Enabled = E0, Unit1Enabled = E1, Unit2Enabled = E2, Unit3Enabled = E3 };
};

template <class BC>
class wnAnimCmdModuleSubBuilder {
    typename wnAnimCmdControlUnitSelect<BC::Unit0Enabled != 0, typename BC::Unit0Config>::Type m_unit0; // +0
    typename wnAnimCmdControlUnitSelect<BC::Unit1Enabled != 0, typename BC::Unit1Config>::Type m_unit1; // +0x12C
    typename wnAnimCmdControlUnitSelect<BC::Unit2Enabled != 0, typename BC::Unit2Config>::Type m_unit2; // +0x258
    typename wnAnimCmdControlUnitSelect<BC::Unit3Enabled != 0, typename BC::Unit3Config>::Type m_unit3; // +0x384
    soAnimCmdControlUnitBuilderUniq<typename BC::Unit4Config> m_unit4;                                    // +0x4B0
    soAnimCmdControlUnitBuilderNull<void> m_unit5;
public:
    wnAnimCmdModuleSubBuilder(soModuleAccesser* acc, const soModuleAccesserBuildData& bd) :
        m_unit0(acc, bd.getAnimCmdData(0, 0), bd.getAnimCmdData(0, 1), bd.getAnimCmdData(0, 2)),
        m_unit1(acc, bd.getAnimCmdData(1, 0), bd.getAnimCmdData(1, 1), bd.getAnimCmdData(1, 2)),
        m_unit2(acc, bd.getAnimCmdData(2, 0), bd.getAnimCmdData(2, 1), bd.getAnimCmdData(2, 2)),
        m_unit3(acc, bd.getAnimCmdData(3, 0), bd.getAnimCmdData(3, 1), bd.getAnimCmdData(3, 2)),
        m_unit4(acc),
        m_unit5(acc, bd.getAnimCmdData(5, 0), bd.getAnimCmdData(5, 1), bd.getAnimCmdData(5, 2)) { }
    ~wnAnimCmdModuleSubBuilder() { }
};

////////////////////////////////////////
// Common build config of the articles (null modules everywhere); an article config derives from it.
////////////////////////////////////////

typedef soTypeList<soIntToType<4>, soTypeListNullType> wnMotionTransitionTypeList;
typedef soTypeList<soIntToType<6>, soTypeListNullType> wnStatusTransitionTypeList;

class wnCommonBuildConfig {
public:
    typedef wnInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soHeapModuleBuildConfig<soHeapModuleImpl> HeapModuleBuildConfig;
    typedef soParamCustomizeModuleBuildConfig<soParamCustomizeModuleImpl> ParamCustomizeModuleBuildConfig;
    typedef soResourceModuleBuildConfig<0, soResourceIdAccesserImpl, soResourceModuleImpl> ResourceModuleBuildConfig;
    typedef soPostureModuleBuildConfig<1, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfig<1, soGroundModuleImpl> GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfig<soSituationModuleImpl> SituationModuleBuildConfig;
    typedef soTeamModuleBuildConfig<soTeamImpl, soTeamModuleImpl> TeamModuleBuildConfig;
    typedef soCollisionShieldModuleBuildConfigNull CollisionShieldModuleBuildConfig;
    typedef soCollisionReflectorModuleBuildConfigNull CollisionReflectorModuleBuildConfig;
    typedef soCollisionCatchModuleBuildConfigNull CollisionCatchModuleBuildConfig;
    typedef ftNullCollisionSearchModuleBuilder CollisionSearchModuleBuilder;
    typedef soCatchModuleBuildConfigNull CatchModuleBuildConfig;
    typedef soCaptureModuleBuildConfigNull CaptureModuleBuildConfig;
    typedef soStopModuleBuildConfig<soStopModuleImpl> StopModuleBuildConfig;
    typedef soTurnModuleBuildConfigNull TurnModuleBuildConfig;
    typedef soShakeModuleBuildConfig<0, soShakeModuleSimple> ShakeModuleBuildConfig;
    typedef wnSoundModuleBuildConfig<0, soSound3dGeneratorAccesserImpl, soSoundModuleImpl> SoundModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soControllerModuleBuildConfigNull ControllerModuleBuildConfig;
    typedef soCameraModuleBuildConfigNull CameraModuleBuildConfig;
    typedef soWorkManageModuleBuildConfig<soWorkManageModuleImpl> WorkManageModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfig<6, soAnimCmdModuleImpl> AnimCmdModuleBuildConfig;
    typedef soKineticModuleBuildConfigMediator<wnKineticModuleImpl, soKineticMediatorNull> KineticModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<7, 6, 1> GeneralWorkBuildConfig;
    typedef ftNullGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnEffectModuleBuildConfig<1, 1, 0, 1, -1, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soComboModuleBuildConfigNull ComboModuleBuildConfig;
    typedef soAreaModuleBuildConfigNull AreaModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigNull PhysicsModuleBuildConfig;
    typedef soSlopeModuleBuildConfigNull SlopeModuleBuildConfig;
    typedef soShadowModuleBuildConfigNull ShadowModuleBuildConfig;
    typedef soItemManageModuleBuildConfigNull ItemManageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfigNull ColorBlendModuleBuildConfig;
    typedef soJostleModuleBuildConfigNull JostleModuleBuildConfig;
    typedef soAbnormalModuleBuildConfigNull AbnormalModuleBuildConfig;
    typedef soSlowModuleBuildConfig<soSlowModuleSimple> SlowModuleBuildConfig;
    typedef soReflectModuleBuildConfig<0, soReflectModuleImpl> ReflectModuleBuildConfig;
    typedef soGlowModuleBuildConfig<soGlowModuleSimple> GlowModuleBuildConfig;
};

////////////////////////////////////////
// soModuleAccesserBuilder<BC>: the article constructor (declared in ft_module_accesser_builder.h)
////////////////////////////////////////

template <class BC>
inline soModuleAccesserBuilder<BC>::soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bdRef) :
    m_moduleAccsr(
        owner,
        soResourceModuleBuilder<typename BC::ResourceModuleBuildConfig>::getModule(&m_resourceModuleBuilder),
        soModelModuleBuilder<typename BC::ModelModuleBuildConfig>::getModule(&m_modelModuleBuilder),
        (soMotionModule*)m_motionBuilder.getModule(),
        (soPostureModule*)m_postureModuleBuilder.getModule(),
        (soGroundModule*)m_groundModuleBuilder.getModule(),
        (soSituationModule*)m_situationModuleBuilder.getModule(),
        (void*)m_teamBuilder.getModule(),
        (soCollisionAttackModule*)m_attackModuleBuilder.getModule(),
        (soCollisionHitModule*)m_hitModuleBuilder.getModule(),
        (soCollisionShieldModule*)m_shieldModuleBuilder.getModule(),
        (soCollisionShieldModule*)m_reflectorModuleBuilder.getModule(),
        ftReflectorBase<typename BC::CollisionReflectorModuleBuildConfig>::getAbsorber(&m_reflectorModuleBuilder),
        (void*)m_collisionCatchModuleBuilder.getModule(),
        (soCollisionSearchModule*)m_searchBuilder.getModule(),
        (soDamageModule*)m_damageModuleBuilder.getModule(),
        (void*)m_catchModuleBuilder.getModule(),
        (void*)m_captureModuleBuilder.getModule(),
        (soStopModule*)m_stopModuleBuilder.getModule(),
        (void*)m_turnModuleBuilder.getModule(),
        (void*)m_shakeModuleBuilder.getModule(),
        (soSoundModule*)m_soundModuleBuilder.getModule(),
        (soLinkModule*)m_linkModuleBuilder.getModule(),
        (soVisibilityModule*)m_visibilityModuleBuilder.getModule(),
        (soControllerModule*)m_controllerModuleBuilder.getModule(),
        (soCameraModule*)m_cameraModuleBuilder.getModule(),
        (soWorkManageModule*)m_workManageModuleBuilder.getModule(),
        (void*)g_soDebugModuleNull,
        (soAnimCmdModule*)m_animCmdBuilder.getModule(),
        (soStatusModule*)m_statusBuilder.getModule(),
        (void*)lbl_27_bss_170, // HYPOTHESIS: the articles' null general term decide module
        (void*)lbl_27_bss_188, // HYPOTHESIS: the articles' null switch decide module
        (soKineticModule*)m_kineticBuilder.getModule(),
        (soEventManageModule*)unk194.getModule(),
        (void*)m_generateArticleBuilder.getModule(),
        (soEffectModule*)m_effectModuleBuilder.getModule(),
        (void*)m_comboModuleBuilder.getModule(),
        (soAreaModule*)m_areaBuilder.getModule(),
        (void*)g_soTerritoryModuleNull,
        (void*)g_soTargetSearchModuleNull,
        (void*)m_physicsModuleBuilder.getModule(),
        (void*)m_slopeModuleBuilder.getModule(),
        (soShadowModule*)m_shadowModuleBuilder.getModule(),
        (soItemManageModule*)m_itemManageModuleBuilder.getModule(),
        (soColorBlendModule*)m_colorBlendModuleBuilder.getModule(),
        (void*)m_jostleModuleBuilder.getModule(),
        (void*)m_abnormalModuleBuilder.getModule(),
        (soSlowModule*)m_slowModuleBuilder.getModule(),
        (void*)m_reflectModuleBuilder.getModule(),
        (void*)&m_heapModuleBuilder,
        (soParamCustomizeModule*)&m_paramCustomizeModuleBuilder,
        (void*)m_glowModuleBuilder.getModule()),
    m_heapModuleBuilder(bdRef.getInstanceHeap(), bdRef.getNWModelInstanceHeap(), bdRef.getNWMotionInstanceHeap(), bdRef.getHeapSlotNo()),
    m_paramCustomizeModuleBuilder(&m_moduleAccsr),
    m_resourceModuleBuilder(bdRef.getMdlResId(), bdRef.getAnmResId(), bdRef.getResGroupNo(), &m_moduleAccsr),
    m_modelModuleBuilder(&m_moduleAccsr, bdRef.getModelScale(), bdRef.getModelExtendNodeTable(), FT_NULL_OBSERVER_DESC),
    m_motionBuilder(&m_moduleAccsr, bdRef.getMotionData()),
    m_postureModuleBuilder(&m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_groundModuleBuilder(&m_moduleAccsr, bdRef.getGroundConditionChecker()),
    m_situationModuleBuilder(ftGetManageId(&m_moduleAccsr), &m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_teamBuilder(bdRef.getTeam(), &m_moduleAccsr),
    m_attackModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_hitModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_shieldModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_reflectorModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_absorberBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_collisionCatchModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_searchBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_damageModuleBuilder(&m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_catchModuleBuilder(&m_moduleAccsr),
    m_captureModuleBuilder(&m_moduleAccsr),
    m_stopModuleBuilder(&m_moduleAccsr),
    m_turnModuleBuilder(&m_moduleAccsr),
    m_shakeModuleBuilder(&m_moduleAccsr, bdRef.getShakeData()),
    m_soundModuleBuilder(&m_moduleAccsr, bdRef.getSoundIdExchanger(), FT_NULL_OBSERVER_DESC),
    m_linkModuleBuilder(&m_moduleAccsr),
    m_visibilityModuleBuilder(&m_moduleAccsr, bdRef.getVisibilityData()),
    m_controllerModuleBuilder(&m_moduleAccsr, ftGetManageId(&m_moduleAccsr)),
    m_cameraModuleBuilder(&m_moduleAccsr, (soSet<soCameraRange>*)bdRef.getCameraRangeSet(), (soSet<soCameraClipSphere>*)bdRef.getCameraClipSphereSet(), FT_NULL_OBSERVER_DESC),
    m_workManageModuleBuilder(&m_moduleAccsr, bdRef.getParamAccesser()),
    m_animCmdBuilder(ftGetManageId(&m_moduleAccsr)),
    m_statusBuilder(&m_moduleAccsr, bdRef.getStatusData(), bdRef.getPreCheckAnimCmdData()),
    m_kineticBuilder(&m_moduleAccsr),
    m_generalWorkBuilder(),
    m_generateArticleBuilder(&m_moduleAccsr),
    m_effectModuleBuilder(&m_moduleAccsr, bdRef.getEffectNodeData(), bdRef.getEffectEmitData(), bdRef.getEffectCommonData(), bdRef.getEffectScreenData(), FT_NULL_OBSERVER_DESC),
    m_comboModuleBuilder(&m_moduleAccsr),
    m_areaBuilder(&m_moduleAccsr, bdRef.getAreaCategory(), FT_NULL_OBSERVER_DESC),
    m_physicsModuleBuilder(&m_moduleAccsr, (bdRef.getTerritoryRect(), bdRef.getTerritoryParam(), bdRef.getTargetSearchParam(), bdRef.getIkData())),
    m_slopeModuleBuilder(&m_moduleAccsr, bdRef.getSlopeAngleLimit()),
    m_shadowModuleBuilder(&m_moduleAccsr),
    m_itemManageModuleBuilder(&m_moduleAccsr, bdRef.getItemNodeData()),
    m_colorBlendModuleBuilder(&m_moduleAccsr),
    m_jostleModuleBuilder(&m_moduleAccsr, bdRef.getJostleData()),
    m_abnormalModuleBuilder(&m_moduleAccsr),
    m_slowModuleBuilder(&m_moduleAccsr),
    m_reflectModuleBuilder(&m_moduleAccsr),
    m_glowModuleBuilder(&m_moduleAccsr) {
    // Register the common work bank only when the build config owns one.
    if (m_generalWorkBuilder.getModule()->isNull() == false)
        m_moduleAccsr.getWorkManageModule().setWork(1, m_generalWorkBuilder.getModule());
}

////////////////////////////////////////
// wnModuleAccesserBuilder<BC> / wnWeaponBuilder<BC>
////////////////////////////////////////

template <class BC>
class wnModuleAccesserBuilder : public soModuleAccesserBuilder<BC> {
    wnAnimCmdModuleSubBuilder<typename BC::AnimCmdModuleSubBuildConfig> m_animCmdSubBuilder;
public:
    wnModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd) :
        soModuleAccesserBuilder<BC>(owner, bd), m_animCmdSubBuilder(&this->m_moduleAccsr, bd) { }
    ~wnModuleAccesserBuilder() { }
};

#include <wn/wn_article_build_info.h>

// The descendants of an article are the articles its own article module generated (none for most articles).
inline void wnDeactivateArticleDescendants(ftNullGenerateArticleManageModuleBuilder&) {
    soSingletonHolder<soArticleMediatorNull>::getInstance()->deactivate();
}
template <class Builder>
inline void wnDeactivateArticleDescendants(Builder& builder) {
    builder.getMediator()->deactivate();
}

template <class BC>
class wnWeaponBuilder : public Weapon {
protected:
    wnModuleAccesserBuilder<BC> m_modules;      // +0xCC
    wnKineticEnergyNormal m_energyNormal;       // the main movement energy (index 0)
    soKineticEnergyRotNormal m_energyRotNormal; // the rotation energy (index 1)
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd), m_energyNormal(), m_energyRotNormal() {
        Weapon::initialize();
        m_moduleAccesser->getKineticModule().addEnergy(&m_energyNormal, 0, soKineticEnergy::AttributeFlag(1), -1);
        m_moduleAccesser->getKineticModule().addEnergy(&m_energyRotNormal, 1, soKineticEnergy::AttributeFlag(1), -1);
    }
    virtual ~wnWeaponBuilder() { }
#ifdef WN_OCTOPUS_ARRAYS
    virtual void deactivateDescendantForce();
#else
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
#endif
};
