#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// soCatchModuleImpl's map and these call sites identify the used virtual slots.
// The unused 0x34 slot remains opaque so the later slots keep their native offsets.
class ftDiddyCatchModuleView : public soCatchModule {
public:
    // HYPOTHESIS: the forwarded accesser is unused; its formal type is unverified.
    virtual void updatePosCling(soModuleAccesser* moduleAccesser); // vtable +0x30
    // HYPOTHESIS: this unused slot has an unverified signature.
    virtual void unk34();                                         // vtable +0x34
    // HYPOTHESIS: clingCut's zero argument is a Boolean selector.
    virtual void clingCut(bool);                                  // vtable +0x38
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data); // vtable +0x3C
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
    getCatchModule(moduleAccesser)->updatePosCling(moduleAccesser);
}

void ftDiddyStatusUniqProcessSpecialSCling::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x11f:
    case 0x121:
        break;
    // MATCH-ONLY: preserve the native switch cleanup boundary.
    case 0x120:
    default:
        getCatchModule(moduleAccesser)->clingCut(false);
        break;
    }
}

bool ftDiddyStatusUniqProcessSpecialSCling::checkDamage(soModuleAccesser* moduleAccesser, void* data) {
    return getCatchModule(moduleAccesser)->checkDamage(moduleAccesser, data);
}

ftDiddyStatusUniqProcessSpecialSCling g_ftDiddyStatusUniqProcessSpecialSCling;
