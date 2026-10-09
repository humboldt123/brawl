#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>
#include <havok/hkVector4.h>

struct hkCollisionDispatcher;

struct hkCdBody;
struct hkPenetrationTarget;

// Collision agent that streams a bounding volume tree against another shape (0x40 bytes).
// Layout from the constructor and the destructor: the hkCollisionAgent 0x08 word, one word copied
// from the third constructor argument, eight zeroed floats, a hkArray<unsigned int> at 0x30 whose
// storage is the single inline entry at 0x3C.
struct hkBvTreeStreamAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

    int unkC;                      // 0x0C copied from *src in the constructor
    hkReal unk10[8];               // 0x10..0x2F zeroed by the constructor (HYPOTHESIS: two vectors)
    hkArray<unsigned int> m_list;  // 0x30 HYPOTHESIS: list managed by hkAgent1nMachine (see cleanup)
    unsigned int m_inlineEntry;    // 0x3C inline storage of m_list (capacity 1)

    // HYPOTHESIS: the first two parameters are not used by the constructor.
    hkBvTreeStreamAgent(void* unused0, void* unused1, const int* src, int unk8Value);
    virtual ~hkBvTreeStreamAgent();

    virtual void cleanup();
    virtual void invalidateTim();
    virtual void warpTime();

    static hkBvTreeStreamAgent* createBvTreeShapeAgent(void* a, void* b, const int* c, int d);
    static hkBvTreeStreamAgent* createShapeBvAgent(void* a, void* b, const int* c, int d);
    static void registerAgent(void* dispatcher);
    static void registerConvexListAgent(void* dispatcher);
    static void registerMultiRayAgent(void* dispatcher);

    // Forwarders to the hkBvTreeAgent static query functions (tail calls in the original).
    void getPenetrations(void* a, void* b, void* c, void* d);
    void getClosestPoints(void* a, void* b, void* c, void* d);
    void linearCast(void* a, void* b, void* c, void* d, void* e);
};

// HYPOTHESIS: agent variant created by createBvTreeShapeAgent. Its vtable (lbl_804868E8) is not
// identified yet; the class adds no members and only replaces the vtable after the base constructor.
struct hkBvTreeStreamAgentVariant : hkBvTreeStreamAgent {
    hkBvTreeStreamAgentVariant(void* unused0, void* unused1, const int* src, int unk8Value)
        : hkBvTreeStreamAgent(unused0, unused1, src, unk8Value) {}
};
