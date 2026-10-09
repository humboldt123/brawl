#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>

struct hkCollisionDispatcher;

// One child entry of the multi-sphere agent: a 32-bit key and the child agent (8 bytes per entry).
struct hkMultiSphereAgentEntry {
    u32 unk0; // 0x00 HYPOTHESIS: key of the child pair
    hkCollisionAgent* agent; // 0x04 child agent (cleanup is called on it)
};

// Multi-sphere agent: a list of child agents. Object size 0x38.
struct hkMultiSphereAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkArray<hkMultiSphereAgentEntry> m_entries; // 0x0C (data, count in m_size, capacity and flags)
    u8 unk18[0x38 - 0x18];                      // 0x18 not identified yet

    // The constructor (fn_802BAEB8) is not recovered yet.
    hkMultiSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    static void* operator new(unsigned long, void* p) { return p; }
    virtual ~hkMultiSphereAgent() {}

    // Registers the two list agents (multi-sphere against all shape types, both orders).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static hkMultiSphereAgent* createListAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    static hkMultiSphereAgent* createListBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
};

// Flipped (symmetric) variant created by createListBAgent; the class name follows the Havok TU map.
struct hkSymmetricAgent_18hkMultiSphereAgent_ : hkMultiSphereAgent {
    hkSymmetricAgent_18hkMultiSphereAgent_(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr)
        : hkMultiSphereAgent(unk0, unk1, unk2, contactMgr) {}
};
