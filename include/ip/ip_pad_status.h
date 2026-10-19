#pragma once
#include <StaticAssert.h>
#include <types.h>

// Shared definition from BrawlHeaders' ip_switch.h, factored out so the
// recorder and input switch can both use the verified four-byte pad format.
struct ipPadStatus {
    u16 m_button;
    s8 m_stickX;
    s8 m_stickY;
};
static_assert(sizeof(ipPadStatus) == 4, "Recorded pad size");
