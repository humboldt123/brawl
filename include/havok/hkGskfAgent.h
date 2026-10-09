#pragma once

#include <havok/hkGskBaseAgent.h>

struct hkContactMgr;

// One entry of the point list of hkGskfAgent (8 bytes, array starts at 0x34). Key 0xFFFF marks a free entry.
struct hkGskfPoint {
    u8 unk0; // 0x00 cleared by createZombie
    u8 unk1; // 0x01 cleared by createZombie
    u16 key; // 0x02
    int unk4; // 0x04
};

// GSK agent with a point list (object size 0x80). The list header is the word at 0x30; its byte at 0x32 is the entry
// count. Entries start at 0x34. The 0x30 word is zeroed by the constructor (the list is left empty).
// The virtual signatures with a key parameter differ from hkCollisionAgent's no-argument declarations
// (HYPOTHESIS: hkCollisionAgent::removePoint, commitPotential and createZombie take the key as u16).
struct hkGskfAgent : hkGskBaseAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    int unk30; // 0x30 list header (cleared by the constructor)
    u8 unk34[0x80 - 0x34]; // 0x34 point entries (see points()) and the rest of the object

    hkGskfAgent(hkCdBody* bodyA, hkCdBody* bodyB, hkContactMgr* contactMgr);

    u8 pointCount() {
        return ((u8*)this)[0x32];
    }
    hkGskfPoint* points() {
        return (hkGskfPoint*)((u8*)this + 0x34);
    }

    // Allocates a hkGskfAgent (0x80 bytes) when contactMgr is set, otherwise a hkGskBaseAgent (0x30 bytes).
    // HYPOTHESIS: the third parameter is unused.
    static hkGskBaseAgent* createGskfAgent(hkCdBody* bodyA, hkCdBody* bodyB, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void removePoint(u16 key);
    virtual void commitPotential(u16 key);
    virtual void createZombie(u16 key);
};
