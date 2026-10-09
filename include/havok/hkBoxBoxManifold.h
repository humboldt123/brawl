#pragma once

#include <havok/hkBase.h>

// Contact manifold of the box-box collision detector (plain value type, not reference counted).
// Layout from hkBoxBoxManifold__addPoint / removePoint / ctor (asm).
struct hkBoxBoxManifold {
    // One manifold point key: two feature bytes and a 16-bit id (4 bytes).
    struct Point {
        u8 unk0; // 0x00 feature index A, compared by addPoint
        u8 unk1; // 0x01 feature index B, compared by addPoint
        u16 unk2; // 0x02 id, copied but not compared
    };

    Point m_points[8];  // 0x00, 8 entries of 4 bytes
    u8 unk20;           // 0x20 HYPOTHESIS: zeroed by the constructor, never read in addPoint/removePoint
    u8 m_numPoints;     // 0x21 number of valid entries in m_points
    hkBool m_complete;  // 0x22 completeness flag

    hkBoxBoxManifold();

    int getNumPoints() const;
    void setComplete(const hkBool& complete);
    hkBool isComplete() const;

    // Adds a point unless a point with the same feature bytes exists. Returns the new index, or -1.
    // HYPOTHESIS: the two leading parameters are unused by the original code (they are passed but never read).
    int addPoint(int unk4, int unk8, const Point* key);
    void removePoint(int index);
};
