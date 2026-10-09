#pragma once

#include <havok/hkConstraintAtom.h>
#include <havok/hkVector4.h>

// Linear soft constraint atom. Index byte at 0x02 selects a 16-byte entry in the caller's table; the two floats at
// 0x04/0x08 are soft parameters copied into the scratch block. HYPOTHESIS: field names not yet identified.
struct hkLinSoftConstraintAtom : hkConstraintAtom {
    u8 m_index;   // 0x02
    float m_unk04; // 0x04
    float m_unk08; // 0x08

    void buildJacobianFromLinSoftAtom(void* a, const hkVector4* b, const hkVector4* table, void* d);
};
