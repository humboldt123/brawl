#pragma once

#include <havok/hkCollisionFilter.h>
#include <havok/hkCollidable.h>
#include <havok/hkMemory.h>

// Group filter (0x9C bytes). Layout from hkGroupFilterClass.cpp.
// Collision info words: bits 0-4 layer, bits 5-9 and 10-14 subsystem ids, bits 16-31 system group.
struct hkGroupFilter : hkCollisionFilter {
    HK_DECLARE_REF_ALLOCATOR(0x25)

    int nextFreeSystemGroup;        // 0x18
    u32 collisionLookupTable[32];   // 0x1C: one bit per layer pair, indexed by the first layer

    hkGroupFilter();
    hkGroupFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkGroupFilter();

    static void finishLoadedObjecthkGroupFilter(void* p);
    static void cleanupLoadedObjecthkGroupFilter(void* p);
    static const void* getVtablehkGroupFilter();

    hkBool isCollisionEnabled(u32 infoA, u32 infoB) const;
    // Overloads 1-4 keep the numbering of the Havok map names (hkGroupFilter__isCollisionEnabledN).
    hkBool isCollisionEnabled1(const hkCollidable* a, const hkCollidable* b) const;
    hkBool isCollisionEnabled2(const void* a, const hkCollidable* b, const void* unk6, const void* c,
                               const void* d) const;
    hkBool isCollisionEnabled3(const void* a, const void* b, const void* c) const;
    hkBool isCollisionEnabled4(const void* a, const hkCollidable* b) const;
    void enableCollisionsUsingBitfield(u32 layersA, u32 layersB);
    void disableCollisionsBetween(u32 layerA, u32 layerB);
    hkBool dummyUnused() const;
};
