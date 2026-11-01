#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_util.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessItemLift : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemLift() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessItemLift g_ftStatusUniqProcessItemLift;

// Status 0x97 (item lift start): starts the lift motion (HYPOTHESIS: motion kind 0xFB) at the item lift motion rate.
void ftStatusUniqProcessItemLift::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x97: {
        soMotionChangeParam changeParam;
        changeParam.m_kind = 0xFB;
        changeParam.m_frame = 0.0f;
        changeParam.m_rate = 1.0f;
        changeParam._12 = 0;
        changeParam._13 = 0;
        changeParam._14 = 0;
        changeParam._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&changeParam);
        moduleAccesser->getMotionModule().setRate(ftUtil::getItemLiftMotionRateMul(moduleAccesser));
        break;
    }
    }
}

// Status 0x9A (item lift): the stick tilt (against the common threshold 0xD77) scales the lift motion rate; a neutral stick
// plays it at full speed and the tilt never slows it below 0.1.
void ftStatusUniqProcessItemLift::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x9A: {
        Vec2f rate;
        ftUtil::getStickXRate(moduleAccesser, soValueAccesser::getConstantFloat(moduleAccesser, 0xD77, 0), &rate);
        float x = rate.m_x;
        if (x <= 0.0f) {
            rate.m_x = 1.0f;
        }
        if (rate.m_x < 0.1f) {
            rate.m_x = 0.1f;
            x = 0.1f;
        }
        moduleAccesser->getWorkManageModule().setFloat(x, 0x21000004);
        moduleAccesser->getMotionModule().setRate(rate.m_x * ftUtil::getItemLiftMotionRateMul(moduleAccesser));
        break;
    }
    }
}

void ftStatusUniqProcessItemLift::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    switch (kind) {
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9D:
        break;
    default:
        if (moduleAccesser->getItemManageModule().isHaveItem(0)) {
            // HYPOTHESIS: the drop arguments are lbl_27_rodata_236C (90.0f) and lbl_27_rodata_2360 (0.0f).
            moduleAccesser->getItemManageModule().dropItem(90.0f, 0.0f, 0);
        }
        break;
    }
}
