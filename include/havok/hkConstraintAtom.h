#pragma once

#include <havok/hkBase.h>
#include <havok/hkMatrix3.h>
#include <havok/hkVector4.h>

struct hkConstraintData;
struct hkConstraintInfo;

// Contact record passed to the modifier callbacks; the contact normal sits at 0x10.
struct hkModifierContact {
    u8 unk00[0x10];
    hkVector4 m_normal; // 0x10
};

// Atom header shared by all constraint atoms: type at 0x0 (hkConstraintAtomClass.cpp). Atoms are plain structs
// (no vtable); the type byte selects the atom behaviour.
struct hkConstraintAtom {
    u16 m_type; // 0x00 (hkConstraintAtom::AtomType)
};

// Hook called while a bridge atom is being solved; the bridge stores a function pointer to its thunk.
typedef void (*hkBridgeBuildJacobianFunc)(hkConstraintData* data);

// Size 0xC (hkConstraintAtomClass.cpp).
struct hkBridgeConstraintAtom : hkConstraintAtom {
    hkBridgeBuildJacobianFunc m_buildJacobianFunc; // 0x04
    hkConstraintData* m_constraintData;            // 0x08

    void init(hkConstraintData* data);
};

// Modifier atom header, size 0xC (hkConstraintAtomClass.cpp).
struct hkModifierConstraintAtom : hkConstraintAtom {
    u16 m_modifierAtomSize;    // 0x02
    u16 m_childSize;           // 0x04
    hkConstraintAtom* m_child; // 0x08

    u32 addModifierDataToConstraintInfo(hkConstraintInfo* info);
    u32 addAllModifierDataToConstraintInfo(hkConstraintInfo* info);
};

// Motion fields touched by the modifier callbacks: inverse inertia at 0x10 and a scale at 0x40.
// HYPOTHESIS: this is the hkMotion-side view used here; hkMotion itself is not declared in this tree.
struct hkModifierMotion {
    u8 unk00[0x10];
    hkMatrix3 m_inverseInertia; // 0x10
    float m_massFactor;         // 0x40
};

// Size 0x20 (hkConstraintAtomClass.cpp).
struct hkMassChangerModifierConstraintAtom : hkModifierConstraintAtom {
    float m_factorA; // 0x0C
    float m_factorB; // 0x10

    int numSolverResults() const;

    void collisionResponseBeginCallback(void* contact, hkModifierMotion* motionA, void* unused, hkModifierMotion* motionB);
    void collisionResponseEndCallback();
};

// Size 0x20 (hkConstraintAtomClass.cpp).
struct hkSoftContactModifierConstraintAtom : hkModifierConstraintAtom {
    float m_tau;             // 0x0C
    float m_maxAcceleration; // 0x10

    int numSolverResults() const;

    void collisionResponseBeginCallback(void* unusedA, void* unusedB, void* unusedC, const hkVector4* srcA,
                                        void* unusedD, const hkVector4* srcB);
    void collisionResponseEndCallback(void* unusedA, void* unusedB, void* unusedC, hkVector4* dstA, void* unusedD,
                                      hkVector4* dstB);
};

// Size 0x20 (hkConstraintAtomClass.cpp).
struct hkMovingSurfaceModifierConstraintAtom : hkModifierConstraintAtom {
    hkVector4 m_velocity; // 0x10

    void collisionResponseBeginCallback(void* unusedA, hkModifierContact* contact, void* unusedB, void* unusedC,
                                        void* unusedD, hkVector4* out);
    void collisionResponseEndCallback(void* unusedA, hkModifierContact* contact, void* unusedB, void* unusedC,
                                      void* unusedD, hkVector4* out);
};
