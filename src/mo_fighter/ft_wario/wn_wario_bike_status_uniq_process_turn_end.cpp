#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/wario/wn_wario_bike_status_uniq_process.h>
#include <wn/wario/wn_wario_bike_link_event.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <mt/mt_prng.h>
#include <math.h>

void wnWarioBikeStatusUniqProcessTurnEnd::initStatus(soModuleAccesser* a) {
    soLinkModule& link = a->getLinkModule();
    ftWarioBikeLinkEvent event(0x83c);
    link.sendEventParents(3, event);
}

void wnWarioBikeStatusUniqProcessTurnEnd::execStatus(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soPostureModule& posture = a->getPostureModule();
    float angle = work.getFloat(0x21000000);
    Vec3f rotation(-angle, 0.0f, 0.0f);
    posture.setRot(&rotation, 0);
}

void wnWarioBikeStatusUniqProcessTurnEnd::execFixPos(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soGroundModule& ground = a->getGroundModule();
    if (ground.isTouch((grCollStatus::TouchMask)8, 0)) {
        float lr = a->getPostureModule().getLr();
        Vec2f normal = ground.getTouchNormal((grCollStatus::TouchMask)8, 0);
        // MATCH-ONLY: keep the native Y-then-X loads before atan2.
        float normalY = normal.m_y;
        float normalX = normal.m_x;
        float angle = 57.29578f * (float)atan2(-(normalX * lr), normalY);
        work.setFloat(angle, 0x21000000);
    }
}

void wnWarioBikeStatusUniqProcessTurnEnd::exitStatus(soModuleAccesser* a, int nextStatus) {
    if (nextStatus == 2) {
        soWorkManageModule& work = a->getWorkManageModule();
        soPostureModule& posture = a->getPostureModule();
        float angle = work.getFloat(0x21000000);
        work.setFloat(-angle, 0x21000000);
        float unk1 = work.getFloat(0x21000001);
        work.setFloat(-unk1, 0x21000001);
        float unk2 = work.getFloat(0x21000002);
        work.setFloat(-unk2, 0x21000002);
        float unk3 = work.getFloat(0x21000003);
        work.setFloat(-unk3, 0x21000003);
        posture.reverseLr();
        // Refresh rotation from the new facing rather than negate the old rotation.
        posture.updateRotYLr();
        Vec3f rotation(angle, 0.0f, 0.0f);
        posture.setRot(&rotation, 0);
        work.offFlag(0x22000002);
        work.setFloat(posture.getLr(), 0x21000008);
    }
}

wnWarioBikeStatusUniqProcessTurnEnd g_wnWarioBikeStatusUniqProcessTurnEnd;
