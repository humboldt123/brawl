#pragma once

// The unit includes this header first: MWCC writes RTTI and vtables to .data in reverse class-definition order. Only the
// status process interface comes in here, so soNullable and soEnable are defined later with the kinetic-energy chain
// (dynamic_cast in the Clash status), as the original's .data order shows. soModuleAccesser is declared for that header.
class soModuleAccesser;
#include <so/status/so_status_uniq_process.h>

struct soLinkEventArgs;

class ftStatusUniqProcessCaptainFinalTarget : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalTarget() { }
    static void linkMessageProcedure(soLinkEventArgs* args, soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalFuraFura : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalFuraFura() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

class ftStatusUniqProcessCaptainFinalClash : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalClash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftStatusUniqProcessCaptainFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptainFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
