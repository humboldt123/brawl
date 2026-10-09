#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector8.h>
#include <havok/hkVelocityAccumulator.h>

typedef float hkReal;

// Constraint solver. Most members are empty in the original (the stage hooks are no-ops here).
struct hkSolver {
    void hkSolveUpload();
    void loadVelocityAccumulators();
    void popVelocityAccumulators();
    void storeVelocityAccumulators();
    void prefetchVelocityAccumulators();
    void storeDelayedResult();
    void loadFixedRegisters();
    void applyAngularImpulse(hkReal impulse, hkVector8* jac, hkVelocityAccumulator* a, hkVelocityAccumulator* b, hkReal* sum);
};

// Solver info; the debug accumulator printer is empty in the original.
struct hkSolverInfo {
    void hkDebugPrintfAccumulators();
};
