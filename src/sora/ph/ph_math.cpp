#include <ph/ph_math.h>

#include <math.h>
#include <mt/mt_common.h>
#include <nw4r/math/math_triangular.h>

f32 phMath::getAngleForNW2Vector(nw4r::math::VEC3* a, nw4r::math::VEC3* b) {
    nw4r::math::VEC3Normalize(a, a);
    nw4r::math::VEC3Normalize(b, b);
    f32 dot = nw4r::math::VEC3Dot(a, b);
    if ((f32)fabs(dot) < 1.0f) {
        return nw4r::math::Atan2Rad(mtSqrtf(1.0f - dot * dot), dot);
    }
    if (dot < 0.0f) {
        return 3.1415927f;
    }
    return 0.0f;
}

f32 phMath::getAngleForNW2VectorVerAcos(nw4r::math::VEC3* a, nw4r::math::VEC3* b) {
    nw4r::math::VEC3Normalize(a, a);
    nw4r::math::VEC3Normalize(b, b);
    f32 dot = nw4r::math::VEC3Dot(a, b);
    if (dot < 1.0f) {
        return acos(dot);
    }
    return 0.0f;
}

void phMath::getNWMatrixSeparation(const nw4r::math::MTX34* src, nw4r::math::MTX34* trans,
                                   nw4r::math::MTX34* rot) {
    PSMTXIdentity(*trans);
    *rot = *src;
    trans->_03 = src->_03;
    trans->_13 = src->_13;
    trans->_23 = src->_23;
    rot->_03 = 0.0f;
    rot->_13 = 0.0f;
    rot->_23 = 0.0f;
}
