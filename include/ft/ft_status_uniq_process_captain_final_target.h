#pragma once

// The unit includes this header first: MWCC writes RTTI and vtables to .data in reverse class-definition order, and the
// original places the kinetic-energy RTTI chain (dynamic_cast in the Clash statuses) before these classes' vtables.
// HYPOTHESIS: in the original, soStatusUniqProcess also came from a header without soNullable/soEnable, which still
// precede these vtables here (so_status_module_impl.h needs soNullable for soStatusModule).
#include <so/status/so_status_module_impl.h>

class soModuleAccesser;
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
