#pragma once

#include <havok/hkBase.h>

struct hkConstraintInfo;

// Runtime info record written by getRuntimeInfo (two words; the ragdoll writes 0x70 and 0xC).
struct hkConstraintRuntimeInfo {
    u32 unk00;
    u32 unk04;
};

// Constraint info block filled by getConstraintInfo (six words; unk10 is a pointer, unk14 its size).
// HYPOTHESIS: unk04 and unk08 are size/offset counters advanced per modifier (hkConstraintAtom.cpp).
struct hkConstraintInfo {
    u32 unk00;
    u32 unk04;
    u32 unk08;
    u32 unk0C;
    void* unk10;
    u32 unk14;
};

// Base constraint description. Reflection layout: hkConstraintDataClass.cpp (size 0xC, userData at 0x8).
// Virtual slot numbers follow the target vtable: slot 2 dtor, slot 3 calcStatistics (hkReferencedObject),
// slots 4..8 are not yet identified, slot 9 is buildJacobian (checked against the bridge atom call thunk).
struct hkConstraintData : hkReferencedObject {
    u32 m_userData; // 0x08

    hkConstraintData();
    hkConstraintData(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: the base ctors set refcount
    virtual ~hkConstraintData() {} // empty in the target (no base dtor call in the derived dtors)

    virtual void unk04();
    virtual void unk05();
    virtual void unk06();
    virtual void unk07();
    virtual void unk08();
    virtual void buildJacobian();
};
