#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkCdBody.h>

struct hkContactMgr;

// Base of the GSK collision agents (GJK/separating-axis pair agents). Object size 0x30; hkGskfAgent is 0x80. The constructor takes the two collision bodies
// and the contact manager, which is stored in the 0x08 word inherited from hkCollisionAgent.
struct hkGskBaseAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u8 unkC[0x18 - 0x0C]; // 0x0C GSK cache/state initialised by the constructor (not identified)
    float unk18;          // 0x18 HYPOTHESIS: cached time; the invalidated value is the constant stored by invalidateTim
    float unk1C;          // 0x1C set by the constructor to the smaller of the two root body values at 0x20
    float unk20;          // 0x20 HYPOTHESIS: per-body values, reset to the same constant by invalidateTim
    float unk24;          // 0x24
    float unk28;          // 0x28
    float unk2C;          // 0x2C

    hkGskBaseAgent(hkCdBody* bodyA, hkCdBody* bodyB, hkContactMgr* contactMgr);
    virtual ~hkGskBaseAgent();

    virtual void cleanup();
    virtual void processCollision(void* unk0, void* unk1, void* unk2);
    virtual void invalidateTim();
    virtual void warpTime(float oldTime, float newTime);
};
