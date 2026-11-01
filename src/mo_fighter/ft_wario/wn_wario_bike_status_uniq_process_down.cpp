#pragma scheduling off
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/wario/wn_wario_bike_status_uniq_process.h>
#include <wn/wario/wn_wario_bike_link_event.h>
#include <so/so_module_accesser.h>
#include <math.h>

void wnWarioBikeStatusUniqProcessDown::initStatus(soModuleAccesser* a) {
    soLinkModule& link = a->getLinkModule();
    ftWarioBikeLinkEvent event(0x83f);
    link.sendEventParents(3, event);
    if (link.isModelConstraint()) link.removeModelConstraint(true);
}

void wnWarioBikeStatusUniqProcessDown::execStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soPostureModule& posture = a->getPostureModule();
    float angle = work.getFloat(0x21000000);
    Vec3f rotation(-angle, 0.0f, 0.0f);
    posture.setRot(&rotation, 0);
}

void wnWarioBikeStatusUniqProcessDown::execFixPos(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soGroundModule& ground = a->getGroundModule();
    if (ground.isTouch((grCollStatus::TouchMask)8, 0)) {
        float lr = a->getPostureModule().getLr();
        Vec2f normal = ground.getTouchNormal((grCollStatus::TouchMask)8, 0);
        // MATCH-ONLY: preserve the native Y-then-X load order before atan2.
        float normalY = normal.m_y;
        float normalX = normal.m_x;
        float angle = 57.29578f * (float)atan2(-(normalX * lr), normalY);
        work.setFloat(angle, 0x21000000);
    }
}

wnWarioBikeStatusUniqProcessDown g_wnWarioBikeStatusUniqProcessDown;
