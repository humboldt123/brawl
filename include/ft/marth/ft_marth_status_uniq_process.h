#pragma once

#include <ft/marth/ft_marth_param.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Status kinds of Marth's special moves. HYPOTHESIS: the names follow the order in which ftMarth registers its
// status processes (kind = 0x112 + registration index) and what each process does.
struct ftMarthStatus {
    enum Kind {
        SpecialNStart = 0x112,
        SpecialS = 0x113,
        SpecialHi = 0x114,
        SpecialLw = 0x115,
        FinalStart = 0x116,
        SpecialNLoop = 0x117,
        SpecialNEnd = 0x118,
        SpecialS2 = 0x11a,
        SpecialS3 = 0x11b,
        SpecialS4 = 0x11c,
        SpecialLwHit = 0x11d,
        FinalDash = 0x11e,
        FinalUnk = 0x11f,
        FinalHit = 0x120,
    };
};

// Work variable ids used by Marth's status processes. HYPOTHESIS: names come from how each id is read and
// written; ids that carry a window slot (Final_*) are followed by up to six consecutive slots.
struct ftMarthWork {
    enum Int {
        SpecialN_ChargeFrame = 0x20000000,
        SpecialN_ChargeFrameMax = 0x20000001,
        SpecialN_Situation = 0x20000002,
        SpecialS_Situation = 0x20000003,
        SpecialLw_Situation = 0x20000000,
        Final_Situation = 0x20000003,
        Final_WindowTaskId = 0x20000004,
        Final_WindowShowTimer = 0x2000000a,
        Final_WindowMoveTimer = 0x20000010,
        Final_WindowCount = 0x20000016,
    };
    enum Float {
        SpecialHi_StickAngle = 0x21000004,
        SpecialLw_Power = 0x21000004,
        SpecialLw_Lr = 0x21000005,
        Final_WindowAngle = 0x21000004,
    };
    enum Flag {
        SpecialHi_Launched = 0x22000010,
        SpecialHi_Falling = 0x22000013,
        SpecialHi_Updated = 0x22000014,
        SpecialHi_ApplyAngle = 0x22000015,
        SpecialHi_LrChanged = 0x22000016,
        SpecialLw_ShieldEnable = 0x22000011,
        SpecialLw_ShieldActive = 0x22000012,
        SpecialLw_Hit = 0x22000013,
        Final_HitConnected = 0x22000011,
        Final_WindowHit = 0x22000012,
    };
};

class ftMarthStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftMarthStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

class ftMarthStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftMarthStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    void updateHpWindow(soModuleAccesser* moduleAccesser);
};

extern ftMarthStatusUniqProcessSpecialS g_ftMarthStatusUniqProcessSpecialS;
extern ftMarthStatusUniqProcessSpecialHi g_ftMarthStatusUniqProcessSpecialHi;
extern ftMarthStatusUniqProcessSpecialLw g_ftMarthStatusUniqProcessSpecialLw;
extern ftMarthStatusUniqProcessFinal g_ftMarthStatusUniqProcessFinal;

class ftMarthStatusUniqProcessSpecialNStart : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessSpecialNStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
class ftMarthStatusUniqProcessSpecialNLoop : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessSpecialNLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
class ftMarthStatusUniqProcessSpecialNEnd : public soStatusUniqProcess {
public:
    virtual ~ftMarthStatusUniqProcessSpecialNEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};
extern ftMarthStatusUniqProcessSpecialNStart g_ftMarthStatusUniqProcessSpecialNStart;
extern ftMarthStatusUniqProcessSpecialNLoop g_ftMarthStatusUniqProcessSpecialNLoop;
extern ftMarthStatusUniqProcessSpecialNEnd g_ftMarthStatusUniqProcessSpecialNEnd;
