#pragma once

#include <havok/hkIterativeLinearCastAgent.h>
#include <havok/hkCdBody.h>

struct hkContactMgr;

// Base of the GSK collision agents (GJK/separating-axis pair agents). Object size 0x30; hkGskfAgent is 0x80. The constructor takes the two collision bodies
// and the contact manager, which is stored in the 0x08 word inherited from hkCollisionAgent.
struct hkGskBaseAgent : hkIterativeLinearCastAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u8 unkC[0x18 - 0x0C]; // 0x0C GSK cache/state initialised by the constructor (not identified)
    float unk18;          // 0x18 HYPOTHESIS: cached time; the invalidated value is the constant stored by invalidateTim
    float unk1C;          // 0x1C set by the constructor to the smaller of the two root body values at 0x20
    // HYPOTHESIS: 0x20..0x2F is Havok's 16-byte aligned separating normal (hkVector4), which gives the class its
    // 16-byte alignment (see the struct attribute).
    float unk20;          // 0x20 HYPOTHESIS: per-body values, reset to the same constant by invalidateTim
    float unk24;          // 0x24
    float unk28;          // 0x28
    float unk2C;          // 0x2C

    hkGskBaseAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkContactMgr* contactMgr);
    // hkGskBaseAgent.o, not written yet; the collector is an hkCdBodyPairCollector (hkCdPointCollector below).
    static void staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                      void* collector);
    static void staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                       void* collector); // hkGskBaseAgent.o, not written yet
    // Inline: the derived agents' destructors (hkGskfAgent, hkPredGskfAgent, hkConvexListAgent) call no base
    // destructor; the out-of-line copy in hkGskBaseAgent.o comes from the vtable there.
    virtual ~hkGskBaseAgent() {}

    virtual void cleanup();
    virtual void processCollision(const hkCdBody& unk0, const hkCdBody& unk1, hkProcessCollisionInput& unk2, void* unk3);
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float oldTime, float newTime, void* arg);
} __attribute__((aligned(16)));
