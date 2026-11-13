#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>
#include <StaticAssert.h>

struct hkRotation;
struct hkQuaternion;

// 3x3 matrix stored as three column vectors (column-major): entry (row r, column c) is m_col<c>[r].
// The fourth lanes are retained by scalar/add operations and cleared by matrix products and setTranspose.
// Column members (not a float array): main.dol has an out-of-line weak hkMatrix3::__as that copies the 12 floats
// through hkVector4's float-wise operator=, and hkRotation (which is an hkMatrix3) is read by column everywhere.
// 16-byte alignment: main.dol's hkClassMember::TYPE_PROPERTIES gives hkMatrix3 alignment 16.
struct hkMatrix3 {
    hkVector4 m_col0; // 0x00
    hkVector4 m_col1; // 0x10
    hkVector4 m_col2; // 0x20

    // One zero vector copied to the three columns (inlined in hkKeyframedRigidMotion and hkMassProperties' ctor).
    void setZero() {
        hkVector4 zero;
        zero.setZero4();
        m_col0 = zero;
        m_col1 = zero;
        m_col2 = zero;
    }

    // this = lhs * rhs. HYPOTHESIS: the name; the inline form is evidenced by setMulInverse, which carries the product
    // inlined after its setTranspose call, while setMul and hkMatrix3_setMulMat3Mat3 are out-of-line copies of it.
    // lhs is copied to aligned locals first, so the product stays correct when this aliases lhs.
    void _setMul(const hkMatrix3& lhs, const hkMatrix3& rhs) {
        hkVector4 columns[3];
        columns[0] = lhs.m_col0;
        columns[1] = lhs.m_col1;
        columns[2] = lhs.m_col2;

        const float* b = &rhs.m_col0.x;
        float* out = &m_col0.x;
        for (int i = 0; i < 3; i++) {
            const float v2 = b[2];
            const float v1 = b[1];
            const float v0 = b[0];
            out[0] = v0 * columns[0].x + v1 * columns[1].x + v2 * columns[2].x;
            out[1] = v0 * columns[0].y + v1 * columns[1].y + v2 * columns[2].y;
            out[2] = v0 * columns[0].z + v1 * columns[1].z + v2 * columns[2].z;
            out[3] = 0.0f;
            b += 4;
            out += 4;
        }
    }

    // Zero matrix with ones on the diagonal (inlined into hkTransform::setIdentity; out-of-line copy at 0x8032BD74).
    void setIdentity() {
        setZero();
        hkReal one = 1.0f;
        m_col0.x = one;
        m_col1.y = one;
        m_col2.z = one;
    }
    // Zero matrix with the three given diagonal entries (hkBoxMotion's inertia getters: the dead stores of the zero
    // temporary stay on the stack).
    void setDiagonal(hkReal m00, hkReal m11, hkReal m22) {
        setZero();
        m_col0.x = m00;
        m_col1.y = m11;
        m_col2.z = m22;
    }
    // Element (row, column) (out-of-line copy at 0x80291270).
    hkReal& operator()(int row, int column) { return (&m_col0.x)[column * 4 + row]; }
    // Column i (out-of-line copies: non-const in hkPoweredChainData.o, const in hkBoxBoxCollisionDetection.o).
    hkVector4& getColumn(int i) { return (&m_col0)[i]; }
    const hkVector4& getColumn(int i) const { return (&m_col0)[i]; }
    // Row `row` of the matrix, w = 0 (out-of-line copy in hkCollideCapsuleUtil.o).
    void getRow(int row, hkVector4& r) const {
        r.set(m_col0.getSimdAt(row), m_col1.getSimdAt(row), m_col2.getSimdAt(row), 0.0f);
    }

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
} __attribute__((aligned(16)));

static_assert(sizeof(hkMatrix3) == 0x30, "hkMatrix3 size");

void hkMatrix3_setMulMat3Mat3(hkMatrix3* dst, const hkMatrix3* lhs, const hkMatrix3* rhs);
void hkMatrix3_invertSymmetric(hkMatrix3* matrix);
