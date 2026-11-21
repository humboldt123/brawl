#include <mt/mt_prng.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Pokemon Trainer: Move (the trainer's move-to-target status between the two Pokemon).
class ftPokeTrainerStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~ftPokeTrainerStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftPokeTrainerStatusUniqProcessMove g_ftPokeTrainerStatusUniqProcessMove;

void ftPokeTrainerStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) {
    float choice;
    if (moduleAccesser->getWorkManageModule().getFloat(0x1100001e) <
        soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0)) {
        float x = moduleAccesser->getWorkManageModule().getFloat(0x1100001f);
        choice = (-1.0f == x) ? -1.0f : 1.0f;
    } else {
        float x = moduleAccesser->getWorkManageModule().getFloat(0x1100001f);
        if (-1.0f == x) {
            choice = (randi(3) < 2) ? -1.0f : 1.0f;
        } else {
            choice = (randi(3) < 2) ? 1.0f : -1.0f;
        }
    }
    if (choice == 1.0f) {
        soMotionChangeParam param;
        param.m_kind = 0x1d4;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    } else {
        soMotionChangeParam param;
        param.m_kind = 0x1d5;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
}

void ftPokeTrainerStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftPokeTrainerStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
