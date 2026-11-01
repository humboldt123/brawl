#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

class muWifiCnctWndTask;

// The Wi-Fi screen of the main menu: "With Friends", "With Anyone" and a third icon that is only shown when the network
// allows it. Each icon opens a screen of the menu task (codes 0x1A, 0x1B, 0x1C); B opens the connection window to
// disconnect before going back to the top screen.
// HYPOTHESIS: the names of the first two entries follow the codes muProcWifiFriend and muProcWifiAnybody are opened with.
class muProcWifi : public muProcMenu {
public:
    enum Entry {
        Entry_Friend = 0,
        Entry_Anybody = 1,
        Entry_Extra = 2,            // only shown when nteGlobal::isEnableButton(0x20) or (0x40)
        Entry_Count = 3,
        Entry_ObjBase = 18,         // the first object of the icons in m_objs
    };

    muWifiCnctWndTask* m_wifiWnd;   // 0x66C the connection window while it is open
    bool m_showExtra;               // 0x670 the third icon is shown

    virtual ~muProcWifi() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcWifi) == 0x674, "Class is wrong size!");
