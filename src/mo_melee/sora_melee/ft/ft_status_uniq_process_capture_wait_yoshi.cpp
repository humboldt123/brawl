#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureWaitYoshi : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureWaitYoshi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureWaitYoshi g_ftStatusUniqProcessCaptureWaitYoshi;

// HYPOTHESIS: being held by the Yoshi egg-lay gives the fighter a single hurtbox on the node 0x12e (radius 3 divided by its
// scale), hides the model, starts the capture, and rumbles when coming from status 0x107.
void ftStatusUniqProcessCaptureWaitYoshi::initStatus(soModuleAccesser* moduleAccesser) {
    float three = 3.0f;
    soCollisionHitData hitData;
    hitData.m_startOffsetPos.m_x = 0.0f;
    hitData.m_startOffsetPos.m_y = 0.0f;
    hitData.m_startOffsetPos.m_z = 0.0f;
    hitData.m_endOffsetPos.m_x = 0.0f;
    hitData.m_endOffsetPos.m_y = 0.0f;
    hitData.m_endOffsetPos.m_z = 0.0f;
    hitData.m_size = three / moduleAccesser->getPostureModule().getScale();
    hitData.m_nodeIndex = moduleAccesser->getModelModule().getCorrectNodeId(0x12E);
    hitData.m_part = static_cast<soCollision::Part>(0);
    hitData.m_height = soCollisionHitData::Height_Low;
    hitData.m_0x1c_3 = 0;
    hitData.m_shapeType = static_cast<soCollision::ShapeType>(0);
    moduleAccesser->getCollisionHitModule().set(0, &hitData, 0);
    moduleAccesser->getVisibilityModule().setWhole(0);
    moduleAccesser->getCollisionHitModule().setStatusAll(3, 0);
    moduleAccesser->getCollisionHitModule().setStatus(0, 0, 0);
    if (!reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot40()) {
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
    }
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == 0x107) {
        moduleAccesser->getControllerModule().setRumble(4, 0, true, -1);
    }
    moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(0), 0);
    moduleAccesser->getSituationModule().setKind(static_cast<SituationKind>(2), false);
}

void ftStatusUniqProcessCaptureWaitYoshi::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

// Leaving the hold (except into 0x108, 0x109 or 0x42) puts the ground shape's safe position at the link offset, releases the
// capture, clears work flag 0x1200003b, restores the hurtbox set and shows the model again.
void ftStatusUniqProcessCaptureWaitYoshi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x108) {
        return;
    }
    if (nextStatus == 0x109) {
        return;
    }
    if (nextStatus == 0x42) {
        return;
    }
    ftLinkModuleSlotView* link = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
    if (link->slot2c(0)) {
        Vec2f offset = link->slotDC(0);
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
        moduleAccesser->getGroundModule().setShapeSafePos(&offset, 0);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x1200003b);
    moduleAccesser->getCollisionHitModule().set(static_cast<soSet<soCollisionHitData>*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA803, 0)), 0);
    moduleAccesser->getVisibilityModule().setWhole(1);
}

bool ftStatusUniqProcessCaptureWaitYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(0x109);
}
