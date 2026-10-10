#include <ft/ft_status_uniq_process_catch_pull.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <types.h>

// HYPOTHESIS: the catch module (soModuleEnumeration::m_catchModule) has virtual slots past the
// eleven declared in so_catch_module.h; these are the ones the cling status calls, names unknown.
class ftStatusCatchModuleLate : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38(int);
    virtual bool unk3C(soModuleAccesser* moduleAccesser, void* data);
};

static inline ftStatusCatchModuleLate* getCatchModule(soModuleAccesser* moduleAccesser) {
    return static_cast<ftStatusCatchModuleLate*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

class ftGanonStatusUniqProcessSpecialHiCling : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialHiCling() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

void ftGanonStatusUniqProcessSpecialHiCling::execFixPos(soModuleAccesser* moduleAccesser) {
    getCatchModule(moduleAccesser)->unk30();
}

void ftGanonStatusUniqProcessSpecialHiCling::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x11d) {
        if (nextStatus >= 0x11d || nextStatus != 0x3b) {
            getCatchModule(moduleAccesser)->unk38(0);
        }
    }
}

bool ftGanonStatusUniqProcessSpecialHiCling::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    return getCatchModule(moduleAccesser)->unk3C(moduleAccesser, data);
}

bool ftGanonStatusUniqProcessSpecialHiCling::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    getCatchModule(moduleAccesser)->unk38(0);
    return true;
}

ftGanonStatusUniqProcessSpecialHiCling g_ftGanonStatusUniqProcessSpecialHiCling;
