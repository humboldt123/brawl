#pragma once

#include <StaticAssert.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <sora_adv_stage/gr_adventure_background.h>
#include <types.h>

// One respawn point of a Subspace Emissary area, read from the locator nodes "AdvRebirthNNP" (the position),
// "AdvRebirthNNTL" and "AdvRebirthNNBR" (the corners of its region). HYPOTHESIS: the struct and member names.
struct grAdventureRebirthPosData {
    float m_pos[2];         // 0x0, x and y
    float m_topLeft[2];     // 0x8
    float m_bottomRight[2]; // 0x10
};
static_assert(sizeof(grAdventureRebirthPosData) == 0x18, "Class is wrong size!");

// A fixed-capacity list of respawn points on the stage instance heap (the same shape as stDataMultiContainer's node
// list). HYPOTHESIS: the class name.
class grAdventureRebirthPosList {
    u16 m_maxCount;                       // 0x0
    u16 m_count;                          // 0x2
    grAdventureRebirthPosData* m_entries; // 0x4

    static const u32 Capacity = 20;

public:
    grAdventureRebirthPosList()
        : m_maxCount(Capacity),
          m_count(0),
          m_entries(static_cast<grAdventureRebirthPosData*>(gfHeapManager::alloc(Heaps::StageInstance, sizeof(m_entries[0]) * Capacity))) {
        gfMemFill(m_entries, 0, sizeof(m_entries[0]) * Capacity);
    }
    ~grAdventureRebirthPosList() {
        if (m_entries != NULL) {
            gfHeapManager::free(m_entries);
        }
    }

    void add(const grAdventureRebirthPosData& data) {
        if (m_count < m_maxCount) {
            m_entries[m_count] = data;
            m_count++;
        }
    }
    void clear() { m_count = 0; }
    grAdventureRebirthPosData* get(int index) { return &m_entries[index]; }
#ifdef ST_ADVENTURE2_IMPL
    u16 getCount() const { return m_count; }
#endif
};
static_assert(sizeof(grAdventureRebirthPosList) == 0x8, "Class is wrong size!");

// The respawn points of an adventure stage area: a locator-node gimmick whose "AdvRebirthNN*" nodes are collected into
// a list that the stage reads from.
class grAdventureRebirthPos : public grAdventureFighterPos {
    grAdventureRebirthPosList m_rebirthPosList; // 0x150

public:
    grAdventureRebirthPos(const char* taskName) : grAdventureFighterPos(taskName) { }
    virtual ~grAdventureRebirthPos();
    virtual void update(float deltaFrame);

    grAdventureRebirthPosData* getRebirthPos(int index);
    void setRebirthPosData();
#ifdef ST_ADVENTURE2_IMPL
    u16 getRebirthPointNum() { return m_rebirthPosList.getCount(); }
#endif

    static grAdventureRebirthPos* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureRebirthPos) == 0x158, "Class is wrong size!");
