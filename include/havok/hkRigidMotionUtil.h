#pragma once

#include <havok/hkBase.h>

// Rigid motion helpers from the hkRigidMotionUtil TU. Parameter types are HYPOTHESIS: the callers pass the
// solver object, a motion or entity pointer, a pointer to a small record (its first word is the motion
// pointer), a count-like word, a stride-like word and the motion pointer again.
struct hkRigidMotionUtil {
    static void hkRigidMotionUtilApplyAccumulators(void* self, void* p1, void* p2, u32 p3, u32 p4, void* p5);
};
