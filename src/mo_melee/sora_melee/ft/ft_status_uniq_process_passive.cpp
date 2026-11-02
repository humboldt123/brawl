#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <math.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/motion/so_motion_change_param.h>
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

// HYPOTHESIS: passive (tech) statuses: 0x60/0x61 ground tech, 0x62 wall tech, 0x63 tech jump, 0x67 wall jump, 0x68/0x69 wall
// cling, 0x64 ceiling tech, 0x65 running into a wall, 0x66 hitting the head. Entering sets the motion, snaps the fighter to the
// wall/ceiling it touches, and plays the touch effect at the contact point.
void ftStatusUniqProcessPassive::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x60:
    case 0x61: {
        Vec2f touchNormal(0.0f, 1.0f);
        Vec2f touchPos;
        if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
            float angle = (float)atan2(-touchNormal.m_x, touchNormal.m_y);
            Vec3f* rot = &Vec3f(0.0f, 0.0f, angle);
            moduleAccesser->getEffectModule().req(EfID(0x2c), &Vec3f(touchPos.m_x, touchPos.m_y, 0.0f), rot, 1.0f, 0, -1);
        }
        moduleAccesser->getControllerModule().setRumble(0xd, 0, false, -1);
        break;
    }
    case 0x62: {
        soMotionChangeParam param;
        param.m_kind = 0xc1;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        float wallDir = 0.0f;
        if (moduleAccesser->getGroundModule().isTouch(4, 0)) {
            wallDir = -1.0f;
        } else if (moduleAccesser->getGroundModule().isTouch(2, 0)) {
            wallDir = 1.0f;
        }
        ftUtil::adjustWall(moduleAccesser, wallDir, wallDir, 1.0f, 1);
        int xluFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a10, 0);
        moduleAccesser->getCollisionHitModule().setXluFrameGlobal(xluFrames, 0);
        Vec2f touchNormal(0.0f, 1.0f);
        Vec2f touchPos;
        if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
            float angle = (float)atan2(-touchNormal.m_x, touchNormal.m_y);
            Vec3f* rot = &Vec3f(0.0f, 0.0f, angle);
            moduleAccesser->getEffectModule().req(EfID(0x2c), &Vec3f(touchPos.m_x, touchPos.m_y, 0.0f), rot, 1.0f, 0, -1);
        }
        moduleAccesser->getControllerModule().setRumble(0xd, 0, false, -1);
        break;
    }
    case 0x63: {
        float motionFrame = 0.0f;
        float wallDir = 0.0f;
        if (moduleAccesser->getGroundModule().isTouch(4, 0)) {
            wallDir = -1.0f;
        } else if (moduleAccesser->getGroundModule().isTouch(2, 0)) {
            wallDir = 1.0f;
        }
        if (moduleAccesser->getMotionModule().getKind() == 0xc1) {
            motionFrame = moduleAccesser->getMotionModule().getFrame();
        } else {
            Vec2f touchNormal(0.0f, 1.0f);
            Vec2f touchPos;
            if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
                float angle = (float)atan2(-touchNormal.m_x, touchNormal.m_y);
            Vec3f* rot = &Vec3f(0.0f, 0.0f, angle);
                moduleAccesser->getEffectModule().req(EfID(0x2c), &Vec3f(touchPos.m_x, touchPos.m_y, 0.0f), rot, 1.0f, 0, -1);
            }
        }
        soMotionChangeParam param;
        param.m_kind = 0xc2;
        param.m_frame = motionFrame;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = (0.0f != motionFrame);
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        if (0.0f != wallDir) {
            ftUtil::adjustWall(moduleAccesser, wallDir, wallDir, 1.0f, 1);
        }
        int xluFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a10, 0);
        moduleAccesser->getCollisionHitModule().setXluFrameGlobal(xluFrames, 0);
        break;
    }
    case 0x67: {
        soMotionChangeParam param;
        param.m_kind = 0xc2;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        float wallDir = 0.0f;
        if (moduleAccesser->getGroundModule().isTouch(4, 0)) {
            wallDir = -1.0f;
        } else if (moduleAccesser->getGroundModule().isTouch(2, 0)) {
            wallDir = 1.0f;
        }
        if (0.0f != wallDir) {
            ftUtil::adjustWall(moduleAccesser, wallDir, wallDir, 1.0f, 1);
            Vec2f touchNormal(0.0f, 1.0f);
            Vec2f touchPos;
            if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
                float angle = (float)atan2(-touchNormal.m_x, touchNormal.m_y);
            Vec3f* rot = &Vec3f(0.0f, 0.0f, angle);
                moduleAccesser->getEffectModule().req(EfID(0x87), &Vec3f(touchPos.m_x, touchPos.m_y, 0.0f), rot, 1.0f, 0, -1);
            }
        }
        moduleAccesser->getWorkManageModule().setFloat(wallDir, 0x11000002);
        int wallJumpFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a1d, 0);
        moduleAccesser->getWorkManageModule().setInt(wallJumpFrames, 0x10000004);
        break;
    }
    case 0x68: {
        soMotionChangeParam param;
        param.m_kind = 0xc4;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        float wallDir = 0.0f;
        if (moduleAccesser->getGroundModule().isTouch(4, 0)) {
            wallDir = -1.0f;
        } else if (moduleAccesser->getGroundModule().isTouch(2, 0)) {
            wallDir = 1.0f;
        }
        if (-1.0f == wallDir) {
            moduleAccesser->getGroundModule().attach(4, 0);
        } else {
            moduleAccesser->getGroundModule().attach(2, 0);
        }
        ftUtil::adjustWall(moduleAccesser, wallDir, wallDir, 1.0f, 1);
        Vec2f touchNormal(0.0f, 1.0f);
        Vec2f touchPos;
        if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
            float angle = (float)atan2(-touchNormal.m_x, touchNormal.m_y);
            Vec3f* rot = &Vec3f(0.0f, 0.0f, angle);
            moduleAccesser->getEffectModule().req(EfID(0x87), &Vec3f(touchPos.m_x, touchPos.m_y, 0.0f), rot, 1.0f, 0, -1);
        }
        break;
    }
    case 0x69: {
        int clingCount = moduleAccesser->getWorkManageModule().getInt(0x1000000c);
        moduleAccesser->getWorkManageModule().incInt(0x1000000c);
        float stay = soValueAccesser::getConstantInt(moduleAccesser, 0x5a22, 0);
        if (clingCount > 0) {
            stay *= (float)pow(soValueAccesser::getConstantFloat(moduleAccesser, 0xc9e, 0), (float)(clingCount - 1));
        }
        moduleAccesser->getWorkManageModule().setInt(stay, 0x20000002);
        if (-1.0f == moduleAccesser->getPostureModule().getLr()) {
            moduleAccesser->getGroundModule().attach(4, 0);
        } else {
            moduleAccesser->getGroundModule().attach(2, 0);
        }
        break;
    }
    case 0x64: {
        soMotionChangeParam param;
        param.m_kind = 0xc3;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        ftUtil::adjustCeil(moduleAccesser, true, moduleAccesser->getPostureModule().getLr());
        moduleAccesser->getControllerModule().setRumble(0xd, 0, false, -1);
        Vec2f touchNormal(0.0f, 1.0f);
        Vec2f touchPos;
        if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
            float angle = (float)atan2(-touchNormal.m_x, touchNormal.m_y);
            Vec3f* rot = &Vec3f(0.0f, 0.0f, angle);
            moduleAccesser->getEffectModule().req(EfID(0x2c), &Vec3f(touchPos.m_x, touchPos.m_y, 0.0f), rot, 1.0f, 0, -1);
        }
        break;
    }
    case 0x65: {
        soMotionChangeParam param;
        param.m_kind = 0xd2;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        float lr = moduleAccesser->getPostureModule().getLr();
        ftUtil::adjustWall(moduleAccesser, lr, -lr, -1.0f, 1);
        moduleAccesser->getControllerModule().setRumble(0xd, 0, false, -1);
        break;
    }
    case 0x66: {
        soMotionChangeParam param;
        param.m_kind = 0xd3;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        ftUtil::adjustCeil(moduleAccesser, true, moduleAccesser->getPostureModule().getLr());
        break;
    }
    case 0x6a:
    case 0x6b:
    case 0x6c:
        break;
    }
}

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
