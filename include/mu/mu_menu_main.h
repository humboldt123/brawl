#pragma once

#include <StaticAssert.h>
#include <mu/menu.h>
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
    u8 m_unk3DC[0xAA0 - 0x3DC];
    int m_startCode;                // 0xAA0 how the scene was started (the code of the menu that opens)
    u8 m_unkAA4[0xAB0 - 0xAA4];
    int m_exitCode;                 // 0xAB0 the menu that opens when this one closes
    int m_exitState;                // 0xAB4
    int m_padLock;                  // 0xAB8 the menus do not read the pads while this is more than 0
};
