#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class wnLucasPKFireStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFireStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnLucasPKFireStatusUniqProcessShoot g_wnLucasPKFireStatusUniqProcessShoot;

void wnLucasPKFireStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getCollisionHitModule().setStatus(0, 0, 0);
    moduleAccesser->getCollisionHitModule().setStatus(1, 3, 0);
}

void wnLucasPKFireStatusUniqProcessShoot::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soEffectModule& effect = moduleAccesser->getEffectModule();
    posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    if (next != 1) {
        effect.req(static_cast<EfID>(0x22), &posture.getPos(), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
    }
}
