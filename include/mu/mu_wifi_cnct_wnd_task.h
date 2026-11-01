#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The window of the main menu that connects to Nintendo Wi-Fi Connection (and asks for a name when there is none) before
// the Wi-Fi screens open. Its own code is still in the uncarved part of sora_menu_main (0x38530 ...).
/// Function names come from the sora_menu_main map.
// HYPOTHESIS: only what muProcTop reads is declared; the size (0x1F4) is the one create allocates.
class muWifiCnctWndTask : public gfTask {
public:
    int m_mode;                     // 0x40 the mode create was called with; HYPOTHESIS: 1 disconnects (muProcWifi on B)
    u8 m_unk44[0x128 - 0x44];
    int m_result;                   // 0x128 HYPOTHESIS: 0 when the connection was made
    u8 m_unk12C[0x138 - 0x12C];
    int m_step;                     // 0x138 the step the window is in; HYPOTHESIS: 12 once it has closed
    u8 m_unk13C[0x1F4 - 0x13C];

    // HYPOTHESIS: the parameter types follow what muProcTop passes (three words of muMenuMain, a buffer of the menu
    // archive, the scene group and a message number).
    static muWifiCnctWndTask* create(int mode, int* unk1, void* buffer, nw4r::g3d::ScnGroup* group, int msgNo, int* unk2,
                                     int* unk3);
    virtual void exit();
};
