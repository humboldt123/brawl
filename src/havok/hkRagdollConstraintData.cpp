// Havok translation unit hkRagdollConstraintData.o (main.dol 0x80325BB0-0x80325FDC).
// Functions in address order (method names from the Havok TU map):
//   0x80325BB0    32  finishLoadedObjecthkRagdollConstraintData   [map: hkRagdollConstraintData__finishLoadedObjecthkRagdollConstraintData]
//   0x80325BD0    20  cleanupLoadedObjecthkRagdollConstraintData   [map: hkRagdollConstraintData__cleanupLoadedObjecthkRagdollConstraintData]
//   0x80325BE4    60  getVtablehkRagdollConstraintData   [map: hkRagdollConstraintData__getVtablehkRagdollConstraintData]
//   0x80325C20    20  getConstraintInfo   [map: hkRagdollConstraintData__getConstraintInfo]
//   0x80325C34    20  getRuntimeInfo   [map: hkRagdollConstraintData__getRuntimeInfo]
//   0x80325C48   396  isValid   [map: hkRagdollConstraintData__isValid]
//   0x80325DD4   100  getConstraintFrameA   [map: hkRagdollConstraintData__getConstraintFrameA]
//   0x80325E38   100  getConstraintFrameB   [map: hkRagdollConstraintData__getConstraintFrameB]
//   0x80325E9C   232  __dt   [map: hkRagdollConstraintData____dt]
//   0x80325F84     8  getType   [map: hkRagdollConstraintData__getType]
//   0x80325F8C    80  __sinit_\hkRagdollConstraintData_cpp   [map: hkRagdollConstraintDatacpp____sinit_]
#include <havok/hkRagdollConstraintData.h>

// Constraint info block copied from the data at 0x10 (0x12A bytes). Helper lives in another TU.
extern "C" void fn_802DE264(void* begin, u32 size, hkConstraintInfo* info);

// Virtual destructor call with the delete flag.
void cleanupLoadedObjecthkRagdollConstraintData(hkRagdollConstraintData* self) {
    delete self;
}

void hkRagdollConstraintData::getConstraintInfo(hkConstraintInfo* info) {
    u8* end = (u8*)this + 0x13A;
    fn_802DE264(m_unk10, (u32)(end - m_unk10), info);
}

void hkRagdollConstraintData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) {
    out->unk04 = 0xC;
    out->unk00 = 0x70;
}

void hkRagdollConstraintData::getConstraintFrameA(hkMatrix3* out) const {
    float* dst = out->elements;
    const float* src = m_constraintFrameA.elements;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
    dst[11] = src[11];
}

void hkRagdollConstraintData::getConstraintFrameB(hkMatrix3* out) const {
    float* dst = out->elements;
    const float* src = m_constraintFrameB.elements;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
    dst[11] = src[11];
}

u32 hkRagdollConstraintData::getType() const {
    return 7; // CONSTRAINT_TYPE_RAGDOLL
}

// Static initialiser: not yet recovered.
static hkBool s_hkRagdollConstraintData_unk(false);

// Not yet decompiled in this unit:
//   0x80325BB0    32  finishLoadedObjecthkRagdollConstraintData   [map: hkRagdollConstraintData__finishLoadedObjecthkRagdollConstraintData]
//   0x80325BE4    60  getVtablehkRagdollConstraintData   [map: hkRagdollConstraintData__getVtablehkRagdollConstraintData]
//   0x80325C48   396  isValid   [map: hkRagdollConstraintData__isValid]
//   0x80325E9C   232  __dt   [map: hkRagdollConstraintData____dt]
