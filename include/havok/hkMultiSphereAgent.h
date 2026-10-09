#pragma once

#include <havok/hkCollisionAgent.h>

struct hkCollisionDispatcher;

// One child entry of the multi-sphere agent: a 32-bit key and the child agent (8 bytes per entry).
struct hkMultiSphereAgentEntry {
    u32 unk0; // 0x00 HYPOTHESIS: key of the child pair
    hkCollisionAgent* agent; // 0x04 child agent (cleanup is called on it)
};

// Multi-sphere agent: a list of child agents. Object size 0x38.
struct hkMultiSphereAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkMultiSphereAgentEntry* m_entries; // 0x0C
    int m_count;                        // 0x10 number of valid entries
    u8 unk14[0x38 - 0x14];              // 0x14 not identified yet

    // The constructor (fn_802BAEB8) is not recovered yet.
    hkMultiSphereAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    virtual ~hkMultiSphereAgent() {}

    static hkMultiSphereAgent* createListAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    static hkMultiSphereAgent* createListBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
};

// Flipped (symmetric) variant created by createListBAgent; the class name follows the Havok TU map.
struct hkSymmetricAgent_18hkMultiSphereAgent_ : hkMultiSphereAgent {
    hkSymmetricAgent_18hkMultiSphereAgent_(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr)
        : hkMultiSphereAgent(unk0, unk1, unk2, contactMgr) {}
};
