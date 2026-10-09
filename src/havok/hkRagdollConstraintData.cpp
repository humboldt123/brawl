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

// HYPOTHESIS: hkRotation::isOrthonormal (hkRotation.cpp, map fn_80285B30) returns an hkBool, read through its byte.
extern "C" hkBool fn_80285B30(const hkMatrix3* m, float tolerance);

// Constraint info block copied from the data at 0x10 (0x12A bytes). Helper lives in another TU.
extern "C" void fn_802DE264(void* begin, u32 size, hkConstraintInfo* info);

void hkRagdollConstraintData::finishLoadedObjecthkRagdollConstraintData(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkRagdollConstraintData(flag);
}

const void* hkRagdollConstraintData::getVtablehkRagdollConstraintData() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x140] __attribute__((aligned(16)));
    ::new (buf) hkRagdollConstraintData(flag);
    return *(const void**)buf;
}

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
    fn_802DE264(m_unk10, (u32)(end - m_unk10), info);
}

void hkRagdollConstraintData::getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) {
    out->unk04 = 0xC;
    out->unk00 = 0x70;
}

void hkRagdollConstraintData::getConstraintFrameA(hkMatrix3* out) const {
    float t;
    t = m_constraintFrameA.elements[0];
    out->elements[0] = t;
    t = m_constraintFrameA.elements[1];
    out->elements[1] = t;
    t = m_constraintFrameA.elements[2];
    out->elements[2] = t;
    t = m_constraintFrameA.elements[3];
    out->elements[3] = t;
    t = m_constraintFrameA.elements[4];
    out->elements[4] = t;
    t = m_constraintFrameA.elements[5];
    out->elements[5] = t;
    t = m_constraintFrameA.elements[6];
    out->elements[6] = t;
    t = m_constraintFrameA.elements[7];
    out->elements[7] = t;
    t = m_constraintFrameA.elements[8];
    out->elements[8] = t;
    t = m_constraintFrameA.elements[9];
    out->elements[9] = t;
    t = m_constraintFrameA.elements[10];
    out->elements[10] = t;
    t = m_constraintFrameA.elements[11];
    out->elements[11] = t;
}

void hkRagdollConstraintData::getConstraintFrameB(hkMatrix3* out) const {
    float t;
    t = m_constraintFrameB.elements[0];
    out->elements[0] = t;
    t = m_constraintFrameB.elements[1];
    out->elements[1] = t;
    t = m_constraintFrameB.elements[2];
    out->elements[2] = t;
    t = m_constraintFrameB.elements[3];
    out->elements[3] = t;
    t = m_constraintFrameB.elements[4];
    out->elements[4] = t;
    t = m_constraintFrameB.elements[5];
    out->elements[5] = t;
    t = m_constraintFrameB.elements[6];
    out->elements[6] = t;
    t = m_constraintFrameB.elements[7];
    out->elements[7] = t;
    t = m_constraintFrameB.elements[8];
    out->elements[8] = t;
    t = m_constraintFrameB.elements[9];
    out->elements[9] = t;
    t = m_constraintFrameB.elements[10];
    out->elements[10] = t;
    t = m_constraintFrameB.elements[11];
    out->elements[11] = t;
}

// HYPOTHESIS: validity chain. Each test runs only when the previous one passed; the tolerances are unknown.
hkBool hkRagdollConstraintData::isValid() const {
    bool ok = false;
    if (fn_80285B30(&m_constraintFrameA, 0.0f).m_bool) {
        ok = true;
    }
    bool okB = false;
    if (ok && fn_80285B30(&m_constraintFrameB, 0.0f).m_bool) {
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
