#pragma once

#include <havok/hkVector4.h>
#include <nw4r/math/math_types.h>

// HYPOTHESIS: phMath (ph_math.o) is a class of static helpers; parameter types come from the callee's register
// use and the callers' argument set-up (the map gives none).
class phMath {
public:
    static f32 getLength(const hkVector4& a, const hkVector4& b);
    static f32 getAngleForNW2Vector(nw4r::math::VEC3* a, nw4r::math::VEC3* b);
    static f32 getAngleForNW2VectorVerAcos(nw4r::math::VEC3* a, nw4r::math::VEC3* b);
    static void getNwMtxForQuat(nw4r::math::MTX34* out, nw4r::math::VEC3* axis, const f32* angle);
    static void getNWMatrixSeparation(const nw4r::math::MTX34* src, nw4r::math::MTX34* trans,
                                      nw4r::math::MTX34* rot);
};
