#pragma once

// Local header (BrawlHeaders has no grCollCollection): gathers the line works of enabled collisions
// whose joint box meets a query box (original object gr_collision_collection.o).

#include <StaticAssert.h>
#include <gr/collision/gr_collision_linework.h>
#include <gf/gf_heap_manager.h>
#include <mt/mt_coll2d.h>
#include <types.h>

class grCollCollection {
public:
    // HYPOTHESIS: bounded line-work array at 0x18; its inline destructor releases the storage.
    struct LineWorkArray {
        // 0x18
        u16 m_capacity;
        // 0x1A
        u16 m_num;
        // 0x1C
        grCollisionLineWork* m_works;

        ~LineWorkArray() {
            if (m_works != NULL) {
                gfHeapManager::free(m_works);
            }
        }
    };

    // 0x0
    clAABBox2D m_aabb;
    // 0x10
    u16 m_unk10; // HYPOTHESIS: line attribute mask to skip
    // 0x12
    u8 m_unk12; // HYPOTHESIS: line category mask, shifted into the line attribute bits
    // 0x13
    bool m_unk13 : 1;
    // 0x14
    grCollision* m_unk14; // HYPOTHESIS: collision skipped when collecting from every collision
    // 0x18
    LineWorkArray m_lineWorks;

    grCollCollection(int capacity); // HYPOTHESIS: parameter type (used unmasked for the allocation size)
    ~grCollCollection();
    void reset();
};
static_assert(sizeof(grCollCollection) == 0x20, "Class is wrong size!");
