#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_kinetic_utility.h>
#include <ft/ft_common_data_accesser.h>
#include <so/stageobject.h>
#include <so/so_module_accesser.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/damage/so_damage.h>
#include <so/so_value_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <nw4r/math/math_arithmetic.h>

// HYPOTHESIS: ftKineticTransactor::changeKineticSub..changeKineticSub81 are the kinetic setups behind the mode switch of
// ftKineticTransactor::changeKineticImpl: each resets and enables the fighter's kinetic energies (0 motion, 1 gravity,
// 2 controller, 3 stop, ...) for one kind of status entry. `speed` is the speed the fighter had before the change.

// Clamps a speed to [lo, hi] the way the original inlines it (two selects).
static inline float clampSpeed(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

void ftKineticTransactor::changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 6, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub3(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    float stickScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xBCA, 0);
    velocity.m_x = stickScale * moduleAccesser->getControllerModule().getStickX();
    float baseY = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC9, 0);
    velocity.m_y = baseY * soValueAccesser::getConstantFloat(moduleAccesser, 0xBC5, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub4(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    float stickScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xBCA, 0);
    velocity.m_x = stickScale * moduleAccesser->getControllerModule().getStickX();
    float baseY = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC9, 0);
    velocity.m_y = baseY * soValueAccesser::getConstantFloat(moduleAccesser, 0xBC5, 0);
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 5, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub5(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 8, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub7(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity;
    velocity.m_x = speed->m_x;
    float lrScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xBF1, 0);
    velocity.m_x += lrScale * moduleAccesser->getPostureModule().getLr();
    velocity.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xBF2, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub8(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity;
    velocity.m_x = speed->m_x;
    velocity.m_x = 0.0f;
    float frame = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    velocity.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xBF2, 0) * frame;
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub12(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 0, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub13(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f clamped = *speed;
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xBBD, 0);
    clamped.m_x = clampSpeed(clamped.m_x, -limit, limit);
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 18, &clamped, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub14(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    float y = speed->m_y;
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 7, &Vec2f(0.0f, y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 19, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub16(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec3f rotA(0.0f, 0.0f, 0.0f);
    Vec3f rotB(0.0f, 0.0f, 0.0f);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, speed->m_y), &rotA);
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 6, &Vec2f(speed->m_x, 0.0f), &rotB);
}

void ftKineticTransactor::changeKineticSub17(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 7, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub18(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 8, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub19(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 9, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub20(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 0, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub21(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 1, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub22(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub23(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f clamped = *speed;
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC3, 0);
    clamped.m_x = clampSpeed(clamped.m_x, -limit, limit);
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, &clamped, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub24(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f clamped = *speed;
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC0, 0);
    clamped.m_x = clampSpeed(clamped.m_x, -limit, limit);
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, &clamped, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub27(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 3, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getKineticModule().getEnergy(4)->enable();
}

void ftKineticTransactor::changeKineticSub28(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 4, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub29(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 5, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub31(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 9, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub32(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 10, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub33(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 11, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub34(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f zero(0.0f, 0.0f);
    Vec3f rot(0.0f, 0.0f, 0.0f);
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 12, &zero, &rot);
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
    moduleAccesser->getWorkManageModule().onFlag(0x2200000d);
}

void ftKineticTransactor::changeKineticSub35(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f zero(0.0f, 0.0f);
    Vec3f rot(0.0f, 0.0f, 0.0f);
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 13, &zero, &rot);
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
    moduleAccesser->getWorkManageModule().onFlag(0x2200000d);
}

void ftKineticTransactor::changeKineticSub36(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 20, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub37(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 5, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub38(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 6, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub39(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 7, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub41(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 8, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f zero(0.0f, 0.0f);
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, &zero, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub57(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 0, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub59(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 12, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub60(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
}

void ftKineticTransactor::changeKineticSub61(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
}

void ftKineticTransactor::changeKineticSub63(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
}

void ftKineticTransactor::changeKineticSub64(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 10, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub65(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 13, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub66(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 14, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub67(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 15, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub69(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 15, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 2, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub71(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 16, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub72(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 16, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 3, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub73(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 16, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub74(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 4, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub75(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub77(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 7, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub78(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 22, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub79(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
}

void ftKineticTransactor::changeKineticSub30(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 4, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub45(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec3f rot(0.0f, 0.0f, 0.0f);
    float y = soValueAccesser::getConstantFloat(moduleAccesser, 0xBE7, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, y), &rot);
}

void ftKineticTransactor::changeKineticSub56(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 12, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
}

void ftKineticTransactor::changeKineticSub62(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
    moduleAccesser->getWorkManageModule().onFlag(0x2200000d);
}

void ftKineticTransactor::changeKineticSub70(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity(speed->m_x, 0.0f);
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD5, 0);
    if ((float)__fabs((double)velocity.m_x) > limit) {
        velocity.m_x = velocity.m_x < 0.0f ? -limit : limit;
    }
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 15, &velocity, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub76(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    float scale = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 7, &Vec2f(speed->m_x * scale, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 5, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub81(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    float y = -soValueAccesser::getConstantFloat(moduleAccesser, 0xDA2, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 6, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub48(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec3f rot(0.0f, 0.0f, 0.0f);
    float x = soValueAccesser::getConstantFloat(moduleAccesser, 0xC0C, 0) * moduleAccesser->getControllerModule().getStickX();
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 6, &Vec2f(x, 0.0f), &rot);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub49(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec3f rot(0.0f, 0.0f, 0.0f);
    float x = soValueAccesser::getConstantFloat(moduleAccesser, 0xC09, 0) * moduleAccesser->getPostureModule().getLr();
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 6, &Vec2f(x, 0.0f), &rot);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub50(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec3f rot(0.0f, 0.0f, 0.0f);
    float x = soValueAccesser::getConstantFloat(moduleAccesser, 0xC0A, 0) * moduleAccesser->getPostureModule().getLr();
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 6, &Vec2f(x, 0.0f), &rot);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, soValueAccesser::getConstantFloat(moduleAccesser, 0xC0B, 0)), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub53(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD5, 0);
    velocity.m_x = scale * moduleAccesser->getPostureModule().getLr();
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub54(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xC09, 0);
    velocity.m_x = scale * moduleAccesser->getPostureModule().getLr();
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub55(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    velocity.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xC94, 0);
    velocity.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xC95, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 2, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub58(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec3f rot(0.0f, 0.0f, 0.0f);
    float x = soValueAccesser::getConstantFloat(moduleAccesser, 0xCC6, 0) * -moduleAccesser->getPostureModule().getLr();
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 6, &Vec2f(x, 0.0f), &rot);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, soValueAccesser::getConstantFloat(moduleAccesser, 0xCC7, 0)), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub11(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity;
    velocity.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xC27, 0);
    float stick = moduleAccesser->getControllerModule().getStickX();
    velocity.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xC28, 0) * stick;
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
}

void ftKineticTransactor::changeKineticSub52(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    float a = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD5, 0);
    float b = soValueAccesser::getConstantFloat(moduleAccesser, 0xC9F, 0);
    velocity.m_x = a * b * moduleAccesser->getPostureModule().getLr();
    float c = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC5, 0);
    velocity.m_y = c * soValueAccesser::getConstantFloat(moduleAccesser, 0xBC9, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub68(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    velocity.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xD1B, 0);
    float limitX = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD5, 0);
    if ((float)__fabs((double)velocity.m_x) > limitX) {
        velocity.m_x = velocity.m_x < 0.0f ? -limitX : limitX;
    }
    float limitY = soValueAccesser::getConstantFloat(moduleAccesser, 0xBD0, 0);
    if ((float)__fabs((double)velocity.m_y) > limitY) {
        velocity.m_y = velocity.m_y < 0.0f ? -limitY : limitY;
    }
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 15, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 2, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub80(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = &moduleAccesser->getWorkManageModule();
    float base = soValueAccesser::getConstantFloat(moduleAccesser, 0x7F8, 0);
    float x = workManage->getFloat(0x21000001) * base;
    x = soValueAccesser::getConstantFloat(moduleAccesser, 0x7F9, 0) + x;
    x = x * -moduleAccesser->getWorkManageModule().getFloat(0x21000002);
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 0, &Vec2f(x, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub9(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed;
    float stickScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC4, 0);
    stickScale = stickScale * moduleAccesser->getControllerModule().getStickX();
    velocity.m_x = velocity.m_x * soValueAccesser::getConstantFloat(moduleAccesser, 0xBC6, 0) + stickScale;
    float offsetY = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC5, 0);
    velocity.m_y = velocity.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xC46, 0) + offsetY;
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xBC7, 0);
    if ((float)__fabs((double)velocity.m_x) > limit) {
        velocity.m_x = velocity.m_x < 0.0f ? -limit : limit;
    }
    velocity.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xD8F, 0);
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 0, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}


// HYPOTHESIS: bit 9 of the status's second flag word keeps the kinetic energy 0 untouched.
struct ftStatusKineticFlagData {
    u32 unk0;
    u32 : 9;
    u32 unk4_9 : 1;
    u32 : 22;
};

void ftKineticTransactor::notifyEventChangeStatus(void* a, void* b, void* c, void* d) {
    soModuleAccesser* moduleAccesser = static_cast<soModuleAccesser*>(d);
    int status = reinterpret_cast<int>(b);
    ftKind kind = static_cast<ftKind>(moduleAccesser->m_stageObject->soGetSubKind());
    if (reinterpret_cast<const ftStatusKineticFlagData*>(g_ftCommonDataAccesser.getFtStatusData(kind, status))->unk4_9 == 1) {
        moduleAccesser->getKineticModule().getEnergy(0)->clearSpeed();
    }
}

void ftKineticTransactor::enableOutsideEnergy(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200000d) == 1) {
        moduleAccesser->getKineticModule().getEnergy(5)->disable();
        moduleAccesser->getKineticModule().getEnergy(7)->disable();
    } else {
        moduleAccesser->getKineticModule().getEnergy(5)->enable();
        moduleAccesser->getKineticModule().getEnergy(7)->enable();
    }
}

void ftKineticTransactor::changeKineticSub51(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    float x = soValueAccesser::getConstantFloat(moduleAccesser, 0xC0A, 0) * moduleAccesser->getPostureModule().getLr() * soValueAccesser::getConstantFloat(moduleAccesser, 0xC99, 0);
    soKineticUtility::resetEnableEnergy(2, moduleAccesser, 3, &Vec2f(x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    float y = soValueAccesser::getConstantFloat(moduleAccesser, 0xC0B, 0);
    if (moduleAccesser->getWorkManageModule().getInt(0x10000003) - 1 > 0) {
        float decay = soValueAccesser::getConstantFloat(moduleAccesser, 0xC98, 0);
        y *= (float)pow(decay, (float)(moduleAccesser->getWorkManageModule().getInt(0x10000003) - 1));
    }
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, y), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub42(bool*, void* pools, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soDamageLog* damageLog = moduleAccesser->getDamageModule().getDamageLog();
    float y = damageLog->m_speed.m_y + moduleAccesser->getWorkManageModule().getFloat(0x11000010);
    float x = damageLog->m_speed.m_x + moduleAccesser->getWorkManageModule().getFloat(0x1100000F);
    Vec2f velocity(x, y);
    soKineticUtility::resetEnableEnergy(4, moduleAccesser, 1, &velocity, &Vec3f(0.0f, 0.0f, 0.0f));
    if (damageLog->m_isVector365) {
        *(bool*)((u8*)pools + 0x178) = true;
    }
}

void ftKineticTransactor::changeKineticSub43(bool*, void* pools, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soDamageLog* damageLog = moduleAccesser->getDamageModule().getDamageLog();
    float y = damageLog->m_speed.m_y + moduleAccesser->getWorkManageModule().getFloat(0x11000010);
    float x = damageLog->m_speed.m_x + moduleAccesser->getWorkManageModule().getFloat(0x1100000F);
    Vec2f velocity(x, y);
    soKineticUtility::resetEnableEnergy(4, moduleAccesser, 2, &velocity, &Vec3f(0.0f, 0.0f, 0.0f));
    if (damageLog->m_isVector365) {
        *(bool*)((u8*)pools + 0x178) = true;
    }
}

void ftKineticTransactor::changeKineticSub44(bool*, void* pools, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soDamageLog* damageLog = moduleAccesser->getDamageModule().getDamageLog();
    Vec2f velocity(damageLog->m_speed.m_x, damageLog->m_speed.m_y);
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xCD9, 0);
    float high = soValueAccesser::getConstantFloat(moduleAccesser, 0xBCF, 0);
    float low = soValueAccesser::getConstantFloat(moduleAccesser, 0xCD8, 0);
    velocity.m_y += scale * (high - low);
    soKineticUtility::resetEnableEnergy(4, moduleAccesser, 2, &velocity, &Vec3f(0.0f, 0.0f, 0.0f));
    if (damageLog->m_isVector365) {
        *(bool*)((u8*)pools + 0x178) = true;
    }
}

void ftKineticTransactor::changeKineticSub40(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    Vec2f velocity = *speed + moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2));
    float lr = moduleAccesser->getPostureModule().getLr();
    float boost = lr * soValueAccesser::getConstantFloat(moduleAccesser, 0xBBD, 0);
    if (velocity.m_x * lr >= 0.0f) {
        boost -= velocity.m_x;
    }
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 0x12, &Vec2f(velocity.m_x + boost, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

// HYPOTHESIS: the void* the transactor setups receive is the fighter's kinetic energy pool; its energy at +0x140 is a
// normal energy whose speed these setups scale.
void ftKineticTransactor::changeKineticSub25(bool*, void* pools, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticEnergyNormal* energy = reinterpret_cast<soKineticEnergyNormal*>((u8*)pools + 0x140);
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xC7F, 0);
    energy->m_speed = energy->getSpeed() * scale;
    soKineticUtility::resetEnableEnergy(3, moduleAccesser, 0, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftKineticTransactor::changeKineticSub26(bool*, void* pools, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticEnergyNormal* energy = reinterpret_cast<soKineticEnergyNormal*>((u8*)pools + 0x140);
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xC7F, 0);
    energy->m_speed = energy->getSpeed() * scale;
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, speed, &Vec3f(0.0f, 0.0f, 0.0f));
}

#pragma auto_inline off
Vec2f ftKineticTransactHelper::preHelpProcess(soModuleAccesser* moduleAccesser, bool clearKind, bool clearSpeed) {
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (clearSpeed == true) {
        moduleAccesser->getKineticModule().unableEnergyAll();
    }
    moduleAccesser->getKineticModule().getEnergy(4)->enable();
    moduleAccesser->getKineticModule().getEnergy(6)->enable();
    return speed;
}

void ftKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= -1 || mode >= 0x52) {
        return;
    }
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(moduleAccesser, true, mode != 0x31);
    changeKineticImpl(mode, pools, &speed, moduleAccesser);
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200000d) == 1) {
        moduleAccesser->getKineticModule().getEnergy(5)->disable();
        moduleAccesser->getKineticModule().getEnergy(7)->disable();
    } else {
        moduleAccesser->getKineticModule().getEnergy(5)->enable();
        moduleAccesser->getKineticModule().getEnergy(7)->enable();
    }
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x1100000f);
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000010);
}
#pragma auto_inline reset

#include <so/work/so_work_manage_module_impl.h>
#include <mt/mt_vector.h>
// TODO: clean up, give variables real names
namespace jj_15FBC8 {
extern "C" {
void changeKineticSub__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub2__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub3__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub6__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub7__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub8__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub12__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub13__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub14__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub15__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub16__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub17__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub18__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub19__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub20__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub22__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub27__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub28__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub29__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub30__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub31__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub32__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub33__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub34__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub35__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub37__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub38__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub39__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub40__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub41__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub42__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub43__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub44__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub45__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub46__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub47__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub48__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub49__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub50__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub51__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub53__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub52__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub54__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub55__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub56__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub57__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub58__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub60__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub59__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub61__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub62__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub63__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub64__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub65__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub66__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub67__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub68__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub69__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub70__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub71__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub72__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub73__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub74__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub75__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub76__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub77__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub78__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub1__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub11__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub21__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub9__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub10__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub36__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub79__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub80__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub81__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub25__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub26__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub4__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub5__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub23__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticSub24__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(char*, int, int, int);
void changeKineticImpl__19ftKineticTransactorFiPvP5Vec2fP16soModuleAccesser(unsigned int x, int b, int c, int d) {
 switch(x) {
case 0: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 1: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub2__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 2: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub3__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 3: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub6__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 4: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub7__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 5: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub8__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 6: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub12__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 7: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub13__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 8: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub14__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 9: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub15__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 10: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub16__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 11: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub17__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 12: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub18__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 13: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub19__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 14: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub20__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 15: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub22__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 16: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub27__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 17: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub28__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 18: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub29__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 19: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub30__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 20: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub31__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 21: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub32__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 22: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub33__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 23: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub34__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 24: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub35__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 25: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub37__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 26: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub38__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 27: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub39__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 28: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub40__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 29: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub41__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 30: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub42__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 31: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub43__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 32: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub44__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 33: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub45__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 34: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub46__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 35: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub47__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 36: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub48__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 37: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub49__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 38: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub50__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 39: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub51__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 40: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub53__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 41: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub52__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 42: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub54__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 43: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub55__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 44: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub56__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 45: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub57__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 46: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub58__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 47: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub60__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 48: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub59__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 49: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub61__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 50: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub62__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 51: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub63__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 52: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub64__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 53: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub65__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 54: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub66__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 55: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub67__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 56: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub68__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 57: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub69__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 58: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub70__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 59: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub71__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 60: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub72__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 61: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub73__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 62: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub74__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 63: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub75__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 64: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub76__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 65: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub77__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 66: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub78__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 67: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub1__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 68: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub11__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 69: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub21__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 70: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub9__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 71: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub10__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 72: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub36__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 73: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub79__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 74: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub80__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 75: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub81__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 76: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub25__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 77: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub26__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 78: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub4__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 79: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub5__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 80: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub23__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
case 81: { struct V { char v; char pad[3]; } v; v.v = 0; changeKineticSub24__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(&v.v, b, c, d); return; }
 }
}
}
}

// TODO: clean up, give variables real names
namespace jj_1603F4 {
union PairBits {
    unsigned long long q;
    struct {
        u32 hi;
        u32 lo;
    } w;
};

union FloatBits {
    u32 u;
    float f;
};

struct Vec2f {
    float x;
    float y;
};

class SubObj {
public:
    virtual void v0();
    virtual void v1();
    virtual unsigned long long eval();

    unsigned char pad0;
    unsigned char on : 1;
    unsigned char low : 7;
    unsigned char pad1[2];
    float data0;
    float data1;

    unsigned int isOn() const {
        return on != 0;
    }
};

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual float slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19(int);
};

struct MidObj {
    char pad[0xc];
    Object_vptr_0* inner;
    char pad2[0x54];
    Object_vptr_0* target;
};

struct AccObj {
    char pad[0xd8];
    MidObj* mid;
};

struct SrcVec {
    float x;
    float y;
};

struct Args {
    char pad0[0x10];
    SubObj subA;
    char padA[0x4c];
    SubObj subB;
    char pad1[0x24];
    SubObj subC;
    char pad2[0x48];
    SubObj subD;
};

extern "C" int isEnableSpeedOperation__6ftUtilFP16soModuleAccesser(void*);
extern "C" const float lbl_27_rodata_1FC0;

extern "C" void addSpeed__19ftKineticTransactorFPvPvP16soModuleAccesser(SrcVec* arg0, Args* arg1, AccObj* arg2) {
    if (isEnableSpeedOperation__6ftUtilFP16soModuleAccesser(arg2) != 0) {
        float val = arg2->mid->inner->slot9();
        float f31 = arg0->y;
        float f30 = arg0->x * val;

        if (lbl_27_rodata_1FC0 != f31) {
            SubObj* b = &arg1->subB;
            if (b->isOn() == 1) {
                Vec2f p;
                *(unsigned long long*)&p = b->eval();
                b->data1 = f31 + p.y;
                arg2->mid->target->slot19(0x22000002);
            } else {
                SubObj* d = &arg1->subD;
                if (d->isOn() == 1) {
                    Vec2f result;
                    Vec2f p2;
                    Vec2f p1;
                    *(unsigned long long*)&p1 = d->eval();
                    f31 += p1.y;
                    *(unsigned long long*)&p2 = d->eval();
                    result.x = p2.x;
                    result.y = f31;
                    d->data0 = result.x;
                    d->data1 = result.y;
                } else {
                    SubObj* a = &arg1->subA;
                    if (a->isOn() == 1) {
                        Vec2f result;
                        Vec2f p2;
                        Vec2f p1;
                        *(unsigned long long*)&p1 = a->eval();
                        f31 += p1.y;
                        *(unsigned long long*)&p2 = a->eval();
                        result.x = p2.x;
                        result.y = f31;
                        a->data0 = result.x;
                        a->data1 = result.y;
                    }
                }
            }
        }

        if (lbl_27_rodata_1FC0 != f30) {
            SubObj* c = &arg1->subC;
            if (c->isOn() == 1) {
                Vec2f result;
                Vec2f p;
                *(unsigned long long*)&p = c->eval();
                result.x = f30 + p.x;
                result.y = lbl_27_rodata_1FC0;
                c->data1 = result.y;
                c->data0 = result.x;
                arg2->mid->target->slot19(0x22000002);
            } else {
                SubObj* d = &arg1->subD;
                if (d->isOn() == 1) {
                    Vec2f result;
                    Vec2f p2;
                    Vec2f p1;
                    *(unsigned long long*)&p1 = d->eval();
                    float first = p1.y;
                    *(unsigned long long*)&p2 = d->eval();
                    result.y = first;
                    result.x = f30 + p2.x;
                    d->data0 = result.x;
                    d->data1 = result.y;
                } else {
                    SubObj* a = &arg1->subA;
                    if (a->isOn() == 1) {
                        Vec2f result;
                        Vec2f p2;
                        Vec2f p1;
                        *(unsigned long long*)&p1 = a->eval();
                        float first = p1.y;
                        *(unsigned long long*)&p2 = a->eval();
                        result.y = first;
                        result.x = f30 + p2.x;
                        a->data0 = result.x;
                        a->data1 = result.y;
                    }
                }
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_160770 {
extern "C" const float lbl_27_rodata_1FC0;
struct V2 { float x, y; };
struct V3 { float x, y, z; };
struct Words { int x, y; };
struct M10 {
 int pad[2];
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
 virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
 virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
 virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
 virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
 virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
 virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
 virtual void v36(); virtual void v37(); virtual void v38(); virtual void v39();
 virtual void v40(); virtual void v41(); virtual void v42(); virtual void v43();
 virtual void v44(); virtual void v45(); virtual void v46(); virtual void v47();
 virtual void v48(); virtual void v49(); virtual void v50(); virtual void v51();
 virtual void v52(); virtual void v53(); virtual void v54(); virtual void v55();
 virtual void v56(); virtual Words v57(int,int);
};
struct Object_vptr_0 {
 virtual void slot0(); virtual void slot1(); virtual unsigned long long slot2();
 virtual int slot3(); virtual void slot4(); virtual void slot5(); virtual void slot6();
 virtual void slot7(); virtual void slot8(); virtual void slot9(); virtual void slot10();
 virtual void slot11(); virtual void slot12(); virtual void slot13(unsigned int,float);
};
struct Modules { char pad0[0x10]; M10* m10; Object_vptr_0* m14; char pad1[0x4c]; soWorkManageModule* m64; };
struct Accesser { char pad[0xd8]; Modules* modules; };
struct Fighter {
 char pad0[0x140]; Object_vptr_0 sub; char pad1[1]; unsigned char flag145;
 char pad2[0x47]; unsigned char flag18d; char pad3[0x1a];
 float f1a8, f1ac; char pad4[0xc]; float f1bc, f1c0, f1c4;
 char pad5[0x11]; unsigned char flag1d9; char pad6[2]; float f1dc, f1e0;
 char pad7[0x14]; float f1f8;
};
extern "C" unsigned long long projectionNormalFollow__16soKineticUtilityFP5Vec2fP5Vec2f(V2*,V2*);
inline void addTo(V2* out,const V2& a,const Vec2f& b) { V2 r; r.x=a.x+b.m_x; r.y=a.y+b.m_y; *(Words*)out=*(Words*)&r; }
extern "C" void resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(int, Accesser*, int, V2*, V3*);
extern "C" void addSpeedOutside__19ftKineticTransactorFiPvPvP16soModuleAccesser(int kind, const V2* speed, Fighter* self, Accesser* acc) {
 switch(kind) {
 case 0:
  if ((self->flag1d9 >> 7) & 1) {
   V2 r;
   r.x = lbl_27_rodata_1FC0; r.y = lbl_27_rodata_1FC0;
   switch (acc->modules->m14->slot3()) {
   default: break;
   case 0: {
   V2 b;
   Words w = acc->modules->m10->v57(8,0);
   unsigned long long result;
   V2 a;
   a.x = speed->x; a.y = lbl_27_rodata_1FC0;
   *(Words*)&b = w;
   result = projectionNormalFollow__16soKineticUtilityFP5Vec2fP5Vec2f(&a,&b);
   r.x = ((float*)&result)[0]; r.y = ((float*)&result)[1];
   if (lbl_27_rodata_1FC0 != r.x || lbl_27_rodata_1FC0 != r.y) {
    self->f1dc = self->f1dc + r.x;
    self->f1e0 = self->f1e0 + r.y;
    self->f1f8 = self->f1f8 + lbl_27_rodata_1FC0;
    self->flag1d9 |= 0x80;
   }
   break;
   }
   }
  }
  break;
 case 1:
  if ((self->flag18d >> 7) & 1) {
   switch (acc->modules->m14->slot3()) {
   case 0:
    if (lbl_27_rodata_1FC0 != speed->x) {
     V2 p; p.x=speed->x; p.y=lbl_27_rodata_1FC0;
     self->f1bc = self->f1bc + p.x; self->f1c0 = self->f1c0 + p.y;
     self->f1c4 = soValueAccesser::getConstantFloat((soModuleAccesser*)acc,0xc10,0);
     V2 q; q.x=soValueAccesser::getConstantFloat((soModuleAccesser*)acc,0xbbc,0); q.y=lbl_27_rodata_1FC0;
     self->f1a8=q.x; self->f1ac=q.y;
    }
    break;
   case 2: case 3:
    if (lbl_27_rodata_1FC0 != speed->x || lbl_27_rodata_1FC0 != speed->y) {
     Vec2f p(speed->x, speed->y);
     self->f1bc = self->f1bc + p.m_x; self->f1c0 = self->f1c0 + p.m_y;
     self->f1c4 = soValueAccesser::getConstantFloat((soModuleAccesser*)acc,0xc10,0);
     V2 q; q.x=soValueAccesser::getConstantFloat((soModuleAccesser*)acc,0xd1a,0); q.y=lbl_27_rodata_1FC0;
     self->f1a8=q.x; self->f1ac=q.y;
    }
    break;
   }
  }
  break;
 case 2: {
  Object_vptr_0* sub=&self->sub;
  if ((self->flag145 >> 7) & 1) {
   switch (acc->modules->m14->slot3()) {
   case 0:
    if (lbl_27_rodata_1FC0 != speed->x) {
     V2 out;
     V2 ret;
     V3 z; z.x=lbl_27_rodata_1FC0; z.y=lbl_27_rodata_1FC0; z.z=lbl_27_rodata_1FC0;
     Vec2f a(speed->x,lbl_27_rodata_1FC0);
     *(unsigned long long*)&ret=sub->slot2();
     addTo(&out,ret,a);
     resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(4,acc,1,&out,&z);
    }
    break;
   case 2: case 3:
    if (lbl_27_rodata_1FC0 != speed->x || lbl_27_rodata_1FC0 != speed->y) {
     V2 out;
     V2 ret;
     Vec2f a(speed->x,speed->y);
     V3 z; z.x=lbl_27_rodata_1FC0; z.y=lbl_27_rodata_1FC0; z.z=lbl_27_rodata_1FC0;
     *(unsigned long long*)&ret=sub->slot2();
     addTo(&out,ret,a);
     resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(4,acc,2,&out,&z);
    }
    break;
   }
  }
  break;
 }
 case 3:
  acc->modules->m64->setFloat(speed->x,0x1100000f);
  acc->modules->m64->setFloat(speed->y,0x11000010);
  break;
 }
}
}

// TODO: clean up, give variables real names
namespace jj_160BF0 {

extern "C" {
extern const float lbl_27_rodata_1FC0;
void resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(int, soModuleAccesser*, int, Vec2f*, Vec3f*);
}

extern "C" void changeKineticSub__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(int, int, const Vec2f* in, soModuleAccesser* accesser) {
    Vec3f r1(lbl_27_rodata_1FC0, lbl_27_rodata_1FC0, lbl_27_rodata_1FC0);
    Vec2f a(lbl_27_rodata_1FC0, in->m_y);
    resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(1, accesser, 0, &a, &r1);
    Vec3f r2(lbl_27_rodata_1FC0, lbl_27_rodata_1FC0, lbl_27_rodata_1FC0);
    Vec2f b(in->m_x, lbl_27_rodata_1FC0);
    resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(2, accesser, 0, &b, &r2);
}
}

// TODO: clean up, give variables real names
namespace jj_161688 {

extern "C" {
extern const float lbl_27_rodata_1FC0;
extern const float lbl_27_rodata_1FD4;
extern void* g_ftCommonData;
float calcScalingParam__6ftUtilFfff(float, float, float);
void resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(int, soModuleAccesser*, int, Vec2f*, Vec3f*);
}

static inline void setVector(Vec2f& v, float x, float y) {
    v.m_x = x;
    v.m_y = y;
}

static inline float commonFloat(int offset, int field) {
    return *(float*)((char*)*(void**)((char*)g_ftCommonData + offset) + field);
}

extern "C" void changeKineticSub6__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(int, int, const Vec2f*, soModuleAccesser* accesser) {
    void* p = soValueAccesser::getConstantIndefinite(accesser, 0xA80A, 0);
    Vec2f speed;
    speed.m_x = *(float*)((char*)p + 8) * accesser->getControllerModule().getStickX();
    int index = accesser->getWorkManageModule().getInt(0x10000001);
    speed.m_y = *(float*)((char*)*(float**)((char*)p + 0x10) + (index << 2) - 8);
    Vec2f mul(lbl_27_rodata_1FD4, lbl_27_rodata_1FD4);
    if (accesser->getParamCustomizeModule().isCustomize(0)) {
        float f = commonFloat(0x30, 0x1C);
        mul.m_y *= calcScalingParam__6ftUtilFfff(mul.m_y, accesser->getPostureModule().getBaseScale(), f);
    } else if (accesser->getParamCustomizeModule().isCustomize(1)) {
        float f = commonFloat(0x34, 0x1C);
        mul.m_y *= calcScalingParam__6ftUtilFfff(mul.m_y, accesser->getPostureModule().getBaseScale(), f);
    }
    if (accesser->getParamCustomizeModule().isCustomize(2)) {
        float* data = *(float**)((char*)g_ftCommonData + 0x38);
        mul.m_x *= data[5];
        mul.m_y *= data[6];
    }
    speed.m_x *= mul.m_x;
    speed.m_y *= mul.m_y;
    Vec2f motion;
    Vec2f gravity;
    Vec3f zero2;
    Vec3f zero1(lbl_27_rodata_1FC0, lbl_27_rodata_1FC0, lbl_27_rodata_1FC0);
    gravity.m_x = lbl_27_rodata_1FC0;
    gravity.m_y = speed.m_y;
    resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(1, accesser, 0, &gravity, &zero1);
    zero2.m_x = lbl_27_rodata_1FC0;
    zero2.m_y = lbl_27_rodata_1FC0;
    zero2.m_z = lbl_27_rodata_1FC0;
    setVector(motion, speed.m_x, lbl_27_rodata_1FC0);
    resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(2, accesser, 4, &motion, &zero2);
}
}

// TODO: clean up, give variables real names
namespace jj_162100 {
struct Vec2f { float m_x; float m_y; Vec2f& operator=(const Vec2f& rhs) { ((u32*)this)[0] = ((const u32*)&rhs)[0]; ((u32*)this)[1] = ((const u32*)&rhs)[1]; return *this; } }; struct Vec3f { float m_x; float m_y; float m_z; };
struct Flags { char pad[5]; unsigned char flag:1; unsigned char rest:7; };
class KineticModule { public:
 virtual void f0()=0; virtual void f1()=0; virtual void f2()=0; virtual void f3()=0; virtual void f4()=0; virtual void f5()=0; virtual void* getEnergy(int)=0; virtual void f7()=0; virtual void f8()=0; virtual void f9()=0; virtual void f10()=0; virtual void f11()=0; virtual void f12()=0; virtual Vec2f getVec(s16*)=0;
};
struct Enumeration { char pad[0x7c]; KineticModule* m_kinetic; }; struct Accesser { char pad[0xd8]; const Enumeration* m_enumerationStart; };
extern "C" { extern const float lbl_27_rodata_1FC0; extern const float lbl_27_rodata_1FD8; void resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(int,Accesser*,int,Vec2f*,Vec3f*); }
extern "C" void changeKineticSub15__19ftKineticTransactorFPbPvP5Vec2fP16soModuleAccesser(int a1,int a2,Vec2f* vec2,Accesser* accesser) {
 Vec2f tempA, tempB;
 s16 val=2;
 Vec2f result=accesser->m_enumerationStart->m_kinetic->getVec(&val);
 Vec2f v2;
 Vec2f sum;
 sum.m_x=vec2->m_x+result.m_x; sum.m_y=vec2->m_y+result.m_y;
 tempB=sum;
 tempA=sum;
 float f2;
 if(tempA.m_x>lbl_27_rodata_1FC0) f2=lbl_27_rodata_1FD8 * -tempA.m_y;
 else f2=lbl_27_rodata_1FD8 * tempA.m_y;
 Vec3f v3={lbl_27_rodata_1FC0,lbl_27_rodata_1FC0,lbl_27_rodata_1FC0};
 v2.m_x=tempA.m_x+f2; v2.m_y=lbl_27_rodata_1FC0;
 resetEnableEnergy__16soKineticUtilityFiP16soModuleAccesseriP5Vec2fP5Vec3f(3,accesser,0x12,&v2,&v3);
 Flags* p=(Flags*)accesser->m_enumerationStart->m_kinetic->getEnergy(4); p->flag=0;
}
}

