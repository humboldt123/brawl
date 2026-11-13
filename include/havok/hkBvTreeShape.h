#pragma once

#include <havok/hkShape.h>
#include <havok/hkShapeCollection.h>
#include <havok/hkArray.h>

struct hkAabb;

// Shape with a bounding volume tree over the children of a shape collection. The image has no reflection unit
// and no code of the shape itself; the layout and the slot come from the BV tree agents.
// HYPOTHESIS: unkC (not read by the agents); the collection pointer at 0x10 (the agents take the child shapes and
// the filter's collection argument from it).
struct hkBvTreeShape : hkShape {
    u32 unkC;                              // 0x0C HYPOTHESIS
    const hkShapeCollection* m_collection; // 0x10

    virtual void unkSlot28() const; // 0x28 HYPOTHESIS: not called by the agents
    virtual void unkSlot2C() const; // 0x2C HYPOTHESIS: not called by the agents
    // Collects the keys of the children whose bounding volume overlaps the box (given in the tree's space).
    virtual void queryAabb(const hkAabb& aabb, hkArray<u32>& hits) const; // 0x30

    const hkShapeCollection* getShapeCollection() const { return m_collection; }
};
