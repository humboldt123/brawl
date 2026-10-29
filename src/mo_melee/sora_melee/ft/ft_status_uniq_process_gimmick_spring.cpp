#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftStatusUniqProcessGimmickSpring::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessGimmickSpring::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
    int value = soValueAccesser::getConstantInt(moduleAccesser, 0x5a0c, 0);
    moduleAccesser->getWorkManageModule().setInt(value, 0x10000012);
}

void ftStatusUniqProcessGimmickSpring::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: each gimmick owns a range of event kinds between two unnamed bounds.
    if (eventInfo->m_kind <= 0x16 || eventInfo->m_kind >= 0x1A) {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    } else {
        switch (eventInfo->m_kind) {
        case Gimmick::Spring_Event_Shoot: {
            soGimmickSpringEventArgs* args = static_cast<soGimmickSpringEventArgs*>(eventInfo);
            moduleAccesser->getWorkManageModule().setFloat(args->m_bounce, 0x21000004);
            moduleAccesser->getWorkManageModule().setFloat(args->m_rot, 0x21000005);
            moduleAccesser->getWorkManageModule().onFlag(0x2200000C);
            moduleAccesser->getStatusModule().changeStatusRequest(0xC4, moduleAccesser);
            break;
        }
        case Gimmick::Spring_Event_Pos: {
            soGimmickSpringEventArgs* args = static_cast<soGimmickSpringEventArgs*>(eventInfo);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_x = args->m_topPos.m_x;
            pos.m_y = args->m_topPos.m_y;
            moduleAccesser->getPostureModule().setPos(&pos);
            break;
        }
        }
    }
}
