// Havok translation unit hk_lin_soft_constraint_atom.o (main.dol 0x8028A7A8-0x8028A860).
// Functions in address order (method names from the Havok TU map):
//   0x8028A7A8   184  buildJacobianFromLinSoftAtom   [map: hkLinSoftConstraintAtom__buildJacobianFromLinSoftAtom]
#include <havok/hkLinSoftConstraintAtom.h>

// Helper in another TU (not yet named): takes the scratch block, the caller's context and a pass-through pointer.
extern "C" void fn_8028E8E8(void* scratch, void* ctx, void* pass);

struct hkLinSoftAtomScratch {
    hkVector4 m_a; // 0x00
    hkVector4 m_b; // 0x10
    hkVector4 m_c; // 0x20
    float m_d;     // 0x30
    float m_e;     // 0x34
};

void hkLinSoftConstraintAtom::buildJacobianFromLinSoftAtom(void* a, const hkVector4* b, const hkVector4* table, void* d) {
    hkLinSoftAtomScratch s;
    s.m_a = b[3];
    s.m_b = table[3];
    s.m_c = table[m_index];
    s.m_d = m_unk04;
    s.m_e = m_unk08;
    fn_8028E8E8(&s, a, d);
}
