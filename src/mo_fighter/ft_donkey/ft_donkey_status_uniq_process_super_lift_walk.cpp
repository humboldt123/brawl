#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <ft/ft_util.h>
#include <types.h>

extern "C" {
// sora_melee helpers (names from the target object; signatures inferred from call sites)
// HYPOTHESIS: parameter names; the formal order (floats between the integers) follows the native argument moves.
void fn_27_1652A8(soModuleAccesser* moduleAccesser, int motionKind, u32 speedWork, float parameter, float speedScale, u32 rateWork, int flags);
void fn_27_164BF0(soModuleAccesser* moduleAccesser, void* data, u32 flagA, float speedMul, u32 flagB);
void fn_27_164E48(soModuleAccesser* moduleAccesser, void* data, u32 flagA, float speedMul, u32 flagB);
void fn_27_165458(soModuleAccesser* moduleAccesser, float current, float target, float acceleration, float deceleration, float limit, u32 speedWork, float speedScale);
// HYPOTHESIS: rodata table passed by address (lbl_92_rodata_78), contents not recovered
extern char lbl_92_rodata_78[];
}

class ftDonkeyStatusUniqProcessSuperLiftWalk : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSuperLiftWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessSuperLiftWalk::initStatus(soModuleAccesser* moduleAccesser) {
    float walkSpeedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    fn_27_1652A8(moduleAccesser, 0x1d1, 0x21000004, soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0), walkSpeedMul,
                 0x21000005, 0);
    fn_27_164BF0(moduleAccesser, lbl_92_rodata_78, 0x21000004, walkSpeedMul, 0x21000005);
}

void ftDonkeyStatusUniqProcessSuperLiftWalk::execStatus(soModuleAccesser* moduleAccesser) {
    float walkSpeedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    u32 workBase = 0x21000000;
    fn_27_165458(moduleAccesser, workManage.getFloat(workBase + 4),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbb9, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbba, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0), workBase + 4, walkSpeedMul);
    fn_27_164E48(moduleAccesser, lbl_92_rodata_78, workBase + 4, walkSpeedMul, workBase + 5);
}

ftDonkeyStatusUniqProcessSuperLiftWalk g_ftDonkeyStatusUniqProcessSuperLiftWalk;
