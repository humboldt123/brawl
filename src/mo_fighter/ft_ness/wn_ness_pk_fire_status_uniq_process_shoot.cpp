#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnNessPKFireStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFireStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnNessPKFireStatusUniqProcessShoot g_wnNessPKFireStatusUniqProcessShoot;

void wnNessPKFireStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getCollisionHitModule().setStatus(0, 0, 0);
    moduleAccesser->getCollisionHitModule().setStatus(1, 3, 0);
}

void wnNessPKFireStatusUniqProcessShoot::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soEffectModule& effect = moduleAccesser->getEffectModule();
    posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    if (next != 1) {
        effect.req(static_cast<EfID>(0x22), &posture.getPos(), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
    }
}
