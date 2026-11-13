#pragma once

#include <havok/hkBase.h>
#include <havok/hkString.h>

struct hkConstraintInfo;
struct hkConstraintInstance;
struct hkConstraintAtom;
struct hkConstraintRuntime;
struct hkSolverResults;

// Runtime info record written by getRuntimeInfo (two words; the ragdoll writes 0x70 and 0xC).
struct hkConstraintRuntimeInfo {
    u32 unk00;
    u32 unk04;
};

// Solver size summary of a constraint (0x10 bytes). Layout and names from main.dol's hkConstraintInfoClass;
// the modifier atoms add their schema and Jacobian sizes to it (hkConstraintAtom.cpp).
struct hkConstraintInfo {
    int m_maxSizeOfJacobians; // 0x00
    int m_sizeOfJacobians;    // 0x04
    int m_sizeOfSchemas;      // 0x08
    int m_numSolverResults;   // 0x0C
};

// getConstraintInfo fills two more words after the reflected summary: the first atom of the constraint and the
// byte size of its atoms (the two values getConstraintInfoUtil takes).
// HYPOTHESIS: a separate record type; the getConstraintInfo symbols take an hkConstraintInfo*.
struct hkConstraintInfoWithAtoms : hkConstraintInfo {
    void* unk10; // 0x10 first atom
    u32 unk14;   // 0x14 size of the atoms in bytes
};

// Base constraint description. Reflection layout: hkConstraintDataClass.cpp (size 0xC, userData at 0x8).
// Virtual slot numbers follow the target vtable: slot 2 dtor, slot 3 calcStatistics (hkReferencedObject),
// slot 4 isValid and slot 5 getType (the derived data vtables), slots 6..8 getRuntimeInfo / getSolverResults /
// addInstance (hkSimpleContactConstraintData overrides 6 and 7; the base 7 and 8 are weak copies in its unit), slot 9
// is buildJacobian (checked against the bridge atom call thunk). Slot 10 is getConstraintInfo (declared by the
// derived classes for now: their non-virtual getConstraintInfo symbols use other signatures).
// HYPOTHESIS: the parameter types of slots 6..8.
struct hkConstraintData : hkReferencedObject {
    u32 m_userData; // 0x08

    hkConstraintData() : m_userData(0) {}
    hkConstraintData(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: the base ctors set refcount
    virtual ~hkConstraintData() {} // empty in the target (no base dtor call in the derived dtors)

    virtual hkBool isValid() const;
    virtual int getType() const; // 11 = the contact constraint (hkConstrainedSystemFilter skips those)
    virtual void getRuntimeInfo(hkBool wantRuntime, hkConstraintRuntimeInfo& infoOut) const;
    virtual hkSolverResults* getSolverResults(hkConstraintRuntime* runtime) const {
        return (hkSolverResults*)runtime;
    }
    virtual void addInstance(hkConstraintInstance* constraint, hkConstraintRuntime* runtime, int sizeOfRuntime) const {
        if (runtime) {
            hkString::memSet(runtime, 0, sizeOfRuntime);
        }
    }
    virtual void buildJacobian();

    // getConstraintInfo record: the summary plus the atoms it was computed from.
    struct ConstraintInfo : hkConstraintInfo {
        const hkConstraintAtom* m_atoms; // 0x10
        u32 m_sizeOfAllAtoms;            // 0x14
    };
    // Sums the solver sizes of a constraint's atoms (weak copy in hkSimpleContactConstraintData.o).
    static void getConstraintInfoUtil(const hkConstraintAtom* atoms, int sizeOfAllAtoms, ConstraintInfo& infoOut);
};
