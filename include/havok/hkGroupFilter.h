#pragma once

#include <havok/hkCollisionFilter.h>
#include <havok/hkCollidable.h>
#include <havok/hkMemory.h>

// Group filter (0x9C bytes). Layout from hkGroupFilterClass.cpp.
// Collision info words: bits 0-4 layer, bits 5-9 and 10-14 subsystem ids, bits 16-31 system group.
struct hkGroupFilter : hkCollisionFilter {
    HK_DECLARE_REF_ALLOCATOR(0x25)

    int m_nextFreeSystemGroup;        // 0x18
    u32 m_collisionLookupTable[32];   // 0x1C: one bit per layer pair, indexed by the first layer

    hkGroupFilter();
    hkGroupFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkGroupFilter();

    static void finishLoadedObjecthkGroupFilter(void* p);
    static void cleanupLoadedObjecthkGroupFilter(void* p);
    static const void* getVtablehkGroupFilter();

    hkBool isCollisionEnabled(u32 infoA, u32 infoB) const;
    // Overloads 1-4 keep the numbering of the Havok map names (hkGroupFilter__isCollisionEnabledN).
    virtual hkBool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const;
    virtual hkBool isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                      const hkShapeCollection& bContainer, u32 bKey) const;
    virtual hkBool isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                      u32 bKey) const;
    virtual hkBool isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const;
    void enableCollisionsUsingBitfield(u32 layersA, u32 layersB);
    void disableCollisionsBetween(u32 layerA, u32 layerB);
    hkBool dummyUnused() const;
};
