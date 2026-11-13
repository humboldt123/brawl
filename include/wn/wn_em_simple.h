#pragma once

// SHADOW of BrawlHeaders wn/wn_em_simple.h (-I include comes first): adds the constructor the enemy weapon pools call
// and the activation entry point.

#include <StaticAssert.h>
#ifdef WN_WEAPON_BUILDER
// The article builders' array specializations have to be declared before em_shot.h instantiates those arrays.
#include <wn/wn_weapon_builder.h>
#endif
#include <em/em_shot.h>
#include <mt/mt_vector.h>
#include <ft/builder/ft_dol_instances.h>
#include <so/so_array.h>
#include <so/event/so_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <gf/gf_gameframe_counter.h>
#include <types.h>
#include <ut/ut_list.h>
#ifndef WN_WEAPON_BUILDER
#include <wn/wn_weapon_builder.h>
#endif

// Classes of sora_melee: wnemSimple only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(s32, 2);
FT_DOL_ARRAY_VECTOR(float, 5);
static_assert(sizeof(soArrayVector<s32, 2>) == 0x14, "Class is wrong size!");
static_assert(sizeof(soArrayVector<float, 5>) == 0x20, "Class is wrong size!");

#ifdef WN_WEAPON_BUILDER
#include <em/wnem/wn_em_resource_module_impl.h>
#include <so/glow/so_glow_module_impl.h>
#include <wn/wn_em_damage_effector_impl.h>
#include <wn/wn_em_damage_transactor_impl.h>

// The shared damage actors are not members: the builder only passes their addresses.
struct wnemDamageTransactorHolder {
    static wnemDamageTransactorImpl* getInstance() { return &g_wnemDamageTransactorImpl; }
};
struct wnemDamageEffectorHolder {
    static wnemDamageEffectorImpl* getInstance() { return &g_wnemDamageEffectorImpl; }
};

// Camera without subjects of its own (the subject array is the shared null array).
// HYPOTHESIS: config name. The map's soCameraModuleBuildConfig<soCameraModuleSimple, 0> has no subject-array destructor.
template <typename T>
class soCameraModuleBuildConfigNoSubject {
public:
    typedef T ModuleType;
};

template <typename T>
class soCameraModuleBuilder<soCameraModuleBuildConfigNoSubject<T> > {
    soSingletonHolder<soArrayNull<soCameraSubject> > m_subjects;
    T m_cameraModule;
public:
    soCameraModuleBuilder(soModuleAccesser* acc, soSet<soCameraRange>* rangeSet, soSet<soCameraClipSphere>* clipSet,
                          soEventObserverRegistrationDesc* regDesc) :
        m_cameraModule(acc, soSingletonHolder<soArrayNull<soCameraSubject> >::getInstance(), rangeSet, clipSet, regDesc) { }
    T* getModule() { return &m_cameraModule; }
};

// Collision search of an enemy article. The task category arrives as the byte the search module takes.
// HYPOTHESIS: config name; the part category is Category_Enemy (the map's leading 1).
template <typename T>
class soCollisionSearchModuleBuildConfigWeapon {
public:
    typedef T ModuleType;
};

template <typename T>
class soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigWeapon<T> > {
    soArrayVector<soCollisionSearchPart, 1> m_parts;
    soArrayVector<soCollisionGroup, 1> m_groups;
    T m_searchModule;
public:
    soCollisionSearchModuleBuilder(soModuleAccesser* acc, int taskId, u8 category) :
        m_parts(1, soCollisionSearchPart(soCollision::Category_Enemy), 0),
        m_groups(1, 0), m_searchModule(acc, taskId, category, &m_parts, true) { }
    T* getModule() { return &m_searchModule; }
};

// One partial animation and one other animation, with the shared null anim-obj cache (the article specializations
// cover partial count 0 only). HYPOTHESIS: the bool passed to soMotionModuleImpl is true when a partial slot exists.
class soMotionAnimObjCacheModule;
class soMotionAnimObjCacheModuleNull;
template <u32 DataNum, typename T, typename TransitionConfig>
class soMotionModuleBuilder<soMotionModuleBuildConfig<DataNum, T, 1, 1, TransitionConfig,
                                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > > {
    soTransitionModuleBuilder<TransitionConfig> m_transitionBuilder;
    soArraySelectHolder<1, soArrayVector<soPartialAnim, 1>, soSingletonHolder<soArrayNull<soPartialAnim> > > m_partialAnims;
    soArraySelectHolder<1, soArrayVector<soOtherAnim, 1>, soSingletonHolder<soArrayNull<soOtherAnim> > > m_otherAnims;
    soSingletonHolder<soArrayNull<soTransitionTermPack> > m_termPacks;
    soSingletonHolder<soArrayNull<u32> > m_u32s;
    soArrayContractibleTable<soMotionDataConst> m_motionData;
    soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> m_cache;
    T m_module;
public:
    soMotionModuleBuilder(soModuleAccesser* acc, void* motionData) :
        m_transitionBuilder(), m_partialAnims(1, 0), m_otherAnims(1, 0),
        m_motionData((const soMotionData*)motionData, DataNum),
        m_module(acc, &m_motionData, m_transitionBuilder.getModule(), m_partialAnims.get(), m_otherAnims.get(),
                 (soArray<soTransitionTermPack>*)lbl_27_bss_9BC, soSingletonHolder<soArrayNull<u32> >::getInstance(),
                 static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), true,
                 (soMotionAnimObjCacheModule*)lbl_27_bss_DBC) {
        acc->getModelModule().isNull();
    }
    ~soMotionModuleBuilder() { }
    soMotionModule* getModule() { return &m_module; }
};

// The enemy weapon's module set. Common articles supply the null kinetic and status modules, the simple
// visibility and slow modules, and the sound and effect builders. Everything named here is what this object emits.
class wnemSimple_ModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    // HYPOTHESIS: capacities after the map prefix 12,22,1,1,0,3,3,3,1,0,1. These eight make the builder 0x2160.
    // HYPOTHESIS: capacities after the map prefix 12,22,1,1,0,3,3,3,1,0,1.
    // Reflector 4 and absorber 2 make the builder 0x2160 once the anim-cmd interpreters are the 0x50 class.
    typedef soInsideEventManageModuleBuildConfig<12, 22, 1, 1, 0, 3, 3, 3, 1, 0, 1, 4, 2, 0, 0, 0, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soResourceModuleBuildConfig<1, soResourceIdAccesserImpl, wnemResourceModuleImpl> ResourceModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 3, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<0, soMotionModuleImpl, 1, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<3, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Enemy, 3, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Enemy, 2, 1, soCollisionHitModuleImpl, 0x3FF, true>
        CollisionHitModuleBuildConfig;
    typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigWeapon<soCollisionSearchModuleImpl> >
        CollisionSearchModuleBuilder;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, wnemDamageTransactorHolder, wnemDamageEffectorHolder>
        DamageModuleBuildConfig;
    typedef soShakeModuleBuildConfig<1, soShakeModuleImpl> ShakeModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfig<7, soAnimCmdModuleImpl> AnimCmdModuleBuildConfig;
    typedef soStatusModuleBuildConfigNull StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soReflectModuleBuildConfig<1, soReflectModuleImpl> ReflectModuleBuildConfig;
    typedef soGlowModuleBuildConfig<soGlowModuleImpl> GlowModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0> AnimCmdModuleSubBuildConfig;
};
static_assert(sizeof(wnWeaponBuilder<wnemSimple_ModuleAccesserBuildConfig>) == 0x2160, "wnemSimple builder size");
#else
class wnemSimple_ModuleAccesserBuildConfig {
    // The article unit (which defines WN_WEAPON_BUILDER) instantiates the real configuration.
};
#endif

class utWeaponObjectBase : public utListNode {
public:
    virtual ~utWeaponObjectBase();
    virtual void setDeactivate(bool deactivate);
};
static_assert(sizeof(utWeaponObjectBase) == 12, "Class is wrong size!");

class wnemSimple : public utWeaponObjectBase, public wnWeaponBuilder<wnemSimple_ModuleAccesserBuildConfig>, public soDamageEventObserver, public soCollisionSearchEventObserver {
public:
    // The shot work block the activation (fn_41_3091C) copies from the firing enemy.
    wnemSimple_Data m_shotWork; // 0x2184
    bool unk21B8;
    bool unk21B9;
    u8 unk21BA[0x1];
    bool unk21BB;
    // 0x21BC, the proc functions the weapon runs when its hp or its lifetime runs out, when it touches the ground and
    // when a shell hits (setProcFnc_HpZero / LifeZero / TouchGround / HitShell). Null when unset.
    void* m_procFncHpZero;
    void* m_procFncLifeZero;
    void* m_procFncTouchGround;
    void* m_procFncHitShell;
    // 0x21CC, the article's link id: the Cataguard's Report string names EM_CATAGUARD_LINK_ID_WPN_PARTS_LANCE (3),
    // Petey Piranha's arms send it with every event, and the weapon manager's searches filter on it.
    s32 m_linkId;
    s32 m_lifeLimit; // 0x21D0, the frame counter value at which the life runs out (SetLifeLimit); 0: no limit
    float unk21D4;
    float unk21D8;
    s32 m_weaponType;     // 0x21DC, the strings name WEAPON_TYPE_SHELL and TypeParts (2: parts)
    float m_maxHitPower;  // 0x21E0, the strongest hit of the frame (notifyEventCollisionAttack)
    float m_attackStopMax; // 0x21E4, the hit stop frames the weapon asked for
    s32 unk21E8;
    s32 unk21EC;
#ifdef WN_EM_SIMPLE_UNIT
    Vec2f m_baseSpeed;     // 0x21F0, HYPOTHESIS: the launch speed of the shot (setupParam)
#else
    u8 unk21F0[0x8];
#endif
    Vec2f m_speed;         // 0x21F8, HYPOTHESIS: the article's speed
    Vec2f m_accel;         // 0x2200, HYPOTHESIS: the article's acceleration
    class wnemSwHolder* m_holder; // 0x2208, the holder that activated it (wnemSwHolder::active / deactivate)
    void* m_procFnc; // 0x220C, the running proc function (changeProcFnc1); its id is m_weaponKind
    u32 m_weaponKind;      // 0x2210, HYPOTHESIS: the weapon kind (emPorky::updateNodeSRT tests 0x2c / 0x2d)
    bool unk2214;
    bool unk2215;
    u8 unk2216;
    // The arm event (Petey Piranha's notifyEventLink) runs its status transition check only when it is set.
    bool unk2217;
    bool unk2218;
    bool m_requestDeactive; // 0x2219, set by RequestDeactive; the proc update then deactivates the weapon
    bool m_isManaged; // 0x221A
    bool m_isFromPool; // 0x221B, HYPOTHESIS: set by wnemSwHolder::active: the weapon was taken from the weapon pool and goes back on deactivation
    gfGameFrameCounter m_frameCounters[2]; // 0x221C, the life counter (0) and a second one; getFrameCounter / resetFrameCounter
    s32* unk2234;
    // Built by the constructor; the weapon procs read them as their int and float parameters.
    soArrayVector<s32, 2> m_intParams;     // 0x2238
    soArrayVector<float, 5> m_floatParams; // 0x224C
#ifdef WN_EM_SIMPLE_UNIT
    // The tail the destructor tears down: an interpreter (kind 6) and its stack, then 0xC bytes with no destructor.
    soAnimCmdInterpreter m_cmdInterpreter;
    soArrayVector<acCmdInterpreterStackData, 8> m_cmdStack;
    u8 unk2368[0xC];
#else
    u8 unk226C[0x108];
#endif

    wnemSimple();
    u32 getOwnerID();
    virtual void setDeactivate(bool x);
    // HYPOTHESIS: parameter types. Called by emWeaponInstanceHolder<wnemSimple, ...> with the article id of the pool, the
    // event manager id of the owner, the kind/heap construction info and the shared parameter block.
    wnemSimple(s32 articleId, s16 manageId, const struct soArticleConstructInfo& info, const class wnemDummy_Param* param);
    bool setProcFnc_TouchGround(s32 p1);
    bool setProcFnc_LifeZero(s32 p1);
    bool setProcFnc_HitShell(s32 p1);
    bool setProcFnc_HpZero(s32 p1);
    bool changeProcFnc(s32 p1, s32 p2);
    bool executeAnimCmd(u32 p1);
    bool executeAnimCmd(s32 p1, s32 p2, s32 p3);
    void SetLifeLimit(s32 p1);
    u32 getFrameCounter(s32 p1);
    void resetFrameCounter(s32 index);
    void setDamage(float damage);
    void OnDead();
    void constraint();
    void removeConstraint();
    void setupCollisionHitStatus();
#ifdef WN_EM_SIMPLE_UNIT
    bool setupParam(struct emShotParam* param, struct emWeaponParamWork* work, float lr);
#endif
    void setupGroundData();
    void setupGlowModule();
    void reqCommonEffect(s32 id);
    void updateStatusProcFunc();
    bool changeProcFnc1(void* proc, s32 p2);
    void RequestDeactive();
    void SetDeadMode();
    virtual ~wnemSimple();
    // The overrides, in the order of the vtable's tail (the Weapon and StageObject slots point at thunks to these).
    virtual void onDeactivate();
    virtual void updatePosture(bool);
    virtual void processFixPositionPreAnimCmd();
    virtual void processHit();
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool reflect();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData*, u32 index, int, soModuleAccesser* moduleAccesser, soCollisionLog*);
    virtual void notifyEventCollisionHit2nd(soCollisionAttackModule*, soCollisionLog*, u32 groupIndex, float posX, float collisionLr, soModuleAccesser* moduleAccesser, bool);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
    virtual void initialize();
    virtual void updateNodeSRT();
    virtual soKind soGetKind();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(wnemSimple) == 0x2374, "Class is wrong size!");
