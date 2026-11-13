#pragma once

#include <havok/hkBase.h>

// Per-contact material properties, stored right after the contact point array.
struct hkContactPointMaterial {
    u8 unk00[0x04];
    u16 m_friction; // 0x04 (8.8 fixed point friction)
    u8 m_restitution; // 0x06 HYPOTHESIS: name
    u8 m_flags;       // 0x07 bit 1 (value 2): the point is solved together with the previous one (pair contact)

    int getFriction8_8();
};

// Per contact point solver properties (0x14 bytes; hkSimpleContactConstraintDataBuildJacobian walks them in
// step with the contact points). HYPOTHESIS: the names of the words around the material.
// The material is a base (hkSimpleConstraintContactMgr::removeContactPoint converts with a null check, +8).
struct hkSolverResults {
    hkReal m_impulseApplied;      // 0x00
    hkReal m_internalSolverData;  // 0x04
};

struct hkContactPointProperties : hkSolverResults, hkContactPointMaterial {
    hkReal m_internalDataA;       // 0x10
};
