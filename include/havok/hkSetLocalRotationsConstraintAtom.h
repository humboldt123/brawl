#pragma once

#include <havok/hkConstraintAtom.h>

// Constraint atom that sets the local rotations of a constraint's bodies. Layout not recovered yet
// (the atom header is shared, see hkConstraintAtom.h).
struct hkSetLocalRotationsConstraintAtom : hkConstraintAtom {
    int numSolverResults() const;
};
