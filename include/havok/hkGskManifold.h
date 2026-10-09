#pragma once

#include <havok/hkBase.h>

// Object that receives the removal of a key from a GSK manifold (virtual slot 0x18, the same slot as hkContactMgr::unk18).
// HYPOTHESIS: the slot 0x18 argument is the 16-bit key of the entry.
struct hkGskPointRemover : hkReferencedObject {
    virtual void unk10();
    virtual void unk14();
    virtual void removePoint(u16 key); // 0x18
};

// One 8-byte entry of the manifold point list (the list starts at manifold + 0x04).
struct hkGskManifoldEntry {
    u8 unk0;  // 0x00 cleared by the zombie routine
    u8 unk1;  // 0x01 cleared by the zombie routine
    u16 key;  // 0x02 0xFFFF marks a free entry
    u32 unk4; // 0x04
};

// Contact manifold of the GSK agents (map name hkGskManifold), embedded in the agent at 0x0C. Its first three bytes
// are element counts: the sizes of the two 2-byte lists and of the 8-byte list that follow the header.
struct hkGskManifold {
    u8 m_countA; // 0x00 HYPOTHESIS: count of 2-byte entries
    u8 m_countB; // 0x01 HYPOTHESIS: count of 2-byte entries
    u8 m_countC; // 0x02 HYPOTHESIS: count of 8-byte entries
    u8 unk3;     // 0x03 part of the 4-byte header (not identified)

    // Size in bytes of the header plus the lists (header of 4 bytes).
    u32 getTotalSizeInBytes() const;
    hkGskManifoldEntry* getEntries() { return (hkGskManifoldEntry*)((u8*)this + 4); }

    // Clears the manifold through hkGskManifoldUtil cleanup (arg receives the removePoint calls), copies the 8-byte count
    // into dst and zeroes the header (map name hkGskManifold__resetGskManifold).
    static void resetGskManifold(hkGskManifold* manifold, hkGskManifold* dst, void* arg);
};
