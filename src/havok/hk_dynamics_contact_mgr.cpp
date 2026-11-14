// Havok translation unit hk_dynamics_contact_mgr.o (main.dol 0x802D7FC8-0x802D7FD0).
// Functions in address order (method names from the Havok TU map):
//   0x802D7FC8     4  toiCollisionResponseEndCallback   [map: hkDynamicsContactMgr__toiCollisionResponseEndCallback]
//   0x802D7FCC     4  toiCollisionResponseBeginCallback   [map: hkDynamicsContactMgr__toiCollisionResponseBeginCallback]

#include <havok/hkDynamicsContactMgr.h>

// The default TOI collision response callbacks do nothing.
void hkDynamicsContactMgr::toiCollisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                                           hkSimpleConstraintInfoInitInput& inA, hkBodyVelocity& velA,
                                                           hkSimpleConstraintInfoInitInput& inB, hkBodyVelocity& velB) {}

void hkDynamicsContactMgr::toiCollisionResponseBeginCallback(const hkContactPoint& cp, hkSimpleConstraintInfoInitInput& inA,
                                                             hkBodyVelocity& velA, hkSimpleConstraintInfoInitInput& inB,
                                                             hkBodyVelocity& velB) {}
