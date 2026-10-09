// Havok translation unit hkConstraintAtom.o (main.dol 0x802D95D0-0x802D9AD8).
// Functions in address order (method names from the Havok TU map):
//   0x802D95D0    16  hkBridgeConstraintAtom_callData   [map: hkConstraintData__hkBridgeConstraintAtom_callData]
//   0x802D95E0     4  buildJacobian   [map: hkConstraintData__buildJacobian]
//   0x802D95E4    20  init   [map: hkBridgeConstraintAtom__init]
//   0x802D95F8   108  collisionResponseBeginCallback   [map: hkMassChangerModifierConstraintAtom__collisionResponseBeginCallback]
//   0x802D9664     4  collisionResponseEndCallback   [map: hkMassChangerModifierConstraintAtom__collisionResponseEndCallback]
//   0x802D9668   140  collisionResponseBeginCallback   [map: hkSoftContactModifierConstraintAtom__collisionResponseBeginCallback]
//   0x802D96F4   344  collisionResponseEndCallback   [map: hkSoftContactModifierConstraintAtom__collisionResponseEndCallback]
//   0x802D984C   180  collisionResponseBeginCallback   [map: hkMovingSurfaceModifierConstraintAtom__collisionResponseBeginCallback]
//   0x802D9900   180  collisionResponseEndCallback   [map: hkMovingSurfaceModifierConstraintAtom__collisionResponseEndCallback]
//   0x802D99B4   120  addModifierDataToConstraintInfo   [map: hkModifierConstraintAtom__addModifierDataToConstraintInfo]
//   0x802D9A2C   104  addAllModifierDataToConstraintInfo   [map: hkModifierConstraintAtom__addAllModifierDataToConstraintInfo]
//   0x802D9A94    68  __sinit_\hkConstraintAtom_cpp   [map: hkConstraintAtomcpp____sinit_]
#pragma fp_contract on
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintData.h>

// Thunk stored in a bridge atom: forwards to the constraint's virtual buildJacobian (slot 9).
static void hkBridgeConstraintAtom_callData(hkConstraintData* data) {
    data->buildJacobian();
}

void hkConstraintData::buildJacobian() {
}

void hkBridgeConstraintAtom::init(hkConstraintData* data) {
    m_constraintData = data;
    m_buildJacobianFunc = hkBridgeConstraintAtom_callData;
}

void hkMassChangerModifierConstraintAtom::collisionResponseBeginCallback(void* contact, hkModifierMotion* motionA,
                                                                         void* unused, hkModifierMotion* motionB) {
    float factorA = m_factorA;
    float factorB = m_factorB;
    motionA->m_massFactor = motionA->m_massFactor * factorA;
    motionA->m_inverseInertia.mul(factorA);
    motionB->m_massFactor = motionB->m_massFactor * factorB;
    motionB->m_inverseInertia.mul(factorB);
}

void hkMassChangerModifierConstraintAtom::collisionResponseEndCallback() {
}

// Saved vector blocks from the soft-contact begin callback. Each block is 0x20 bytes with a destructor
// (constructed through __construct_array in the TU initialiser). HYPOTHESIS: the block name is unknown.
struct hkSoftContactSavedBlock {
    float m_values[8];

    ~hkSoftContactSavedBlock();
};

static hkSoftContactSavedBlock s_softContactSaved[2];

void hkSoftContactModifierConstraintAtom::collisionResponseBeginCallback(void* unusedA, void* unusedB, void* unusedC,
                                                                         const hkVector4* srcA, void* unusedD,
                                                                         const hkVector4* srcB) {
    const float* a = (const float*)srcA;
    const float* b = (const float*)srcB;
    float* dstA = s_softContactSaved[0].m_values;
    float* dstB = s_softContactSaved[1].m_values;
    dstA[0] = a[0];
    dstA[1] = a[1];
    dstA[2] = a[2];
    dstA[3] = a[3];
    dstA[4] = a[4];
    dstA[5] = a[5];
    dstA[6] = a[6];
    dstA[7] = a[7];
    dstB[0] = b[0];
    dstB[1] = b[1];
    dstB[2] = b[2];
    dstB[3] = b[3];
    dstB[4] = b[4];
    dstB[5] = b[5];
    dstB[6] = b[6];
    dstB[7] = b[7];
}

// Blends one saved block back into the destination vectors: dst = u * saved + t * dst (t = tau).
static inline void hkSoftContactBlend(float* dst, const float* saved, float t, float u) {
    dst[0] = u * saved[0] + t * dst[0];
    dst[1] = u * saved[1] + t * dst[1];
    dst[2] = u * saved[2] + t * dst[2];
    dst[3] = u * saved[3] + t * dst[3];
    dst[4] = u * saved[4] + t * dst[4];
    dst[5] = u * saved[5] + t * dst[5];
    dst[6] = u * saved[6] + t * dst[6];
    dst[7] = u * saved[7] + t * dst[7];
}

void hkSoftContactModifierConstraintAtom::collisionResponseEndCallback(void* unusedA, void* unusedB, void* unusedC,
                                                                       hkVector4* dstA, void* unusedD,
                                                                       hkVector4* dstB) {
    float t = m_tau;
    float u = 1.0f - t;
    hkSoftContactBlend((float*)dstA, s_softContactSaved[0].m_values, t, u);
    hkSoftContactBlend((float*)dstB, s_softContactSaved[1].m_values, t, u);
}

void hkMovingSurfaceModifierConstraintAtom::collisionResponseBeginCallback(void* unusedA, hkModifierContact* contact,
                                                                           void* unusedB, void* unusedC, void* unusedD,
                                                                           hkVector4* out) {
    const hkVector4& normal = contact->m_normal;
    float dot = m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z;
    hkVector4 proj;
    hkVector4 rel;
    proj.x = dot * m_velocity.x;
    proj.y = dot * m_velocity.y;
    proj.z = dot * m_velocity.z;
    proj.w = dot * m_velocity.w;
    rel.x = m_velocity.x - proj.x;
    rel.y = m_velocity.y - proj.y;
    rel.z = m_velocity.z - proj.z;
    rel.w = m_velocity.w - proj.w;
    out->x += rel.x;
    out->y += rel.y;
    out->z += rel.z;
    out->w += rel.w;
}

void hkMovingSurfaceModifierConstraintAtom::collisionResponseEndCallback(void* unusedA, hkModifierContact* contact,
                                                                         void* unusedB, void* unusedC, void* unusedD,
                                                                         hkVector4* out) {
    const hkVector4& normal = contact->m_normal;
    float dot = m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z;
    hkVector4 proj;
    hkVector4 rel;
    proj.x = dot * m_velocity.x;
    proj.y = dot * m_velocity.y;
    proj.z = dot * m_velocity.z;
    proj.w = dot * m_velocity.w;
    rel.x = m_velocity.x - proj.x;
    rel.y = m_velocity.y - proj.y;
    rel.z = m_velocity.z - proj.z;
    rel.w = m_velocity.w - proj.w;
    out->x -= rel.x;
    out->y -= rel.y;
    out->z -= rel.z;
    out->w -= rel.w;
}

#pragma dont_inline on
// Returns a flag mask: 2 for soft-contact and viscous-surface atoms, 0 otherwise. Mass-changer and
// moving-surface atoms advance the accumulator offsets.
u32 hkModifierConstraintAtom::addModifierDataToConstraintInfo(hkConstraintInfo* info) {
    if (m_type == 0x19) { // VISCOUS_SURFACE
        return 2;
    }
    if (m_type > 0x19) {
        if (m_type < 0x1B) { // MOVING_SURFACE
            info->unk08 += 0x20;
            info->unk04 += 0x20;
        }
        return 0;
    }
    if (m_type == 0x17) { // SOFT_CONTACT
        return 2;
    }
    if (m_type > 0x17) { // MASS_CHANGER
        info->unk08 += 0x20;
        info->unk04 += 0x40;
    }
    return 0;
}

#pragma dont_inline reset
u32 hkModifierConstraintAtom::addAllModifierDataToConstraintInfo(hkConstraintInfo* info) {
    u32 flags = 0;
    hkModifierConstraintAtom* atom = this;
    while (atom->m_type >= 0x17) {
        flags |= atom->addModifierDataToConstraintInfo(info);
        atom = (hkModifierConstraintAtom*)atom->m_child;
    }
    return flags;
}

// Unnamed static flag (lbl_805A0F18), initialised to false.
static hkBool s_hkConstraintAtom_unk(false);
