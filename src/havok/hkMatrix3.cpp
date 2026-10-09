#include <havok/hkMatrix3.h>

#include <math.h>

void hkMatrix3::transpose() {
    float* m = elements;
    float value9;
    float value8;
    float value4;

    value4 = m[4];
    m[4] = m[1];
    m[1] = value4;

    value8 = m[8];
    m[8] = m[2];
    m[2] = value8;

    value9 = m[9];
    m[9] = m[6];
    m[6] = value9;
}

void hkMatrix3_setMulMat3Mat3(hkMatrix3* dst, const hkMatrix3* lhs, const hkMatrix3* rhs) {
    const float* a = lhs->elements;
    const float* b = rhs->elements;
    float* out = dst->elements;

    const float a0 = a[0];
    const float a1 = a[1];
    const float a2 = a[2];
    const float a4 = a[4];
    const float a5 = a[5];
    const float a6 = a[6];
    const float a8 = a[8];
    const float a9 = a[9];
    const float a10 = a[10];

    const float b0 = b[0];
    const float b1 = b[1];
    const float b2 = b[2];
    const float b4 = b[4];
    const float b5 = b[5];
    const float b6 = b[6];
    const float b8 = b[8];
    const float b9 = b[9];
    const float b10 = b[10];

    float sum0 = b1 * a4;
    sum0 = b0 * a0 + sum0;
    const float r0 = b2 * a8 + sum0;
    float sum1 = b1 * a5;
    sum1 = b0 * a1 + sum1;
    const float r1 = b2 * a9 + sum1;
    float sum2 = b1 * a6;
    sum2 = b0 * a2 + sum2;
    const float r2 = b2 * a10 + sum2;
    float sum4 = b5 * a4;
    sum4 = b4 * a0 + sum4;
    const float r4 = b6 * a8 + sum4;
    float sum5 = b5 * a5;
    sum5 = b4 * a1 + sum5;
    const float r5 = b6 * a9 + sum5;
    float sum6 = b5 * a6;
    sum6 = b4 * a2 + sum6;
    const float r6 = b6 * a10 + sum6;
    float sum8 = b9 * a4;
    sum8 = b8 * a0 + sum8;
    const float r8 = b10 * a8 + sum8;
    float sum9 = b9 * a5;
    sum9 = b8 * a1 + sum9;
    const float r9 = b10 * a9 + sum9;
    float sum10 = b9 * a6;
    sum10 = b8 * a2 + sum10;
    const float r10 = b10 * a10 + sum10;

    out[0] = r0;
    out[1] = r1;
    out[2] = r2;
    out[3] = 0.0f;
    out[4] = r4;
    out[5] = r5;
    out[6] = r6;
    out[7] = 0.0f;
    out[8] = r8;
    out[9] = r9;
    out[10] = r10;
    out[11] = 0.0f;
}

void hkMatrix3_invertSymmetric(hkMatrix3* matrix) {
    matrix->invertSymmetric();
}

void hkMatrix3::setTranspose(const hkMatrix3& other) {
    const float* src = other.elements;
    float* dst = elements;

    const float m0 = src[0];
    const float m1 = src[1];
    const float m2 = src[2];
    const float m4 = src[4];
    const float m5 = src[5];
    const float m6 = src[6];
    const float m8 = src[8];
    const float m9 = src[9];
    const float m10 = src[10];

    dst[0] = m0;
    dst[1] = m4;
    dst[2] = m8;
    dst[3] = 0.0f;
    dst[4] = m1;
    dst[5] = m5;
    dst[6] = m9;
    dst[7] = 0.0f;
    dst[8] = m2;
    dst[9] = m6;
    dst[10] = m10;
    dst[11] = 0.0f;
}

void hkMatrix3::setMul(const hkMatrix3& lhs, const hkMatrix3& rhs) {
    const float* a = lhs.elements;
    const float* b = rhs.elements;
    float* out = elements;

    const float a0 = a[0];
    const float a1 = a[1];
    const float a2 = a[2];
    const float a4 = a[4];
    const float a5 = a[5];
    const float a6 = a[6];
    const float a8 = a[8];
    const float a9 = a[9];
    const float a10 = a[10];

    const float b0 = b[0];
    const float b1 = b[1];
    const float b2 = b[2];
    const float b4 = b[4];
    const float b5 = b[5];
    const float b6 = b[6];
    const float b8 = b[8];
    const float b9 = b[9];
    const float b10 = b[10];

    float sum0 = b1 * a4;
    sum0 = b0 * a0 + sum0;
    const float r0 = b2 * a8 + sum0;
    float sum1 = b1 * a5;
    sum1 = b0 * a1 + sum1;
    const float r1 = b2 * a9 + sum1;
    float sum2 = b1 * a6;
    sum2 = b0 * a2 + sum2;
    const float r2 = b2 * a10 + sum2;
    float sum4 = b5 * a4;
    sum4 = b4 * a0 + sum4;
    const float r4 = b6 * a8 + sum4;
    float sum5 = b5 * a5;
    sum5 = b4 * a1 + sum5;
    const float r5 = b6 * a9 + sum5;
    float sum6 = b5 * a6;
    sum6 = b4 * a2 + sum6;
    const float r6 = b6 * a10 + sum6;
    float sum8 = b9 * a4;
    sum8 = b8 * a0 + sum8;
    const float r8 = b10 * a8 + sum8;
    float sum9 = b9 * a5;
    sum9 = b8 * a1 + sum9;
    const float r9 = b10 * a9 + sum9;
    float sum10 = b9 * a6;
    sum10 = b8 * a2 + sum10;
    const float r10 = b10 * a10 + sum10;

    out[0] = r0;
    out[1] = r1;
    out[2] = r2;
    out[3] = 0.0f;
    out[4] = r4;
    out[5] = r5;
    out[6] = r6;
    out[7] = 0.0f;
    out[8] = r8;
    out[9] = r9;
    out[10] = r10;
    out[11] = 0.0f;
}

void hkMatrix3::setMul(float scalar, const hkMatrix3& other) {
    const float* src = other.elements;
    float* dst = elements;

    const float m0 = src[0];
    const float m1 = src[1];
    const float m2 = src[2];
    const float m3 = src[3];
    const float m4 = src[4];
    const float m5 = src[5];
    const float m6 = src[6];
    const float m7 = src[7];
    const float m8 = src[8];
    const float m9 = src[9];
    const float m10 = src[10];
    const float m11 = src[11];

    dst[0] = scalar * m0;
    dst[1] = scalar * m1;
    dst[2] = scalar * m2;
    dst[3] = scalar * m3;
    dst[4] = scalar * m4;
    dst[5] = scalar * m5;
    dst[6] = scalar * m6;
    dst[7] = scalar * m7;
    dst[8] = scalar * m8;
    dst[9] = scalar * m9;
    dst[10] = scalar * m10;
    dst[11] = scalar * m11;
}

void hkMatrix3::setMulInverse(const hkMatrix3& lhs, const hkMatrix3& rhs) {
    hkMatrix3 transposed;
    transposed.setTranspose(rhs);
    setMul(lhs, transposed);
}

hkBool hkMatrix3::isApproximatelyEqual(float epsilon, const hkMatrix3& other) const {
    const float* lhs = elements;
    const float* rhs = other.elements;

    for (int i = 0; i < 12; ++i) {
        if (epsilon < (float)fabs(lhs[i] - rhs[i])) {
            return hkBool(false);
        }
    }
    return hkBool(true);
}

hkResult hkMatrix3::invert(float epsilon) {
    float* m = elements;

    const float m0 = m[0];
    const float m1 = m[1];
    const float m2 = m[2];
    const float m4 = m[4];
    const float m5 = m[5];
    const float m6 = m[6];
    const float m8 = m[8];
    const float m9 = m[9];
    const float m10 = m[10];

    const float c0 = m6 * m8 - m4 * m10;
    const float c1 = m5 * m10 - m6 * m9;
    const float c2 = m4 * m9 - m5 * m8;
    float determinant = m1 * c0;
    determinant = m0 * c1 + determinant;
    determinant = m2 * c2 + determinant;

    if ((float)fabs(determinant) <= epsilon * epsilon * epsilon) {
        return HK_FAILURE;
    }

    const float invDeterminant = 1.0f / determinant;
    m[0] = invDeterminant * c1;
    m[1] = invDeterminant * c0;
    m[2] = invDeterminant * c2;
    m[3] = invDeterminant * 0.0f;
    m[4] = invDeterminant * (m9 * m2 - m10 * m1);
    m[5] = invDeterminant * (m10 * m0 - m8 * m2);
    m[6] = invDeterminant * (m8 * m1 - m9 * m0);
    m[7] = invDeterminant * 0.0f;
    m[8] = invDeterminant * (m1 * m6 - m2 * m5);
    m[9] = invDeterminant * (m2 * m4 - m0 * m6);
    m[10] = invDeterminant * (m0 * m5 - m1 * m4);
    m[11] = invDeterminant * 0.0f;
    transpose();
    return HK_SUCCESS;
}

void hkMatrix3::invertSymmetric() {
    float* m = elements;

    const float m0 = m[0];
    const float m1 = m[1];
    const float m2 = m[2];
    const float m4 = m[4];
    const float m5 = m[5];
    const float m6 = m[6];
    const float m8 = m[8];
    const float m9 = m[9];
    const float m10 = m[10];

    const float c0 = m6 * m8 - m4 * m10;
    const float c1 = m5 * m10 - m6 * m9;
    const float c2 = m4 * m9 - m5 * m8;
    float determinant = m1 * c0;
    determinant = m0 * c1 + determinant;
    determinant = m2 * c2 + determinant;

    if (!(determinant >= 1.6940659e-21f)) {
        determinant = 1.6940659e-21f;
    }

    const float invDeterminant = 1.0f / determinant;
    m[0] = invDeterminant * c1;
    m[1] = invDeterminant * c0;
    m[2] = invDeterminant * c2;
    m[3] = invDeterminant * 0.0f;
    m[4] = invDeterminant * (m9 * m2 - m10 * m1);
    m[5] = invDeterminant * (m10 * m0 - m8 * m2);
    m[6] = invDeterminant * (m8 * m1 - m9 * m0);
    m[7] = invDeterminant * 0.0f;
    m[8] = invDeterminant * (m1 * m6 - m2 * m5);
    m[9] = invDeterminant * (m2 * m4 - m0 * m6);
    m[10] = invDeterminant * (m0 * m5 - m1 * m4);
    m[11] = invDeterminant * 0.0f;
}

void hkMatrix3::add(const hkMatrix3& other) {
    float* dst = elements;
    const float* src = other.elements;
    const float m1 = src[1];
    const float m2 = src[2];
    const float m3 = src[3];
    const float m4 = src[4];
    const float m5 = src[5];
    const float m6 = src[6];
    const float m7 = src[7];
    const float m8 = src[8];
    const float m9 = src[9];
    const float m10 = src[10];
    const float m11 = src[11];

    dst[0] = dst[0] + src[0];
    dst[1] = dst[1] + m1;
    dst[2] = dst[2] + m2;
    dst[3] = dst[3] + m3;
    dst[4] = dst[4] + m4;
    dst[5] = dst[5] + m5;
    dst[6] = dst[6] + m6;
    dst[7] = dst[7] + m7;
    dst[8] = dst[8] + m8;
    dst[9] = dst[9] + m9;
    dst[10] = dst[10] + m10;
    dst[11] = dst[11] + m11;
}

void hkMatrix3::mul(float scalar) {
    float* m = elements;

    m[0] = m[0] * scalar;
    m[1] = m[1] * scalar;
    m[2] = m[2] * scalar;
    m[3] = m[3] * scalar;
    m[4] = m[4] * scalar;
    m[5] = m[5] * scalar;
    m[6] = m[6] * scalar;
    m[7] = m[7] * scalar;
    m[8] = m[8] * scalar;
    m[9] = m[9] * scalar;
    m[10] = m[10] * scalar;
    m[11] = m[11] * scalar;
}

void hkMatrix3::changeBasis(const hkRotation& basis) {
    hkMatrix3 temp;
    const hkMatrix3& rotation = basis;
    temp.setMulInverse(*this, rotation);
    setMul(rotation, temp);
}
