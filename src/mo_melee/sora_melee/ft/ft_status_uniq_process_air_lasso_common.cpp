#include <ft/ft_status_uniq_process_air_lasso.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessAirLassoCommon g_ftStatusUniqProcessAirLassoCommon;

// Sets otherLine when the touched line does not belong to the joint of the hooked ledge.
static inline void airLassoCheckJoint(soGroundModule& ground, int lineId, bool& otherLine) {
    int cliffId = ground.getCliffId(0);
    if (!ground.isSameJoint(&cliffId, &lineId)) {
        otherLine = true;
    }
}

// The ceiling check. BYTES: its line id sits above the cliff id on the stack, so the cliff id is declared first.
static inline bool airLassoCeiling(soGroundModule& ground) {
    bool otherLine = false;
    if (ground.isTouch(1, 0)) {
        int cliffId;
        int lineId = ground.getUpLineId(0);
        cliffId = ground.getCliffId(0);
        if (!ground.isSameJoint(&cliffId, &lineId)) {
            otherLine = true;
        }
    }
    return otherLine;
}

// HYPOTHESIS: while the lasso holds a ledge, touching the ceiling, or a wall the fighter faces (left wall when lr is 1), on a line that does
// not belong to the hooked ledge sets work flag 0x22000015 (the hang is cut).
void ftStatusUniqProcessAirLassoCommon::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (!ground.isStCliff(0)) {
        return;
    }
    float lr = moduleAccesser->getPostureModule().getLr();
    bool cut = false;
    // BYTES: the ceiling result shares the wall checks' touch flag; with a flag of its own the wall flags swap registers.
    bool otherLine;
    bool touch = airLassoCeiling(ground);
    if (touch) {
        cut = true;
    }

    otherLine = false;
    touch = false;
    if (!cut && ground.isTouch(2, 0) && 1.0f == lr) {
        touch = true;
    }
    if (touch) {
        airLassoCheckJoint(ground, ground.getLeftLineId(0), otherLine);
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
        airLassoCheckJoint(ground, ground.getRightLineId(0), otherLine);
    }
    if (otherLine) {
        cut = true;
    }
    work.setFlag(cut, 0x22000015);
}
