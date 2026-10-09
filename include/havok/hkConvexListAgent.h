#pragma once

#include <havok/hkCollisionAgent.h>

// Collision agent for a convex shape against a convex-list shape (HK_SHAPE_CONVEX_LIST). Object size 0x80.
// The constructor (not recovered yet) fills the 0x0C..0x2F sub-object and the pair pointers; the
// 0x30 sub-object is torn down by cleanup(). Layout is HYPOTHESIS until the constructor is matched.
struct hkConvexListAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u8 unkC[0x18 - 0x0C];
    float m_unk18;                 // 0x18 set to 0.0f by the constructor
    u8 unk1C[0x2C - 0x1C];
    float m_unk2C;                 // 0x2C set to 0.0f by the constructor
    u8 m_subObject[0x10];          // 0x30 embedded list-like sub-object; its helpers take this+0x30 as `this`
    float m_unk40;                 // 0x40 set by switchToStreamMode
    u8 unk44[0x74 - 0x44];
    void* m_pairData;              // 0x74 HYPOTHESIS: pointer read from the constructor's third argument
    s8 m_streamMode;               // 0x78 nonzero selects the stream-mode helpers
    u8 unk79;                      // 0x79
    u16 m_unk7A;                   // 0x7A set to 0x19 by switchToStreamMode
    u8 unk7C[0x80 - 0x7C];

    virtual ~hkConvexListAgent();

    // Registers the create/static-query functions for the convex-list pairs with the dispatcher.
    static void registerAgent(void* dispatcher);
    // HYPOTHESIS: the first three parameters are the two bodies and the pair data; flag selects
    // between this agent (nonzero) and the hkListAgent fallback (zero).
    static hkCollisionAgent* createConvexListAgent(void* a, void* b, void* c, int flag);
    static hkCollisionAgent* createListConvexAgent(void* a, void* b, void* c, int flag);

    static void staticGetClosestPoints(void* a, void* b, void* c, void* d);
    static void staticGetPenetrations(void* a, void* b, void* c, void* d);
    static void staticLinearCast(void* a, void* b, void* c, void* d, void* e);

    virtual void invalidateTim();
    virtual void warpTime();
    virtual void removePoint();
    virtual void commitPotential();
    virtual void createZombie();
    // HYPOTHESIS: non-virtual until the vtable is checked.
    void updateShapeCollectionFilter(void* a, void* b, void* d);
    void switchToStreamMode();
    void switchToGskMode();

    virtual void cleanup();
    virtual void processCollision(); // HYPOTHESIS: slot order follows hkPhantomAgent
    virtual void getClosestPoints(void* a, void* b, void* c, void* d);
    virtual void getPenetrations(void* a, void* b, void* c, void* d);
    virtual void linearCast(void* a, void* b, void* c, void* d, void* e);
};
