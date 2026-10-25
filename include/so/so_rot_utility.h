#pragma once

#include <mt/mt_vector.h>

class soRotUtility {
public:
    // Wrap each Euler component into the native rotation interval.
    static Vec3f clampDeg(const Vec3f& rotation);
    // HYPOTHESIS: wraps an angle in radians into the native interval (named by the sora_melee map).
    static float clampRad(float rotation);
};
