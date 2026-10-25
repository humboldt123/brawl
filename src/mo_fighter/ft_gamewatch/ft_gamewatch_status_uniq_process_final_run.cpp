#include <ft/ft_common_data_accesser.h>
#include <ft/ft_util.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <types.h>

// HYPOTHESIS: run parameter block pointer stored at ftData+0xB8; floats at +0x14, +0x18 and +0x1C.
struct ftGameWatchRunParam {
    u8 unk0[0x14];
    float unk14;
    float unk18;
    float unk1C;
};

#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>

class ftGameWatchStatusUniqProcessFinalRun : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalRun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    void setMotionRate(soModuleAccesser* moduleAccesser);
};

ftGameWatchStatusUniqProcessFinalRun g_ftGameWatchStatusUniqProcessFinalRun;

// Native movement multiplier and countdown threshold.
static const float s_runSpeedMul = 1.0f;
static const float s_runCountdownThreshold = 0.0f;

// HYPOTHESIS: helper parameter names and formal source types/order; native GP/FP argument values are verified.
// The shared walk utility initializes and updates the final movement speed.
extern "C" void fn_27_1652A8(soModuleAccesser* moduleAccesser, int motionKind,
                             int speedWork, int rateWork, int flags,
                             float parameter, float speedScale);
extern "C" void fn_27_165458(soModuleAccesser* moduleAccesser, int speedWork, float current,
                             float target, float acceleration, float deceleration,
                             float speedScale, float speedLimit);

static ftGameWatchRunParam* getRunParam() {
    ftData* data = g_ftCommonDataAccesser.getData(Fighter_GameWatch);
    return *reinterpret_cast<ftGameWatchRunParam**>(reinterpret_cast<u8*>(data) + 0xB8);
}

void ftGameWatchStatusUniqProcessFinalRun::initStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunParam* param = getRunParam();
    float motionSpeed = param->unk1C;
    fn_27_1652A8(moduleAccesser, 0x1f7, 0x21000005, 0x21000007,
                 0, motionSpeed, s_runSpeedMul);
    setMotionRate(moduleAccesser);
    ftGameWatchStatusUniqProcessFinalSwim::initStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRun::execStatus(soModuleAccesser* moduleAccesser) {
    ftGameWatchRunParam* param = getRunParam();
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    float speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xc37, 0);
    float current = workManage.getFloat(0x21000005);

    fn_27_165458(moduleAccesser, 0x21000005, current, param->unk1C,
                 param->unk14, param->unk18, speedScale, s_runSpeedMul);
    setMotionRate(moduleAccesser);

    // Advance the countdown only while it is positive.
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000004) > s_runCountdownThreshold) {
        moduleAccesser->getWorkManageModule().subFloat(s_runSpeedMul, 0x21000004);
    }
    ftGameWatchStatusUniqProcessFinalSwim::execStatus(moduleAccesser);
}

void ftGameWatchStatusUniqProcessFinalRun::setMotionRate(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    double magnitude = fabs(workManage.getFloat(0x21000005)); // MATCH-ONLY: the double result is narrowed afterwards
    float speed = (float)magnitude;
    float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    rate = speed * rate;
    moduleAccesser->getMotionModule().setRate(rate);
}
