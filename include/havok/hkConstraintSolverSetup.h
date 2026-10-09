#pragma once

#include <havok/hkBase.h>

// Constraint solver setup helpers (hkConstraintSolverSetup TU). Only the members implemented so far.
struct hkEntity;

struct hkConstraintSolverSetup {
    void integrate(void* a, void* b, void* c);
    void oneStepIntegrate(hkEntity** entities, int count, u8* base);
    void shutdownSolver();
};
