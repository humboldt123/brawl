#pragma once

#include <so/status/so_status_module_impl.h>

class ftPurinStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    // MATCH-ONLY: retain the native out-of-line global constructor.
    ftPurinStatusUniqProcessSpecialS() __attribute__((never_inline)) { }
    virtual ~ftPurinStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: pointer-first source order; the native ABI uses r4 and f1.
    float getAngleSpecialAirSPurin(soModuleAccesser* moduleAccesser, float stickY);
};

class ftPurinStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    // MATCH-ONLY: retain the native out-of-line global constructor.
    ftPurinStatusUniqProcessSpecialHi() __attribute__((never_inline)) { }
    virtual ~ftPurinStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

extern ftPurinStatusUniqProcessSpecialS g_ftPurinStatusUniqProcessSpecialS;
extern ftPurinStatusUniqProcessSpecialHi g_ftPurinStatusUniqProcessSpecialHi;

class ftPurinStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    // MATCH-ONLY: the global instance retains an out-of-line constructor.
    ftPurinStatusUniqProcessSpecialLw() __attribute__((never_inline)) { }
    virtual ~ftPurinStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execStatus(soModuleAccesser*);
};

class ftPurinStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    // MATCH-ONLY: the global instance retains an out-of-line constructor.
    ftPurinStatusUniqProcessFinal() __attribute__((never_inline)) { }
    virtual ~ftPurinStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execStatus(soModuleAccesser*);
    virtual void execStop(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    void setModelTurn(soModuleAccesser*) __attribute__((never_inline));
};

extern ftPurinStatusUniqProcessSpecialLw g_ftPurinStatusUniqProcessSpecialLw;
extern ftPurinStatusUniqProcessFinal g_ftPurinStatusUniqProcessFinal;
