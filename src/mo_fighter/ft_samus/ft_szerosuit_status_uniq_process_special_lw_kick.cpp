#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSZerosuitStatusUniqProcessSpecialLwKick : public soStatusUniqProcess {
public:
    virtual ~ftSZerosuitStatusUniqProcessSpecialLwKick() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftSZerosuitStatusUniqProcessSpecialLwKick::initStatus(soModuleAccesser*) { }

// HYPOTHESIS: extendParam[2] holds the kick tuning floats; params[4] is the stick magnitude limit that
// gates the facing-kick flags. Flags 0x1b/0x1c are the kick-input and kick-facing windows.
void ftSZerosuitStatusUniqProcessSpecialLwKick::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    const float* params = (const float*)g_ftCommonDataAccesser.getData(static_cast<ftKind>(0x18))->extendParam[2];
    float lr = posture.getLr();
    float stickX = controller.getStickX();
    // HYPOTHESIS: zero threshold (lbl_94_rodata_78 == 0.0f) for the sign tests below.
    if (__fabsf(stickX) >= params[4]) {
        float stickSign = (float)((stickX < 0.0f) ? -1 : 1);
        float lrSign = (float)((lr < 0.0f) ? -1 : 1);
        if (lrSign == stickSign) {
            workManage.onFlag(0x2200001c);
        }
    }
    if (workManage.isFlag(0x2200001b)) {
        workManage.offFlag(0x2200001b);
        if (workManage.isFlag(0x2200001c)) {
            posture.reverseLr();
            posture.updateRotYLr();
        }
    }
}

void ftSZerosuitStatusUniqProcessSpecialLwKick::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x115 || static_cast<u32>(nextStatus - 0x11a) <= 5u) {
        return;
    }
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    if (workManage.isFlag(0x22000017)) {
        posture.reverseLr();
        posture.updateRotYLr();
    }
}

ftSZerosuitStatusUniqProcessSpecialLwKick g_ftSZerosuitStatusUniqProcessSpecialLwKick;
