#pragma once

#include <havok/hkConstraintAtom.h>

// Constraint atom that sets the local translations of a constraint's bodies. Layout not recovered yet
// (the atom header is shared, see hkConstraintAtom.h).
struct hkSetLocalTranslationsConstraintAtom : hkConstraintAtom {
    int numSolverResults() const;
};
