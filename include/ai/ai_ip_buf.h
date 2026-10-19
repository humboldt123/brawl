#pragma once

#include <mt/mt_vector.h>
#include <types.h>

// Ring buffer of the last 16 frames of a CPU character's stick and button input.
class aiIpBuf {
public:
    Vec2f m_stick[16];  // 0x00
    u32 m_button[16];   // 0x80
    int m_index;        // 0xC0: slot of the newest frame

    aiIpBuf();
    void update(Vec2f* stick, u32* button);
    void getData(int framesBack, Vec2f* stick, u32* button);
    Vec2f getStick(int framesBack);
    u32 getButton(int framesBack);
};
static_assert(sizeof(aiIpBuf) == 0xC4, "Class is wrong size!");
