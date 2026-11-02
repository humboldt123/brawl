#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
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
    soKineticUtility::resetEnableEnergy(1, moduleAccesser, 7, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
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
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 12, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
    moduleAccesser->getWorkManageModule().offFlag(0x2200000d);
}

void ftKineticTransactor::changeKineticSub35(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser) {
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 13, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    moduleAccesser->getKineticModule().getEnergy(4)->disable();
    moduleAccesser->getWorkManageModule().offFlag(0x2200000d);
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
    soKineticUtility::resetEnableEnergy(0, moduleAccesser, 2, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
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
