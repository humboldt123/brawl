#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_ASSIGN_NOINLINE // MATCH-ONLY: the original assigns the carried vectors through the out-of-line operator=
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <ft/kirby/ft_kirby_min.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: Kirby's Hammer (down-B charge swing) status process; class and constant names are unknown.
class ftKirbyStatusUniqProcessHammer : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessHammer() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// Starting the swing in the air stops the fall for a moment (the gravity energy starts from the Hammer rise speed); with the
// parameter 0x5dc2 set the horizontal state of the stick energy moves to the stop energy, which keeps the momentum of the swing.
void ftKirbyStatusUniqProcessHammer::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() != 2) {
        return;
    }
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    kinetic.changeKinetic(0, moduleAccesser);
    float speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
    gravity.enable();
    Vec2f speed(0.0f, speedY);
    gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    if (soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0) == 1) {
        Vec2f carried = controller.getSpeed();
        Vec2f limit = controller.m_speedLimit;
        Vec2f target = controller.m_speedTarget;
        Vec2f accel = controller.m_accel;
        Vec2f brake = controller.m_brake;
        controller.disable();
        stop.enable();
        Vec2f unused;
        stop.resetEnergy(9, &unused, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.m_speed = carried;
        stop.m_speedLimit = limit;
        stop.m_speedTarget = target;
        stop.m_accel = accel;
        stop.m_brake = brake;
    }
}

// HYPOTHESIS: the extend parameter group of the Hammer: a pointer whose word at +8 is the number of frames of the hammer rise.
struct ftKirbyHammerExtend {
    int unk0;
    const int* unk4;
};

// Leaving the swing removes the hammer article; a charged swing (frames from the extend parameters) keeps the charge in the work.
void ftKirbyStatusUniqProcessHammer::exitStatus(soModuleAccesser* moduleAccesser, int) {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_Kirby);
    moduleAccesser->getGenerateArticleManageModule().removeExist(*reinterpret_cast<int*>(reinterpret_cast<u8*>(data) + 0x120), 0);
    ftKirbyHammerExtend* extend = reinterpret_cast<ftKirbyHammerExtend*>(dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).getExtendParam());
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float frames = extend->unk4[2];
    if (frames > 0.0f) {
        workManage.onFlag(0x12000003);
        workManage.setFloat(frames, 0x11000000);
        workManage.setFloat(1.0f, 0x11000001);
    }
}

void ftKirbyStatusUniqProcessHammer::execStatus(soModuleAccesser* moduleAccesser) {
}

ftKirbyStatusUniqProcessHammer g_ftKirbyStatusUniqProcessHammer;
