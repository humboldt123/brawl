#include <ft/ft_damage_transactor_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// sora_melee: soDamageValueAccesser::getConstantStatusKind (the map name); ft_damage_transactor_impl.cpp calls it by this
// stand-in name too.
extern "C" int fn_27_61334(soModuleAccesser* moduleAccesser, int statusKind);

// Diddy Kong's damage transactor (handed to his damage module, see ft_diddy.h): it keeps the knockback speed and the
// damage frames in work floats and redirects damage taken in two of his own statuses.
// HYPOTHESIS: statuses 0x125/0x127 are the Rocketbarrel flight statuses and 0x126/0x128 their damaged forms.
class ftDiddyDamageTransactorImpl : public ftDamageTransactorImpl {
public:
    virtual ~ftDiddyDamageTransactorImpl() { }
    virtual bool onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog);
};

bool ftDiddyDamageTransactorImpl::onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser,
                                                              soDamageLog* damageLog) {
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_speed.m_x, 0x21000009);
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_speed.m_y, 0x2100000a);
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_frame, 0x2100000b);
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x125) {
        return moduleAccesser->getStatusModule().changeStatusRequest(0x126, moduleAccesser);
    }
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x127) {
        // HYPOTHESIS: a hit stronger than constant 0xfb3 sets the latch 0x22000013 (the barrel is lost).
        if (damageLog->m_reaction > soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
        }
        return moduleAccesser->getStatusModule().changeStatusRequest(0x128, moduleAccesser);
    }
    int status = fn_27_61334(moduleAccesser, statusKind);
    return moduleAccesser->getStatusModule().changeStatusRequest(status, moduleAccesser);
}

ftDiddyDamageTransactorImpl g_ftDiddyDamageTransactorImpl;
