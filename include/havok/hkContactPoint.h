#pragma once

#include <havok/hkVector4.h>

// Contact point record, 0x20 bytes. Position at 0x00, normal at 0x10; the contact distance is the
// w component of the normal vector (0x1C). Accessors from the hkSimpleContactConstraintInfo TU.
struct hkContactPoint {
    hkVector4 m_position; // 0x00
    hkVector4 m_normal;   // 0x10 (w = distance)

    // Empty (out-of-line copy in hkCollideCapsuleUtil.o, a bare blr).
    hkContactPoint() {}

    // 0x802F2ECC (map: hkContactPoint::getPosition1, out of line in hkSimulation.o): hkCollideCapsuleUtilCapsVsTri
    // writes through it, so it is the non-const overload.
    hkVector4& getPosition();
    // 0x802947F0 (map: hkContactPoint::getPosition, hkSimpleContactConstraintInfo.o). HYPOTHESIS: the const overload.
    const hkVector4& getPosition() const;
    const hkVector4& getNormal() const;
    // Returns the normal at 0x10; hkCollideCapsuleUtilCapsVsTri writes through it (non-const).
    hkVector4& getSeparatingNormal();
    hkReal getDistance() const;
    // Out-of-line copy in hkSimpleContactConstraintInfo.o (0x80294804, not written yet): updates props'
    // m_internalDataA from the distance and returns the contact rhs. HYPOTHESIS: the parameter meanings.
    hkReal calculateRhs(struct hkContactPointProperties* props, int* unused, hkReal distance, hkReal deltaTime) const;
    // Added by the hkCollideCapsuleUtil unit: writes the w component of the normal (the contact distance).
    void setDistance(hkReal distance);
};
