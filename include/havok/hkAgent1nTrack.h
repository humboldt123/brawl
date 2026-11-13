#pragma once

#include <havok/hkArray.h>
#include <havok/hkMemory.h>
#include <havok/hkTransform.h>

struct hkCdBody;
struct hkContactMgr;
struct hkProcessCollisionInput;
struct hkShapeCollection;

// Track of the child agents of a 1-n agent (0x10 bytes), managed by the hkAgent1nMachine functions: a directory of
// agent sectors with room for one entry in place. The deleting destructor frees 0x10 bytes in class 0x25.
// HYPOTHESIS: the SectorDirEntry word is the sector pointer (map name hkArray<hkAgent1nTrack::SectorDirEntry>).
struct hkAgent1nTrack {
    HK_DECLARE_SIZED_DELETE(0x25)

    struct SectorDirEntry {
        void* m_sector;
    };
    hkInplaceArray<SectorDirEntry, 1> m_sectors; // 0x00

    ~hkAgent1nTrack() {}
    static void* operator new(unsigned long, void* where) { return where; }
};

// Pointer plus a byte offset (Havok's hkAddByteOffset; its out-of-line hkAddByteOffset<void> copy is in
// hkPredGskAgent3.o). The agent3 functions return the end of their agent data with it.
template <class T>
T* hkAddByteOffset(T* base, int offset) {
    return (T*)((char*)base + offset);
}

// Header of one agent in an agent track sector. The cylinder agent keeps its flags in the 0x04 word: "body is a
// cylinder" (1 for A, 2 for B) and "the cylinder runs as a capsule" (4 for A, 8 for B).
// HYPOTHESIS: m_numContactPoints (the agent3 process functions copy the manifold's point count there).
struct hkAgentEntry {
    u8 unk0;               // 0x00
    u8 unk1;               // 0x01
    u8 m_numContactPoints; // 0x02
    u8 unk3;               // 0x03
    u32 unk4;              // 0x04
};

// Input of the agent3 (stream) functions: the bodies, the collision input, the contact manager and A's transform in
// B's space.
struct hkAgent3Input {
    const hkCdBody* m_bodyA;                 // 0x00
    const hkCdBody* m_bodyB;                 // 0x04
    const hkProcessCollisionInput* m_input;  // 0x08
    hkContactMgr* m_contactMgr;              // 0x0C
    hkTransform m_aTb;                       // 0x10
};

// The process input adds the linear time-of-impact info of the step (0x70 bytes).
// HYPOTHESIS: m_distAtT1 (not written by hkBvTreeStreamAgent::processCollision; the cylinder agent compares it with
// the separation limits and stores it as the separating plane's distance).
struct hkAgent3ProcessInput : hkAgent3Input {
    hkReal m_distAtT1;         // 0x50
    u8 unk54[0xC];             // 0x54
    hkVector4 m_linearTimInfo; // 0x60

    hkAgent3ProcessInput() {}
};

// HYPOTHESIS: hkAgent1nMachine_Process (map name): runs the track's agents on the sorted hit keys (all kept agents
// when hitList is null) and adds/removes agents for keys that appeared/disappeared.
extern "C" void fn_802FCCF0(hkAgent1nTrack* track, hkAgent3ProcessInput* input, const hkShapeCollection* collection,
                            const u32* hitList, int numHits, void* output);
