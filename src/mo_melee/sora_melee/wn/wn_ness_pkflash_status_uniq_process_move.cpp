#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <cmath>
#include <so/stageobject.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <wn/ness/wn_ness_pkflash.h>

class wnNessPKFlashStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFlashStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnNessPKFlashStatusUniqProcessMove g_wnNessPKFlashStatusUniqProcessMove;

void wnNessPKFlashStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) { }
// Flying PK Flash: the charge (work float 0x21000001) grows by one per frame up to param unk0, the stick accelerates it
// sideways (dead zone 0.2), and the flash grows from scale unk4 to unk8 with the charge.
void wnNessPKFlashStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    wnNessPKFlash& flash = dynamic_cast<wnNessPKFlash&>(moduleAccesser->getStageObject());
    const wnNessPKFlashParam* param = flash.getParam();
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

void wnNessPKFlashStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, moduleAccesser);
    soKineticEnergy* gravity = kinetic.getEnergy(2);
    gravity->clearSpeed();
    gravity->disable();
    if (nextStatus == 1) {
        int effectHandle = work.getInt(0x20000000);
        moduleAccesser->getEffectModule().kill(effectHandle, true, true);
        soEffectModule& effect = moduleAccesser->getEffectModule();
        effect.reqFollow(static_cast<EfID>(0xB000A), 0,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
    }
}
