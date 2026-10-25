#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Eruption (Ike Special N) end: the hitboxes of the eruption keep the charge power.
class ftIkeStatusUniqProcessSpecialNEnd : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialNEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftIkeStatusUniqProcessSpecialNEnd g_ftIkeStatusUniqProcessSpecialNEnd;

void ftIkeStatusUniqProcessSpecialNEnd::initStatus(soModuleAccesser* acc) {
    acc->getKineticModule().getEnergy(0)->enable();
}

void ftIkeStatusUniqProcessSpecialNEnd::execFixPos(soModuleAccesser* acc) {
    // HYPOTHESIS: constant ids 0x5dc1 (base power) and 0x5dc2 (power per charge step) from the Ike constant table.
    soWorkManageModule& work = acc->getWorkManageModule();
    int perStep = acc->getConstantIntKirby(0x5dc2);
    int power = acc->getConstantIntKirby(0x5dc1) + (work.getInt(0x20000000) / 30) * perStep;
    // Each complete thirty-frame charge step raises every active hitbox's power.
    for (int i = 0; i < (s32)acc->getCollisionAttackModule().getPartSize(); i++) {
        if (acc->getCollisionAttackModule().isAttack(i, false)) {
            acc->getCollisionAttackModule().setPower(i, power, false);
        }
    }
}

void ftIkeStatusUniqProcessSpecialNEnd::exitStatus(soModuleAccesser*, int) { }
