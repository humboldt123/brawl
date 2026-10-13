#pragma once

#include <gr/gr_pirates.h>
#include <snd/snd_3d_generator.h>

// The rain.
class grPiratesRain : public grPirates {
protected:
    u8* m_eventWork;            // 0x160 the event (weather) of the stage
    u8 unk164;                  // 0x164
    char _165[3];
    float unk168;               // 0x168
    float unk16C;               // 0x16C
    snd3DGenerator m_sndGenerator; // 0x170
    s32 unk178;                 // 0x178 the id of the sound that plays (-1 = none)

public:
    grPiratesRain(const char* taskName);
    static grPiratesRain* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesRain();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setEventWork(u8* event) { m_eventWork = event; }
};
static_assert(sizeof(grPiratesRain) == 0x17C, "grPiratesRain layout");
