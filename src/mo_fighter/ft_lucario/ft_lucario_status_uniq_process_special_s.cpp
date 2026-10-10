#include <so/catch/so_catch_module.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftLucarioStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftLucarioStatusUniqProcessSpecialSThrow : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialSThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

// MATCH-ONLY: soCatchModule in the shared header stops at catchCut; the target also calls a later
// virtual (vtable slot 15) that is not declared there. This local view extends the same layout.
class ftCatchModuleView {
public:
    virtual ~ftCatchModuleView() { }
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();
    virtual void setCatch(int taskId);
    virtual bool isCatch();
    virtual void onCatchFlag();
    virtual void offCatchFlag();
    virtual void catchCut(bool, bool);
    virtual void unk9();
    virtual void unk10();
    virtual void unk11();
    virtual bool unk12();
};

static ftCatchModuleView* getCatchView(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftCatchModuleView*>(moduleAccesser->m_enumerationStart->m_catchModule);
}

void ftLucarioStatusUniqProcessSpecialS::initStatus(soModuleAccesser*) {}
void ftLucarioStatusUniqProcessSpecialS::execStatus(soModuleAccesser*) {}
void ftLucarioStatusUniqProcessSpecialS::exitStatus(soModuleAccesser*, int) {}

void ftLucarioStatusUniqProcessSpecialSThrow::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
    // HYPOTHESIS: the catch module is typed void* in the accessor; cast it to soCatchModule.
    soCatchModule* catchModule = static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule);
    catchModule->setCatch(-1);
}

void ftLucarioStatusUniqProcessSpecialSThrow::execFixPos(soModuleAccesser*) {}

void ftLucarioStatusUniqProcessSpecialSThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soCatchModule* catchModule = static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule);
    catchModule->catchCut(false, false);
}

bool ftLucarioStatusUniqProcessSpecialSThrow::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        return false;
    }
    return getCatchView(moduleAccesser)->unk12();
}

ftLucarioStatusUniqProcessSpecialS g_ftLucarioStatusUniqProcessSpecialS;
ftLucarioStatusUniqProcessSpecialSThrow g_ftLucarioStatusUniqProcessSpecialSThrow;
