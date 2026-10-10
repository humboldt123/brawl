#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialSuperJumpLanding : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpLanding() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDededeStatusUniqProcessSpecialSuperJumpLanding::initStatus(soModuleAccesser* moduleAccesser) {
    execStatus(moduleAccesser);
}

void ftDededeStatusUniqProcessSpecialSuperJumpLanding::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: posture getter (slot 0x2c) assumed to be getLr.
    if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
        Vec3f rotate = moduleAccesser->getModelModule().getNodeRotate(6);
        rotate.m_y = 180.0f;
        moduleAccesser->getModelModule().setNodeRotate(6, &rotate);
    }
}

void ftDededeStatusUniqProcessSpecialSuperJumpLanding::execStop(soModuleAccesser* moduleAccesser) {
    execStatus(moduleAccesser);
}

void ftDededeStatusUniqProcessSpecialSuperJumpLanding::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getModelModule().clearNodeSRT(6);
}

ftDededeStatusUniqProcessSpecialSuperJumpLanding g_ftDededeStatusUniqProcessSpecialSuperJumpLanding;
