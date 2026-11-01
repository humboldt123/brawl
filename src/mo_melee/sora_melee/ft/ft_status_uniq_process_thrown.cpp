#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <gf/gf_task.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessThrown : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessThrown() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessThrown g_ftStatusUniqProcessThrown;

// Status of a fighter being thrown (the thrower's link node): the thrown fighter hangs on the capture module until the throw
// releases it.
void ftStatusUniqProcessThrown::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
    if (soValueAccesser::getConstantInt(moduleAccesser, 0x5A98, 0) == 1) {
        moduleAccesser->getMotionModule().setKindOffset(4);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003B)) {
        moduleAccesser->getCollisionHitModule().setStatusAll(3, 0);
        moduleAccesser->getCollisionHitModule().setStatus(0, 0, 0);
    }
}

void ftStatusUniqProcessThrown::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

// Leaving the throw (status 0x50 is the throw release): the fighter is dropped onto the ground below the thrower when there
// is some, and snapped to the ground collision; throw-only state (the hit statuses and visibility) is reset.
void ftStatusUniqProcessThrown::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    bool moved = false;
    if (kind == 0x50) {
        gfTask* task = gfTask::getTask(moduleAccesser->getLinkModule().getParentTaskId(0));
        StageObject* thrower = dynamic_cast<StageObject*>(task);
        soGroundModule* throwerGround = soExternalValueAccesser::getGroundModule(thrower);
        Vec3f position = moduleAccesser->getPostureModule().getPos();
        Vec3f groundPosition;
        if (throwerGround->getDownCorrectPos(&position, &groundPosition, 0) == true) {
            moduleAccesser->getPostureModule().setPos(&position);
            moved = true;
        }
    }
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
    if (moved == true) {
        moduleAccesser->m_stageObject->updateNodeSRT();
        moduleAccesser->getGroundModule().update(1);
        moduleAccesser->getGroundModule().test(0);
        Vec3f correctPos = moduleAccesser->getGroundModule().getCorrectPos(0);
        moduleAccesser->getPostureModule().setPos(&correctPos);
        moduleAccesser->getGroundModule().attachGround(0);
        if (moduleAccesser->getGroundModule().isTouch(8, 0) == true) {
            correctPos = moduleAccesser->getGroundModule().getCorrectPos(0);
            moduleAccesser->getPostureModule().setPos(&correctPos);
            moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
            moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003B)) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003B);
        moduleAccesser->getCollisionHitModule().set(
            static_cast<soSet<soCollisionHitData>*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA803, 0)), 0);
        moduleAccesser->getVisibilityModule().setWhole(1);
    }
}

bool ftStatusUniqProcessThrown::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot38(moduleAccesser, damageInfo);
}
