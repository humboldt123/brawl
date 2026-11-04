#pragma once

// Snake's C4 charge (article kind 0x18), his down special: the plastic explosive that sticks to the ground, to a wall or
// to a fighter and blows up when Snake presses his detonator (wnSnakeC4Switch). It searches for fighters to stick to
// (soCollisionSearchEventObserver), falls with a gravity energy and slides with a ground movement energy.

#include <so/collision/so_collision_search_event_presenter.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <so/camera/so_camera_module_simple.h>
#include <ft/ft_kinetic_energy.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_weapon_builder.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(8);
// The array instances the sora_melee constructors build.
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 2);
FT_DOL_ARRAY_VECTOR(soControllerImpl, 1);
FT_DOL_ARRAY_VECTOR(soControllerClatter, 1);
FT_DOL_ARRAY_VECTOR(soAreaContactLog, 4);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soKineticEnergy*>, 4);
// The manager of the charge's four energies: its constructor and methods are sora_melee's.
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

// The parameter block of the charge (reached through the data block at +0x34).
// HYPOTHESIS: meaning of the fields.
struct wnSnakeC4Param {
    float m_throwSpeedY; // +0x0: vertical speed the thrown charge starts falling with
    float m_gravity;    // +0x4: downward acceleration while the charge falls
    float m_speedLimit; // +0x8: falling speed limit
    // HYPOTHESIS: the mash-off frames of a fighter the charge sticks to are m_clatterFrames - m_clatterFramesBase.
    s32 m_clatterFrames;     // +0xC
    s32 m_clatterFramesBase; // +0x10
    float m_clatterRate;     // +0x14
    float m_dropDamage; // +0x18: damage to the fighter it sticks to that makes it fall off for sure
    s32 m_life;         // +0x1C: frames the charge stays before it fizzles out
    float m_blinkChance; // +0x20: 1 / chance of the alternate look on activation
    float m_groundStickRange; // +0x24: reach of the stick test below the charge while stuck to the ground
    float m_wallStickRange;   // +0x28: reach of the stick test to the side while stuck to a wall
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSnakeC4Data {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeC4Param* m_param; // +0x34
};

// The controller of the charge reads its owner's controller through the link (one controller and one clatter slot).
template <>
class soControllerModuleBuilder<wnControllerModuleBuildConfig<soControllerModuleLinkRef, 1, 1> > :
    public soArraySelectHolder<1, soArrayVector<soControllerImpl, 1>, soArrayNull<soControllerImpl> > {
    soArraySelectHolder<1, soArrayVector<soControllerClatter, 1>, soSingletonHolder<soArrayNull<soControllerClatter> > > m_clatters;
    soControllerModuleLinkRef m_controllerModule;
public:
    soControllerModuleBuilder(soModuleAccesser* acc, s16 manageId) :
        soArraySelectHolder<1, soArrayVector<soControllerImpl, 1>, soArrayNull<soControllerImpl> >(1, 0), m_clatters(1, 0),
        m_controllerModule(acc, manageId, this->get(), m_clatters.get()) { }
    soControllerModuleLinkRef* getModule() { return &m_controllerModule; }
};

// The camera module of the charge has no camera subject of its own (the shared null subject array).
// HYPOTHESIS: config spelling.
template <typename T>
class wnCameraModuleBuildConfigNoSubject {
public:
    typedef T ModuleType;
};
template <typename T>
class soCameraModuleBuilder<wnCameraModuleBuildConfigNoSubject<T> > {
    soSingletonHolder<soArrayNull<soCameraSubject> > m_subjects;
    T m_cameraModule;
public:
    soCameraModuleBuilder(soModuleAccesser* acc, soSet<soCameraRange>* rangeSet, soSet<soCameraClipSphere>* clipSet,
                          soEventObserverRegistrationDesc* regDesc) :
        m_cameraModule(acc, soSingletonHolder<soArrayNull<soCameraSubject> >::getInstance(), rangeSet, clipSet, regDesc) { }
    T* getModule() { return &m_cameraModule; }
};

// The fighter search of the charge (one search part). The builder takes the owner's task category as a byte, like the
// article attack builder. HYPOTHESIS: config spelling.
template <typename T>
class wnCollisionSearchModuleBuildConfig {
public:
    typedef T ModuleType;
};
template <typename T>
class soCollisionSearchModuleBuilder<wnCollisionSearchModuleBuildConfig<T> > {
    soArrayVector<soCollisionSearchPart, 1> m_parts;
    soArrayVector<soCollisionGroup, 1> m_groups;
    T m_searchModule;
public:
    soCollisionSearchModuleBuilder(soModuleAccesser* acc, int taskId, u8 category) :
        m_parts(1, soCollisionSearchPart(soCollision::Category_Fighter), 0),
        m_groups(1, 0), m_searchModule(acc, taskId, category, &m_parts, true) { }
    T* getModule() { return &m_searchModule; }
};

class wnSnakeC4ModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<7, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<2, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soCollisionSearchModuleBuilder<wnCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > CollisionSearchModuleBuilder;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 1, 1> ControllerModuleBuildConfig;
    typedef wnCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<8, soGeneralWorkBuildConfig<2, 2, 1>, 8, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 4> KineticModuleBuildConfig;
    typedef soAreaModuleBuildConfigWeapon<1, 4> AreaModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 8, 0, 0, 1, 7, 0, 0, 1, 7, 0, 0, 1, 7, 0, 0> AnimCmdModuleSubBuildConfig;
};

// The link event that sets the charge's visibility (kind 0x37; same layout as the Pikmin one).
struct wnSnakeC4LinkEventVisibility : soLinkEventArgs {
    u8 m_visible; // +0x5
    wnSnakeC4LinkEventVisibility(int kind) : soLinkEventArgs(kind), m_visible(0) { }
};

class wnSnakeC4 : public wnWeaponBuilder<wnSnakeC4ModuleAccesserBuildConfig>, public soCollisionSearchEventObserver {
public:
    const wnSnakeC4Param* m_param;                       // +0x23E8
    wnKineticEnergyGravity m_energyGravity;              // +0x23EC: the fall (energy 2)
    soKineticEnergyGroundMovement m_energyGroundMovement; // +0x2400: carried by the ground it sticks to (energy 3)

    wnSnakeC4(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4Data* data);
    virtual ~wnSnakeC4() { }
    void activate(int founderTaskId, int etcResourceId, int team, float lr);
    void shoot();
    virtual void processUpdate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void updateNodeSRT();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
};

static_assert(sizeof(wnSnakeC4) == 0x2430, "C4 article layout");
