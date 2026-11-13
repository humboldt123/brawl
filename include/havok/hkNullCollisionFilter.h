#pragma once

#include <havok/hkCollisionFilter.h>
#include <havok/hkMemory.h>

// Collision filter that accepts every pair (0x18 bytes, same base layout as hkCollisionFilter).
// Layout from hkNullCollisionFilterClass.cpp. It has no members of its own.
struct hkNullCollisionFilter : hkCollisionFilter {
    HK_DECLARE_REF_ALLOCATOR(0x25)

    hkNullCollisionFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkNullCollisionFilter();

    static void finishLoadedObjecthkNullCollisionFilter(void* p);
    static void cleanupLoadedObjecthkNullCollisionFilter(void* p);
    static const void* getVtablehkNullCollisionFilter();

    // The four overloads of the Havok map (hkNullCollisionFilter__isCollisionEnabled, ...1 to ...3). The
    // arguments are never read, so their types are not recovered; they are pointers here.
    virtual hkBool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const;
    virtual hkBool isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                      const hkShapeCollection& bContainer, u32 bKey) const;
    virtual hkBool isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                      u32 bKey) const;
    virtual hkBool isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const;
};
