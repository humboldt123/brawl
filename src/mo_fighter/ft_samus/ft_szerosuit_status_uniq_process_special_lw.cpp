#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <so/area/so_area_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSZerosuitStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftSZerosuitStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: extendParam[2] holds the special-lw tuning block; params[5..8] are the two corners of the
// wall-check area that is registered for this move (area id 9).
void ftSZerosuitStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    soAreaModule& area = moduleAccesser->getAreaModule();
    area.enableArea(9, true, -1);
    const float* params = (const float*)g_ftCommonDataAccesser.getData(static_cast<ftKind>(0x18))->extendParam[2];
    Vec2f lower(params[5], params[6]);
    Vec2f upper(params[7], params[8]);
    area.setAreaShape(9, &lower, &upper);
}

// The per-frame area-hit loop (gfTask lookup and kill/notify calls) is not reconstructed yet.
void ftSZerosuitStatusUniqProcessSpecialLw::execFixPos(soModuleAccesser*) { }

void ftSZerosuitStatusUniqProcessSpecialLw::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
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

ftSZerosuitStatusUniqProcessSpecialLw g_ftSZerosuitStatusUniqProcessSpecialLw;
