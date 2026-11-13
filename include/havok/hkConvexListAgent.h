#pragma once

#include <havok/hkPredGskfAgent.h>
#include <havok/hkConvexShape.h>

struct hkCollisionDispatcher;

// Collision agent for a convex shape against a list shape (HK_SHAPE_CONVEX_LIST). Object size 0x80: derives from
// hkPredGskfAgent (its constructor's exception table destroys that base; the vtables of hkGskfAgent and hkPredGskfAgent
// take their destructors from this unit) and puts its own members in the base's tail padding at 0x74. In GSK mode the
// list is treated as one convex shape (hkConvexListConvexShape); stream mode (switchToStreamMode) reuses the manifold
// area at 0x30 for a child agent track.
struct hkConvexListAgent : hkPredGskfAgent {
    void* m_pairData;              // 0x74 HYPOTHESIS: the input's dispatcher (stored by the constructor)
    s8 m_streamMode;               // 0x78 set by the constructor; nonzero selects the GSK helpers (HYPOTHESIS: m_inGskMode)
    u8 unk79;                      // 0x79 cleared by the constructor
    u16 m_unk7A;                   // 0x7A set to 0x19 by switchToStreamMode
    u8 unk7C[0x80 - 0x7C];

    // Not inlined into createConvexListAgent (called there).
    hkConvexListAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr)
        __attribute__((never_inline));

    // The 0x30 sub-object (GSK manifold in GSK mode, list-agent stream data in stream mode).
    void* subObject() {
        return &unk30;
    }
    // 0x40 float of the stream-mode sub-object.
    float& streamUnk40() {
        return *(float*)(unk34 + 0x0C);
    }

    virtual ~hkConvexListAgent();

    // Registers the create/static-query functions for the convex-list pairs with the dispatcher.
    static void registerAgent(void* dispatcher);
    // HYPOTHESIS: the first three parameters are the two bodies and the pair data; flag selects
    // between this agent (nonzero) and the hkListAgent fallback (zero).
    static hkCollisionAgent* createConvexListAgent(void* a, void* b, void* c, int flag);
    static hkCollisionAgent* createListConvexAgent(void* a, void* b, void* c, int flag);

    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d, void* e);

    // Overrides of the hkCollisionAgent slots (vtable 0x28..0x38 of __vt__17hkConvexListAgent), base parameter lists.
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);
    virtual void removePoint(u16 key);
    virtual void commitPotential(u16 key);
    virtual void createZombie(u16 key);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& d);
    void switchToStreamMode();
    void switchToGskMode();

    virtual void cleanup();
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d); // HYPOTHESIS: slot order follows hkPhantomAgent
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d);
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* d);
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d, void* e);
};

// HYPOTHESIS: hkSymmetricAgent<hkConvexListAgent> (map name); only its linear cast is written.
struct hkSymmetricAgent_17hkConvexListAgent_ : hkConvexListAgent {
    hkSymmetricAgent_17hkConvexListAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                          hkContactMgr* contactMgr)
        : hkConvexListAgent(bodyA, bodyB, input, contactMgr) {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkConvexListAgent> (map name); its static linear cast is written.
// Created by createListConvexAgent (vtable 0x80486A6C).
struct hkSymmetricAgentLinearCast_17hkConvexListAgent_ : hkSymmetricAgent_17hkConvexListAgent_ {
    hkSymmetricAgentLinearCast_17hkConvexListAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                    hkCollisionInput& input, hkContactMgr* contactMgr)
        : hkSymmetricAgent_17hkConvexListAgent_(bodyA, bodyB, input, contactMgr) {}
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};
