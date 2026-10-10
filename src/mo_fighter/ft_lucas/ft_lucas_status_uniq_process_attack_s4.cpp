#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class ftLucasStatusUniqProcessAttackS4 : public soStatusUniqProcess {
public:
    virtual ~ftLucasStatusUniqProcessAttackS4() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftLucasStatusUniqProcessAttackS4::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000010)) {
        moduleAccesser->getCollisionReflectorModule().setStatus(0, 1, 2);
        work.offFlag(0x22000010);
    }
    if (work.isFlag(0x22000011)) {
        moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
        work.offFlag(0x22000011);
    }
}

ftLucasStatusUniqProcessAttackS4 g_ftLucasStatusUniqProcessAttackS4;
