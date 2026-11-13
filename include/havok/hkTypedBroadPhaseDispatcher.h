#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>

struct hkBroadPhaseHandlePair;
struct hkCollidableCollidableFilter;

// Turns the pairs the broad phase reports into agents (map names hkTypedBroadPhaseDispatcher::addPairs,
// removePairs, removeDuplicates; the world keeps one at 0x58).
// HYPOTHESIS: the parameter types beyond the call shape. The filter argument is the world's collision filter seen
// through its first interface (the callers pass filter + 8, null-checked).
struct hkTypedBroadPhaseDispatcher {
    void addPairs(hkBroadPhaseHandlePair* pairs, int numPairs, const hkCollidableCollidableFilter* filter);
    void removePairs(hkBroadPhaseHandlePair* pairs, int numPairs);
    // drops the pairs that are in both lists
    static void removeDuplicates(hkArray<hkBroadPhaseHandlePair>& newPairs, hkArray<hkBroadPhaseHandlePair>& delPairs);
};
