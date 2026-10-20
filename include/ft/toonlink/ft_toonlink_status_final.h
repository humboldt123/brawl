#pragma once

// Toon Link's Final Smash (the Triforce Slash, the same moves as Link's): the three statuses it runs through and the hooks the fighter class calls while
// they run. The statuses: Final (the Triforce shows up and Link looks for a target in front of him), Dash (Link rushes
// to the caught opponent) and Combo (the slashes). HYPOTHESIS: names of the statuses follow the module map.
//
// Work values used (all HYPOTHESIS):
//   int   0x20000001 effect handle of the Triforce around Link, 0x20000002 effect handle of the second effect
//   int   0x20000003 task id of the caught fighter (-1 while none), 0x20000004 second value of the capture request
//   float 0x21000004 distance to the caught fighter
//   flag  0x22000017 the catch search was asked for, 0x22000018 the search shape is registered,
//         0x22000019 the search found a fighter and the catch must be completed
//   float 0x11000013/0x11000014 the position stored at activation (see ftLink::onActivate)

#include <so/status/so_status_module_impl.h>
#include <mt/mt_vector.h>

class soCollisionLog;
class soModuleAccesser;

class ftToonLinkStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);

    // The fighter forwards its collision search events here while the search is registered (status 0x116).
    static void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    static bool notifyEventCollisionSearchCheck(soModuleAccesser* moduleAccesser);
    static void catch_success(soModuleAccesser* moduleAccesser);
};

class ftToonLinkStatusUniqProcessFinalDash : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessFinalDash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);

    static Vec3f getCapturePos(soModuleAccesser* moduleAccesser);
    static Vec3f getObjectivePos(soModuleAccesser* moduleAccesser);
};

class ftToonLinkStatusUniqProcessFinalCombo : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessFinalCombo() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);

    static void checkFinish(soModuleAccesser* moduleAccesser);
};
