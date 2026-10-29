#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <math.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftStatusUniqProcessPassive : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessPassive() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessPassive g_ftStatusUniqProcessPassive;

void ftStatusUniqProcessPassive::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessPassive::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessPassive::execStop(soModuleAccesser* moduleAccesser) {
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    if (angle != 0.0f) {
        ftUtil::setXAngle(moduleAccesser, angle);
    }
}

void ftStatusUniqProcessPassive::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x68:
    case 0x69: {
        float lr = moduleAccesser->getPostureModule().getLr();
        float wallAngle = ftUtil::setAngleWallNormal(moduleAccesser, lr);
        moduleAccesser->getWorkManageModule().setFloat(wallAngle, 0x21000004);
        Vec2f normal(0.0f, 1.0f);
        if (lr == 1.0f) {
            normal = moduleAccesser->getGroundModule().getTouchNormal(2, 0);
        } else {
            normal = moduleAccesser->getGroundModule().getTouchNormal(4, 0);
        }
        float angle = 90.0f + 57.29578f * (-lr * atan2f(normal.m_x, normal.m_y));
        if (angle >= soValueAccesser::getConstantFloat(moduleAccesser, 0xc9c, 0)
            || angle <= soValueAccesser::getConstantFloat(moduleAccesser, 0xc9d, 0)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
        }
        break;
    }
    case 0x62:
    case 0x63:
    case 0x67:
        if (moduleAccesser->getMotionModule().getRate() == 0.0f) {
            float wallAngle = ftUtil::setAngleWallNormal(moduleAccesser, moduleAccesser->getPostureModule().getLr());
            moduleAccesser->getWorkManageModule().setFloat(wallAngle, 0x21000004);
        }
        break;
    }
}

void ftStatusUniqProcessPassive::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x66) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = 0.0f;
    }
}
