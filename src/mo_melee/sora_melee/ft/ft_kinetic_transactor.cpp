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
    Vec3f rot(0.0f, 0.0f, 0.0f);
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, &zero, &rot);
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

Vec2f ftKineticTransactHelper::preHelpProcess(soModuleAccesser* moduleAccesser, int clearKind, int clearSpeed) {
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if ((u32)clearSpeed == 1) {
        moduleAccesser->getKineticModule().unableEnergyAll();
    }
    moduleAccesser->getKineticModule().getEnergy(4)->enable();
    moduleAccesser->getKineticModule().getEnergy(6)->enable();
    return speed;
}
