#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the fighter attribute block is copied whole (0x2E4 bytes); the four floats at 0x64 seed the air gravity.
struct ftDiddyAirParam {
    u8 unk0[0x64];
    float unk64;
    float unk68;
    float unk6C;
    float unk70;
    u8 unk74[0x270];
};

class ftDiddyStatusUniqProcessSpecialBarrelJet : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJet() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    static void changeKineticAir(soModuleAccesser* moduleAccesser);
};

class ftDiddyStatusUniqProcessSpecialBarrelJetCharge : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJetCharge() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000001);
    moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        ftDiddyStatusUniqProcessSpecialBarrelJet::changeKineticAir(moduleAccesser);
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::execStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().addInt(1, 0x20000000);
    moduleAccesser->getWorkManageModule().setFloat(moduleAccesser->getMotionModule().getWeight(), 0x2100000c);
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().isSituationChanged() == true) {
        if (moduleAccesser->getSituationModule().getKind() != 0) {
            ftDiddyStatusUniqProcessSpecialBarrelJet::changeKineticAir(moduleAccesser);
        }
    } else if (moduleAccesser->getSituationModule().getKind() != 0) {
        ftDiddyAirParam param = *reinterpret_cast<ftDiddyAirParam*>(g_ftCommonDataAccesser.getParam(Fighter_Diddy));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_gravity = -param.unk64;
        gravity.m_fallSpeedMax = param.unk68;
        gravity.unk18 = param.unk6C;
        gravity.unk1C = param.unk70;
    }

    soMotionModule& motion = moduleAccesser->getMotionModule();
    float prevTilt = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float nextTilt = moduleAccesser->getControllerModule().getStickX() - prevTilt;
    float divisor = soValueAccesser::getConstantFloat(moduleAccesser, 0xfad, 0);
    nextTilt = nextTilt * soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0) / divisor;
    nextTilt = prevTilt + nextTilt;
    double absTiltValue = fabs(nextTilt);
    float absTilt = absTiltValue;
    moduleAccesser->getWorkManageModule().setFloat(nextTilt, 0x21000004);
    if (absTilt < soValueAccesser::getConstantFloat(moduleAccesser, 0xc4b, 0)) {
        motion.remove2ndAnimChr();
        motion.setWeightRate(1.0f - absTilt - motion.getPrevWeight());
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000001);
    } else {
        int motionKind;
        int otherKind;
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            motionKind = 0x1e8;
            otherKind = 0x1ea;
        } else {
            motionKind = 0x1e9;
            otherKind = 0x1eb;
        }
        int current = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        motionKind = nextTilt * moduleAccesser->getPostureModule().getLr() > 0.0f ? motionKind : otherKind;
        if (current != motionKind || motion.get2ndKind() == -1) {
            motion.remove2ndAnimChr();
            motion.add2ndAnimChr(motion.getFrame(), 1.0f, motionKind, false);
            if (moduleAccesser->getSituationModule().isSituationChanged()) {
                motion.setWeight(moduleAccesser->getWorkManageModule().getFloat(0x2100000c));
            }
            moduleAccesser->getWorkManageModule().setInt(motionKind, 0x20000001);
        }
        if (!moduleAccesser->getSituationModule().isSituationChanged()) {
            motion.setWeightRate(1.0f - absTilt - motion.getPrevWeight());
        }
    }
    float weight = motion.getWeight();
    float angle = (1.0f - weight) * soValueAccesser::getConstantFloat(moduleAccesser, 0xfad, 0);
    if (nextTilt * moduleAccesser->getPostureModule().getLr() > 0.0f) {
        angle = 90.0f - angle;
    } else {
        angle = 90.0f + angle;
    }
    moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000005);
}

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::execStop(soModuleAccesser* moduleAccesser) { }

void ftDiddyStatusUniqProcessSpecialBarrelJetCharge::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getMotionModule().remove2ndAnimChr();
}

ftDiddyStatusUniqProcessSpecialBarrelJetCharge g_ftDiddyStatusUniqProcessSpecialBarrelJetCharge;
