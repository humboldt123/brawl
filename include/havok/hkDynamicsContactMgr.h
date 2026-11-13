#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>

struct hkContactPoint;
struct hkContactPointProperties;
struct hkConstraintInstance;

typedef u16 hkContactPointId;

// Base of the dynamics contact managers (hkReportContactMgr, hkSimpleConstraintContactMgr). Slots after hkContactMgr's
// (from hkSimpleConstraintContactMgr's vtable lbl_804878C8): 0x30 getContactPointProperties, 0x34 getContactPoint,
// 0x38 getAllContactPointIds, 0x3C getType, 0x40/0x44 toi collision response begin/end, 0x48 getConstraintInstance.
// HYPOTHESIS: the parameter lists (the simple manager's overrides index its id table with the id).
struct hkDynamicsContactMgr : hkContactMgr {
    virtual hkContactPointProperties* getContactPointProperties(hkContactPointId id); // base returns 0
    virtual hkContactPoint* getContactPoint(hkContactPointId id);
    virtual void getAllContactPointIds(hkArray<hkContactPointId>& contactPointIds) const; // base: empty
    virtual int getType() const;
    // HYPOTHESIS: the four opaque arguments are the solver init inputs and velocities of the two bodies (the simple
    // manager forwards them unchanged to its contact constraint data).
    virtual void toiCollisionResponseBeginCallback(const hkContactPoint& cp, void* inA, void* velA, void* inB,
                                                   void* velB);
    virtual void toiCollisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied, void* inA,
                                                 void* velA, void* inB, void* velB);
    virtual hkConstraintInstance* getConstraintInstance(); // base returns 0
};
