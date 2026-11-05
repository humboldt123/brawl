#define WN_PIT_IKARUS_FULL_BUILDER // the complete Ikarus builder (see the header)
#define WN_WEAPON_BUILDER
#define IP_INPUT_BUTTON_RAW // BYTES: IpNull::getButton returns the raw button mask (8 bytes, not a struct copy)
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#ifdef WN_PIT_IKARUS_FULL_BUILDER
#define SO_KINETIC_ENERGY_3F_FULL // BYTES: the builder holds the full 3f energy (size 0x48); its dtor is then expanded in this unit
#endif
#define WN_KINETIC_GENERIC_BUILDER
#define SO_KINETIC_ENERGY_3F_DEFAULT_CTOR // BYTES: the Ikarus pool constructs its 3f energy here (five Vec3f defaults)
#include <wn/wn_module_accesser_builder.h>
// The status unique-process vector and the hit part vector are built by their sora_melee constructors.
WN_STATUS_PROC_VECTOR_ARG_CTOR(5);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/kinetic/so_kinetic_transactor.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_type_list.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/pit/wn_pit_ikarus.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/wn_kinetic_transactor.h>

#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/damage/so_damage.h>

// The Ikarus emits the damage observer's weak teardown and its empty event handlers (the vtable it stores).
inline soDamageEventObserver::~soDamageEventObserver() { }
inline void soDamageEventObserver::notifyEventOnDamage(soDamage*, bool, soModuleAccesser*) { }
inline void soDamageEventObserver::notifyEventAddDamage(soDamage*, soModuleAccesser*) { }
// The gravity energy's weak teardown is emitted here too (its pool lives in the Ikarus builder).
inline wnKineticEnergyGravity::~wnKineticEnergyGravity() { }

// The status processes are defined in their own units; this unit only hands out their instances.
class wnPitIkarusStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    static wnPitIkarusStatusUniqProcessEntry* getInstance();
    virtual ~wnPitIkarusStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};
class wnPitIkarusStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    static wnPitIkarusStatusUniqProcessTurn* getInstance();
    virtual ~wnPitIkarusStatusUniqProcessTurn() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
class wnPitIkarusStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    static wnPitIkarusStatusUniqProcessAttack* getInstance();
    virtual ~wnPitIkarusStatusUniqProcessAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};
extern wnPitIkarusStatusUniqProcessEntry g_wnPitIkarusStatusUniqProcessEntry;
extern wnPitIkarusStatusUniqProcessTurn g_wnPitIkarusStatusUniqProcessTurn;
extern wnPitIkarusStatusUniqProcessAttack g_wnPitIkarusStatusUniqProcessAttack;

inline wnPitIkarusStatusUniqProcessEntry* wnPitIkarusStatusUniqProcessEntry::getInstance() {
    return &g_wnPitIkarusStatusUniqProcessEntry;
}
inline wnPitIkarusStatusUniqProcessTurn* wnPitIkarusStatusUniqProcessTurn::getInstance() {
    return &g_wnPitIkarusStatusUniqProcessTurn;
}
inline wnPitIkarusStatusUniqProcessAttack* wnPitIkarusStatusUniqProcessAttack::getInstance() {
    return &g_wnPitIkarusStatusUniqProcessAttack;
}

// Hit-stop constants passed to soStopModuleImpl::calcHitStopFrame (same values and layout as in
// wn_snake_nikita_missile.cpp): MWCC keeps these external-linkage objects at the start of .rodata, ahead of the
// builder constructor's pooled literals.
// HYPOTHESIS: names and the exact original declaration (likely an extern declared in a shared weapon header).
extern const float g_wnHitStopFrameMul = 0.3846154f;
extern const float g_wnHitStopFrameAdd = 5.0f;
extern const float g_wnHitStopFrameMax = 30.0f;

#pragma dont_inline on // BYTES: the native constructor calls every helper (builder, accessors, getInstance) out of line
wnPitIkarus::wnPitIkarus(s32 articleId, const wnArticleBuildInfo& info, const wnPitIkarusData* data) :
    wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>(
        "PitIkarus", articleId, info,
        wnModuleAccesserBuildData(0x7d, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &unk22D8,
                                  0, 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1), unk22D4(data->m_param), unk22D8(data->m_param) {
    soStatusUniqProcess* uniqProcs[5] = {};
    uniqProcs[0] = wnPitIkarusStatusUniqProcessEntry::getInstance();
    uniqProcs[1] = wnPitIkarusStatusUniqProcessTurn::getInstance();
    uniqProcs[3] = wnPitIkarusStatusUniqProcessAttack::getInstance();
    getModuleAccesser()->getStatusModule().addRangeUniqProc(uniqProcs, 5);
    getModuleAccesser()->getCollisionHitModule().add(
        reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}
#pragma dont_inline reset

wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}

void wnPitIkarus::activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0,
                        pos, 0.0f, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagBit7 = true;
    desc.flagBit6 = false;
    desc.flagBit5 = false;
    desc.flagBit4 = false;
    desc.flagBit3 = false;
    desc.flagBit2 = false;
    desc.flagBit1 = false;
    desc.flagBit0 = false;
    desc.flag4DBit7 = false;
    desc.flag4DBit6 = false;
    Weapon::activate(&desc);
    m_moduleAccesser->getTeamModule().setHitTeam(-1);
    m_moduleAccesser->getTeamModule().set2nd(5);
    m_moduleAccesser->getTeamModule().setHit2nd(5);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000001);
    m_moduleAccesser->getPostureModule().setScale(unk2);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

bool wnPitIkarus::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags != 0) {
        m_moduleAccesser->getStatusModule().changeStatusForce(4, m_moduleAccesser);
        return true;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPitIkarus::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: a hit while the Ikarus is in its fly statuses (2-3) that is strong enough sends it back the way it came.
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 2:
    case 3:
        if (damage->m_damage >= soValueAccesser::getConstantFloat(moduleAccesser, 0xFAA, 0)) {
            m_moduleAccesser->getWorkManageModule().setFloat(-damage->m_lr, 0x11000001);
            moduleAccesser->getPostureModule().setLr(damage->m_lr);
            moduleAccesser->getStatusModule().changeStatusForce(4, moduleAccesser);
        }
        if (damage->m_reaction > 0.0f) {
            int frames = soStopModuleImpl::calcHitStopFrame(damage->m_powerMax, 0.3846154f, 5.0f, 1.0f, 30.0f);
            moduleAccesser->getStopModule().setHitStopFrame(frames, true);
        }
        break;
    }
}

#ifndef WN_PIT_IKARUS_FULL_BUILDER
soKineticEnergy3f::~soKineticEnergy3f() { }
#endif


template <typename E>
void soKineticTransactor::updateEnergy(E* energy, soModuleAccesser* moduleAccesser) {
    if (energy->isEnable() == true && !energy->isSuspend())
        energy->updateEnergy(moduleAccesser);
}
template void soKineticTransactor::updateEnergy<soKineticEnergy3f>(soKineticEnergy3f*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<soKineticEnergyRotNormal>(soKineticEnergyRotNormal*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<wnKineticEnergyNormal>(wnKineticEnergyNormal*, soModuleAccesser*);
template void soKineticTransactor::updateEnergy<wnKineticEnergyGravity>(wnKineticEnergyGravity*, soModuleAccesser*);

// HYPOTHESIS: the transactor reaches an energy of the Ikarus instance pools through an overload selected by an empty
// soIntToType<pool level> tag (as in wn_diddy_barrel_kinetic_transactor.cpp).
static inline soKineticEnergy3f* getEnergy(void* pools, const soIntToType<0>&) {
    return reinterpret_cast<soKineticEnergy3f*>(static_cast<u8*>(pools) + 0x10);
}
static inline soKineticEnergyRotNormal* getEnergy(void* pools, const soIntToType<1>&) {
    return reinterpret_cast<soKineticEnergyRotNormal*>(static_cast<u8*>(pools) + 0x64);
}

static inline const soKineticEnergy3f* getEnergy(const void* pools, const soIntToType<0>&) {
    return reinterpret_cast<const soKineticEnergy3f*>(static_cast<const u8*>(pools) + 0x10);
}
static inline const wnKineticEnergyNormal* getEnergy(const void* pools, const soIntToType<2>&) {
    return reinterpret_cast<const wnKineticEnergyNormal*>(static_cast<const u8*>(pools) + 0xB4);
}

void wnPitIkarusKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soKineticEnergy3f* energy = getEnergy(pools, soIntToType<0>());
    energy->resetEnergy(-1, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* moduleAccesser) const {
    moduleAccesser->getKineticModule().unableEnergyAll();

    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
    float angle = 90.0f * workManage.getFloat(0x21000000) / frames;
    soKineticEnergyRotNormal* energy = getEnergy(pools, soIntToType<1>());
    Vec3f rotation(0.0f, angle, 0.0f);
    energy->m_rotSpeed = rotation;
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    wnKineticEnergyNormal* energy = const_cast<wnKineticEnergyNormal*>(getEnergy(pools, soIntToType<2>()));
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKineticSub(const void* pools, soModuleAccesser* moduleAccesser) const {
    moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    moduleAccesser->getKineticModule().unableEnergyAll();

    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    Vec3f speed;
    speed.m_x = moduleAccesser->getWorkManageModule().getFloat(0x11000001) * params->getParamFloat(moduleAccesser, 0xFA4, 0);
    speed.m_y = params->getParamFloat(moduleAccesser, 0xFA5, 0);
    float rnd = randf();
    float high = params->getParamFloat(moduleAccesser, 0xFA9, 0);
    float low = params->getParamFloat(moduleAccesser, 0xFA8, 0);
    speed.m_z = low + rnd * (high - low);
    soKineticEnergy3f* energy = const_cast<soKineticEnergy3f*>(getEnergy(pools, soIntToType<0>()));
    energy->m_speed3f = speed;

    float gravity = -params->getParamFloat(moduleAccesser, 0xFA6, 0);
    Vec3f accel;
    accel.m_x = 0.0f;
    accel.m_y = gravity;
    accel.m_z = 0.0f;
    energy->m_accel3f = accel;
    float fall = -params->getParamFloat(moduleAccesser, 0xFA7, 0);
    Vec3f limit;
    limit.m_x = -1.0f;
    limit.m_y = fall;
    limit.m_z = -1.0f;
    energy->m_limit3f = limit;
    energy->enable();
}

void wnPitIkarusKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x1F) {
        wnKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }

    switch (mode) {
    case 0x20: {
        wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    case 0x21: {
        const wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    case 0x22: {
        wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(static_cast<const void*>(pools), moduleAccesser);
        break;
    }
    case 0x23: {
        const wnPitIkarusKineticTransactor empty = {};
        empty.changeKineticSub(static_cast<const void*>(pools), moduleAccesser);
        break;
    }
    }
}
