#pragma once

#include <havok/hkBase.h>

// Base of the dynamics contact managers (hkReportContactMgr and friends). Only the virtuals defined in
// hkContactUpdater.cpp and hkReportContactMgr.cpp are recovered; their parameter lists are partly unknown.
struct hkDynamicsContactMgr : hkReferencedObject {
    virtual int getContactPointProperties();   // HYPOTHESIS: return type (the original returns 0)
    virtual void getAllContactPointIds();
    virtual void* getConstraintInstance();     // HYPOTHESIS: return type (the original returns 0)
};
