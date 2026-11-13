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
#include <havok/hkRegistry.h>

// Constraint info block copied from the data at 0x10 (0x12A bytes). Helper lives in another TU.

void hkRagdollConstraintData::finishLoadedObjecthkRagdollConstraintData(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkRagdollConstraintData(flag);
}

#pragma push
#pragma auto_inline off
const void* hkRagdollConstraintData::getVtablehkRagdollConstraintData() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x140] __attribute__((aligned(16)));
    ::new (buf) hkRagdollConstraintData(flag);
    return *(const void**)buf;
}
#pragma pop

hkRagdollConstraintData::~hkRagdollConstraintData() {
    for (int i = 0; i < 3; i++) {
        if (m_motors[i]) {
            ((hkReferencedObject*)m_motors[i])->removeReference();
        }
    }
}

// Virtual destructor call with the delete flag.
void cleanupLoadedObjecthkRagdollConstraintData(hkRagdollConstraintData* self) {
    ((hkBaseObject*)self)->~hkBaseObject();
}

void hkRagdollConstraintData::getConstraintInfo(hkConstraintInfo* info) {
    u8* end = (u8*)this + 0x13A;
    getConstraintInfoUtil((const hkConstraintAtom*)m_unk10, (u32)(end - m_unk10), *(ConstraintInfo*)info);
}

void hkRagdollConstraintData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) {
    out->unk04 = 0xC;
    out->unk00 = 0x70;
}

void hkRagdollConstraintData::getConstraintFrameA(hkMatrix3* out) const {
    float t;
    t = m_constraintFrameA.m_col0.x;
    out->m_col0.x = t;
    t = m_constraintFrameA.m_col0.y;
    out->m_col0.y = t;
    t = m_constraintFrameA.m_col0.z;
    out->m_col0.z = t;
    t = m_constraintFrameA.m_col0.w;
    out->m_col0.w = t;
    t = m_constraintFrameA.m_col1.x;
    out->m_col1.x = t;
    t = m_constraintFrameA.m_col1.y;
    out->m_col1.y = t;
    t = m_constraintFrameA.m_col1.z;
    out->m_col1.z = t;
    t = m_constraintFrameA.m_col1.w;
    out->m_col1.w = t;
    t = m_constraintFrameA.m_col2.x;
    out->m_col2.x = t;
    t = m_constraintFrameA.m_col2.y;
    out->m_col2.y = t;
    t = m_constraintFrameA.m_col2.z;
    out->m_col2.z = t;
    t = m_constraintFrameA.m_col2.w;
    out->m_col2.w = t;
}

void hkRagdollConstraintData::getConstraintFrameB(hkMatrix3* out) const {
    float t;
    t = m_constraintFrameB.m_col0.x;
    out->m_col0.x = t;
    t = m_constraintFrameB.m_col0.y;
    out->m_col0.y = t;
    t = m_constraintFrameB.m_col0.z;
    out->m_col0.z = t;
    t = m_constraintFrameB.m_col0.w;
    out->m_col0.w = t;
    t = m_constraintFrameB.m_col1.x;
    out->m_col1.x = t;
    t = m_constraintFrameB.m_col1.y;
    out->m_col1.y = t;
    t = m_constraintFrameB.m_col1.z;
    out->m_col1.z = t;
    t = m_constraintFrameB.m_col1.w;
    out->m_col1.w = t;
    t = m_constraintFrameB.m_col2.x;
    out->m_col2.x = t;
    t = m_constraintFrameB.m_col2.y;
    out->m_col2.y = t;
    t = m_constraintFrameB.m_col2.z;
    out->m_col2.z = t;
    t = m_constraintFrameB.m_col2.w;
    out->m_col2.w = t;
}

// HYPOTHESIS: validity chain. Each test runs only when the previous one passed; the tolerances are unknown.
hkBool hkRagdollConstraintData::isValid() const {
    bool ok = false;
    if (static_cast<const hkRotation&>(m_constraintFrameA).isOrthonormal(0.0f)) {
        ok = true;
    }
    bool okB = false;
    if (ok && static_cast<const hkRotation&>(m_constraintFrameB).isOrthonormal(0.0f)) {
        okB = true;
    }
    bool okC = false;
    if (okB && m_unk118 == 0.0f) {
        okC = true;
    }
    bool okD = false;
    if (okC && m_unk11C >= 0.0f) {
        okD = true;
    }
    bool okE = false;
    if (okD && m_unk11C < 1.0f) {
        okE = true;
    }
    bool okF = false;
    if (okE && m_unk12C <= m_unk130) {
        okF = true;
    }
    bool okG = false;
    if (okF && m_unk104 <= m_unk108) {
        okG = true;
    }
    return okG;
}

int hkRagdollConstraintData::getType() const {
    return 7; // CONSTRAINT_TYPE_RAGDOLL
}

// Not yet decompiled in this unit:
//   0x80325BB0    32  finishLoadedObjecthkRagdollConstraintData   [map: hkRagdollConstraintData__finishLoadedObjecthkRagdollConstraintData]
//   0x80325BE4    60  getVtablehkRagdollConstraintData   [map: hkRagdollConstraintData__getVtablehkRagdollConstraintData]
//   0x80325C48   396  isValid   [map: hkRagdollConstraintData__isValid]
//   0x80325E9C   232  __dt   [map: hkRagdollConstraintData____dt]

static hkStaticTypeInfo hkRagdollConstraintDataTypeInfo("hkRagdollConstraintData",
    hkRagdollConstraintData::finishLoadedObjecthkRagdollConstraintData,
    (void (*)(void*))cleanupLoadedObjecthkRagdollConstraintData,
    hkRagdollConstraintData::getVtablehkRagdollConstraintData());
