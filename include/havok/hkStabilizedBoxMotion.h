#pragma once

#include <havok/hkBoxMotion.h>

// Stabilized box-inertia motion (type 5). Same vtable as hkBoxMotion except the dtor.
struct hkStabilizedBoxMotion : hkBoxMotion {
    hkStabilizedBoxMotion(const hkVector4& position, const hkQuaternion& rotation);
    hkStabilizedBoxMotion(hkFinishLoadedObjectFlag f) : hkBoxMotion(f) {}

    // Loaded-object support (static init registration; see src/havok/hkStabilizedBoxMotion.cpp).
    static void finishLoadedObjecthkStabilizedBoxMotion(void* p);
    static void cleanupLoadedObjecthkStabilizedBoxMotion(void* p);
    static const void* getVtablehkStabilizedBoxMotion() __attribute__((never_inline));
};
