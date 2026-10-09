#pragma once

#include <havok/hkMemory.h>

// Base of the Havok collision agents. Agents are reference counted (hkReferencedObject) and
// allocated with memory class 0x1d. Only the 0x08 word is known so far (written to zero by the
// constructor); the remaining virtual method signatures are not identified yet.
struct hkCollisionAgent : hkReferencedObject {
    int unk8; // 0x08 HYPOTHESIS: per-agent state, zeroed by the constructor

    hkCollisionAgent() {
        unk8 = 0;
    }
    virtual ~hkCollisionAgent();

    virtual void invalidateTim();
    virtual void warpTime();
    virtual void removePoint();
    virtual void commitPotential();
    virtual void createZombie();
};
