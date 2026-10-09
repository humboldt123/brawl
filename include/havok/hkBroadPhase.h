#pragma once

#include <havok/hkBase.h>
#include <havok/hkCollidable.h>

// Broad-phase base class. Only the reference-counted base is recovered so far.
struct hkBroadPhase : hkReferencedObject {
};

// Pair of typed broad-phase handles reported by the broad phase (8 bytes).
// HYPOTHESIS: same layout as the pair used by hkPhantom.cpp (which keeps its own copy).
struct hkBroadPhaseHandlePair {
    hkTypedBroadPhaseHandle* m_a; // 0x00
    hkTypedBroadPhaseHandle* m_b; // 0x04
};
