#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <wn/lucas/wn_lucas_pkflash.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class wnLucasPKFlashStatusUniqProcessBang : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFlashStatusUniqProcessBang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnLucasPKFlashStatusUniqProcessBang g_wnLucasPKFlashStatusUniqProcessBang;

// The flash bursts: the explosion effect at the flash, sized by how far the charge went (thresholds 0.85 / 0.65 /
// 0.45 of the full charge, not read further), and the attack power from the charge (param unk28 + unk2C * charge).
void wnLucasPKFlashStatusUniqProcessBang::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    float charge = work.getFloat(0x21000001);
    wnLucasPKFlash& flash = dynamic_cast<wnLucasPKFlash&>(moduleAccesser->getStageObject());
    const wnLucasPKFlashParam* param = flash.getParam();
    float ratio = charge / param->unk0;
    // BYTES: the original picks a size from the charge ratio that nothing reads; its two compares and the unread 0.45
    // literal stay in the code and the pool.
    int size;
    if (ratio > 0.84999996f) { // 0x3F599999 in the original pool (one step below 0.85f)
        size = 0;
    } else if (ratio > 0.65f) {
        size = 1;
    } else if (ratio > 0.45f) {
        size = 2;
    } else {
        size = 3;
    }
    soEffectModule& effect = moduleAccesser->getEffectModule();
    effect.req(static_cast<EfID>(0x1B000E), &posture.getPos(), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
    work.setInt(param->unk28 + param->unk2C * charge, 0x20000001);
}

void wnLucasPKFlashStatusUniqProcessBang::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    if (attack.isAttack(0, false)) {
        // HYPOTHESIS: the work value at 0x20000001 is the attack power for hit 0.
        attack.setPower(0, moduleAccesser->getWorkManageModule().getInt(0x20000001), false);
    }
}
