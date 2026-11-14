#include <ft/metaknight/ft_metaknight_status_views.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialSRush : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialSRush() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftMetaknightStatusUniqProcessSpecialSRush::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getControllerModule().getStickY() != 0.0f) {
        float stick = moduleAccesser->getControllerModule().getStickY();
        float clamped = stick;
        if (stick < -1.0f) {
            clamped = -1.0f;
        } else if (stick > 1.0f) {
            clamped = 1.0f;
        }
        Vec3f rotation = moduleAccesser->getPostureModule().getRot(0);
        rotation.m_x += -clamped * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
        moduleAccesser->getPostureModule().setRot(&rotation, 0);
    }
}

void ftMetaknightStatusUniqProcessSpecialSRush::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11a) {
        Vec3f zero(0.0f, 0.0f, 0.0f);
        moduleAccesser->getPostureModule().setRot(&zero, 0);
        moduleAccesser->getPostureModule().updateRotYLr();
    } else if (moduleAccesser->getGroundModule().attachGround(0) == true) {
        moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
    }
}

ftMetaknightStatusUniqProcessSpecialSRush g_ftMetaknightStatusUniqProcessSpecialSRush;
