#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>
#include <havok/hkBvAgent.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkPenetrationTarget;

// Bounding-volume-tree collision agent (object size 0x40, memory class 0x1d). Derives from hkCollisionAgent: the
// 0x08 word is the contact manager passed to the constructor. The constructor zeroes the entry list at 0x0C
// (an hkArray of 12-byte entries, empty with the don't-deallocate flag set), the two vectors at 0x20 and 0x30 and
// the word at 0x14 (set to 0x80000000, the flag word of the empty list).
struct hkBvTreeAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

    // HYPOTHESIS: 12-byte entry of the list at 0x0C (map name hkBvTreeAgent::hkBvAgentEntryInfo).
    struct hkBvAgentEntryInfo {
        u32 unk0; // HYPOTHESIS
        u32 unk4; // HYPOTHESIS
        u32 unk8; // HYPOTHESIS
    };

    hkArray<hkBvAgentEntryInfo> m_entries; // 0x0C
    int unk18;                             // 0x18 not written by the constructor
    int unk1C;                             // 0x1C not written by the constructor
    float unk20[4];                        // 0x20 zeroed by the constructor (HYPOTHESIS: vector)
    float unk30[4];                        // 0x30 zeroed by the constructor (HYPOTHESIS: vector)

    hkBvTreeAgent(hkContactMgr* contactMgr) __attribute__((never_inline));
    virtual ~hkBvTreeAgent() {}

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static hkBvTreeAgent* createBvTreeShapeAgent(void* a, void* b, void* c, hkContactMgr* contactMgr);
    static hkBvTreeAgent* createShapeBvAgent(void* a, void* b, void* c, hkContactMgr* contactMgr);
    static hkBvTreeAgent* createBvBvAgent(void* a, void* b, void* c, hkContactMgr* contactMgr);

    virtual void calcStatistics(void* collector) const;
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);
    virtual void processCollision(void* a, void* b, void* c);
    virtual void cleanup();
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);

    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);
};

// HYPOTHESIS: the symmetric variant is the object created by createBvTreeShapeAgent (its vtable is set after the base
// constructor). Declared here; the members are defined in hkBvTreeAgent.cpp.

// Penetration target wrapper built by the symmetric getPenetrations forwarders. Same layout as hkSymmetricTarget
// (flag at 0x04, target at 0x08); the members are assigned in the body, which orders the stores as the original does.
struct hkSymmetricFlagTarget : hkPenetrationTarget {
    hkSymmetricFlagTarget(hkPenetrationTarget* target) : m_flag(0), m_target(target) {}
    virtual void unkSlot08();
    virtual void forwardPenetrations(void* a, void* b, void* c);

    u8 m_flag;                     // 0x04
    hkPenetrationTarget* m_target; // 0x08
};

// Penetration target wrapper built by the symmetric getClosestPoints forwarders (no flag word).
struct hkSymmetricClosestTarget : hkPenetrationTarget {
    hkSymmetricClosestTarget(hkPenetrationTarget* target) : unk4(0.0f), m_target(target) {}
    virtual void unkSlot08();
    virtual void forwardPenetrations(void* a, void* b, void* c);
    float unk4;                    // 0x04 zeroed by the constructor
    hkPenetrationTarget* m_target; // 0x08
};

// HYPOTHESIS: symmetric variant of hkBvTreeAgent; the class name is the map name (hkSymmetricAgentLinearCast<hkBvTreeAgent>): each wrapper swaps the
// first two pair arguments and forwards to the hkBvTreeAgent function.
struct hkSymmetricAgentLinearCast_13hkBvTreeAgent : hkBvTreeAgent {
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    virtual void updateShapeCollectionFilter(void* a, void* b, void* c);
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);

    hkSymmetricAgentLinearCast_13hkBvTreeAgent(hkContactMgr* contactMgr) : hkBvTreeAgent(contactMgr) {}
};
