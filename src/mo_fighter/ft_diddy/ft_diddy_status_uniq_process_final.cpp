// MATCH-ONLY: this unit calls the out-of-line Vec3f constructor and Vec2f assignment, as ft_diddy.cpp does.
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <cm/cm_camera_controller.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <so/area/so_area_enviroment_element_checker_impl.h>
#include <so/area/so_area_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int, soModuleAccesser*);
// HYPOTHESIS: the weak ABS<f> copy that the module map places in the pop-gun unit (still unnamed there).
extern "C" float fn_115_E59C(float);

// HYPOTHESIS: roles of the work and parameter IDs are inferred from their use here.
class ftDiddyStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);

    void updateControl(soModuleAccesser* moduleAccesser);
    float updateSpeed(float speed, float accel, float brake, float limit);
    void updateControlJetEffect(soModuleAccesser* moduleAccesser);
    void removeJetEffect(soModuleAccesser* moduleAccesser);
    void setControlRotate(soModuleAccesser* moduleAccesser);
    void stopSe(soModuleAccesser* moduleAccesser);
};

void ftDiddyStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getDamageModule().setDamageMul(soValueAccesser::getConstantFloat(moduleAccesser, 0xfcc, 0));
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116: {
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(4, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        int finalFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc3, 0) * 60;
        moduleAccesser->getWorkManageModule().setInt(finalFrames, 0x20000000);
        if (moduleAccesser->getSituationModule().getKind() == 3) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000017);
        }
        moduleAccesser->getWorkManageModule().onFlag(0x22000018);
        moduleAccesser->getWorkManageModule().onFlag(0x22000019);
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000004);
        break;
    }
    case 0x12c:
        if (moduleAccesser->getWorkManageModule().getInt(0x20000004) < 0) {
            int handle = moduleAccesser->getSoundModule().playSE((SndID)0x1491, false, false, 0);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000004);
        }
        setControlRotate(moduleAccesser);
        break;
    case 0x12e:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            moduleAccesser->getPostureModule().setLr(-1.0f);
        } else {
            moduleAccesser->getPostureModule().setLr(1.0f);
        }
        moduleAccesser->getPostureModule().updateRotYLr();
        moduleAccesser->getStageObject().updateNodeSRT();
        moduleAccesser->getWorkManageModule().onFlag(0x22000014);
        break;
    }
    moduleAccesser->getAreaModule().setWhole(0);
}

void ftDiddyStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.resetEnergy(0x16, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.m_accel = Vec2f(0.0f, 0.0f);
            stop.m_speedTarget = Vec2f(0.0f, 0.0f);
            stop.m_brake = Vec2f(0.0f, 0.0f);
            stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
            stop.enable();
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.resetEnergy(0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
            gravity.unk1C = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
            gravity.enable();
            moduleAccesser->getWorkManageModule().offFlag(0x22000012);
            moduleAccesser->getWorkManageModule().onFlag(0x22000014);
            int handle = moduleAccesser->getEffectModule().reqFollow((EfID)0xf90001, 0x12, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 6, 0, -1);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000001);
            moduleAccesser->getWorkManageModule().onFlag(0x2200001a);
        }
        break;
    case 0x12c:
        // HYPOTHESIS: the original compares the elapsed time with this limit without acting on the result.
        if (moduleAccesser->getWorkManageModule().getInt(0x20000000) <= soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0)) {
        }
    case 0x12d: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        if (gravity.getSpeed().m_y <= 0.0f) {
            gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
            gravity.unk1C = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
        } else {
            gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbe, 0);
            gravity.unk1C = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbe, 0);
        }
        break;
    }
    case 0x12e:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            float speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xfce, 0);
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            moduleAccesser->getWorkManageModule().offFlag(0x22000012);
        }
        break;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
        updateControl(moduleAccesser);
        setControlRotate(moduleAccesser);
        updateControlJetEffect(moduleAccesser);
    }
    moduleAccesser->getAreaModule().setWhole(0);
}

void ftDiddyStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getAreaModule().setWhole(0);
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
        setControlRotate(moduleAccesser);
    }
}

void ftDiddyStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(6, false);
    float waterY;
    // HYPOTHESIS: testWater takes the node position as a Vec2f and writes the water surface height.
    if (moduleAccesser->getAreaModule().getEnviromentElementChecker()->testWater((Vec2f*)&nodePos, (int*)&waterY) == 1) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000017) && moduleAccesser->getWorkManageModule().getInt(0x10000027) == 0) {
            ftUtil::setWaterDiveEffect(moduleAccesser, &Vec3f(nodePos.m_x, waterY, nodePos.m_y));
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_speed = Vec2f(0.0f, 0.0f);
            moduleAccesser->getWorkManageModule().setInt(0x1e, 0x10000027);
        }
        moduleAccesser->getWorkManageModule().onFlag(0x22000017);
    } else {
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
    }
    if (moduleAccesser->getGroundModule().isTouch(8, 0)) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = 0.0f;
    }
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    CameraController* camera = CameraController::getInstance();
    if (pos.m_x < camera->unk158) {
        pos.m_x = camera->unk158;
        moduleAccesser->getWorkManageModule().offFlag(0x22000018);
    } else if (pos.m_x > camera->unk15C) {
        pos.m_x = camera->unk15C;
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
    }
    if (pos.m_y > camera->unk160) {
        pos.m_y = camera->unk160;
    }
    moduleAccesser->getPostureModule().setPos(&pos);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            if ((u8)moduleAccesser->getGroundModule().getCorrect(0) == soGroundShapeImpl::Correct_Ground) {
                moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
            }
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                moduleAccesser->getSituationModule().setKind((SituationKind)2, true);
            }
        }
        break;
    }
}

void ftDiddyStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    switch (status) {
    case 0x116:
    case 0x12c:
    case 0x12d:
        break;
    case 0x12e:
        removeJetEffect(moduleAccesser);
        stopSe(moduleAccesser);
        break;
    default: {
        removeJetEffect(moduleAccesser);
        stopSe(moduleAccesser);
        moduleAccesser->getEffectModule().removeCommon(0x26);
        moduleAccesser->getEffectModule().removeCommon(0x25);
        // HYPOTHESIS: the word at ftData+0x90 is the article id of the Final Smash barrels.
        ftData* data = g_ftCommonDataAccesser.getData(Fighter_Diddy);
        moduleAccesser->getGenerateArticleManageModule().removeExist(*reinterpret_cast<int*>(reinterpret_cast<u8*>(data) + 0x90), 0);
        for (int i = 0; i < 9; i++) {
            moduleAccesser->getAreaModule().enableArea(i, true, -1);
        }
        break;
    }
    }
}

void ftDiddyStatusUniqProcessFinal::updateControl(soModuleAccesser* moduleAccesser) {
    float lr = moduleAccesser->getPostureModule().getLr();
    float stickX = moduleAccesser->getControllerModule().getStickX();
    float stickY = moduleAccesser->getControllerModule().getStickY();
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float angleMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc5, 0);
    float sign;
    float angleSpeed;
    if ((stickX < 0.0f && moduleAccesser->getWorkManageModule().isFlag(0x22000018)) ||
        (stickX > 0.0f && moduleAccesser->getWorkManageModule().isFlag(0x22000019))) {
        angleSpeed = lr * (stickX * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc8, 0));
        moduleAccesser->getWorkManageModule().onFlag(0x22000018);
        moduleAccesser->getWorkManageModule().onFlag(0x22000019);
    } else {
        angleSpeed = angle;
        float returnSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc9, 0);
        if (fn_115_E59C(angle) > returnSpeed) {
            sign = (angle < 0.0f ? -1 : 1);
            angleSpeed = sign * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc9, 0);
        }
        angleSpeed = -angleSpeed;
    }
    angle += angleSpeed;
    if (angle < -angleMax) {
        angle = -angleMax;
    } else if (angle > angleMax) {
        angle = angleMax;
    }
    moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
    float rotX = angle * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc6, 0);
    if (rotX < 0.0f) {
        rotX = 360.0f + rotX;
    }
    moduleAccesser->getWorkManageModule().setFloat(rotX, 0x21000005);
    float rotY = angle / angleMax * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc7, 0);
    rotY *= lr;
    if (rotY < 0.0f) {
        rotY = 360.0f + rotY;
    }
    moduleAccesser->getWorkManageModule().setFloat(rotY, 0x21000006);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    float lift = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbf, 0);
    if (stickY > 0.0f) {
        lift += stickY * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
    } else if (stickY < 0.0f) {
        lift += stickY * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0);
    } else {
        moduleAccesser->getMotionModule().setRate(1.0f);
    }
    lift += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc0, 0) * fn_115_E59C(angle / angleMax);
    Vec2f thrust(0.0f, lift);
    if (fn_115_E59C(angle) > 0.0f) {
        angle *= lr;
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        thrust.rot(0.017453292f * (360.0f - angle), &thrust);
    }
    float bodyAngle = 180.0f + rotX * lr;
    if (bodyAngle > 360.0f) {
        bodyAngle -= 360.0f;
    } else if (bodyAngle < 0.0f) {
        bodyAngle += 360.0f;
    }
    moduleAccesser->getWorkManageModule().setFloat(bodyAngle, 0x21000007);
    gravity.m_gravity = thrust.m_y - soValueAccesser::getConstantFloat(moduleAccesser, 0xfbc, 0);
    float speedX = updateSpeed(stop.getSpeed().m_x, thrust.m_x, soValueAccesser::getConstantFloat(moduleAccesser, 0xfca, 0), soValueAccesser::getConstantFloat(moduleAccesser, 0xfcb, 0));
    stop.m_speed = Vec2f(speedX, 0.0f);
}

float ftDiddyStatusUniqProcessFinal::updateSpeed(float speed, float accel, float brake, float limit) {
    float result;
    float friction = 0.0f;
    if (speed < 0.0f) {
        friction = brake;
    } else if (speed > 0.0f) {
        friction = -brake;
    }
    if (accel * friction <= 0.0f) {
        result = speed + (accel + friction);
        if (fn_115_E59C(accel) < fn_115_E59C(friction)) {
            result = result + (accel + friction);
            if (friction < 0.0f) {
                if (result < 0.0f) {
                    result = 0.0f;
                }
            } else if (result > 0.0f) {
                result = 0.0f;
            }
        }
    } else {
        result = speed + (accel + friction);
    }
    if (fn_115_E59C(result) > limit) {
        result = limit * (result < 0.0f ? -1 : 1);
    }
    return result;
}

void ftDiddyStatusUniqProcessFinal::updateControlJetEffect(soModuleAccesser* moduleAccesser) {
    float stickX = moduleAccesser->getControllerModule().getStickX();
    if (moduleAccesser->getControllerModule().getStickY() > 0.0f || fn_115_E59C(stickX) > 0.1f) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
            int handle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
            if (moduleAccesser->getEffectModule().isExistEffect(handle)) {
                moduleAccesser->getEffectModule().remove(handle);
            }
            moduleAccesser->getWorkManageModule().offFlag(0x2200001a);
        }
        if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001b)) {
            int handle = moduleAccesser->getEffectModule().reqFollow((EfID)0xf90002, 0x12, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 6, 0, -1);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000002);
            moduleAccesser->getWorkManageModule().onFlag(0x2200001b);
        }
    } else {
        if (moduleAccesser->getWorkManageModule().isFlag(0x2200001b)) {
            int handle = moduleAccesser->getWorkManageModule().getInt(0x20000002);
            if (moduleAccesser->getEffectModule().isExistEffect(handle)) {
                moduleAccesser->getEffectModule().remove(handle);
            }
            moduleAccesser->getWorkManageModule().offFlag(0x2200001b);
        }
        if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
            int handle = moduleAccesser->getEffectModule().reqFollow((EfID)0xf90001, 0x12, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 6, 0, -1);
            moduleAccesser->getWorkManageModule().setInt(handle, 0x20000001);
            moduleAccesser->getWorkManageModule().onFlag(0x2200001a);
        }
    }
}

void ftDiddyStatusUniqProcessFinal::removeJetEffect(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001a)) {
        int handle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        if (moduleAccesser->getEffectModule().isExistEffect(handle)) {
            moduleAccesser->getEffectModule().remove(handle);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x2200001a);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001b)) {
        int handle = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        if (moduleAccesser->getEffectModule().isExistEffect(handle)) {
            moduleAccesser->getEffectModule().remove(handle);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x2200001b);
    }
}

void ftDiddyStatusUniqProcessFinal::setControlRotate(soModuleAccesser* moduleAccesser) {
    Vec3f rotX = moduleAccesser->getModelModule().getNodeRotate(4);
    rotX.m_x = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    moduleAccesser->getModelModule().setNodeRotate(4, &rotX);
    Vec3f rotY = moduleAccesser->getModelModule().getNodeRotate(5);
    rotY.m_y = moduleAccesser->getWorkManageModule().getFloat(0x21000006);
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x12c || moduleAccesser->getStatusModule().getStatusKind() == 0x12d) {
        if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
            rotY.m_y += 180.0f;
        }
    }
    moduleAccesser->getModelModule().setNodeRotate(5, &rotY);
}

void ftDiddyStatusUniqProcessFinal::stopSe(soModuleAccesser* moduleAccesser) {
    int handle = moduleAccesser->getWorkManageModule().getInt(0x20000004);
    if (handle >= 0) {
        moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000004);
    }
}

ftDiddyStatusUniqProcessFinal g_ftDiddyStatusUniqProcessFinal;
