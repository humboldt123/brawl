// BYTES: the native kinetic helpers call the vector assignment and the Vec3f constructor out of line.
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/peach/ft_peach_kinetic_transactor.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// The energies of the mediator pools (the first argument selects the pool by its index in the energy list; the
// value-initialized tag temporary is the zero byte each use leaves on the stack).
static inline ftKineticEnergyMotion* getEnergy(void* pools, const soIntToType<0>&) {
    return static_cast<ftKineticPoolMotionPool*>(static_cast<ftKineticPoolJostle*>(pools))->getSub().getInstanceAt(0);
}

static inline ftKineticEnergyGravity* getEnergy(void* pools, const soIntToType<1>&) {
    return static_cast<ftKineticPoolGravityPool*>(static_cast<ftKineticPoolJostle*>(pools))->getSub().getInstanceAt(0);
}

static inline ftKineticEnergyController* getEnergy(void* pools, const soIntToType<2>&) {
    return static_cast<ftKineticPoolControllerPool*>(static_cast<ftKineticPoolJostle*>(pools))->getSub().getInstanceAt(0);
}

static inline ftKineticEnergyStop* getEnergy(void* pools, const soIntToType<3>&) {
    return static_cast<ftKineticPoolStopPool*>(static_cast<ftKineticPoolJostle*>(pools))->getSub().getInstanceAt(0);
}

// TODO(nonmatching): 99.5%, 214/215 instructions. The native code passes the shared preprocessing flag as `li r4, 1; mr r5, r4`
// (the siblings get that from `#pragma opt_propagation off`); here it is `li r4, 1; li r5, 1`. The pragma also stops the
// compiler from eliding the copies of the inlined Vec2f multiplication below, so it was not used. The native stack layout
// (product lowest, its return copy above the controller speed temporary) is the one the inlined operator* gives with
// propagation on, so the float block was compiled with propagation on; under the pragma no spelling of it (operator/,
// named locals, reference binding, out-parameter helpers) reproduced both the copy count and the slot order. Yoshi, Luigi
// and Pikmin have the same `li r4, 1; mr r5, r4` natively, so the spelling is shared by the family; with propagation on
// none of these gave it: int/const bool/u8/s16 locals, `true, true`, `1, 1`, a write to the flag after the call, a later
// test of the flag, `process ? 1 : 0`, `process == true`, `int(process)`, a bool reference, an aggregate of two ints
// (which changed the register allocation), and a static inline helper defined under `#pragma opt_propagation off`
// (the pragma does not follow the helper into its caller).
void ftPeachKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    // Leaving mode 0x6a (HYPOTHESIS: Peach's float) rescales the carried gravity and controller speeds by the mode's own
    // constants.
    if (acc->getKineticModule().getKineticType() == 0x6a) {
        ftKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
        gravity->m_speedY = gravity->getSpeed().m_y / soValueAccesser::getConstantFloat(acc, 0xfb3, 0);
        ftKineticEnergyController* controller = getEnergy(pools, soIntToType<2>());
        float controllerScale = soValueAccesser::getConstantFloat(acc, 0xfb2, 0);
        controller->m_speed = controller->getSpeed() * (1.0f / controllerScale);
    }
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    soModuleAccesser* processAccesser = acc;
    bool process = true;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(processAccesser, process, process);
    switch (mode) {
    case 0x64:
        changeKineticSub(soIntToType<0x64>(), pools, &speed, acc);
        break;
    case 0x65:
        changeKineticSub1(soIntToType<0x65>(), pools, &speed, acc);
        break;
    case 0x66:
        changeKineticSub2(soIntToType<0x66>(), pools, &speed, acc);
        break;
    case 0x67:
        changeKineticSub3(soIntToType<0x67>(), pools, &speed, acc);
        break;
    case 0x68:
        changeKineticSub4(soIntToType<0x68>(), pools, &speed, acc);
        break;
    case 0x69:
        changeKineticSub5(soIntToType<0x69>(), pools, &speed, acc);
        break;
    case 0x6a:
        changeKineticSub6(soIntToType<0x6a>(), pools, &speed, acc);
        break;
    case 0x6b:
        changeKineticSub7(soIntToType<0x6b>(), pools, &speed, acc);
        break;
    case 0x6c:
        changeKineticSub8(soIntToType<0x6c>(), pools, &speed, acc);
        break;
    case 0x6d:
        changeKineticSub9(soIntToType<0x6d>(), pools, &speed, acc);
        break;
    case 0x6e:
        changeKineticSub10(soIntToType<0x6e>(), pools, &speed, acc);
        break;
    case 0x6f:
        changeKineticHook(soIntToType<0x6f>(), pools, acc);
        break;
    case 0x70:
        changeKineticHook(soIntToType<0x70>(), pools, acc);
        break;
    case 0x71:
        changeKineticHook(soIntToType<0x71>(), pools, acc);
        break;
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}

void ftPeachKineticTransactor::changeKineticSub(const soIntToType<0x64>&, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    float controllerSpeed = speed->m_x;
    controllerSpeed *= soValueAccesser::getConstantFloat(acc, 0xfb7, 0);
    Vec2f controllerEntry;
    controllerEntry.m_x = controllerSpeed;
    controllerEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &controllerEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f gravityEntry;
    gravityEntry.m_x = 0.0f;
    gravityEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_gravity = -soValueAccesser::getConstantFloat(acc, 0xfb8, 0);
}

void ftPeachKineticTransactor::changeKineticSub1(const soIntToType<0x65>&, void* pools, Vec2f*, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(0x12);
    ftKineticEnergyMotion* motion = getEnergy(pools, soIntToType<0>());
    motion->m_speedMul = soValueAccesser::getConstantFloat(acc, 0xfb6, 0);
}

void ftPeachKineticTransactor::changeKineticSub2(const soIntToType<0x66>&, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f controllerEntry;
    controllerEntry.m_x = speed->m_x;
    controllerEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &controllerEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    // HYPOTHESIS: the vertical speed goes into the first component of the gravity entry, as in the native setup.
    Vec2f gravityEntry;
    gravityEntry.m_x = speed->m_y;
    gravityEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    float gravityValue = gravity->m_gravity;
    gravity->m_gravity = gravityValue * soValueAccesser::getConstantFloat(acc, 0xfba, 0);
    float fallSpeedMax = gravity->m_fallSpeedMax;
    gravity->m_fallSpeedMax = fallSpeedMax * soValueAccesser::getConstantFloat(acc, 0xfba, 0);
}

void ftPeachKineticTransactor::changeKineticSub3(const soIntToType<0x67>&, void* pools, Vec2f*, soModuleAccesser* acc) {
    float controllerSpeed = soValueAccesser::getConstantFloat(acc, 0xfa5, 0);
    if (acc->getPostureModule().getLr() >= 0.0f) {
        controllerSpeed = -controllerSpeed;
    }
    Vec2f controllerEntry;
    controllerEntry.m_x = controllerSpeed;
    controllerEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &controllerEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController* controller = getEnergy(pools, soIntToType<2>());
    float accelX = soValueAccesser::getConstantFloat(acc, 0xfa6, 0);
    if (acc->getPostureModule().getLr() >= 0.0f) {
        accelX = -accelX;
    }
    Vec2f accel;
    accel.m_x = accelX;
    accel.m_y = 0.0f;
    controller->m_accel = accel;
    Vec2f target;
    target.m_x = soValueAccesser::getConstantFloat(acc, 0xfa7, 0);
    target.m_y = 0.0f;
    controller->m_speedTarget = target;
    soKineticUtility::resetEnableEnergy(0, acc, 6, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftPeachKineticTransactor::changeKineticSub4(const soIntToType<0x68>&, void*, Vec2f*, soModuleAccesser* acc) {
    Vec2f gravityEntry;
    float gravityEntryY = soValueAccesser::getConstantFloat(acc, 0xfa8, 0);
    gravityEntry.m_x = 0.0f;
    gravityEntry.m_y = gravityEntryY;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f motionEntry;
    motionEntry.m_x = 0.0f;
    motionEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(0, acc, 6, &motionEntry, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftPeachKineticTransactor::changeKineticSub5(const soIntToType<0x69>&, void* pools, Vec2f*, soModuleAccesser* acc) {
    float stopSpeed = soValueAccesser::getConstantFloat(acc, 0xfa9, 0);
    if (acc->getWorkManageModule().isFlag(0x22000010) == true) {
        stopSpeed = soValueAccesser::getConstantFloat(acc, 0xfaa, 0);
    }
    if (acc->getPostureModule().getLr() < 0.0f) {
        stopSpeed = -stopSpeed;
    }
    Vec2f stopEntry;
    stopEntry.m_x = stopSpeed;
    stopEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(3, acc, 6, &stopEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop* stop = getEnergy(pools, soIntToType<3>());
    Vec2f accel;
    accel.m_x = 0.0f;
    accel.m_y = 0.0f;
    stop->m_accel = accel;
    Vec2f target;
    target.m_x = -1.0f;
    target.m_y = -1.0f;
    stop->m_speedTarget = target;
    Vec2f limit;
    limit.m_x = -1.0f;
    limit.m_y = -1.0f;
    stop->m_speedLimit = limit;
    Vec2f gravityEntry;
    float gravityEntryY = soValueAccesser::getConstantFloat(acc, 0xfab, 0);
    gravityEntry.m_x = 0.0f;
    gravityEntry.m_y = gravityEntryY;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_gravity = -soValueAccesser::getConstantFloat(acc, 0xfac, 0);
    gravity->m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 0xfad, 0);
}

void ftPeachKineticTransactor::changeKineticSub6(const soIntToType<0x6a>&, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f stopEntry;
    stopEntry.m_x = speed->m_x;
    stopEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(3, acc, 6, &stopEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop* stop = getEnergy(pools, soIntToType<3>());
    Vec2f target;
    target.m_x = 0.0f;
    target.m_y = 0.0f;
    stop->m_speedTarget = target;
    Vec2f brake;
    brake.m_x = soValueAccesser::getConstantFloat(acc, 0xfae, 0);
    brake.m_y = 0.0f;
    stop->m_brake = brake;
    Vec2f gravityEntry;
    float gravityEntryY = speed->m_y;
    gravityEntry.m_x = 0.0f;
    gravityEntry.m_y = gravityEntryY;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_gravity = -soValueAccesser::getConstantFloat(acc, 0xfaf, 0);
    gravity->m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 0xfad, 0);
}

void ftPeachKineticTransactor::changeKineticSub7(const soIntToType<0x6b>&, void*, Vec2f*, soModuleAccesser* acc) {
    float controllerSpeed = soValueAccesser::getConstantFloat(acc, 0xfb0, 0);
    if (acc->getPostureModule().getLr() < 0.0f) {
        controllerSpeed = -controllerSpeed;
    }
    Vec2f controllerEntry;
    controllerEntry.m_x = controllerSpeed;
    controllerEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &controllerEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f gravityEntry;
    float gravityEntryY = soValueAccesser::getConstantFloat(acc, 0xfb1, 0);
    gravityEntry.m_x = 0.0f;
    gravityEntry.m_y = gravityEntryY;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftPeachKineticTransactor::changeKineticSub8(const soIntToType<0x6c>&, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f stopEntry;
    stopEntry.m_x = speed->m_x / acc->getConstantFloatKirby(0xfa0);
    stopEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(3, acc, 6, &stopEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop* stop = getEnergy(pools, soIntToType<3>());
    Vec2f accel;
    accel.m_x = 0.0f;
    accel.m_y = 0.0f;
    stop->m_accel = accel;
    Vec2f target;
    target.m_x = 0.0f;
    target.m_y = 0.0f;
    stop->m_speedTarget = target;
    Vec2f brake;
    brake.m_x = acc->getConstantFloatKirby(0xfa1);
    brake.m_y = -1.0f;
    stop->m_brake = brake;
    Vec2f limit;
    limit.m_x = -1.0f;
    limit.m_y = -1.0f;
    stop->m_speedLimit = limit;
    Vec2f gravityEntry;
    float gravityEntryY = speed->m_y;
    gravityEntry.m_x = 0.0f;
    gravityEntry.m_y = gravityEntryY;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
}

void ftPeachKineticTransactor::changeKineticSub9(const soIntToType<0x6d>&, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f stopEntry;
    stopEntry.m_x = speed->m_x;
    stopEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(3, acc, 6, &stopEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop* stop = getEnergy(pools, soIntToType<3>());
    Vec2f accel;
    accel.m_x = 0.0f;
    accel.m_y = 0.0f;
    stop->m_accel = accel;
    Vec2f target;
    target.m_x = 0.0f;
    target.m_y = 0.0f;
    stop->m_speedTarget = target;
    Vec2f brake;
    brake.m_x = acc->getConstantFloatKirby(0xfa1);
    brake.m_y = -1.0f;
    stop->m_brake = brake;
    Vec2f limit;
    limit.m_x = -1.0f;
    limit.m_y = -1.0f;
    stop->m_speedLimit = limit;
    if (acc->getWorkManageModule().isFlag(0x1200003d) == false) {
        Vec2f gravityEntry;
        float gravityEntryY = acc->getConstantFloatKirby(0xfa2);
        gravityEntry.m_x = 0.0f;
        gravityEntry.m_y = gravityEntryY;
        soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
        acc->getWorkManageModule().onFlag(0x1200003d);
    } else {
        Vec2f gravityEntry;
        gravityEntry.m_x = 0.0f;
        gravityEntry.m_y = 0.0f;
        soKineticUtility::resetEnableEnergy(1, acc, 0, &gravityEntry, &Vec3f(0.0f, 0.0f, 0.0f));
    }
    ftKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_gravity = -acc->getConstantFloatKirby(0xfa3);
    gravity->m_fallSpeedMax = acc->getConstantFloatKirby(0xfa4);
}

void ftPeachKineticTransactor::changeKineticSub10(const soIntToType<0x6e>&, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f controllerEntry;
    controllerEntry.m_x = speed->m_x;
    controllerEntry.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &controllerEntry, &Vec3f(0.0f, 0.0f, 0.0f));
}

// Stick tilt of mode 0x65: the stick's horizontal deflection above its dead zone leans the motion energy up to the
// configured angle (degrees in the parameters, radians in the energy).
void ftPeachKineticTransactor::updateEnergySub(ftKineticEnergyMotion* energy, const soIntToType<0x65>&, soModuleAccesser* acc) {
    float deadZone = soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
    float stick = acc->getControllerModule().getStickX();
    float deflection = fabs(stick);
    if (deflection > deadZone) {
        float ratio = (deflection - deadZone) / (1.0f - deadZone);
        float tilt = ratio * soValueAccesser::getConstantFloat(acc, 0xfb5, 0);
        float radians;
        if (stick > 0.0f) {
            radians = -(0.017453292f * tilt);
        } else {
            radians = 0.017453292f * tilt;
        }
        double currentWide = fabs(energy->m_rotation);
        float currentMagnitude = currentWide;
        double radiansWide = fabs(radians);
        float radiansMagnitude = radiansWide;
        float current = energy->m_rotation;
        if (radiansMagnitude > currentMagnitude) {
            current = radians;
        }
        energy->m_rotation = current;
    }
    ftKineticTransactor::updateEnergy(energy, acc);
}

// While the stick is held past the threshold of mode 0x66 the controller energy targets the configured speed.
void ftPeachKineticTransactor::updateEnergySub1(ftKineticEnergyController* energy, const soIntToType<0x66>&, soModuleAccesser* acc) {
    double wide = fabs(acc->getControllerModule().getStickX());
    float deflection = wide;
    if (deflection >= soValueAccesser::getConstantFloat(acc, 0xc49, 0)) {
        energy->m_accelMul = soValueAccesser::getConstantFloat(acc, 0xbd3, 0) * soValueAccesser::getConstantFloat(acc, 0xfb9, 0);
        Vec2f target;
        target.m_x = soValueAccesser::getConstantFloat(acc, 0xbd5, 0) * soValueAccesser::getConstantFloat(acc, 0xfb9, 0);
        target.m_y = 0.0f;
        energy->m_speedTarget = target;
    } else {
        Vec2f accel;
        accel.m_x = 0.0f;
        accel.m_y = 0.0f;
        energy->m_accel = accel;
        Vec2f target;
        target.m_x = 0.0f;
        target.m_y = 0.0f;
        energy->m_speedTarget = target;
    }
    ftKineticTransactor::updateEnergy(energy, acc);
}

template <>
void ftPeachKineticTransactor::updateEnergy<ftKineticEnergyMotion>(ftKineticEnergyMotion* energy, soModuleAccesser* acc) {
    if (acc->getKineticModule().getKineticType() <= 0x63) {
        ftKineticTransactor::updateEnergy(energy, acc);
        return;
    }
    if (energy->isEnable() == false || energy->isSuspend() == true) {
        return;
    }
    {
        switch (acc->getKineticModule().getKineticType()) {
        case 0x64:
            updateEnergyHook(soIntToType<0x64>(), energy, acc);
            break;
        case 0x65:
            updateEnergySub(energy, soIntToType<0x65>(), acc);
            break;
        case 0x66:
            updateEnergyHook(soIntToType<0x66>(), energy, acc);
            break;
        case 0x67:
            updateEnergyHook(soIntToType<0x67>(), energy, acc);
            break;
        case 0x68:
            updateEnergyHook(soIntToType<0x68>(), energy, acc);
            break;
        case 0x69:
            updateEnergyHook(soIntToType<0x69>(), energy, acc);
            break;
        case 0x6a:
            updateEnergyHook(soIntToType<0x6a>(), energy, acc);
            break;
        case 0x6b:
            updateEnergyHook(soIntToType<0x6b>(), energy, acc);
            break;
        case 0x6c:
            updateEnergyHook(soIntToType<0x6c>(), energy, acc);
            break;
        case 0x6d:
            updateEnergyHook(soIntToType<0x6d>(), energy, acc);
            break;
        case 0x6e:
            updateEnergyHook(soIntToType<0x6e>(), energy, acc);
            break;
        case 0x6f:
            updateEnergyHook(soIntToType<0x6f>(), energy, acc);
            break;
        case 0x70:
            updateEnergyHook(soIntToType<0x70>(), energy, acc);
            break;
        case 0x71:
            updateEnergyHook(soIntToType<0x71>(), energy, acc);
            break;
        }
    }
}

template <>
void ftPeachKineticTransactor::updateEnergy<ftKineticEnergyController>(ftKineticEnergyController* energy, soModuleAccesser* acc) {
    if (acc->getKineticModule().getKineticType() <= 0x63) {
        ftKineticTransactor::updateEnergy(energy, acc);
        return;
    }
    if (energy->isEnable() == false || energy->isSuspend() == true) {
        return;
    }
    {
        switch (acc->getKineticModule().getKineticType()) {
        case 0x64:
            updateEnergyHook(soIntToType<0x64>(), energy, acc);
            break;
        case 0x65:
            updateEnergyHook(soIntToType<0x65>(), energy, acc);
            break;
        case 0x66:
            updateEnergySub1(energy, soIntToType<0x66>(), acc);
            break;
        case 0x67:
            updateEnergyHook(soIntToType<0x67>(), energy, acc);
            break;
        case 0x68:
            updateEnergyHook(soIntToType<0x68>(), energy, acc);
            break;
        case 0x69:
            updateEnergyHook(soIntToType<0x69>(), energy, acc);
            break;
        case 0x6a:
            updateEnergyHook(soIntToType<0x6a>(), energy, acc);
            break;
        case 0x6b:
            updateEnergyHook(soIntToType<0x6b>(), energy, acc);
            break;
        case 0x6c:
            updateEnergyHook(soIntToType<0x6c>(), energy, acc);
            break;
        case 0x6d:
            updateEnergyHook(soIntToType<0x6d>(), energy, acc);
            break;
        case 0x6e:
            updateEnergyHook(soIntToType<0x6e>(), energy, acc);
            break;
        case 0x6f:
            updateEnergyHook(soIntToType<0x6f>(), energy, acc);
            break;
        case 0x70:
            updateEnergyHook(soIntToType<0x70>(), energy, acc);
            break;
        case 0x71:
            updateEnergyHook(soIntToType<0x71>(), energy, acc);
            break;
        }
    }
}

template <>
void ftPeachKineticTransactor::updateEnergy<ftKineticEnergyStop>(ftKineticEnergyStop* energy, soModuleAccesser* acc) {
    if (acc->getKineticModule().getKineticType() <= 0x63) {
        ftKineticTransactor::updateEnergy(energy, acc);
        return;
    }
    if (energy->isEnable() == false || energy->isSuspend() == true) {
        return;
    }
    {
        switch (acc->getKineticModule().getKineticType()) {
        case 0x64:
            updateEnergyHook(soIntToType<0x64>(), energy, acc);
            break;
        case 0x65:
            updateEnergyHook(soIntToType<0x65>(), energy, acc);
            break;
        case 0x66:
            updateEnergyHook(soIntToType<0x66>(), energy, acc);
            break;
        case 0x67:
            updateEnergyHook(soIntToType<0x67>(), energy, acc);
            break;
        case 0x68:
            updateEnergyHook(soIntToType<0x68>(), energy, acc);
            break;
        case 0x69:
            updateEnergyHook(soIntToType<0x69>(), energy, acc);
            break;
        case 0x6a:
            updateEnergyHook(soIntToType<0x6a>(), energy, acc);
            break;
        case 0x6b:
            updateEnergyHook(soIntToType<0x6b>(), energy, acc);
            break;
        case 0x6c:
            updateEnergyShared(soIntToType<0x6c>(), energy, acc);
            break;
        case 0x6d:
            updateEnergyHook(soIntToType<0x6d>(), energy, acc);
            break;
        case 0x6e:
            updateEnergyHook(soIntToType<0x6e>(), energy, acc);
            break;
        case 0x6f:
            updateEnergyHook(soIntToType<0x6f>(), energy, acc);
            break;
        case 0x70:
            updateEnergyHook(soIntToType<0x70>(), energy, acc);
            break;
        case 0x71:
            updateEnergyHook(soIntToType<0x71>(), energy, acc);
            break;
        }
    }
}
