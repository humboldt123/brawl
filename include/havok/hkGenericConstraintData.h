#pragma once

#include <havok/hkBase.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintData.h>
#include <havok/hkArray.h>
#include <havok/hkMatrix3.h>
#include <havok/hkVector4.h>

struct hkConstraintModifier;
struct hkConstraintMotor;

// Scheme block embedded in the generic constraint data (0x40 bytes, hkGenericConstraintSchemeClass.cpp).
// The first four words are the constraint info summary copied out by getConstraintInfo.
struct hkGenericConstraintDataScheme {
    u32 m_info[4];                              // 0x00 (maxSizeOfJacobians, sizeOfJacobians, sizeOfSchemas, numSolverResults)
    hkArray<hkVector4> m_data;                  // 0x10
    hkArray<int> m_commands;                    // 0x1C
    hkArray<hkConstraintModifier*> m_modifiers; // 0x28
    hkArray<hkConstraintMotor*> m_motors;       // 0x34

    // Finish-loading form: the member arrays are not default-constructed (the loader owns them).
    hkGenericConstraintDataScheme(hkFinishLoadedObjectFlag flag)
        : m_data(flag), m_commands(flag), m_modifiers(flag), m_motors(flag) {}
};

// Generic constraint data (0x58 bytes, hkGenericConstraintDataClass.cpp). The bridge atom sits at 0x0C and
// points back at this object; the scheme follows at 0x18.
struct hkGenericConstraintData : hkConstraintData {
    hkBridgeConstraintAtom m_bridgeAtom; // 0x0C
    hkGenericConstraintDataScheme m_scheme; // 0x18

    HK_DECLARE_REF_ALLOCATOR(0x2A)

    hkGenericConstraintData(hkFinishLoadedObjectFlag flag) __attribute__((never_inline));

    static void finishLoadedObjecthkGenericConstraintData(void* p);
    static void cleanupLoadedObjecthkGenericConstraintData(void* p);
    static const void* getVtablehkGenericConstraintData();

    virtual ~hkGenericConstraintData();

    void getConstraintInfo(hkConstraintInfo* info);
    void getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out);
    // HYPOTHESIS: two-argument buildJacobian; hides the base no-argument virtual.
    void buildJacobian(void* a, void* b);
    // HYPOTHESIS: member taking the scheme pointer first; body not yet recovered.
    void hatchScheme(hkGenericConstraintDataScheme* scheme, void* a, void* b);
    hkBool isValid() const;
    u32 getType() const;
};

// Parameter block for the generic constraint solver (empty constructor).
struct hkGenericConstraintDataParameters {
    hkGenericConstraintDataParameters();
};
