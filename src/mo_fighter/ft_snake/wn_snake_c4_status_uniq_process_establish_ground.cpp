#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
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

class wnSnakeC4StatusUniqProcessEstablishGround : public soStatusUniqProcess {
public:
    virtual ~wnSnakeC4StatusUniqProcessEstablishGround() { }
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the stick position is a mutable Vec3f*, as soPostureModule::setPos takes it.
    bool stickGround(soModuleAccesser* moduleAccesser, Vec3f* pos);
};

wnSnakeC4StatusUniqProcessEstablishGround g_wnSnakeC4StatusUniqProcessEstablishGround;

void wnSnakeC4StatusUniqProcessEstablishGround::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (soSlow::getInstance()->isAdjust() == false) {
        return;
    }
    Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(3, false);
    nodePos.m_z = moduleAccesser->getGroundModule().getZ();
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000001) == true) {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        Vec3f hitPos(0.0f, 0.0f, 0.0f);
        Vec3f normal(0.0f, 0.0f, 0.0f);
        Vec3f down;
        Vec3f dir;
        Vec3fSub(&dir, &nodePos, &pos);
        if (moduleAccesser->getGroundModule().rayCheck(&pos, &dir, &hitPos, &normal, true) == true) {
            hitPos.m_z = moduleAccesser->getGroundModule().getZ();
            if (stickGround(moduleAccesser, &hitPos) == true) {
                return;
            }
            moduleAccesser->getWorkManageModule().onFlag(0x22000002);
        } else {
            float length = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            down.m_x = 0.0f;
            down.m_y = -length;
            down.m_z = 0.0f;
            if (moduleAccesser->getGroundModule().rayCheck(&nodePos, &down, &hitPos, &normal, true) == true) {
                hitPos.m_z = moduleAccesser->getGroundModule().getZ();
                Vec3f target;
                Vec3fAdd(&target, &nodePos, &down);
                if (stickGround(moduleAccesser, &target) == true) {
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

bool wnSnakeC4StatusUniqProcessEstablishGround::stickGround(soModuleAccesser* moduleAccesser, Vec3f* pos) {
    if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
        fn_122_1AF04(moduleAccesser);
    }
    moduleAccesser->getPostureModule().setPos(pos);
    if (moduleAccesser->getGroundModule().attachGround(0) == false) {
        return false;
    }
    moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    fn_122_1B000(moduleAccesser);
    return true;
}
