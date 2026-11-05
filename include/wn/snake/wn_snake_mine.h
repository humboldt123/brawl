#pragma once

// Snake's land mine (article kind 0x16, his down smash). Snake buries it in the ground; it arms, then waits for a fighter to
// step into its search area and detonates (status 3). It listens for collision search hits (a fighter in range) and for
// damage (hitting the mine sets it off).

#include <so/collision/so_collision_search_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <ft/ft_kinetic_energy.h>
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(5);
// The array instances the sora_melee constructors build.
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 3);
#include <wn/snake/wn_snake_shared_arrays.h> // the hit parts and the contact logs
FT_DOL_ARRAY_VECTOR(soCollisionAttackAbsolute, 1);
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 3);

// The parameter block of the mine (4 float and 1 int parameters, ids 4000.. and 24000).
// HYPOTHESIS: meaning of the parameters.
struct wnSnakeMineParam {
    float unk0;
    s32 unk4;
    float unk8;
    float unkC;
    float unk10;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSnakeMineData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeMineParam* m_param; // +0x34
    u8 unk38[1];                     // start of the collision hit data set handed to the hit module
};

class wnSnakeMineParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 1, 0, 0> {
public:
    wnSnakeMineParamAccesser(const wnSnakeMineParam* param) : wnExtendParamAccesser<3999, 4, 23999, 1, 0, 0>(0x16) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk8;
        m_floats[2] = &param->unkC;
        m_floats[3] = &param->unk10;
        m_ints[0] = &param->unk4;
    }
    virtual ~wnSnakeMineParamAccesser() { }
};

// Camera module without subjects of its own (the subject array is the shared null array).
// HYPOTHESIS: config name.
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

// Collision search module of an article: like the fighters' builder, but the task category reaches it as the byte the
// search module takes (HYPOTHESIS: config name).
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
        m_parts(1, soCollisionSearchPart(soCollision::Category_Fighter), 0),
        m_groups(1, 0), m_searchModule(acc, taskId, category, &m_parts, true) { }
    T* getModule() { return &m_searchModule; }
};

class wnSnakeMineModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soPostureModuleBuildConfig<3, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 3, 1, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3FF, true>
        CollisionHitModuleBuildConfig;
    typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigWeapon<soCollisionSearchModuleImpl> >
        CollisionSearchModuleBuilder;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>, soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<5, soGeneralWorkBuildConfig<2, 2, 1>, 5, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 12> KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soAreaModuleBuildConfigWeapon<1, 4> AreaModuleBuildConfig;
};

// The mine's statuses (status data order).
enum wnSnakeMineStatus {
    wnSnakeMine_Status_Carried = 0, // HYPOTHESIS: brought out by Snake, before shoot() puts it down
    wnSnakeMine_Status_Armed = 1,   // lying on the ground, waiting for a target (proximity search active)
    wnSnakeMine_Status_Fall = 2,    // put down in the air: thrown with a spin until it lands
    wnSnakeMine_Status_Explode = 3, // detonation (proximity, or any damage)
};

// Work ids of the mine.
enum wnSnakeMineWork {
    wnSnakeMine_WorkFloat_PowerMul = 0x11000001,     // attack power multiplier handed to activate(), applied on explosion
    wnSnakeMine_WorkFlag_TargetInRange = 0x22000000, // a target was seen by the collision search this frame
};

// Parameter ids of the float parameters (wnSnakeMineParam unk0, unk8, unkC, unk10).
enum wnSnakeMineParamId {
    wnSnakeMine_Param_TriggerLift = 4000, // HYPOTHESIS: height model node 1 is raised by while armed
    wnSnakeMine_Param_FallSpeedY = 4001,
    wnSnakeMine_Param_FallAccelY = 4002,
    wnSnakeMine_Param_FallSpeedLimitY = 4003,
};

// Kinetic energy ids: the builder's common energies, then the mine's own ground movement.
enum wnSnakeMineEnergy {
    wnSnakeMine_Energy_Normal = 0, // the fall throw
    wnSnakeMine_Energy_Rot = 1,    // the spin while falling
    wnSnakeMine_Energy_Ground = 2, // ground movement (moves with the floor it lies on)
};

class wnSnakeMine : public wnWeaponBuilder<wnSnakeMineModuleAccesserBuildConfig>,
                    public soCollisionSearchEventObserver,
                    public soDamageEventObserver {
public:
    const wnSnakeMineParam* m_param;           // +0x2814
    wnSnakeMineParamAccesser m_paramAccesser;  // +0x2818
    soKineticEnergyGroundMovement m_energyGround; // +0x2854: kinetic energy 2, the outside ground movement

    wnSnakeMine(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeMineData* data);
    virtual ~wnSnakeMine() { }
    virtual void processCollision();
    virtual void updateNodeSRT();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
    void activate(int founderTaskId, int etcResourceId, int team, float powerMul);
    void shoot(const Vec3f& position);
};
static_assert(sizeof(wnSnakeMine) == 0x2884, "wnSnakeMine layout is wrong!");
