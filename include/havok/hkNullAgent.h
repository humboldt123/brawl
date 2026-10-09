#pragma once

#include <havok/hkCollisionAgent.h>

// Agent that never produces contacts. A single static instance is returned by createNullAgent()
// and getNullAgent().
struct hkNullAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkNullAgent();
    virtual ~hkNullAgent();

    static void staticGetClosestPoints();
    static void staticGetPenetrations();
    static void staticLinearCast();
    static hkNullAgent* createNullAgent();
    static hkNullAgent* getNullAgent();

    virtual void cleanup();
    virtual void processCollision();
    virtual void linearCast();
    virtual void getClosestPoints();
    virtual void getPenetrations();
};
