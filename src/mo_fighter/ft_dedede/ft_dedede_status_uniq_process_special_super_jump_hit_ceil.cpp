#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/motion/so_motion_change_param.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: ftUtil::adjustCeil lives in the sora_melee REL (map name ftUtil__adjustCeil, fn_27_193C20); called by its
// address symbol because ft_util.h does not declare it yet.
extern "C" void fn_27_193C20(soModuleAccesser* moduleAccesser, bool);

class ftDededeStatusUniqProcessSpecialSuperJumpHitCeil : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpHitCeil() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void setTurnTransN(soModuleAccesser* moduleAccesser);
};

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::initStatus(soModuleAccesser* moduleAccesser) {
    // Plain-old-data copy of soMotionChangeParam (the real type has a user constructor the target does not call)
    struct ChangeParam {
        int kind;
        float frame;
        float rate;
        u8 unk14, unk15, unk16, unk17;
    };
    ChangeParam change;
    change.kind = 0x1F9;
    change.frame = 0.0f;
    change.rate = 1.0f;
    change.unk14 = 0;
    change.unk15 = 0;
    change.unk16 = 0;
    change.unk17 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(reinterpret_cast<soMotionChangeParam*>(&change));
    moduleAccesser->getPostureModule().getLr();
    fn_27_193C20(moduleAccesser, true);
    setTurnTransN(moduleAccesser);
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::execStatus(soModuleAccesser* moduleAccesser) {
    setTurnTransN(moduleAccesser);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) <= soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0)) {
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000002);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) == soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0)) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        Vec3f accel(0.0f, 0.0f, 0.0f);
        Vec2f speed(0.0f, 0.0f);
        gravity.resetEnergy(0, &speed, &accel, moduleAccesser);
        gravity.enable();
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::execStop(soModuleAccesser* moduleAccesser) {
    setTurnTransN(moduleAccesser);
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getModelModule().clearNodeSRT(5);
}

void ftDededeStatusUniqProcessSpecialSuperJumpHitCeil::setTurnTransN(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: posture getter assumed to be getLr.
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f rotate = moduleAccesser->getModelModule().getNodeRotate(5);
        rotate.m_y = 180.0f;
        moduleAccesser->getModelModule().setNodeRotate(5, &rotate);
    }
}

ftDededeStatusUniqProcessSpecialSuperJumpHitCeil g_ftDededeStatusUniqProcessSpecialSuperJumpHitCeil;
