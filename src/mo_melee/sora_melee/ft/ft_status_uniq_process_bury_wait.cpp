#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <math.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBuryWait : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBuryWait() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessBuryWait g_ftStatusUniqProcessBuryWait;

// While buried against a surface the dig effect follows the touched point and is rotated to the surface normal.
void ftStatusUniqProcessBuryWait::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soGroundModule* ground = &moduleAccesser->getGroundModule();
    if (ground->isTouch(8, 0)) {
        Vec2f normal = ground->getTouchNormal(8, 0);
        float normalX = normal.m_x;
        float normalY = normal.m_y;
        float angle = (float) atan2(normalX, normalY);
        Vec2f touchPos = ground->getTouchPos(8, 0);
        int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        Vec3f position;
        position.m_x = touchPos.m_x;
        position.m_y = touchPos.m_y;
        position.m_z = 0.0f;
        moduleAccesser->getEffectModule().setPos(effectHandle, &position);
        Vec3f rotation(0.0f, 0.0f, -angle);
        moduleAccesser->getEffectModule().setRot(effectHandle, &rotation);
    }
}

// Leaving the buried status removes the dig-out effect and, when the fighter touches a surface, snaps it to the touched point (at
// the ground shape's z) and re-bases the shape's safe position on its centre.
void ftStatusUniqProcessBuryWait::exitStatus(soModuleAccesser* moduleAccesser, int) {
    int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getEffectModule().kill(effectHandle, true, true);
    soGroundModule* ground = &moduleAccesser->getGroundModule();
    if (ground->isTouch(8, 0)) {
        float z = ground->getZ();
        Vec3f position(ground->getTouchPos(8, 0), z);
        moduleAccesser->getPostureModule().setPos(&position);
        ground->setShapeSafePos(&ground->getCenterPos(0), 0);
    }
    ground->setOffsetY(0.0f, 0);
}
