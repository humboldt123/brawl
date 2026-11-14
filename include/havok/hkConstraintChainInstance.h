#pragma once

#include <havok/hkConstraintInstance.h>
#include <havok/hkConstraintData.h>
#include <havok/hkArray.h>

struct hkAction;

// Constraint data shared by the chain instances. HYPOTHESIS: slot 0x28 (the derived chain data classes declare
// their getConstraintInfo non-virtually for now).
struct hkConstraintChainData : hkConstraintData {
    virtual void unk28();
    virtual int getNumConstraintInfos(); // 0x2C (hkWorld::addConstraint calls it on chains)
};

// A constraint instance over a chain of entities; its action keeps the chain simulated together.
struct hkConstraintChainInstance : hkConstraintInstance {
    hkArray<hkEntity*> m_chainedEntities; // 0x2C
    hkAction* m_action;                   // 0x38 (hkConstraintChainInstanceAction)

    hkConstraintChainData* getData() const { return (hkConstraintChainData*)m_data; }
};

// Wraps another constraint data and can break it. HYPOTHESIS: the layout before 0x18 (bridge atoms) and the names;
// hkWorld::addConstraint fills the child runtime size and solver result count of type 12 data on its first add.
struct hkBreakableConstraintData : hkConstraintData {
    u8 m_atoms[0x0C];                  // 0x0C
    hkConstraintData* m_constraintData; // 0x18
    u16 m_childRuntimeSize;             // 0x1C
    u16 m_childNumSolverResults;        // 0x1E
};
