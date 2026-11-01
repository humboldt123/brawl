// BYTES: the native code assigns the vectors through the out-of-line operator=.
#define MT_VEC2F_ASSIGN_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/pikachu/ft_pikachu.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Quick Attack's stick helper: dead zone and full-tilt thresholds (two constants) map the stick onto 0..1 keeping the sign.
float regulate(register float value, float low, float high);

// Pikachu's own kinetic mode: 0x64 is the Quick Attack aim, 0x65..0x68 only switch the outside energy on.
void ftPikachuKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    switch (mode) {
    case 0x64: {
        bool result = false;
        changeKineticSub(&result, pools, acc);
        break;
    }
    case 0x65: {
        volatile bool result = false;
        break;
    }
    case 0x66: {
        volatile bool result = false;
        break;
    }
    case 0x67: {
        volatile bool result = false;
        break;
    }
    case 0x68: {
        volatile bool result = false;
        break;
    }
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}

void ftPikachuKineticTransactor::changeKineticSub(bool*, void*, soModuleAccesser* acc) {
    soKineticModule& kinetic = acc->getKineticModule();
    kinetic.unableEnergyAll();
    kinetic.clearSpeedAll();
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    controller.resetEnergy(0xb, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), acc);
    controller.m_accel = Vec2f(0.0f, 0.0f);
    controller.enable();
}

void ftPikachuKineticTransactor::updateEnergySub(soKineticEnergyNormal* energy, void*, soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f stick;
    stick.m_x = acc->getControllerModule().getStickX();
    stick.m_y = acc->getControllerModule().getStickY();
    float high;
    float low = soValueAccesser::getConstantFloat(acc, 0xc2d, 0);
    high = soValueAccesser::getConstantFloat(acc, 0xc36, 0);
    stick.m_x = regulate(stick.m_x, low, high);
    stick.m_y = regulate(stick.m_y, low, high);
    Vec2f gain;
    gain.m_x = soValueAccesser::getConstantFloat(acc, 0xfcb, 0);
    gain.m_y = soValueAccesser::getConstantFloat(acc, 0xfcc, 0);
    Vec2f speed;
    speed.m_x = gain.m_x * stick.m_x;
    speed.m_y = gain.m_y * stick.m_y;
    float scale = work.getFloat(0x21000004);
    speed.m_x *= scale;
    speed.m_y *= scale;
    energy->m_speed = speed;
    energy->m_speedLimit = speed;
}

float regulate(register float value, float low, float high) {
    float sign;
    if (value >= 0.0f) {
        sign = 1.0f;
    } else {
        sign = -1.0f;
        double wide = fabs(value);
        value = wide;
    }
    value = (value - low) / (high - low);
    register float zero;
    register float one;
    one = 1.0f;
    zero = 0.0f;
    register float lowBounded;
    register float t;
    t = value - zero;
    // MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect asm helper.
    asm { fsel lowBounded, t, value, zero }
    t = lowBounded - one;
    register float clamped;
    asm { fsel clamped, t, one, lowBounded }
    return sign * clamped;
}
