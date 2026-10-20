#pragma once

#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <types.h>

class soAnimObj;
class soStatusData;
class StageObject;
struct ftWarioSpecialLwParam;

// The link events a Wario bike (or its rider) sends to the other side extend soLinkEventArgs by one int and one byte.
// HYPOTHESIS: the member names follow how ftWarioCommon::notifyEventLink uses them.
struct ftWarioLinkEventArgs : public soLinkEventArgs {
    u32 m_value;  // 0x08 a node id (event 8), a hit stop duration (event 0x843) or a status kind (event 0x841)
    bool m_flag;  // 0x0C hit stop: may be shaken out (SDI)
};

// The part of the rider that Wario and Wario-Man share: the Waft gauge (the charge of Wario's down special, which
// builds up with time), the bike link and the changing between the two forms. Wario and Wario-Man forward their fighter
// overrides to the one instance of this class; the modules come in as soModuleAccesser.
// HYPOTHESIS: the work variables (LA-Basic 0x40..0x44, LA-Flag 0x3f..0x41) are named by use, they are not original.
class ftWarioCommon {
public:
    // LA-Basic: 0x40 a countdown, 0x41 the Waft charge in frames, 0x42 its level (0..3), 0x43 the frames since the bike was
    // ridden (starts at the remount cooldown), 0x44 the selected rider's task ID; LA-Flag: 0x3f the gauge is full, 0x40/0x41 see below
    ftWarioCommon() { }
    ~ftWarioCommon() { }

    void onActivate(soModuleAccesser* acc, soAnimObj* animObj, int remountCooldown);
    void onDeactivate(soModuleAccesser* acc, soAnimObj* animObj);
    void onStart(soModuleAccesser* acc, int startKind);
    void toDead(soModuleAccesser* acc, int deadReason); // HYPOTHESIS: the reason is passed along but not used
    void processUpdate(ftWarioSpecialLwParam* param, soModuleAccesser* acc, soAnimObj* animObj);
    bool notifyEventLink(ftWarioLinkEventArgs* args, soModuleAccesser* acc, StageObject* sender, int unk4); // unk4 is passed along but not used
    void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* acc);
    bool activeArticle(StageObject* bike, soModuleAccesser* acc);
    void toChange(soModuleAccesser* acc, int entryId, bool unk1, bool unk2);
    void setupChangeSucceedWork(soModuleAccesser* acc, void* changeSucceedWork);
    void changeSucceedCore(soModuleAccesser* acc, void* changeSucceedWork, int unk2); // unk2 is not used
};

extern ftWarioCommon g_ftWarioCommon;
