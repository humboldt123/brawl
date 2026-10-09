// Havok translation unit hkContinuousGsk.o (main.dol 0x80313514-0x80315780).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80313514     8  getSweptTransform   [map: hkMotionState__getSweptTransform]
//   0x8031351C     4  getRotation   [map: hkTransform__getRotation1]
//   0x80313520   628  hk4dGskCollidePointWithPlaneFinal   [map: hk4dGskVertexCollidePointsInput__hk4dGskCollidePointWithPlaneFinal]
//   0x80313794  1276  hk4dGskCollidePointsWithPlane   [map: hkContinuousGsk__hk4dGskCollidePointsWithPlane]
//   0x80313C90   804  hk4dGskCalcPointVelocityAssumingAngularVelError   [map: hkMotionState__hk4dGskCalcPointVelocityAssumingAngularVelError]
//   0x80313FB4  1312  hk4dGskCollideCalcToi   [map: hkContinuousGsk__hk4dGskCollideCalcToi]
//   0x803144D4   116  calcAngularTimInfo   [map: hkSweptTransformUtil__calcAngularTimInfo]
//   0x80314548    12  __ct   [map: hkGsk____ct]
//   0x80314554   192  init   [map: hkGsk__init]
//   0x80314614  3040  hk4dGskCalcToiFinal   [map: hkAgent3ProcessInput__hk4dGskCalcToiFinal]
//   0x803151F4   388  hk4dGskCalcWorstCaseProjectedVelocity   [map: hkMotionState__hk4dGskCalcWorstCaseProjectedVelocity]
//   0x80315378   352  hk4dGskCollPInputInitialize   [map: hkAgent3ProcessInput__hk4dGskCollPInputInitialize]
//   0x803154D8    12  __ct   [map: hk4dGskVertexCollidePointsOutput____ct]
//   0x803154E4   596  hk4dGskCalcSeparatingNormal   [map: hkTransform__hk4dGskCalcSeparatingNormal]
//   0x80315738    20  checkForChangesAndUpdateCache   [map: hkGsk__checkForChangesAndUpdateCache]
//   0x8031574C    48  hkDeallocateStack<9hkVector4>   [map: hkVector4__hkDeallocateStack_9hkVector4_]
//   0x8031577C     4  __ct   [map: hk4dGskVertexCollidePointsInput____ct]

#include <havok/hkGsk.h>
#include <havok/hk4dGskVertexCollidePointsInput.h>
#include <havok/hk4dGskVertexCollidePointsOutput.h>

hkGsk::hkGsk() {
    unk10 = 0;
}

hk4dGskVertexCollidePointsOutput::hk4dGskVertexCollidePointsOutput() {
    unk0 = 0.0f;
}

void hkGsk::checkForChangesAndUpdateCache(void* arg) {
    if (unk14 != 0) {
        exitAndExportCacheImpl(arg);
    }
}

hk4dGskVertexCollidePointsInput::hk4dGskVertexCollidePointsInput() {}
