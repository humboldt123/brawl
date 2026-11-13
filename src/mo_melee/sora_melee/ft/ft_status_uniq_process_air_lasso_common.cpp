#include <ft/ft_status_uniq_process_air_lasso.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessAirLassoCommon g_ftStatusUniqProcessAirLassoCommon;

// HYPOTHESIS: while the lasso holds a ledge, touching the ceiling, or a wall the fighter faces (left wall when lr is 1), on a line that does
// not belong to the hooked ledge sets work flag 0x22000015 (the hang is cut).
void ftStatusUniqProcessAirLassoCommon::execFixPos(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (!ground.isStCliff(0)) {
        return;
    }
    float lr = moduleAccesser->getPostureModule().getLr();
    bool cut = false;
    bool otherLine = false;
    if (ground.isTouch(1, 0)) {
        int lineId = ground.getUpLineId(0);
        int cliffId = ground.getCliffId(0);
        if (!ground.isSameJoint(&cliffId, &lineId)) {
            otherLine = true;
        }
    }
    if (otherLine) {
        cut = true;
    }

    otherLine = false;
    bool touch = false;
    if (!cut && ground.isTouch(2, 0) && 1.0f == lr) {
        touch = true;
    }
    if (touch) {
        int lineId = ground.getLeftLineId(0);
        int cliffId = ground.getCliffId(0);
        if (!ground.isSameJoint(&cliffId, &lineId)) {
            otherLine = true;
        }
    }
    if (otherLine) {
        cut = true;
    }

    otherLine = false;
    touch = false;
    if (!cut && ground.isTouch(4, 0) && -1.0f == lr) {
        touch = true;
    }
    if (touch) {
        int lineId = ground.getRightLineId(0);
        int cliffId = ground.getCliffId(0);
        if (!ground.isSameJoint(&cliffId, &lineId)) {
            otherLine = true;
        }
    }
    if (otherLine) {
        cut = true;
    }
    work.setFlag(cut, 0x22000015);
}
