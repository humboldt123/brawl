#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/wario/wn_wario_bike_status_uniq_process.h>
#include <so/so_module_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <math.h>

void wnWarioBikeStatusUniqProcessEscape::execFixPos(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soKineticModule& kinetic = a->getKineticModule();
    soPostureModule& posture = a->getPostureModule();
    soStatusModule& status = a->getStatusModule();
    float speedX = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_x;
    bool speedInRange = false;
    bool front = work.isFlag(0x22000004);
    bool rear = work.isFlag(0x22000005);
    bool groundTouch = work.isFlag(0x22000006);
    float angle = work.getFloat(0x21000000);
    float groundAngle = work.getFloat(0x21000002);
    wnWarioBikeParam* param = dynamic_cast<wnWarioBike&>(a->getStageObject()).m_param;
    float lr = posture.getLr();
    if (lr == 1.0f) {
        if (speedX <= param->unk8C) speedInRange = true;
    } else {
        if (speedX >= -param->unk8C) speedInRange = true;
    }
    if (speedInRange && (groundTouch || front || rear)) {
        status.changeStatusRequest(11, a);
    }

    if (groundTouch) {
        if (angle > groundAngle) {
            angle -= param->unk64;
            if (angle < groundAngle) angle = groundAngle;
        } else if (angle < groundAngle) {
            angle += param->unk64;
            if (angle > groundAngle) angle = groundAngle;
        }
    } else {
        if (!front && rear) {
            angle -= param->unk64;
            // MATCH-ONLY: the native airborne branches retain these ground clamps.
            if (groundTouch && angle < groundAngle) angle = groundAngle;
        } else if (!rear && front) {
            angle += param->unk64;
            if (groundTouch && angle > groundAngle) angle = groundAngle;
        } else {
            status.changeStatusRequest(13, a);
        }
    }
    work.setFloat(angle, 0x21000000);
}

wnWarioBikeStatusUniqProcessEscape g_wnWarioBikeStatusUniqProcessEscape;
