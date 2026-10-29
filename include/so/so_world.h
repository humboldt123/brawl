// Local BrawlHeaders shadow: adds the world defaults and the static initializer (so_world.o).
#pragma once

#include <StaticAssert.h>
#include <types.h>

class soWorld {
public:
    soWorld() : m_gravityUp(1.0f), m_gravityDown(1.0f), m_8(1.0f), m_12(0), m_13(1) { }
    static void initialize();

    float m_gravityUp;
    float m_gravityDown;
    float m_8;
    u8 m_12;
    u8 m_13;
    char _14[2];
};
static_assert(sizeof(soWorld) == 0x10, "Class is wrong size!");

extern soWorld* g_soWorld;
