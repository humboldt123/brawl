// Havok translation unit hkBuildJacobianFromAtoms.o (main.dol 0x8028A860-0x8028D184).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8028A860     8  numSolverResults   [map: hkLinSoftConstraintAtom__numSolverResults]
//   0x8028A868   172  buildJacobianFromLinLimitAtom   [map: hkLinLimitConstraintAtom__buildJacobianFromLinLimitAtom]
//   0x8028A914     8  numSolverResults   [map: hkLinLimitConstraintAtom__numSolverResults]
//   0x8028A91C   204  buildJacobianFromLinFrictionAtom   [map: hkLinFrictionConstraintAtom__buildJacobianFromLinFrictionAtom]
//   0x8028A9E8    32  hkSkipSolverResults   [map: hkConstraintQueryOut__hkSkipSolverResults]
//   0x8028AA08     8  numSolverResults   [map: hkLinFrictionConstraintAtom__numSolverResults]
//   0x8028AA10   456  buildJacobianFromLinMotorAtom   [map: hkLinMotorConstraintAtom__buildJacobianFromLinMotorAtom]
//   0x8028ABD8     8  numSolverResults   [map: hkLinMotorConstraintAtom__numSolverResults]
//   0x8028ABE0   320  buildJacobianFrom2dAngAtom   [map: hk2dAngConstraintAtom__buildJacobianFrom2dAngAtom]
//   0x8028AD20     8  numSolverResults   [map: hk2dAngConstraintAtom__numSolverResults]
//   0x8028AD28   268  buildJacobianFromAngAtom   [map: hkAngConstraintAtom__buildJacobianFromAngAtom]
//   0x8028AE34     8  numSolverResults   [map: hkAngConstraintAtom__numSolverResults]
//   0x8028AE3C   684  buildJacobianFromAngLimitAtom   [map: hkAngLimitConstraintAtom__buildJacobianFromAngLimitAtom]
//   0x8028B0E8     8  numSolverResults   [map: hkAngLimitConstraintAtom__numSolverResults]
//   0x8028B0F0   568  buildJacobianFromConeLimitAtom   [map: hkConeLimitConstraintAtom__buildJacobianFromConeLimitAtom]
//   0x8028B328     8  numSolverResults   [map: hkConeLimitConstraintAtom__numSolverResults]
//   0x8028B330   680  bulidJacobianFromTwistLimitAtom   [map: hkTwistLimitConstraintAtom__bulidJacobianFromTwistLimitAtom]
//   0x8028B5D8     8  numSolverResults   [map: hkTwistLimitConstraintAtom__numSolverResults]
//   0x8028B5E0   140  buildJacobianFromAngFrictionAtom   [map: hkAngFrictionConstraintAtom__buildJacobianFromAngFrictionAtom]
//   0x8028B66C     8  numSolverResults   [map: hkAngFrictionConstraintAtom__numSolverResults]
//   0x8028B674  1620  buildJacobianFromAngMotorAtom   [map: hkAngMotorConstraintAtom__buildJacobianFromAngMotorAtom]
//   0x8028BCC8     8  numSolverResults   [map: hkAngMotorConstraintAtom__numSolverResults]
//   0x8028BCD0  2472  buildJacobianFromRagdollMotorAtom   [map: hkRagdollMotorConstraintAtom__buildJacobianFromRagdollMotorAtom]
//   0x8028C678     8  numSolverResults   [map: hkRagdollMotorConstraintAtom__numSolverResults]
//   0x8028C680  2340  buildJacobianFromPulleyAtom   [map: hkPulleyConstraintAtom__buildJacobianFromPulleyAtom]
//   0x8028CFA4     8  numSolverResults   [map: hkPulleyConstraintAtom__numSolverResults]
//   0x8028CFAC   472  buildJacobianFromSoftContactModifier   [map: hkSoftContactModifierConstraintAtom__buildJacobianFromSoftContactModifier]

#include <havok/hkConstraintAtom.h>

// Local declarations for the atom classes that are not in a header yet (owned by this unit).
// HYPOTHESIS: byte fields are unidentified; only the numSolverResults accessors are recovered here.
struct hkLinSoftConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkLinLimitConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkLinFrictionConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkLinMotorConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hk2dAngConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkAngConstraintAtom : hkConstraintAtom {
    u8 unk02;
    u8 unk03; // 0x03
    int numSolverResults() const;
};
struct hkAngLimitConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkConeLimitConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkTwistLimitConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkAngFrictionConstraintAtom : hkConstraintAtom {
    u8 unk02[2];
    u8 unk04; // 0x04
    int numSolverResults() const;
};
struct hkAngMotorConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkRagdollMotorConstraintAtom : hkConstraintAtom { int numSolverResults() const; };
struct hkPulleyConstraintAtom : hkConstraintAtom { int numSolverResults() const; };

int hkLinSoftConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinFrictionConstraintAtom::numSolverResults() const {
    return 1;
}

int hkLinMotorConstraintAtom::numSolverResults() const {
    return 1;
}

int hk2dAngConstraintAtom::numSolverResults() const {
    return 2;
}

int hkAngConstraintAtom::numSolverResults() const {
    return unk03;
}

int hkAngLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkConeLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkTwistLimitConstraintAtom::numSolverResults() const {
    return 1;
}

int hkAngFrictionConstraintAtom::numSolverResults() const {
    return unk04;
}

int hkAngMotorConstraintAtom::numSolverResults() const {
    return 1;
}

int hkRagdollMotorConstraintAtom::numSolverResults() const {
    return 3;
}

int hkPulleyConstraintAtom::numSolverResults() const {
    return 1;
}
