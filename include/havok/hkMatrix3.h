#pragma once

#include <havok/hkBase.h>
#include <StaticAssert.h>

struct hkRotation;

// Local declaration for the math class absent from the BrawlHeaders submodule.
// Native storage is three columns of four floats. The fourth lanes are retained
// by scalar/add operations and cleared by matrix products and setTranspose.
// HYPOTHESIS: 16-byte type alignment follows the aligned native matrix temporaries.
struct __attribute__((aligned(16))) hkMatrix3 {
    float elements[12];

    void transpose();
    void setTranspose(const hkMatrix3& other);
    void setMul(const hkMatrix3& lhs, const hkMatrix3& rhs);
    void setMul(float scalar, const hkMatrix3& other);
    void setMulInverse(const hkMatrix3& lhs, const hkMatrix3& rhs);
    hkBool isApproximatelyEqual(float epsilon, const hkMatrix3& other) const;
    hkResult invert(float epsilon);
    void invertSymmetric();
    void add(const hkMatrix3& other);
    void mul(float scalar);
    // HYPOTHESIS: callers establish the basis storage, but not the source type name.
    void changeBasis(const hkRotation& basis);
};

// HYPOTHESIS: hkRotation specializes the same matrix storage without new fields.
struct hkRotation : hkMatrix3 {};

static_assert(sizeof(hkMatrix3) == 0x30, "hkMatrix3 size");

void hkMatrix3_setMulMat3Mat3(hkMatrix3* dst, const hkMatrix3* lhs, const hkMatrix3* rhs);
void hkMatrix3_invertSymmetric(hkMatrix3* matrix);
