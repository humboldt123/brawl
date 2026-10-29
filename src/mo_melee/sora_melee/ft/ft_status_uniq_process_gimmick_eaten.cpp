#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <types.h>

void ftStatusUniqProcessGimmickEaten::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessGimmick::initStatus(moduleAccesser);
    soGimmickChappyEventArgs_In args;
    presentEvent(&args, moduleAccesser);
}

void ftStatusUniqProcessGimmickEaten::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessGimmickEaten::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
}

void ftStatusUniqProcessGimmickEaten::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: each gimmick owns a range of event kinds between two unnamed bounds.
    if (eventInfo->m_kind <= 0x26 || eventInfo->m_kind >= 0x29) {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    } else {
        switch (eventInfo->m_kind) {
        case Gimmick::Chappy_Event_Pos: {
            soGimmickChappyEventArgs* args = static_cast<soGimmickChappyEventArgs*>(eventInfo);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_x = args->m_pos.m_x;
            pos.m_y = args->m_pos.m_y;
            moduleAccesser->getPostureModule().setPos(&pos);
            break;
        }
        }
    }
}
