#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define SO_MODULE_ACCESSER_FALCON_PUNCH_HELPERS
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_posture_view.h>
#include <math.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftCaptainStatusUniqProcessFalconPunch : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconPunch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFalconPunchTurn : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconPunchTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftCaptainStatusUniqProcessFalconPunch g_ftCaptainStatusUniqProcessFalconPunch;
ftCaptainStatusUniqProcessFalconPunchTurn g_ftCaptainStatusUniqProcessFalconPunchTurn;

void ftCaptainStatusUniqProcessFalconPunch::initStatus(soModuleAccesser* moduleAccesser) {
    // Work float 0x21000004 receives the current posture value.
    float posture = reinterpret_cast<ftStatusPostureView&>(moduleAccesser->getPostureModule()).unk2c();
    moduleAccesser->getWorkManageModule().setFloat(posture, 0x21000004);
}

void ftCaptainStatusUniqProcessFalconPunch::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        moduleAccesser->process_air();
    }
}

// HYPOTHESIS: the motion energy keeps a float at +0x38 (the shared header models it as unk38) and a float at +0x44
// that is negated when the stored value changes. Accessed by offset because the shared header types them differently.
static float& ftCaptainFalconPunchEnergyFloat(ftKineticEnergyMotion& energy, int offset) {
    return *reinterpret_cast<float*>(reinterpret_cast<char*>(&energy) + offset);
}

void ftCaptainStatusUniqProcessFalconPunchTurn::initStatus(soModuleAccesser* moduleAccesser) { }

void ftCaptainStatusUniqProcessFalconPunchTurn::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int kind = moduleAccesser->getSituationModule().getKind();
    int prevKind = workManage.getInt(0x20000002);
    workManage.setInt(kind, 0x20000002);
    if (kind != prevKind) {
        ftKineticEnergyMotion& energy =
            dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
        float posture = workManage.getFloat(0x21000004);
        if (posture != ftCaptainFalconPunchEnergyFloat(energy, 0x38)) {
            ftCaptainFalconPunchEnergyFloat(energy, 0x44) = -ftCaptainFalconPunchEnergyFloat(energy, 0x44);
            ftCaptainFalconPunchEnergyFloat(energy, 0x38) = posture;
        }
    }
    if (kind == 2) {
        moduleAccesser->process_air();
    }
}

// BYTES: a float absolute value through an inline function (like MSL's C++ fabs overload): the inlined parameter copy
// places fabs before the stick value's copy; __fabsf directly schedules it after, and __fabs/fabs add an frsp.
static inline float absInline(float x) { return __fabsf(x); }

// Air step of Falcon Punch: decides the punch direction, then per motion phase (work int 0x20000000) switches the
// kinetic type once, guarded by work flags 0x22000014 / 0x22000015.
void soModuleAccesser::process_air() {
    soWorkManageModule& workManage = getWorkManageModule();
    soKineticModule& kinetic = getKineticModule();
    dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    dir_decide();
    switch (workManage.getInt(0x20000000)) {
    case 0:
        if (!workManage.isFlag(0x22000014)) {
            kinetic.changeKinetic(10, this);
            workManage.onFlag(0x22000014);
        }
        break;
    case 1: {
        float mul = getConstantFloatKirby(0xfa4);
        Vec3f speedMul(mul, mul, 1.0f);
        kinetic.mulSpeed(&speedMul, soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
        break;
    }
    case 2:
        if (!workManage.isFlag(0x22000015)) {
            kinetic.changeKinetic(0, this);
            workManage.onFlag(0x22000015);
        }
        break;
    }
}

// Once flag 0x22000012 is set (and 0x22000013 not yet), aims the punch with the stick: the vertical stick past a dead
// zone (0xfa0..0xfa1) scales a maximum angle (0xfa2, degrees); speed 0xfa3 is split along that angle between the
// normal energy (horizontal, facing) and the gravity energy (vertical).
void soModuleAccesser::dir_decide() {
    soWorkManageModule& workManage = getWorkManageModule();
    if (workManage.isFlag(0x22000012) && !workManage.isFlag(0x22000013)) {
        soKineticModule& kinetic = getKineticModule();
        soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        kinetic.changeKinetic(0, this);
        kinetic.getEnergy(2)->disable();
        float stickY = getControllerModule().getStickY();
        float tiltMax;
        float tilt = absInline(stickY);
        tiltMax = getConstantFloatKirby(0xfa1);
        if (tilt > tiltMax) {
            tilt = tiltMax;
        }
        float tiltMin = getConstantFloatKirby(0xfa0);
        tilt -= tiltMin;
        if (tilt < 0.0f) {
            tilt = 0.0f;
        }
        if (stickY < 0.0f) {
            tilt = -tilt;
        }
        float range = tiltMax - tiltMin;
        float angle = 0.0174533f * (tilt * getConstantFloatKirby(0xfa2) / range);
        float lr = getPostureModule().getLr();
        float speed = getConstantFloatKirby(0xfa3);
        Vec2f horizontal(cosf(angle), 0.0f);
        horizontal *= speed;
        horizontal.m_x *= lr;
        normal.m_speed = horizontal;
        normal.m_speedLimit = horizontal;
        normal.m_speedTarget = horizontal;
        normal.m_brake = Vec2f(0.0f, 0.0f);
        normal.enable();
        gravity.m_speedY = speed * sinf(angle);
        workManage.onFlag(0x22000013);
    }
}
