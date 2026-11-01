#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
// Use the native Fighter observer override before transitive Fighter includes; this preserves the verified primary
// vtable slots of getOwner/getFtKind.
#define FT_MODULE_BUILDER
#include <cm/cm_camera_controller.h>
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_catch_module_view.h>
#include <ft/ft_link_event_throw.h>
#include <math.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessThrowKirby : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessThrowKirby() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessThrowKirby g_ftStatusUniqProcessThrowKirby;

// Kirby's throw (swallowed copy of a throw): same link event as the common throw, with Kirby's fighter kind in it.
void ftStatusUniqProcessThrowKirby::initStatus(soModuleAccesser* moduleAccesser) {
    int throwStatus = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    ftLinkEventThrow event(0xB, throwStatus + 4);
    event.m_hasMotionBit = true;
    event.m_unk14 = dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getFtKind();
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(event.m_output0, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(event.m_output1, 0x20000004);
    if (throwStatus == 0x74) {
        moduleAccesser->getCameraModule().setEnableUpdatePos(false, -1);
    }
    moduleAccesser->getWorkManageModule().onFlag(0x12000029);
    moduleAccesser->getWorkManageModule().offFlag(0x1200001A);
    moduleAccesser->getCollisionHitModule().setInvincibleFrameGlobal(soValueAccesser::getConstantInt(moduleAccesser, 0x5A2E, 0), false, 0);
}

// Falling out of the camera range ends the throw: above the camera's vertical band the fighter is put in the air.
void ftStatusUniqProcessThrowKirby::execFixPos(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getGroundModule().setTestCollStopStatus(true, 0);
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (speed.m_y < 0.0f) {
        Vec3f position = moduleAccesser->getPostureModule().getPos();
        float rangeTop = *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x160);
        float rangeBottom = *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x150);
        float limit = rangeBottom + 0.5f * __fabsf(rangeTop - rangeBottom);
        bool aboveBand = false;
        if (position.m_y > limit) {
            aboveBand = true;
        }
        if (aboveBand) {
            moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(0), 0);
            moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        } else {
            moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        }
    }
}

// Leaving the throw tells the catch module the throw is over, except when the throw animation (status 0xE) has already ended.
void ftStatusUniqProcessThrowKirby::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    switch (kind) {
    case 0xE:
        if (!moduleAccesser->getMotionModule().isEnd()) {
            reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->notifyThrowExit(0, 0);
        }
        break;
    default:
        reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->notifyThrowExit(0, 0);
        break;
    }
}

bool ftStatusUniqProcessThrowKirby::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkThrowDamage(moduleAccesser, damageInfo);
}
