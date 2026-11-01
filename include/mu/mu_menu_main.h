#pragma once

#include <StaticAssert.h>
#include <mu/menu.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class muProcMenu;

// The scene of the menus: the part of it that the menu of the rules reads and writes.
// HYPOTHESIS: the class has many more fields (the size is not known).
class muMenuMain : public muMenu {
public:
    u8 m_unk350[0x3AC - 0x350];
    muProcMenu* m_proc;             // 0x3AC the screen that is running (HYPOTHESIS: the menu runs one proc at a time)
    u8 m_unk3B0[0x3D8 - 0x3B0];
    int m_padNo;                    // 0x3D8 the pad the scene was started with
    u8 m_unk3DC[0xA8C - 0x3DC];
    // HYPOTHESIS: model files of the menu archive that the screens hand to the name menu and the windows they open
    // (muProcOptBtn passes 0xA98 to muNameTask::create1 as the keyboard models and 0xA8C/0xA9C as the Wii Remote and
    // window models; the Wi-Fi connection window, which asks for a name, takes 0xA94, 0xA98 and 0xA9C).
    nw4r::g3d::ResFile m_resFileA8C;    // 0xA8C
    nw4r::g3d::ResFile m_resFileA90;    // 0xA90
    nw4r::g3d::ResFile m_resFileA94;    // 0xA94
    nw4r::g3d::ResFile m_resFileA98;    // 0xA98 the keyboard of the name menu
    nw4r::g3d::ResFile m_resFileA9C;    // 0xA9C the windows
    int m_startCode;                // 0xAA0 how the scene was started (the code of the menu that opens)
    u8 m_unkAA4[0xAB0 - 0xAA4];
    int m_exitCode;                 // 0xAB0 the menu that opens when this one closes
    int m_exitState;                // 0xAB4
    int m_padLock;                  // 0xAB8 the menus do not read the pads while this is more than 0

    void* getBuffer(int index);     // (name from the sora_menu_main map) HYPOTHESIS: a file of the menu archive by index
    void addCommonObject();         // (name from the sora_menu_main map)
};
