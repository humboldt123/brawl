#pragma once

#include <havok/hkBase.h>
#include <havok/hkContactPoint.h>
#include <havok/hkContactPointMaterial.h>

// Simple contact constraint atom. Contact points start at the first 16-byte aligned address after 0x37;
// the material records follow them, 0x20 bytes each.
struct hkSimpleContactConstraintAtom {
    u8 unk00[0x06];
    u16 m_numContactPoints; // 0x06 HYPOTHESIS: count of contact points
    u8 unk08[0x37 - 0x08];

    hkContactPoint* getContactPoints();
    hkContactPointMaterial* getContactPointProperties();
};
