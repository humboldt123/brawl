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
// HYPOTHESIS: rodata table passed by address (lbl_92_rodata_90), contents not recovered
extern char lbl_92_rodata_90[];
}

class ftDonkeyStatusUniqProcessShoulderWalk : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessShoulderWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessShoulderWalk::initStatus(soModuleAccesser* moduleAccesser) {
    float walkSpeedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    fn_27_1652A8(moduleAccesser, 0x1da, 0x21000004, soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0), walkSpeedMul,
                 0x21000005, 0);
    fn_27_164BF0(moduleAccesser, lbl_92_rodata_90, 0x21000004, walkSpeedMul, 0x21000005);
}

void ftDonkeyStatusUniqProcessShoulderWalk::execStatus(soModuleAccesser* moduleAccesser) {
    float walkSpeedMul = ftUtil::getWalkSpeedMul(moduleAccesser);
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    fn_27_165458(moduleAccesser, workManage.getFloat(0x21000004),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbbb, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbb9, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xbba, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xc2e, 0), 0x21000004, walkSpeedMul);
    fn_27_164E48(moduleAccesser, lbl_92_rodata_90, 0x21000004, walkSpeedMul, 0x21000005);
}

ftDonkeyStatusUniqProcessShoulderWalk g_ftDonkeyStatusUniqProcessShoulderWalk;
