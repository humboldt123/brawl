#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <math.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>


class ftZeldaStatusUniqProcessSpecialHi2 : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialHi2() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// Farore's Wind uses Zelda's third extend-parameter record (ftData + 0x84).
// Farore's Wind parameters (ftData::extendParam[2]); initStatus reads the leading floats by index.
struct ftZeldaSpecialHiParam {
    float m_floats[10];
    s32 m_extraLandAngle; // 0x28: degrees added to 90 for the landing-angle check in execFixPosCounter
};

void ftZeldaStatusUniqProcessSpecialHi2::initStatus(soModuleAccesser* moduleAccesser) {
    bool useInputDirection = false;
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soCollisionHitModule& collisionHit = moduleAccesser->getCollisionHitModule();
    const ftData* data = g_ftCommonDataAccesser.getData(Fighter_Zelda);
    const float* params = static_cast<const float*>(data->extendParam[2]);

    Vec2f stick(controller.getStickX(), controller.getStickY());
    Vec2f launchSpeed;
    float magnitude = stick.length();
    if (magnitude > 1.0f) {
        magnitude = 1.0f;
    }

    if (magnitude < params[6]) {
        useInputDirection = true;
    } else if (ground.isTouch(static_cast<grCollStatus::TouchMask>(0xff), 0)) {
        Vec2f normal = ground.getTouchNormal(0xff, 0);
        if (normal.angle(&stick) < 1.57079637f) {
            useInputDirection = true;
        }
        if (ground.isPassableGround(0)) {
            useInputDirection = true;
        }
    } else {
        useInputDirection = true;
    }

    if (stick.m_x > 1.1920929e-7f) {
        posture.setLr(1.0f);
    } else if (stick.m_x < -1.1920929e-7f) {
        posture.setLr(-1.0f);
    }
    posture.updateRotYLr();

    // HYPOTHESIS: a grounded stick into a solid floor slides along it (no vertical speed); otherwise it launches toward
    // the stick (straight up when the stick is near neutral).
    float angle;
    if (useInputDirection) {
        if (magnitude > params[6]) {
            angle = atan2f(stick.m_y, stick.m_x * posture.getLr());
        } else {
            angle = 1.57079637f;
            magnitude = 1.0f;
        }
        launchSpeed.m_x = (params[8] + params[7] * magnitude) * cosf(angle) * posture.getLr();
        launchSpeed.m_y = (params[8] + params[7] * magnitude) * (float)sin(angle);
        situation.setKind(static_cast<SituationKind>(2), false);
        ground.setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
    } else {
        angle = atan2f(stick.m_y, stick.m_x * posture.getLr());
        launchSpeed.m_x = (params[8] + params[7] * magnitude) * cosf(angle) * posture.getLr();
        launchSpeed.m_y = 0.0f;
    }

    kinetic.clearSpeedAll();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    stop.m_speed = launchSpeed;
    stop.enable();
    collisionHit.setWhole(2, 0);
}

void ftZeldaStatusUniqProcessSpecialHi2::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    const ftData* data = g_ftCommonDataAccesser.getData(Fighter_Zelda);
    const ftZeldaSpecialHiParam* params = static_cast<const ftZeldaSpecialHiParam*>(data->extendParam[2]);
    if (situation.getKind() == 0) {
        return;
    }

    int touchMask = 0;
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(1), 0)) {
        touchMask = 1;
    }
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
        touchMask = 4;
    }
    if (ground.isTouch(static_cast<grCollStatus::TouchMask>(2), 0)) {
        touchMask = 2;
    }
    if (touchMask == 0) {
        return;
    }

    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    Vec2f normal = ground.getTouchNormal(touchMask, 0);
    float angle = speed.angle(&normal);
    s32 extraAngle = params->m_extraLandAngle;
    float maxAngle = ((float)extraAngle + 90.0f) * 0.0174532924f;
    if (angle > maxAngle) {
        work.onFlag(0x22000012);
    }
}

void ftZeldaStatusUniqProcessSpecialHi2::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    if (workManage.isFlag(0x22000012)) {
        workManage.offFlag(0x22000012);
        status.changeStatusRequest(0x11a, moduleAccesser);
    }
}

void ftZeldaStatusUniqProcessSpecialHi2::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11a) {
        moduleAccesser->getVisibilityModule().setWhole(1);
    }
}

ftZeldaStatusUniqProcessSpecialHi2 g_ftZeldaStatusUniqProcessSpecialHi2;
