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
    // Used by derived agents that take the per-pair value in their constructor (hkPhantomAgent).
    explicit hkCollisionAgent(int unk8Value) {
        unk8 = unk8Value;
    }
    virtual ~hkCollisionAgent() {}

    virtual void invalidateTim();
    virtual void warpTime();
    virtual void removePoint();
    virtual void commitPotential();
    virtual void createZombie();
};

// Contact manager that agents store in their 0x08 word (passed as the last argument of the create functions).
// Only the virtual slots used by the sphere agents are declared; names are HYPOTHESIS until the class is recovered.
struct hkContactMgr : hkReferencedObject {
    virtual void unk10(); // 0x10 HYPOTHESIS
    virtual void unk14(); // 0x14 HYPOTHESIS
    virtual void unk18(); // 0x18 HYPOTHESIS: called by hkSphereSphereAgent::cleanup on the 0x08 object
};
