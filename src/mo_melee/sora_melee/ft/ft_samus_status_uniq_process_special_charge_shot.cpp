#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/situation/so_situation_module_impl.h>

// Class shapes from the module map; the empty method bodies are not reconstructed yet.
class ftSamusStatusUniqProcessSpecialNStart : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftSamusStatusUniqProcessSpecialAirNStart : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

class ftSamusStatusUniqProcessSpecialNHold : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNHold() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialNFire : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialNFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftSamusStatusUniqProcessSpecialAirNFire : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessSpecialAirNFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftSamusStatusUniqProcessSpecialNStart g_ftSamusStatusUniqProcessSpecialNStart;
ftSamusStatusUniqProcessSpecialAirNStart g_ftSamusStatusUniqProcessSpecialAirNStart;
ftSamusStatusUniqProcessSpecialNHold g_ftSamusStatusUniqProcessSpecialNHold;
ftSamusStatusUniqProcessSpecialNFire g_ftSamusStatusUniqProcessSpecialNFire;
ftSamusStatusUniqProcessSpecialAirNFire g_ftSamusStatusUniqProcessSpecialAirNFire;

// HYPOTHESIS: flag 0x1200003E requests a fresh charge: clear work bank 2, the charge and the air flag 0x22000014 follow the situation.
void ftSamusStatusUniqProcessSpecialNStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& request = moduleAccesser->getWorkManageModule();
    if (request.isFlag(0x1200003E)) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.clearAll(2);
        work.setInt(0, 0x10000042);
        work.onFlag(0x22000010);
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            work.offFlag(0x22000014);
        } else {
            work.onFlag(0x22000014);
        }
        request.offFlag(0x1200003E);
    }
}
void ftSamusStatusUniqProcessSpecialNStart::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftSamusStatusUniqProcessSpecialAirNStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& request = moduleAccesser->getWorkManageModule();
    if (request.isFlag(0x1200003E)) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.clearAll(2);
        work.setInt(0, 0x10000042);
        work.onFlag(0x22000010);
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            work.offFlag(0x22000014);
        } else {
            work.onFlag(0x22000014);
        }
        request.offFlag(0x1200003E);
    }
}
void ftSamusStatusUniqProcessSpecialAirNStart::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftSamusStatusUniqProcessSpecialNHold::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
void ftSamusStatusUniqProcessSpecialNHold::execFixPos(soModuleAccesser* moduleAccesser) { }
void ftSamusStatusUniqProcessSpecialNHold::exitStatus(soModuleAccesser* moduleAccesser, int) { }
// HYPOTHESIS: unless it follows status 0x11C, the charge in work int 0x10000041 moves to 0x10000042 and is cleared.
void ftSamusStatusUniqProcessSpecialNFire::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0x11C) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setInt(work.getInt(0x10000041), 0x10000042);
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    }
    moduleAccesser->getEffectModule().removeCommon(0x1A);
}
void ftSamusStatusUniqProcessSpecialNFire::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
    }
}
void ftSamusStatusUniqProcessSpecialNFire::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftSamusStatusUniqProcessSpecialAirNFire::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0x11A) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setInt(work.getInt(0x10000041), 0x10000042);
        moduleAccesser->getWorkManageModule().setInt(0, 0x10000041);
    }
    moduleAccesser->getEffectModule().removeCommon(0x1A);
}
void ftSamusStatusUniqProcessSpecialAirNFire::execStatus(soModuleAccesser* moduleAccesser) { }
void ftSamusStatusUniqProcessSpecialAirNFire::exitStatus(soModuleAccesser* moduleAccesser, int) { }
