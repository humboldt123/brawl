#pragma once

#include <havok/hkMotionState.h>

// Helpers that act on a body's hkMotionState (swept transform stored inside it).
// Signatures come from the TU's asm; names are the TU map names.
struct hkSweptTransformUtil {
    static void setTimeInformation(hkMotionState& ms, hkReal t0, hkReal t1);

    // Declared only because warpToRotation calls it; the definition is not decompiled yet.
    static void warpTo();

    // Tail-calls warpTo (HYPOTHESIS: the second argument is a transform whose translation is passed on).
    static void warpToRotation(hkMotionState& ms, const hkMotionState& src);
};
