#pragma once

#include <havok/hkBoxMotion.h>

// Thin-box inertia motion (type 8). Same vtable as hkBoxMotion except the dtor.
struct hkThinBoxMotion : hkBoxMotion {
    hkThinBoxMotion(const hkVector4& position, const hkQuaternion& rotation);
    hkThinBoxMotion(hkFinishLoadedObjectFlag f) : hkBoxMotion(f) {}

    // Loaded-object support (static init registration; see src/havok/hkThinBoxMotion.cpp).
    static void finishLoadedObjecthkThinBoxMotion(void* p);
    static void cleanupLoadedObjecthkThinBoxMotion(void* p);
    static const void* getVtablehkThinBoxMotion() __attribute__((never_inline));
};
