#pragma once
#include <so/status/so_status_module_impl.h>

// Shared by Purin and Kirby's copied Rollout. The native singleton initializer
// associates each four-byte object with the corresponding status vtable.

class ftPurinStatusUniqProcessSpecialNStart : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNStart();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNStart g_ftPurinStatusUniqProcessSpecialNStart;

class ftPurinStatusUniqProcessSpecialNHold : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNHold();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNHold g_ftPurinStatusUniqProcessSpecialNHold;

class ftPurinStatusUniqProcessSpecialNHoldMax : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNHoldMax();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNHoldMax g_ftPurinStatusUniqProcessSpecialNHoldMax;

class ftPurinStatusUniqProcessSpecialNRoll : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNRoll();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNRoll g_ftPurinStatusUniqProcessSpecialNRoll;

class ftPurinStatusUniqProcessSpecialNRollAir : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNRollAir();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNRollAir g_ftPurinStatusUniqProcessSpecialNRollAir;

class ftPurinStatusUniqProcessSpecialNTurn : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNTurn();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPosCounter(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNTurn g_ftPurinStatusUniqProcessSpecialNTurn;

class ftPurinStatusUniqProcessSpecialNEnd : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNEnd();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNEnd g_ftPurinStatusUniqProcessSpecialNEnd;

class ftPurinStatusUniqProcessSpecialNHitEnd : public soStatusUniqProcess {
public:
    virtual ~ftPurinStatusUniqProcessSpecialNHitEnd();
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};
extern ftPurinStatusUniqProcessSpecialNHitEnd g_ftPurinStatusUniqProcessSpecialNHitEnd;

class ftPurinStatusUniqProcessSpecialNUtility {
public:
    // HYPOTHESIS: source utility methods are static; both native callers use
    // only the module accesser. Bodies confirm hit response and reversal.
    static bool ftProcHitSpecialNPurin(soModuleAccesser*);
    static void ftProcDamageTurnPurinSpecialN(soModuleAccesser*);
};
