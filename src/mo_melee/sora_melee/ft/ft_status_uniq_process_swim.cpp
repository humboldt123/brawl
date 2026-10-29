#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/area/so_area_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessSwim : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSwim g_ftStatusUniqProcessSwim;

// HYPOTHESIS: value stored in work int 0x10000027 when the fighter leaves the water into status 0 or 0xb.
static int s_swimExitWorkValue = 0x1e;

void ftStatusUniqProcessSwim::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwim::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessSwim::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xbb: {
        bool inWater = moduleAccesser->getAreaModule().getEnviromentElementChecker()->isWater();
        int count = moduleAccesser->getWorkManageModule().getInt(0x20000004);
        if (inWater == false) {
            count++;
            if (count >= 3) {
                if (moduleAccesser->getGroundModule().isTouch(8, 0) == true) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
                } else {
                    moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
                }
            }
        } else {
            count = 0;
        }
        moduleAccesser->getWorkManageModule().setInt(count, 0x20000004);
        break;
    }
    }
}

void ftStatusUniqProcessSwim::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessSwim::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xb4:
        moduleAccesser->getMotionModule().remove2ndAnimChr();
        break;
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
        break;
    case 0xbb: {
        Vec3f rot = moduleAccesser->getPostureModule().getRot(0);
        rot.m_y = 0.0f;
        moduleAccesser->getPostureModule().setRot(&rot, 0);
        break;
    }
    case 0xbc:
        moduleAccesser->getEffectModule().killAll(4, true, true);
        break;
    }
    switch (nextStatus) {
    case 0xb3:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
    case 0xbb:
    case 0xbc:
        break;
    default:
        moduleAccesser->getAreaModule().setAreaShapeType(3, static_cast<soAreaInstance::ShapeType>(3));
        switch (nextStatus) {
        case 0:
        case 0xb:
            moduleAccesser->getWorkManageModule().setInt(s_swimExitWorkValue, 0x10000027);
            break;
        }
        break;
    }
}
