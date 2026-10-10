#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Catch-module view: the header covers the vtable prefix up to catchCut; the later
// slots used by this status are not yet named, so they are kept as unkN.
class ftDiddyCatchModuleView : public soCatchModule {
public:
    virtual void unk2C(soModuleAccesser* moduleAccesser);
    virtual void unk30();
    virtual void unk34(bool);
    virtual bool unk38(soModuleAccesser* moduleAccesser, void* data);
};

class ftDiddyStatusUniqProcessSpecialSCling : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialSCling() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
};

static inline ftDiddyCatchModuleView* getCatchModule(soModuleAccesser* moduleAccesser) {
    return static_cast<ftDiddyCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

void ftDiddyStatusUniqProcessSpecialSCling::execFixPos(soModuleAccesser* moduleAccesser) {
    getCatchModule(moduleAccesser)->unk2C(moduleAccesser);
}

void ftDiddyStatusUniqProcessSpecialSCling::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x120) {
        if (nextStatus < 0x120) {
            if (nextStatus >= 0x11f) {
                return;
            }
        } else {
            if (nextStatus < 0x122) {
                return;
            }
        }
    }
    getCatchModule(moduleAccesser)->unk34(false);
}

bool ftDiddyStatusUniqProcessSpecialSCling::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    return getCatchModule(moduleAccesser)->unk38(moduleAccesser, data);
}

ftDiddyStatusUniqProcessSpecialSCling g_ftDiddyStatusUniqProcessSpecialSCling;
