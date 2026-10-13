#pragma once

#include <StaticAssert.h>
#include <gf/gf_pad_status.h>
#include <types.h>

// The pads of the boot and title screens: the status of the four controllers (and of the other four pads of the system
// menu), and what button the screens wait for (start, or any button that skips the strap and the boot).
class muBootPad {
public:
    gfPadStatus m_pad[4];                       // 0x000
    gfPadStatus m_pad2[4];                      // 0x100 HYPOTHESIS: the same controllers as seen by the menu (pad numbers 4 - 7)

    muBootPad();
    virtual ~muBootPad();

    void update();
    bool isPressStartButton();
    bool isPressStrapSkipButton();
    bool isPressStrapSkipButton1(const gfPadStatus* pad);
    bool isPressBootSkipButton();
    bool isPressBootSkipButton1(const gfPadStatus* pad);
    bool isOnEdge(u32 mask);
};
static_assert(sizeof(muBootPad) == 0x204, "Class is wrong size!");
