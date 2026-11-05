#pragma once

#include <so/so_array.h>
#include <so/collision/so_collision_search_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_weapon_builder.h>
#include <wn/wn_article_pool.h>
#include <wn/weapon.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/snake/so_kinetic_energy_local_normal.h>
// The missile shares the land mine's camera (no subjects) and weapon collision search builders.
#include <wn/snake/wn_snake_mine.h>

struct wnSnakeNikitaModuleAccesserBuildConfig;

// The array instances the sora_melee constructors build (the land mine's header declares the others).
WN_STATUS_PROC_VECTOR_ARG_CTOR(3);
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 2);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soKineticEnergy*>, 4);
// The manager of the missile's four energies: its constructor and methods are sora_melee's (as for the C4).
template <>
class soInstanceManagerFullPropertyVector<soKineticEnergy*, 4> : public soInstanceManagerFullProperty<soKineticEnergy*> {
    soArrayVector<soInstanceUnitFullProperty<soKineticEnergy*>, 4> m_arrayVector; // 0x10
    bool m_unk1;
public:
    soInstanceManagerFullPropertyVector(bool p1);
    ~soInstanceManagerFullPropertyVector() { }
    virtual soKineticEnergy*& at(s32 id);
    virtual soKineticEnergy*& atIndex(s32 idx);
    virtual s32 getId(s32 idx);
    virtual u32 size() const;
    virtual bool isContain(s32 id) const;
    virtual void erase(s32 id);
    virtual void clear();
    virtual void set(soKineticEnergy* const& elm, s32 id);
    virtual s32 add(soKineticEnergy*& elm, s32 id, soAttributeFlag attr, s16 p4);
    virtual u32 capacity();
    virtual soKineticEnergy*& atIndexFast(s32 idx);
    virtual soInstanceUnitFullProperty<soKineticEnergy*>& atUnitIndexFast(s32 idx);
    virtual s32 getIndex(s32 id) const;
    virtual void getAttributeArray(soAttributeFlag targetAttr, soArray<soKineticEnergy**>& arr);
    virtual soAttributeFlag getAttribute(s32 id) const;
    virtual void getPriorityArray(soArray<soKineticEnergy**>& arr);
};

// The missile's parameter block (read by its constructor).
// HYPOTHESIS: layout of the untouched part and the field meanings.
struct wnSnakeNikitaMissileParam {
    u8 unk0[0x1C];
    float m_rotAccel;   // +0x1C: rotation energy acceleration (z)
    float m_rotBrake;   // +0x20: rotation energy brake (z)
    u8 unk24[0x24];
    float m_gravity;    // +0x48: gravity energy acceleration (negated)
    float m_speedLimit; // +0x4C: gravity energy speed limit
};

// The static data block of the missile article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSnakeNikitaMissileData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeNikitaMissileParam* m_param; // +0x34
    u8 unk38[1];                              // start of the collision hit data set handed to the hit module
};

class wnSnakeNikitaMissileModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soPostureModuleBuildConfig<2, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soMotionModuleBuildConfig<4, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3FF, true>
        CollisionHitModuleBuildConfig;
    typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigWeapon<soCollisionSearchModuleImpl> >
        CollisionSearchModuleBuilder;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>, soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef soCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<4, 5, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 4> KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0, 1, 4, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soReflectModuleBuildConfig<1, soReflectModuleImpl> ReflectModuleBuildConfig; // the missile can be reflected
};

// The generic weapon builder (module storage and the two common energies); only its constructor is a separate
// function in this unit (fn_122_2460C), every module builder is expanded inside it.
template <>
class wnWeaponBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig> m_modules; // +0xCC
    wnKineticEnergyNormal m_energyNormal;       // +0x247C: kinetic energy 0
    soKineticEnergyRotNormal m_energyRotNormal; // +0x24B4: kinetic energy 1
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd)
        __attribute__((never_inline)); // BYTES: the native constructor is not expanded into the missile constructor
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(this->m_modules.m_generateArticleBuilder);
    }
};

// Kinetic energy ids: the builder's common energies, then the missile's own.
enum wnSnakeNikitaMissileEnergy {
    wnSnakeNikitaMissile_Energy_Normal = 0,
    wnSnakeNikitaMissile_Energy_Rot = 1,
    wnSnakeNikitaMissile_Energy_LocalNormal = 2, // the steered flight (angle in unk34)
    wnSnakeNikitaMissile_Energy_Gravity = 3,     // the fall once the missile runs out
};

// Snake's Nikita missile (article kind 0x1D): the remote-controlled missile the launcher pools.
class wnSnakeNikitaMissile : public wnWeaponBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig>,
                             public soDamageEventObserver,
                             public soCollisionSearchEventObserver {
public:
    const wnSnakeNikitaMissileParam* m_param;  // +0x2510
    soKineticEnergyLocalNormal m_energyLocal;  // +0x2514: kinetic energy 2
    wnKineticEnergyGravity m_energyGravity;    // +0x254C: kinetic energy 3

    wnSnakeNikitaMissile(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeNikitaMissileData* data);
    void setAutonomy();
    // HYPOTHESIS: parameter names (the launcher passes its node position and a 90 degree yaw).
    void activate(int founderTaskId, int resourceId, int team, Vec3f pos, Vec3f rot);
    virtual void processCollision();
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog,
                                            soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};

// ---- the launcher (article kind 0x1C): Snake's Nikita, which pools two missiles ------------------------------------

// The static data block of the launcher (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout beyond the resource group.
struct wnSnakeNikitaData {
    s32 m_resGroupNo;
};

// The mediator of the launcher's article manage module: one pool of two missiles (map: soArticleMediatorImpl over
// soInstancePoolInfo<wnSnakeNikitaMissile, 2, wnInstanceHolder, soIntToType<0> >, kind info soKindInfoGeneric<2, 28>).
// HYPOTHESIS: wrapper names (the pool templates of wn/wn_article_pool.h).
typedef wnArticleHierarchy<wnSnakeNikitaMissile, 2, soInstancePoolRoot> wnSnakeNikitaMissilePool;
class wnSnakeNikitaMediator : public soArticleMediator {
    wnSnakeNikitaMissilePool m_pools; // +0x8
    u8 m_autoRecycle;                 // +0x4AE0
    WN_ARTICLE_SLOT_FRIENDS
public:
    wnSnakeNikitaMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~wnSnakeNikitaMediator() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser*, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser*, s32 articleId);
};
FT_DOL_ARRAY_VECTOR(soArticle*, 2);
typedef wnArticleManageModuleBuilder<wnSnakeNikitaMediator, 2> wnSnakeNikitaArticleManageModuleBuilder;

// One status (no kinetic module follows, so no tail word as in the F.L.U.D.D. status builder).
typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
    soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > wnSnakeNikitaStatusBuildConfig;
template <>
class soStatusModuleBuilder<wnSnakeNikitaStatusBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWork;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transition;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_processes;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_requests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* data, void* preCheck) :
        m_statusTable(data, 1), m_generalWork(), m_transition(), m_processes(), m_requests(-1),
        m_module(acc, &m_statusTable, m_processes.get(), m_generalWork.getModule(), m_transition.getModule(), &m_requests,
                 preCheck, 1, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

class wnSnakeNikitaModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<3, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnSnakeNikitaStatusBuildConfig StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnSnakeNikitaArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

// The launcher owns no movement energies. Its module storage is only spelled out in the launcher's own translation
// unit (WN_SNAKE_NIKITA_LAUNCHER_TU): elsewhere the weak builder teardown it brings changes the missile builder.
#ifdef WN_SNAKE_NIKITA_LAUNCHER_TU
template <>
class wnWeaponBuilder<wnSnakeNikitaModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSnakeNikitaModuleAccesserBuildConfig> m_modules; // +0xCC
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the F.L.U.D.D. builder.
    u32 m_modulesPad;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};
#else
template <>
class wnWeaponBuilder<wnSnakeNikitaModuleAccesserBuildConfig> : public Weapon {
    u8 m_opaqueModules[0x67B0 - sizeof(Weapon)]; // the module storage above (+0xCC) and its trailing word
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd);
};
#endif

// Snake's Nikita launcher: it shoots and steers the missiles it pools.
class wnSnakeNikita : public wnWeaponBuilder<wnSnakeNikitaModuleAccesserBuildConfig> {
public:
    wnSnakeNikita(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeNikitaData* data);
    void activate(int founderTaskId, int resourceId, int team, float lr);
    static bool activeArticle(wnSnakeNikitaMissile* missile, soModuleAccesser* acc);
    virtual void notifyArticleEventRemove(int unk1, int* unk2);
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser,
                                 StageObject* linkedObject, int unk4);
    virtual void onDeactivate();
    void forceFallMissile();

private:
    void notifyEnd(int isEnd);

    // HYPOTHESIS: The constructor's soArrayVector_Ul_2 at this offset records
    // task IDs for Nikita missiles managed by this owner.
    soArrayVector<unsigned long, 2> m_unkMissileTaskIds; // +0x67B0
};
