#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftStatusUniqProcessGimmickCatapult::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionChangeParam param;
    param.m_kind = 0x17E;
    param.m_frame = 1.0f;
    param.m_rate = 0.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);
    ftStatusUniqProcessGimmick::initStatus(moduleAccesser);
    soGimmickCatapultEventArgs_On args;
    presentEvent(&args, moduleAccesser);
}

void ftStatusUniqProcessGimmickCatapult::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessGimmickCatapult::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
    moduleAccesser->getWorkManageModule().setInt(8, 0x1000000F);
}
