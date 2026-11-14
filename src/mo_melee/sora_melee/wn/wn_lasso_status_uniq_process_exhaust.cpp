#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <mt/mt_vector.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>
#include <types.h>

class wnLassoStatusUniqProcessExhaust : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessExhaust() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessExhaust g_wnLassoStatusUniqProcessExhaust;

static inline void wnLassoChangeMotionExhaust(soModuleAccesser* moduleAccesser) {
    soMotionChangeParam motionParam;
    motionParam.m_kind = -1;
    motionParam.m_frame = 0.0f;
    motionParam.m_rate = 1.0f;
    motionParam._12 = 0;
    motionParam._13 = 0;
    motionParam._14 = 0;
    motionParam._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
}

void wnLassoStatusUniqProcessExhaust::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
        moduleAccesser->getWorkManageModule().offFlag(0x12000003);
        // HYPOTHESIS: motion kind -1 with the frame and rate constants from lbl_27_rodata_2B50 / 2B54 (values unverified).
        wnLassoChangeMotionExhaust(moduleAccesser);
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        int count = moduleAccesser->getWorkManageModule().getInt(0x10000005);
        Vec3f linkPos = wnLassoGetLink(moduleAccesser)->unk174VecOut();
        Vec3f modelPos = wnLassoGetAltModel(moduleAccesser)->unk98VecIntBool(count, false);
        wnLassoGetAltStatus(moduleAccesser)->unkA4Int(7);
        wnLassoGetAltStatus(moduleAccesser)->unk68VecPair(&linkPos, &modelPos);
    } else {
        wnLassoGetAltStatus(moduleAccesser)->unkA4Int(1);
    }
}

void wnLassoStatusUniqProcessExhaust::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000000);
        moduleAccesser->getWorkManageModule().onFlag(0x22000001);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000001)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000001);
        wnLassoGetAltStatus(moduleAccesser)->unkA4Int(1);
    }
}
