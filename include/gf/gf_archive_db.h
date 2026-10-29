#pragma once
// Local BrawlHeaders shadow: adds the inline entry release that utArchiveManager expands.

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <sr/sr_common.h>
#include <types.h>

struct gfArchiveDatabaseEntry {
    gfArchive* m_archive;
    bool unk4_b7 : 1;
    bool m_isOwner : 1;
    bool m_isActive : 1;
    gfArchiveDatabaseEntry* m_next;
};
static_assert(sizeof(gfArchiveDatabaseEntry) == 0xC, "Class is the wrong size!");

class gfArchiveDatabase {
    u16 m_capacity;
    u16 m_size;
    gfArchiveDatabaseEntry* m_entries;
    gfArchiveDatabaseEntry m_rawData[]; // UBFIX: C99 flexible array member

    s32 findAvail() const {
        for (s32 i = 0; i < m_capacity; i++) {
            if (!m_entries[i].m_isActive) {
                return i;
            }
        }
        return -1;
    }
public:
    static gfArchiveDatabase* create(u32 size, Heaps::HeapType heap);

    void* operator new (size_t size, Heaps::HeapType heap, u32 count) {
        return gfHeapManager::alloc(heap,
            count * sizeof(gfArchiveDatabaseEntry) + size);
    }

    gfArchiveDatabase(u32 cap) {
        m_entries = m_rawData;
        m_capacity = cap;
        m_size = 0;
        for (s32 i = 0; i < m_capacity; i++) {
            gfArchiveDatabaseEntry* ep = &m_entries[i];
            ep->m_archive = nullptr;
            ep->unk4_b7 = ep->m_isActive = false;
            ep->m_isOwner = true;
            ep->m_next = nullptr;
        }
    }
    ~gfArchiveDatabase();
    s32 add(gfArchive* arc, bool makeOwner);
    gfArchive* get(s32 i) const;

    // HYPOTHESIS: name; drops one archive reference of entry i (no out-of-line copy exists).
    void release(s32 i) {
        gfArchiveDatabaseEntry* ep = &m_entries[i];
        if (!--ep->m_archive->m_refCount) {
            if (ep->m_isOwner) {
                delete ep->m_archive;
            }
            ep->m_archive = nullptr;
            ep->m_isActive = false;
        }
        if (!m_entries[i].m_isActive) {
            m_size--;
        }
    }
};
static_assert(sizeof(gfArchiveDatabase) == 0x8, "Class is the wrong size!");
