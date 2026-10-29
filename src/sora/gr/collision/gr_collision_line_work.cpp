#include <gr/collision/gr_collision_collection.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <types.h>

grCollCollection::grCollCollection(int capacity) {
    m_lineWorks.m_capacity = capacity;
    m_unk10 = 0;
    m_unk12 = 7;
    m_unk13 = true;
    m_lineWorks.m_num = 0;
    m_lineWorks.m_works = static_cast<grCollisionLineWork*>(gfHeapManager::alloc(Heaps::Tmp, capacity * sizeof(grCollisionLineWork)));
    gfMemFill(m_lineWorks.m_works, 0, capacity * sizeof(grCollisionLineWork));
}

grCollCollection::~grCollCollection() { }

void grCollCollection::reset() {
    m_unk10 = 0;
    m_unk12 = 7;
    m_unk13 = true;
    m_lineWorks.m_num = 0;
}
