#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

struct hkWorld;

// Brawl physics world wrapper (0x08 bytes). Layout: 0x00 vptr, 0x04 m_hkWorld (hkWorld*, cleared by the constructor).
struct phWorld {
    phWorld();                     // HYPOTHESIS: no arguments in the target
    virtual ~phWorld();            // slot 0x00
#ifdef SORA_MINIGAME_PH_WORLD_NONVIRTUAL_STEPTIME
    void stepTime(hkReal dt);
#else
    virtual void stepTime(hkReal dt); // HYPOTHESIS: forwards dt to hkWorld::stepDeltaTime
#endif

    hkWorld* m_hkWorld; // 0x04
};
