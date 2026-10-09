#pragma once

#include <havok/hkConstraintData.h>
#include <havok/hkMatrix3.h>
#include <havok/hkMemory.h>

struct hkConstraintMotor;

// Ragdoll constraint data. Frames and motors follow the target accessors: constraint frame A is 12 floats at
// 0x20, frame B at 0x60, three motor pointers at 0xE0 (hkRagdollConstraintData.cpp).
// HYPOTHESIS: the 0x0C..0x20 and 0x50..0xE0 gaps are not yet identified.
struct hkRagdollConstraintData : hkConstraintData {
    u8 m_unk0C[0x04];           // 0x0C
    u8 m_unk10[0x10];           // 0x10
    hkMatrix3 m_constraintFrameA; // 0x20
    hkMatrix3 m_constraintFrameB; // 0x60
    u8 m_unk90[0x50];           // 0x90
    hkConstraintMotor* m_motors[3]; // 0xE0
    u8 m_unkEC[0x18];           // 0xEC
    float m_unk104;            // 0x104 HYPOTHESIS: limit compared against m_unk108
    float m_unk108;            // 0x108
    u8 m_unk10C[0x0C];          // 0x10C
    float m_unk118;            // 0x118 HYPOTHESIS: checked for equality in isValid
    float m_unk11C;            // 0x11C HYPOTHESIS: bounded in isValid
    u8 m_unk120[0x0C];          // 0x120
    float m_unk12C;            // 0x12C
    float m_unk130;            // 0x130
    u8 m_unk134[0x0C];          // 0x134 (object size 0x140, from the vtable getter's stack buffer)

    HK_DECLARE_REF_ALLOCATOR(0x2a)

    // Finish-loading ctor: only the base (vtable and reference count) is set up.
    hkRagdollConstraintData(hkFinishLoadedObjectFlag flag) : hkConstraintData(flag) {}

    static void finishLoadedObjecthkRagdollConstraintData(void* p);
    static const void* getVtablehkRagdollConstraintData();

    virtual ~hkRagdollConstraintData();

    void getConstraintInfo(hkConstraintInfo* info);
    void getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out);
    hkBool isValid() const;
    void getConstraintFrameA(hkMatrix3* out) const;
    void getConstraintFrameB(hkMatrix3* out) const;
    u32 getType() const; // HYPOTHESIS: virtual in Havok; slot not yet identified
};
