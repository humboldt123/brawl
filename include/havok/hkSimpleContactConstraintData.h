#pragma once

#include <havok/hkConstraintData.h>
#include <havok/hkArray.h>
#include <havok/hkMemory.h>
#include <havok/hkSimpleContactConstraintAtom.h>

struct hkConstraintInstance;

// hkSimpleContactConstraintAtomUtil::deallocateAtom (unsplit, 0x802DA2B4). HYPOTHESIS: name.
extern "C" void fn_802DA2B4(hkSimpleContactConstraintAtom* atom);

// Maps contact point ids to point indices in the atom (0xFF = free id). The wrapper adds the third destructor level
// the target's inlined destructor shows (three null-check branches). HYPOTHESIS: name.
struct hkDynamicsCpIdMgr {
    hkInplaceArray<u8, 8> m_values; // 0x00
};

// Constraint data of a contact constraint (0x30 bytes), embedded in hkSimpleConstraintContactMgr at 0x10.
// Layout from its constructor and destructor; vtable lbl_80487B10. The id table maps a contact point id to the
// point's index in the atom (0xFF = free id).
// HYPOTHESIS: the member names; unk20 is never touched by the functions here.
struct hkSimpleContactConstraintData : hkConstraintData {
    HK_DECLARE_REF_ALLOCATOR(0x2a)

    hkDynamicsCpIdMgr m_idMgrA;                // 0x0C
    u8 unk20[0x28 - 0x20];                     // 0x20
    hkConstraintInstance* m_constraint;        // 0x28
    hkSimpleContactConstraintAtom* m_atom;     // 0x2C

    hkSimpleContactConstraintData(hkConstraintInstance* constraint);
    // Weak: the out-of-line copy sits in hkSimpleConstraintContactMgr.o, whose destructor also inlines it.
    virtual ~hkSimpleContactConstraintData() {
        if (m_atom) {
            fn_802DA2B4(m_atom);
        }
    }

    virtual hkBool isValid() const {
        return true;
    }
    virtual int getType() const {
        return 11;
    }
    virtual void getRuntimeInfo(hkBool wantRuntime, hkConstraintRuntimeInfo& infoOut) const;
    virtual hkSolverResults* getSolverResults(hkConstraintRuntime* runtime) const;
    virtual void getConstraintInfo(ConstraintInfo& infoOut) const;           // slot 10
    // HYPOTHESIS: the four opaque arguments (see hkDynamicsContactMgr's toi callbacks).
    virtual void collisionResponseBeginCallback(const hkContactPoint& cp, void* inA, void* velA, void* inB,
                                                void* velB);
    virtual void collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied, void* inA,
                                              void* velA, void* inB, void* velB);

    // Adds a contact point to the atom; returns its id and hands out the point and its properties.
    int allocateContactPoint(hkContactPoint*& cpOut, hkContactPointProperties*& cpPropsOut);
    // Removes the point with this id.
    void freeContactPoint(u16 id);
};
