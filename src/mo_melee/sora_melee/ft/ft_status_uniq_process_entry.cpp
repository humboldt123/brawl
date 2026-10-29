#include <ft/fighter.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessEntry g_ftStatusUniqProcessEntry;

void ftStatusUniqProcessEntry::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessEntry::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getMotionModule().isEnd() == true) {
        moduleAccesser->getGroundModule().setShapeKind(soGroundShapeImpl::Shape_Rhombus_Modify, 0);
        if (moduleAccesser->getGroundModule().attachGround(0) == true) {
            moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
            moduleAccesser->getStatusModule().changeStatusRequest(Fighter::Status::Wait, moduleAccesser);
        } else {
            moduleAccesser->getSituationModule().setKind(Situation_Air, false);
            moduleAccesser->getStatusModule().changeStatusRequest(Fighter::Status::Fall, moduleAccesser);
        }
    }
}

void ftStatusUniqProcessEntry::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
