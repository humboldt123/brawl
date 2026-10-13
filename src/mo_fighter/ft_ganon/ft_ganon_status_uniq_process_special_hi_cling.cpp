#include <ft/ft_status_uniq_process_catch_pull.h>
#include <ft/ganon/ft_ganon_status_uniq_process_special_hi.h>
#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <types.h>

// The constructor-installed soCatchModuleImpl table places these methods after catchCut.
class ftStatusCatchModuleClingView : public soCatchModule {
public:
    // HYPOTHESIS: this hook consumes no argument; its original signature is unverified.
    virtual void updatePosCling();
    // HYPOTHESIS: this unused hook has an unverified signature.
    virtual void updateLRCling();
    // HYPOTHESIS: the native selector tests zero/one; its source type is unverified.
    virtual void clingCut(bool cut);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
};

static inline ftStatusCatchModuleClingView* getCatchModule(soModuleAccesser* moduleAccesser) {
    return static_cast<ftStatusCatchModuleClingView*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

void ftGanonStatusUniqProcessSpecialHiCling::execFixPos(soModuleAccesser* moduleAccesser) {
    getCatchModule(moduleAccesser)->updatePosCling();
}

void ftGanonStatusUniqProcessSpecialHiCling::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11d) {
        if (nextStatus >= 0x11d || nextStatus != 0x3b) {
            getCatchModule(moduleAccesser)->clingCut(false);
        }
    }
}

bool ftGanonStatusUniqProcessSpecialHiCling::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    return getCatchModule(moduleAccesser)->checkDamage(moduleAccesser, data);
}

bool ftGanonStatusUniqProcessSpecialHiCling::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    getCatchModule(moduleAccesser)->clingCut(false);
    return true;
}

ftGanonStatusUniqProcessSpecialHiCling g_ftGanonStatusUniqProcessSpecialHiCling;
