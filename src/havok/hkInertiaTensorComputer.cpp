// Havok translation unit hkInertiaTensorComputer.o (main.dol 0x8032C42C-0x8032D42C).
// Functions in address order (method names from the Havok TU map):
//   0x8032C42C   120  __ct   [map: hkMassProperties____ct]
//   0x8032C4A4   100  __as   [map: hkMatrix3____as]
//   0x8032C508  1336  computeCapsuleVolumeMassProperties   [map: hkInertiaTensorComputer__computeCapsuleVolumeMassProperties]
//   0x8032CA40    88  acos   [map: hkMath__acos]
//   0x8032CA98     4  setRotation   [map: hkTransform__setRotation]
//   0x8032CA9C   128  setIdentity   [map: hkTransform__setIdentity]
//   0x8032CB1C    36  setTranslation   [map: hkTransform__setTranslation]
//   0x8032CB40   216  __ct   [map: hkMassElement____ct]
//   0x8032CC18     8  getTranslation   [map: hkTransform__getTranslation]
//   0x8032CC20   244  __ct   [map: hkInplaceArray_13hkMassElement_3_____ct]
//   0x8032CD14   148  __dt   [map: hkInplaceArray_13hkMassElement_3_____dt]
//   0x8032CDA8   172  __as   [map: hkTransform____as]
//   0x8032CE54   376  pushBack   [map: hkArray_13hkMassElement___pushBack]
//   0x8032CFCC   776  combineMassProperties   [map: hkInertiaTensorComputer__combineMassProperties]
//   0x8032D2D4   172  shiftInertiaToCom   [map: hkInertiaTensorComputer__shiftInertiaToCom]
//   0x8032D380   172  shiftInertiaFromCom   [map: hkInertiaTensorComputer__shiftInertiaFromCom]
#include <havok/hkInertiaTensorComputer.h>
#include <havok/hkTransform.h>
#include <havok/hkMath.h>
#include <havok/hkQuaternion.h>

// Tail-calls the rotation setter with this as the rotation.
void hkTransform::setRotation(const hkQuaternion& q) {
    m_rotation.set(q);
}

void hkTransform::setTranslation(const hkVector4& t) {
    m_translation = t;
}



hkVector4& hkTransform::getTranslation() {
    return m_translation;
}




// BYTES: compiled without inlining; every vector, transform and array call goes to its out-of-line copy.
#pragma push
#pragma dont_inline on
// A capsule is a cylinder along the axis plus two half spheres; the mass is split by volume. The capsule frame has
// its z axis along startAxis->endAxis and its origin halfway between them.
hkResult hkInertiaTensorComputer::computeCapsuleVolumeMassProperties(const hkVector4& startAxis,
                                                                    const hkVector4& endAxis, hkReal radius,
                                                                    hkReal mass, hkMassProperties& result) {
    if (mass <= 0.0f) {
        return HK_FAILURE;
    }
    if (radius <= 0.0f) {
        return HK_FAILURE;
    }

    hkVector4 axis;
    axis.setSub4(endAxis, startAxis);
    hkReal height = axis.length3();
    hkTransform capsuleToLocal;
    if (height > 0.0f) {
        axis.normalize3();
        hkVector4 canonicalZ;
        canonicalZ.set(0.0f, 0.0f, 1.0f, 0.0f);
        if ((hkMath::fabs)(axis.dot3(canonicalZ)) < 1.0f - 1e-5f) {
            hkVector4 rotAxis;
            rotAxis.setCross(canonicalZ, axis);
            rotAxis.normalize3();
            hkReal rotAngle = hkMath::acos(axis.dot3(canonicalZ));
            hkQuaternion q(rotAxis, rotAngle);
            capsuleToLocal.setRotation(q);
        } else {
            capsuleToLocal.setIdentity();
        }
    } else {
        capsuleToLocal.setIdentity();
    }
    {
        hkVector4 centre;
        centre.setAdd4(startAxis, endAxis);
        centre.mul4(0.5f);
        capsuleToLocal.setTranslation(centre);
    }

    hkReal volumeOfCaps = 4.0f / 3.0f * 3.14159265f * radius * radius * radius;
    hkReal volumeOfCylinder = radius * (3.14159265f * radius) * height;
    hkReal totalVolume = volumeOfCaps + volumeOfCylinder;
    hkReal massOfCaps = mass * volumeOfCaps / totalVolume;
    hkReal massOfCylinder = mass * volumeOfCylinder / totalVolume;

    hkInplaceArray<hkMassElement, 3> elements;
    {
        hkMassElement cylinder;
        cylinder.m_transform = capsuleToLocal;
        cylinder.m_properties.m_centerOfMass.set(0.0f, 0.0f, 0.0f, 0.0f);
        cylinder.m_properties.m_inertiaTensor.setIdentity();
        cylinder.m_properties.m_inertiaTensor(0, 0) =
            0.25f * (radius * radius) + height / 2.0f * height / 2.0f / 3.0f;
        cylinder.m_properties.m_inertiaTensor(1, 1) = cylinder.m_properties.m_inertiaTensor(0, 0);
        cylinder.m_properties.m_inertiaTensor(2, 2) = 0.5f * (radius * radius);
        cylinder.m_properties.m_inertiaTensor.mul(massOfCylinder);
        cylinder.m_properties.m_centerOfMass.setZero4();
        cylinder.m_properties.m_volume = volumeOfCylinder;
        cylinder.m_properties.m_mass = massOfCylinder;
        elements.pushBack(cylinder);
    }
    {
        hkMassElement cap;
        cap.m_transform = capsuleToLocal;
        hkVector4 offset;
        offset.set(0.0f, 0.0f, height / 2.0f, 0.0f);
        offset.setRotatedDir(capsuleToLocal.getRotation(), offset);
        offset.add4(cap.m_transform.getTranslation());
        cap.m_transform.setTranslation(offset);
        cap.m_properties.m_centerOfMass.set(0.0f, 0.0f, 0.375f * radius, 0.0f);
        hkMassProperties sphere;
        computeSphereVolumeMassProperties(radius, massOfCaps, sphere);
        cap.m_properties.m_inertiaTensor = sphere.m_inertiaTensor;
        cap.m_properties.m_inertiaTensor.mul(0.5f);
        shiftInertiaToCom(cap.m_properties.m_centerOfMass, 0.5f * massOfCaps, cap.m_properties.m_inertiaTensor);
        cap.m_properties.m_volume = 0.5f * volumeOfCaps;
        cap.m_properties.m_mass = 0.5f * massOfCaps;
        elements.pushBack(cap);
    }
    {
        hkMassElement cap;
        cap.m_transform = capsuleToLocal;
        hkVector4 offset;
        offset.set(0.0f, 0.0f, -(height / 2.0f), 0.0f);
        offset.setRotatedDir(capsuleToLocal.getRotation(), offset);
        offset.add4(cap.m_transform.getTranslation());
        cap.m_transform.setTranslation(offset);
        cap.m_properties.m_centerOfMass.set(0.0f, 0.0f, -0.375f * radius, 0.0f);
        hkMassProperties sphere;
        computeSphereVolumeMassProperties(radius, massOfCaps, sphere);
        cap.m_properties.m_inertiaTensor = sphere.m_inertiaTensor;
        cap.m_properties.m_inertiaTensor.mul(0.5f);
        shiftInertiaToCom(cap.m_properties.m_centerOfMass, 0.5f * massOfCaps, cap.m_properties.m_inertiaTensor);
        cap.m_properties.m_volume = 0.5f * volumeOfCaps;
        cap.m_properties.m_mass = 0.5f * massOfCaps;
        elements.pushBack(cap);
    }
    combineMassProperties(elements, result);
    return HK_SUCCESS;
}
#pragma pop

// Clamped arc cosine: 0 or pi at and beyond +-1, the C library's acos otherwise (map name hkMath::acos).
hkReal hkMath::acos(hkReal r) {
    if ((hkReal)fabs(r) >= 1.0f) {
        hkReal angle = r > 0.0f ? 0.0f : 3.14159265f;
        return angle;
    }
    return (hkReal)::acos(r);
}

// inertia(r, c) is elements[c * 4 + r]. moving the tensor onto the center of mass takes away mass * (shift^2 terms) on the
// diagonal and puts the shift products back on the off-diagonals (which are symmetric)
void hkInertiaTensorComputer::shiftInertiaToCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia) {
    float* m = &inertia.m_col0.x;
    m[0] -= mass * (shift.y * shift.y + shift.z * shift.z);
    m[5] -= mass * (shift.z * shift.z + shift.x * shift.x);
    m[10] -= mass * (shift.x * shift.x + shift.y * shift.y);
    float massX = mass * shift.x;
    m[4] = m[1] += massX * shift.y;
    float massY = mass * shift.y;
    m[9] = m[6] += massY * shift.z;
    float massZ = mass * shift.z;
    m[2] = m[8] += massZ * shift.x;
}

// same thing the other way round: signs flip
void hkInertiaTensorComputer::shiftInertiaFromCom(hkVector4& shift, hkReal mass, hkMatrix3& inertia) {
    float* m = &inertia.m_col0.x;
    m[0] += mass * (shift.y * shift.y + shift.z * shift.z);
    m[5] += mass * (shift.z * shift.z + shift.x * shift.x);
    m[10] += mass * (shift.x * shift.x + shift.y * shift.y);
    float massX = mass * shift.x;
    m[4] = m[1] -= massX * shift.y;
    float massY = mass * shift.y;
    m[9] = m[6] -= massY * shift.z;
    float massZ = mass * shift.z;
    m[2] = m[8] -= massZ * shift.x;
}

hkResult hkInertiaTensorComputer::combineMassProperties(const hkArray<hkMassElement>& elements, hkMassProperties& result) {
    // everything has to end up in one common space, so first find the new center of mass there
    hkVector4 groupCenterOfMass;
    groupCenterOfMass.setZero4();
    hkReal groupMass = 0.0f;
    hkReal groupVolume = 0.0f;
    int i;
    for (i = 0; i < elements.m_size; i++) {
        hkVector4 centerOfMassInCommon;
        centerOfMassInCommon.setTransformedPos(elements[i].m_transform, elements[i].m_properties.m_centerOfMass);
        groupCenterOfMass.addMul4(elements[i].m_properties.m_mass, centerOfMassInCommon);
        groupMass += elements[i].m_properties.m_mass;
        groupVolume += elements[i].m_properties.m_volume;
    }

    if (groupMass <= 0.0f) {
        return 1; // HK_FAILURE
    }

    groupCenterOfMass.setMul4(1.0f / groupMass, groupCenterOfMass);
    result.m_centerOfMass = groupCenterOfMass;
    result.m_mass = groupMass;
    result.m_volume = groupVolume;
    result.m_inertiaTensor.setZero();

    // each tensor gets rotated into the common space, moved away from its own center of mass to the group's, and added up
    for (i = 0; i < elements.m_size; i++) {
        hkMatrix3 inertiaTensorInCommonSpace;
        inertiaTensorInCommonSpace = elements[i].m_properties.m_inertiaTensor;
        inertiaTensorInCommonSpace.changeBasis(elements[i].m_transform.m_rotation);

        hkVector4 shift;
        shift.setTransformedPos(elements[i].m_transform, elements[i].m_properties.m_centerOfMass);
        shift.setSub4(shift, groupCenterOfMass);
        shiftInertiaFromCom(shift, elements[i].m_properties.m_mass, inertiaTensorInCommonSpace);

        result.m_inertiaTensor.add(inertiaTensorInCommonSpace);
    }
    return 0; // HK_SUCCESS
}

// fn_8032C35C: body from https://github.com/jj-link/brawl src/havok/fn_8032C35C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8032C35C {
extern "C" {
extern "C" const float lbl_805A4848;
extern "C" const float lbl_805A484C;
extern "C" const float lbl_805A4854;
}
static inline float multiply(float a, float b) { return a * b; }
// TODO: cleanup (shape from jj-link/brawl fn_8032C35C)
extern "C" int fn_8032C35C(float* p, float radius, float mass) {
    float arr[4] __attribute__((aligned(16)));
    float zero = lbl_805A4848;
    if (mass <= zero) return 1;
    if (radius <= zero) return 1;
    float inertia = lbl_805A484C * (radius * (mass * radius));
    p[8] = zero;
    p[13] = zero;
    p[18] = zero;
    arr[3] = zero;
    arr[2] = zero;
    arr[1] = zero;
    arr[0] = zero;
    p[9] = zero;
    p[10] = zero;
    p[11] = zero;
    p[12] = zero;
    p[14] = zero;
    p[15] = zero;
    p[16] = zero;
    p[17] = zero;
    p[19] = zero;
    p[8] = inertia;
    p[13] = inertia;
    p[18] = inertia;
    p[7] = zero;
    p[6] = zero;
    p[5] = zero;
    p[4] = zero;
    p[0] = radius * multiply(lbl_805A4854 * radius, radius);
    p[1] = mass;
    return 0;
}
} // namespace jj_fn_8032C35C
