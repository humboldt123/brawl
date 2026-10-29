#pragma once

// SHADOW of BrawlHeaders ut/ut_stageobject_pool.h (-I include comes first): names the two lists and the flag after
// their uses, and adds the constructor that emWeaponManager inlines. Only em/em_weapon_manager.h includes it.

#include <StaticAssert.h>
#include <types.h>
#include <ut/ut_list.h>

// A fixed set of objects (the array at m_stageObjects) that move between two lists: the free ones (m_freeList) and the
// active ones (m_activeList). getObject and releaseObject are defined by the unit that owns the pool's instantiation.
template <typename T, int C>
class utStageObjectPool {
public:
    T* m_stageObjects;
    u32 m_numStageObjects;
    utList m_freeList;   // 0x08, objects ready to be handed out by getObject
    utList m_activeList; // 0x14, objects handed out and not released yet
    // HYPOTHESIS: the original tests this to refuse getObject and to guard clean's teardown; the constructor sets it.
    bool m_isEmpty;
    char _pad[3];

    utStageObjectPool() {
        m_freeList.init();
        m_activeList.init();
        m_stageObjects = NULL;
        m_numStageObjects = 0;
        m_isEmpty = true;
    }
    // Defined in the class: the owning unit emits these three right after the constructor, before the other members.
    virtual ~utStageObjectPool() {
        clean();
    }
    virtual void clean() {
        if (m_isEmpty) {
            reset();
            if (m_stageObjects != NULL) {
                delete[] m_stageObjects;
                m_stageObjects = NULL;
                m_numStageObjects = 0;
            }
        }
    }
    virtual void reset() {
        // A stage object is a utListNode at the start of a T.
        while (m_activeList.getLength() != 0) {
            static_cast<T*>(m_activeList.getNode(0))->deactivate(false);
        }
    }
    virtual utListNode* getObject(bool deactivateOldest);
    virtual void releaseObject(utListNode* object);
};
static_assert(sizeof(utStageObjectPool<utListNode, 0>) == 40, "Class is wrong size!");
