#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <gf/gf_task_scheduler.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the status of a Pikmin article that has been called back by Olimar (Special Lw): on exit the Pikmin is put at
// the position of the fighter whose task id is kept in the work int 0x10000000, and its kinetic energies are reset.
class wnPikminPikminStatusUniqProcessSpecialLwComp : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessSpecialLwComp() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPikminPikminStatusUniqProcessSpecialLwComp g_wnPikminPikminStatusUniqProcessSpecialLwComp;

void wnPikminPikminStatusUniqProcessSpecialLwComp::exitStatus(soModuleAccesser* moduleAccesser, int) {
    int taskId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, taskId);
    StageObject* object = dynamic_cast<StageObject*>(task);
    if (object != NULL) {
        Vec3f pos = soExternalValueAccesser::getPos(object);
        Vec2f safePos;
        safePos.m_x = pos.m_x;
        safePos.m_y = pos.m_y;
        moduleAccesser->getGroundModule().setShapeSafePos(&safePos, 0);
        moduleAccesser->getKineticModule().changeKinetic(0, moduleAccesser);
    }
}
