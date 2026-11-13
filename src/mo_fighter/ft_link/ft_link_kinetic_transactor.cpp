// ftLinkKineticTransactor: changeKinetic and its sub-dispatchers (native .text 0xECC8..0xF26C).
// BYTES: the native code assigns the vectors through the out-of-line operators.
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_SUB_FRESH_REGS
#define MT_VEC3F_SUB_NOINLINE
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_SCALE_XY_IN_PLACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/link/ft_link.h>
#include <ft/link/ft_link_status_final.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Link's own kinetic modes: 0x64..0x66 set up the hookshot/dash energies, 0x67..0x6d have no handler.
#pragma push
#pragma opt_propagation off
void ftLinkKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }

    soModuleAccesser* processAccesser = moduleAccesser;
    bool process = true;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(processAccesser, process, process);
    switch (mode) {
    case 0x64: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub(pools, &speed, moduleAccesser);
        break;
    }
    case 0x65: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub1(pools, &speed, moduleAccesser);
        break;
    }
    case 0x66: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub2(pools, &speed, moduleAccesser);
        break;
    }
    case 0x67: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub3(pools, &speed, moduleAccesser);
        break;
    }
    case 0x68: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub4(pools, &speed, moduleAccesser);
        break;
    }
    case 0x69: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub5(pools, &speed, moduleAccesser);
        break;
    }
    case 0x6a: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub6(pools, &speed, moduleAccesser);
        break;
    }
    case 0x6b: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub7(pools, &speed, moduleAccesser);
        break;
    }
    case 0x6c: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub8(pools, &speed, moduleAccesser);
        break;
    }
    case 0x6d: {
        ftLinkKineticTransactor empty = {};
        empty.changeKineticSub9(pools, &speed, moduleAccesser);
        break;
    }
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(moduleAccesser);
}
#pragma pop

void ftLinkKineticTransactor::changeKineticSub(void* pools, Vec2f* prevSpeed, soModuleAccesser* moduleAccesser) {
    ftKineticTransactor::changeKineticImpl(0, pools, prevSpeed, moduleAccesser);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(
        *moduleAccesser->getKineticModule().getEnergy(1));
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        gravity.m_speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA5, 0);
    }
    gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xBCF, 0) *
                        soValueAccesser::getConstantFloat(moduleAccesser, 0xFA6, 0);
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(
        *moduleAccesser->getKineticModule().getEnergy(2));
    controller.m_accelMul = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA8, 0);
    controller.mulXSpeedMax(soValueAccesser::getConstantFloat(moduleAccesser, 0xFA9, 0));
}

// BYTES: mt_vector.h declares Vec3f::length out of line (other units such as ft_ike carry the weak copy);
// this unit expands the same body inline at its call.
inline float Vec3f::length() {
    float lengthSquared = m_z * m_z + (m_x * m_x + m_y * m_y);
    if (static_cast<float>(__fabs(lengthSquared)) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSquared * rsqrtf(lengthSquared);
}

// Pulls Link toward the hookshot's objective point: the speed is the offset, capped at a constant.
void ftLinkKineticTransactor::changeKineticSub1(void* pools, Vec2f* prevSpeed, soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    ftKineticTransactor::changeKineticImpl(10, pools, prevSpeed, moduleAccesser);
    kinetic.clearSpeedAll();
    kinetic.unableEnergyAll();
    Vec3f objective;
    objective = ftLinkStatusUniqProcessFinalDash::getObjectivePos(moduleAccesser);
    Vec3f diff;
    diff = objective - posture.getPos();
    if (diff.length() > 1.0f) {
        diff.normalize();
        Vec3fScale(&diff, &diff, soValueAccesser::getConstantFloat(moduleAccesser, 0xFAE, 0));
    }
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    Vec2f speed;
    speed.m_x = diff.m_x;
    speed.m_y = diff.m_y;
    stop.m_speed = speed;
    Vec2f limit;
    limit.m_x = diff.m_x;
    limit.m_y = diff.m_y;
    stop.m_speedLimit = limit;
    Vec2f brake;
    brake.m_x = 0.0f;
    brake.m_y = 0.0f;
    stop.m_brake = brake;
    stop.enable();
}

void ftLinkKineticTransactor::changeKineticSub2(void* pools, Vec2f* prevSpeed, soModuleAccesser* moduleAccesser) {
    ftKineticTransactor::changeKineticImpl(0x11, pools, prevSpeed, moduleAccesser);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(
        *moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(1, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.m_speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAF, 0);
    gravity.m_gravity = 0.0f;
    gravity.enable();
}
