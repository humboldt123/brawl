#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

class muWifiCnctWndTask;

// The top screen of the main menu: six icons (Group, Solo, Wi-Fi, Vault, Options, Data). Every icon but Wi-Fi opens
// a screen of the menu task (the code of an icon is its entry plus 1); Wi-Fi first opens the connection window.
class muProcTop : public muProcMenu {
public:
    enum Entry {
        Entry_Wifi = 2,             // opens the Wi-Fi connection window first
        Entry_Count = 6,
    };

    muWifiCnctWndTask* m_wifiWnd;   // 0x66C the Wi-Fi connection window while it is open
    u32 m_waitCount;                // 0x670 the frames the screen has waited for the icons to come in

    virtual ~muProcTop() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcTop) == 0x674, "Class is wrong size!");
