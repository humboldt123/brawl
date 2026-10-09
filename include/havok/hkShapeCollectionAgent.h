#pragma once

#include <havok/hkCollisionAgent.h>

// Collision agent for a shape collection against another shape (0x38 bytes). Only the factories and
// the registration are recovered so far; the members are not identified yet.
struct hkShapeCollectionAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

    u8 unkC[0x38 - 0x0C]; // 0x0C..0x37 members not identified yet

    // HYPOTHESIS: the first two parameters are the two shapes in some order; the factories pass them
    // in opposite orders.
    hkShapeCollectionAgent(void* a, void* b, void* c, int unk8Value);

    static hkShapeCollectionAgent* createListAAgent(void* a, void* b, void* c, int d);
    static hkShapeCollectionAgent* createListBAgent(void* a, void* b, void* c, int d);
    static void registerAgent(void* dispatcher);
};

// HYPOTHESIS: agent variant created by createListBAgent. Its vtable (lbl_80486F38) is not identified
// yet; the class adds no members and only replaces the vtable after the base constructor.
struct hkShapeCollectionAgentVariant : hkShapeCollectionAgent {
    hkShapeCollectionAgentVariant(void* a, void* b, void* c, int unk8Value)
        : hkShapeCollectionAgent(a, b, c, unk8Value) {}
};
