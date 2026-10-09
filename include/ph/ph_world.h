#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

struct hkWorld;

// Brawl physics world wrapper (0x08 bytes). Layout: 0x00 vptr, 0x04 m_hkWorld (hkWorld*, cleared by the constructor).
struct phWorld {
    phWorld();                     // HYPOTHESIS: no arguments in the target
    virtual ~phWorld();            // slot 0x00
    virtual void stepTime(hkReal dt); // HYPOTHESIS: forwards dt to hkWorld::stepDeltaTime

    hkWorld* m_hkWorld; // 0x04
};
