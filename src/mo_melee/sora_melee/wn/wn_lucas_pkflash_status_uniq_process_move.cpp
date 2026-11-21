#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/lucas/wn_lucas_pkflash.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class wnLucasPKFlashStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFlashStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

wnLucasPKFlashStatusUniqProcessMove g_wnLucasPKFlashStatusUniqProcessMove;

void wnLucasPKFlashStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) {
}

// Flying PK Flash: the charge (work float 0x21000001) grows by one per frame up to param unk0, the stick accelerates it
// sideways (dead zone 0.2), and the flash grows from scale unk4 to unk8 with the charge.
void wnLucasPKFlashStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    wnLucasPKFlash& flash = dynamic_cast<wnLucasPKFlash&>(moduleAccesser->getStageObject());
    const wnLucasPKFlashParam* param = flash.getParam();
    float charge = work.getFloat(0x21000001);
    charge += 1.0f;
    if (charge > param->unk0) {
        charge = param->unk0;
    }
    work.setFloat(charge, 0x21000001);
    float stickX = controller.getStickX();
    float speedX = 0.0f;
    if (__fabsf(stickX) > 0.2f) {
        speedX = stickX * param->unk14;
    }
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    energy.m_accel = Vec2f(speedX, 0.0f);
    posture.setScale(param->unk4 + charge * ((param->unk8 - param->unk4) / param->unk0));
}

void wnLucasPKFlashStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, moduleAccesser);
    soKineticEnergy* energy = kinetic.getEnergy(2);
    energy->clearSpeed();
    energy->disable();
    if (nextStatus == 1) {
        int handle = work.getInt(0x20000000);
        moduleAccesser->getEffectModule().kill(handle, true, true);
        soEffectModule& effect = moduleAccesser->getEffectModule();
        effect.reqFollow(static_cast<EfID>(0x1B000D), 0,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
    }
}
