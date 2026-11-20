// MATCH-ONLY: retain the native vector helper calls and aggregate copies.
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_SCALE_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define WN_WEAPON_BUILDER
#define SO_GENERAL_WORK_SIMPLE_CTOR_INLINE
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/poke/wn_poke_fushigisou_leaf_cutter.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <mt/mt_prng.h>
#include <math.h>


extern soStatusUniqProcess g_wnPokeFushigisouLeafCutterStatusUniqProcessMove;

wnPokeFushigisouLeafCutter::wnPokeFushigisouLeafCutter(s32 articleId, const wnArticleBuildInfo& info,
                                                       const wnPokeFushigisouLeafCutterData* data) :
    wnWeaponBuilder<wnPokeFushigisouLeafCutterModuleAccesserBuildConfig>(
        "PokeFushigisouLeafCutter", articleId, info,
        wnModuleAccesserBuildData(73, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[2] = {0, 0};
    uniqProcs[0] = &g_wnPokeFushigisouLeafCutterStatusUniqProcessMove;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}

// HYPOTHESIS: descriptive names, corroborated by the movement-status reader.
enum {
    Work_Float_SpinAngle = 0x11000001,
    Work_Float_SpinStep = 0x11000002,
    Work_Float_Heading = 0x11000003,
    Work_Float_TargetHeading = 0x11000004,
};

void wnPokeFushigisouLeafCutter::activate(int founderTaskId, int resourceId, int team,
                                        Vec3f* position, float lr, float speed, float angle) {
    const wnPokeFushigisouLeafCutterParam* param;
    int life = m_param->lifeFrames;
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, *position, lr, team, life, 5,
                        0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);

    float sine = (float)sin(0.017453292f * angle);
    float cosine = (float)cos(0.017453292f * angle);
    Vec2f velocity(lr * (speed * cosine), speed * sine);
    float brake = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    wnKineticEnergyNormal& normal =
        dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    normal.resetEnergy(1, &velocity, &Vec3f(0.0f, 0.0f, 0.0f), m_moduleAccesser);
    Vec2f braking;
    braking.m_x = brake;
    braking.m_y = 0.0f;
    normal.m_brake = braking;
    normal.enable();
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&velocity);

    param = m_param;
    float spin = param->spinMin + (param->spinMax - param->spinMin) * randf();
    m_moduleAccesser->getWorkManageModule().setFloat(spin, Work_Float_SpinStep);
    m_moduleAccesser->getWorkManageModule().setFloat(angle, Work_Float_Heading);
    m_moduleAccesser->getWorkManageModule().setFloat(angle, Work_Float_TargetHeading);
    updateNodeSRT();
}

// The root follows the article's posture. Node 1 spins in the flight plane;
// its 90-degree Z rotation places the leaf model flat in that plane.
void wnPokeFushigisouLeafCutter::updateNodeSRT() {
    Vec3f scale;
    Vec3f rotation;
    Vec3f leafRotation;
    Vec3f position;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    rotation.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    leafRotation.m_x = 0.0f;
    leafRotation.m_y = getModuleAccesser()->getWorkManageModule().getFloat(Work_Float_Heading) +
                       getModuleAccesser()->getWorkManageModule().getFloat(Work_Float_SpinAngle);
    leafRotation.m_z = 90.0f;
    m_moduleAccesser->getModelModule().setNodeRotate(1, &leafRotation);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

bool wnPokeFushigisouLeafCutter::notifyEventAnimCmd(acAnimCmd* cmd,
                                                  soModuleAccesser* accesser, int index) {
    if (Weapon::notifyEventAnimCmd(cmd, accesser, index)) return true;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) return false;
    if (cmd->getType() <= -1 || cmd->getType() >= 11) return false;
    switch (cmd->getType()) {
    case 6: {
        wnKineticEnergyNormal& normal =
            dynamic_cast<wnKineticEnergyNormal&>(*accesser->getKineticModule().getEnergy(0));
        Vec2f velocity = normal.getSpeed();
        Vec2f surface;
        Vec2f::copy(surface, accesser->getGroundModule().getTouchNormal(
            static_cast<grCollStatus::TouchMask>(0xFF), 0));
        float incidence = 57.29578f * surface.angle(&velocity);
        if (incidence - 90.0f < soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA4, 0)) {
            float retention = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA5, 0);
            float lr = m_moduleAccesser->getPostureModule().getLr();
            float dot = surface.m_x * velocity.m_x + surface.m_y * velocity.m_y;
            // MATCH-ONLY: retain the native vector expression value copies.
            Vec2f doubledResult;
            surface.scale(2.0f, &doubledResult);
            Vec2f doubledNormal = doubledResult;
            Vec2f projectionResult;
            doubledNormal.scale(dot, &projectionResult);
            Vec2f projection = projectionResult;
            Vec2f reflected = velocity - projection;
            velocity.m_x = reflected.m_x;
            velocity.m_y = reflected.m_y;
            velocity.scale(retention, &velocity);
            normal.m_speed = velocity;
            float angle = 57.29578f * (float)atan2(velocity.m_y, velocity.m_x * lr);
            m_moduleAccesser->getWorkManageModule().setFloat(angle, Work_Float_TargetHeading);
        } else {
            m_moduleAccesser->getStatusModule().changeStatusForce(1, m_moduleAccesser);
        }
        return true;
    }
    }
    return false;
}

bool wnPokeFushigisouLeafCutter::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) return false;
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// TODO: clean up, give variables real names
namespace jj_22E10 {
extern "C" void* fn_116_3884(void*, short);
extern "C" void* fn_116_12F0C(void*, short);
extern "C" void* fn_116_12F98(void*, short);
extern "C" void* fn_116_13024(void*, short);
extern "C" void* fn_116_22EA0(void*, short);

extern "C" void* fn_116_22E10(void* object, short deleting) {
    if (object != 0) {
        unsigned char* storage = static_cast<unsigned char*>(object);
        fn_116_3884(storage + 0x2bc, -1);
        fn_116_12F0C(storage + 0x208, -1);
        fn_116_12F98(storage + 0x154, -1);
        fn_116_13024(storage + 0xa0, -1);
        fn_116_22EA0(object, -1);
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_230AC {
extern "C" void __dt__19soGeneralWorkSimpleFv(void*, short);
extern "C" void __dl__FPv(void*);

extern "C" void* fn_116_230AC(void* object, short deleting) {
    if (object != 0) {
        __dt__19soGeneralWorkSimpleFv(static_cast<char*>(object) + 0x54, -1);
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_23AB0 {
struct Storage { void* primary; void* secondary; };
struct Registration { void* next; void* object; void* destructor; };
class View {
public:
    virtual void slot00(); virtual void slot01(); virtual void slot02();
    virtual void slot03(); virtual void slot04(); virtual void slot05();
    virtual void dispatch();
};
extern "C" {
extern signed char lbl_116_bss_1F4;
extern Storage lbl_116_bss_1F8;
extern Registration lbl_116_bss_5B0;
extern unsigned char lbl_116_data_26C4[], lbl_116_data_2698[];
extern unsigned char lbl_116_data_2630[], lbl_116_data_74F8[];
void* __dt__21soArticleMediatorNullFv(void*, short);
void __register_global_object(void*, void*, void*);
void fn_116_23AB0() {
    if (lbl_116_bss_1F4 == 0) {
        lbl_116_bss_1F8.primary = lbl_116_data_26C4;
        lbl_116_bss_1F8.secondary = lbl_116_data_2698;
        lbl_116_bss_1F8.primary = lbl_116_data_2630;
        lbl_116_bss_1F8.secondary = lbl_116_data_2630 + 0x10;
        lbl_116_bss_1F8.primary = lbl_116_data_74F8;
        lbl_116_bss_1F8.secondary = lbl_116_data_74F8 + 0x10;
        __register_global_object(&lbl_116_bss_1F8, reinterpret_cast<void*>(__dt__21soArticleMediatorNullFv), &lbl_116_bss_5B0);
        lbl_116_bss_1F4 = 1;
    }
    reinterpret_cast<View*>(&lbl_116_bss_1F8)->dispatch();
}
}
}

// TODO: clean up, give variables real names
namespace jj_23B5C {
extern "C" void fn_116_22D18(void*);

extern "C" void fn_116_23B5C(void* self) {
    fn_116_22D18(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_23B64 {
extern "C" void fn_116_22D18(void*);

extern "C" void fn_116_23B64(void* self) {
    fn_116_22D18(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_23B6C {
extern "C" void fn_116_22D18(void*);

extern "C" void fn_116_23B6C(void* self) {
    fn_116_22D18(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_23B74 {
extern "C" void fn_116_1D740(void*);

extern "C" void fn_116_23B74(void* self) {
    fn_116_1D740(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_23B7C {
extern "C" void notifyEventAnimCmd__26wnPokeFushigisouLeafCutterFP9acAnimCmdP16soModuleAccesseri(void*);

extern "C" void fn_116_23B7C(void* self) {
    notifyEventAnimCmd__26wnPokeFushigisouLeafCutterFP9acAnimCmdP16soModuleAccesseri(reinterpret_cast<u8*>(self) - 0x48);
}
}

// TODO: clean up, give variables real names
namespace jj_23B84 {
extern "C" void notifyEventCollisionAttackCheck__26wnPokeFushigisouLeafCutterFUl(void*);

extern "C" void fn_116_23B84(void* self) {
    notifyEventCollisionAttackCheck__26wnPokeFushigisouLeafCutterFUl(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_23B8C {
extern "C" void fn_116_1D740(void*);

extern "C" void fn_116_23B8C(void* self) {
    fn_116_1D740(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_23B94 {
extern "C" void fn_116_23AB0(void*);

extern "C" void fn_116_23B94(void* self) {
    fn_116_23AB0(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_23B9C {
extern "C" void fn_116_1D740(void*);

extern "C" void fn_116_23B9C(void* self) {
    fn_116_1D740(reinterpret_cast<u8*>(self) - 0x88);
}
}

void operator delete(void*);
// TODO: clean up, give variables real names
namespace jj_22D18 {

extern "C" {
void* fn_116_37E8(void*, short);
void* __dt__21wnKineticEnergyNormalFv(void*, short);
void* fn_116_22DA0(void*, short);
void* __dt__6WeaponFv(void*, short);
}

extern "C" void* fn_116_22D18(void* object, short deleting) {
    if (object != 0) {
        unsigned char* members = static_cast<unsigned char*>(object);
        short memberDeleting = -1;
        fn_116_37E8(members + 0x1670, memberDeleting);
        __dt__21wnKineticEnergyNormalFv(members + 0x1638, memberDeleting);
        fn_116_22DA0(members + 0xCC, memberDeleting);
        __dt__6WeaponFv(object, static_cast<short>(0));
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_22DA0 {
extern "C" void* fn_116_22E10(void*, short);
extern "C" void* fn_116_22F2C(void*, short);

extern "C" void* fn_116_22DA0(void* object, short deleting) {
    if (object != 0) {
        fn_116_22E10(static_cast<unsigned char*>(object) + 0x1228, -1);
        fn_116_22F2C(object, 0);
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_231EC {

extern "C" void* __dt__Q215soKineticEnergy13AttributeFlagFv(void*, short);

extern "C" void* fn_116_231EC(void* object, short deleting) {
    if (object != 0) {
        if (object != 0) {
            short baseDeleting = 0;
            __dt__Q215soKineticEnergy13AttributeFlagFv(object, baseDeleting);
        }
        if (deleting > 0) {
            operator delete(object);
        }
    }
    return object;
}
}

