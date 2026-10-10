#pragma once

#include <havok/hkArray.h>
#include <types.h>

class ph2ndaryWorld;
class ph2ndaryLine;


// Weapon / secondary-physics controller (main.dol map class ph2ndaryController).
// Object size 0xF4, created by phInterface::get2ndaryController.
class ph2ndaryController {
public:
    ph2ndaryController(void* owner, int a, int b);
    ~ph2ndaryController();

    void calacCallBackTimingCFor2ndary(int a, int b, int c, int d);
    void ph2ndaryMain(int a, int b, int c, int d, int e, int f);
    bool isHitCollisionLastNode2Pos(u32 mask);
    void processDefault();
    void draw();

    hkArray<ph2ndaryLine*> m_lines;   // 0x00 (data, count, capacity)
    u8 unk0C[0x40 - 0x0C];            // 0x0C
    u8 m_active;                      // 0x40, set to 1 by the owner
    u8 unk41[0x5C - 0x41];            // 0x41
    u8 m_winding;                     // 0x5C, cleared by resetWinding
    u8 unk5D[0xE8 - 0x5D];            // 0x5D
    u32 m_0xE8;                       // 0xE8
    u32 m_flags;                      // 0xEC
    u8 unkF0[0xF4 - 0xF0];            // 0xF0
};

// Pair wrapper (main.dol map class ph2ndary2Controller). Object size 0x0C.
class ph2ndary2Controller {
public:
    ph2ndary2Controller(void* a, void* b);
    ~ph2ndary2Controller();

    bool isHitCollisionLastNode2Pos();

    ph2ndaryController* m_controller[2];   // 0x00, 0x04
    u8 unk08[3];                           // 0x08..0x0A
};
