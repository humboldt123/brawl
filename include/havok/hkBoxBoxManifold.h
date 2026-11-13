#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Feature pair of one manifold point (4 bytes): the feature of each box and the contact point id. The constructor
// only clears the id (out-of-line copy in hkBoxBoxCollisionDetection.o). HYPOTHESIS: the member names; same layout as
// hkBoxBoxManifold::Point.
struct hkFeatureContactPoint {
    u8 m_featureIdA;      // 0x00
    u8 m_featureIdB;      // 0x01
    u16 m_contactPointId; // 0x02

    hkFeatureContactPoint() { m_contactPointId = 0; }
};

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
    // 0x20 zeroed by the constructor; findAdditionalManifoldPoints marks the manifold complete once it reaches 4.
    // HYPOTHESIS: a count of face-vertex points.
    u8 unk20;
    u8 m_numPoints;     // 0x21 number of valid entries in m_points
    hkBool m_complete;  // 0x22 completeness flag
    // 0x23 when set, calcManifold runs checkManifoldNormalConsistency after updating m_manifoldNormal.
    // HYPOTHESIS: the name.
    hkBool m_manifoldNormalValid;
    u8 unk24[0x30 - 0x24];      // 0x24 not identified
    hkVector4 m_manifoldNormal; // 0x30 written by hkBoxBoxCollisionDetection::calcManifoldNormal
    // 0x40 the manifold normal rotated into B's frame when it was accepted (queryManifoldNormalConsistency).
    // HYPOTHESIS: the name.
    hkVector4 m_manifoldNormalB;

    hkBoxBoxManifold();

    int getNumPoints() const;
    void setComplete(const hkBool& complete);
    hkBool isComplete() const;

    // Adds a point unless a point with the same feature bytes exists. Returns the new index, or -1. key is not
    // const: the copy into m_points keeps its loads and stores in source order (they may alias).
    // HYPOTHESIS: the two leading parameters are unused by the original code (they are passed but never read).
    int addPoint(int unk4, int unk8, Point* key);
    void removePoint(int index);
};
