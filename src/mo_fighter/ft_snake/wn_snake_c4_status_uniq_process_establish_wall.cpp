#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" {
// wnSnakeC4Utility::detachConstraint, stickStageRot and dropFall (module map); outside any configured unit.
void fn_122_1AF04(soModuleAccesser*);
void fn_122_1B000(soModuleAccesser*);
void fn_122_1B130(soModuleAccesser*);
} // extern "C"

class wnSnakeC4StatusUniqProcessEstablishWall : public soStatusUniqProcess {
public:
    virtual ~wnSnakeC4StatusUniqProcessEstablishWall() { }
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
};

// MATCH-ONLY: wnSnakeC4StatusUniqProcessEstablishWall::stickWall (module map) keeps its address name: the
// original passes the word-sized touch mask to soGroundModule::attach, whose header parameter is u8.
extern "C" bool fn_122_1ACA4(wnSnakeC4StatusUniqProcessEstablishWall* process, soModuleAccesser* moduleAccesser, Vec3f* pos);

wnSnakeC4StatusUniqProcessEstablishWall g_wnSnakeC4StatusUniqProcessEstablishWall;

void wnSnakeC4StatusUniqProcessEstablishWall::execFixCamera(soModuleAccesser* moduleAccesser) {
    Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(3, false);
    nodePos.m_z = moduleAccesser->getGroundModule().getZ();
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000001) == true) {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        Vec3f side;
        Vec3f dir;
        Vec3fSub(&dir, &nodePos, &pos);
        if (moduleAccesser->getGroundModule().rayCheck(&pos, &dir, true) == true) {
            if (fn_122_1ACA4(this, moduleAccesser, &nodePos) == true) {
                return;
            }
            moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        } else {
            side.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            side.m_y = 0.0f;
            side.m_z = 0.0f;
            if (moduleAccesser->getPostureModule().getLr() < 0.0f) {
                side.m_x = -side.m_x;
            }
            if (moduleAccesser->getGroundModule().rayCheck(&nodePos, &side, true) == true) {
                Vec3f target;
                Vec3fAdd(&target, &nodePos, &side);
                if (fn_122_1ACA4(this, moduleAccesser, &target) == true) {
                    return;
                }
                moduleAccesser->getWorkManageModule().onFlag(0x22000002);
            }
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000002) == true) {
        fn_122_1B130(moduleAccesser);
    }
    moduleAccesser->getPostureModule().setPos(&nodePos);
}

bool fn_122_1ACA4(wnSnakeC4StatusUniqProcessEstablishWall* process, soModuleAccesser* moduleAccesser, Vec3f* pos) {
    if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
        moduleAccesser->getLinkModule().removeModelConstraint(true);
    }
    moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    moduleAccesser->getPostureModule().setPos(pos);
    if (moduleAccesser->getLinkModule().isValidParentShape(3) == true) {
        Vec2f center;
        Vec2f::copy(center, moduleAccesser->getLinkModule().getParentShapeCenterPos(3));
        moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
    }
    moduleAccesser->getGroundModule().update(1);
    moduleAccesser->getGroundModule().test(0);
    int touchFlag = moduleAccesser->getGroundModule().getTouchFlag(0);
    if (touchFlag == 0) {
        return false;
    }
    moduleAccesser->getGroundModule().attach(touchFlag, 0);
    fn_122_1B000(moduleAccesser);
    moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    return true;
}
