#include <so/damage/so_damage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Eruption (Ike Special N) end at full charge.
class ftIkeStatusUniqProcessSpecialNEndMax : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialNEndMax() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftIkeStatusUniqProcessSpecialNEndMax g_ftIkeStatusUniqProcessSpecialNEndMax;

void ftIkeStatusUniqProcessSpecialNEndMax::initStatus(soModuleAccesser* acc) {
    acc->getKineticModule().getEnergy(0)->enable();
}

void ftIkeStatusUniqProcessSpecialNEndMax::execFixPos(soModuleAccesser* acc) {
    if (acc->getCollisionAttackModule().isAttack(0, false)) {
        if (!acc->getWorkManageModule().isFlag(0x22000012)) {
            // HYPOTHESIS: the constant id is the self damage Ike takes when the full charge hits.
            float value = acc->getConstantFloatKirby(0xfa2);
            acc->getDamageModule().addDamage(value, 0);
            acc->getWorkManageModule().onFlag(0x22000012);
        }
    }
}

void ftIkeStatusUniqProcessSpecialNEndMax::exitStatus(soModuleAccesser*, int) { }
