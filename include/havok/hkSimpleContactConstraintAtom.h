#pragma once

#include <havok/hkBase.h>
#include <havok/hkContactPoint.h>
#include <havok/hkContactPointMaterial.h>

// Contact data kept in the atom after the counts (read and updated by hkSimpleContactConstraintDataBuildJacobian).
// HYPOTHESIS: the member names; the offsets are asm-verified.
struct hkSimpleContactConstraintDataInfo {
    u16 m_flags;           // 0x08 bit 2 (value 4): the angular friction part is enabled
    u16 m_index;           // 0x0A axis index (0..2) used to build the friction directions
    hkReal m_data0C;       // 0x0C first word of the friction solver data (its address goes to init2dFriction)
    hkReal m_frictionRhs0; // 0x10 rhs of the first friction direction
    hkReal m_data14;       // 0x14
    hkReal m_frictionRhs1; // 0x18 rhs of the second friction direction
    hkReal m_data1C;       // 0x1C
    hkReal m_angularRhs;   // 0x20 rhs of the angular friction direction
    hkReal m_contactRadius; // 0x24 mean distance of the contact points from their centre
};

// Simple contact constraint atom. Contact points start at the first 16-byte aligned address after 0x37;
// the property records (0x14 bytes each) follow the reserved contact points (0x20 bytes each).
struct hkSimpleContactConstraintAtom {
    u16 m_type;                     // 0x00 (hkConstraintAtom::TYPE_CONTACT)
    u16 m_sizeOfAllAtoms;           // 0x02 byte size of the atom with its points and properties
    u16 m_numContactPoints;         // 0x04
    u16 m_numReservedContactPoints; // 0x06
    hkSimpleContactConstraintDataInfo m_info; // 0x08
    u8 unk28[0x37 - 0x28];

    // Weak copies in hkSimpleContactConstraintInfo.o (its dont_inline region); inlined everywhere else.
    hkContactPoint* getContactPoints() {
        return (hkContactPoint*)(((u32)this + 0x37) & ~0xF);
    }
    hkContactPointProperties* getContactPointProperties() {
        return (hkContactPointProperties*)((u8*)getContactPoints() + (m_numReservedContactPoints << 5));
    }
};
