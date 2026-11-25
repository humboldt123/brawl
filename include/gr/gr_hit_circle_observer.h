#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <types.h>

class grHitCircleSubject;

// HYPOTHESIS: hit-circle record passed through addHitCircle and stored by grHitCircleObserverTeam.
struct grHitCircleData {
    grHitCircleSubject* m_subject;
    Vec2f m_pos;
    float m_radius;
    u32 m_id;
    u8 m_groupIndex;
    int m_team;
    bool m_isEnable;
};
static_assert(sizeof(grHitCircleData) == 0x20, "Class is wrong size!");

// HYPOTHESIS: doubly-linked list node allocated on Heaps::StageInstance by the hit-circle observer classes.
template <typename T>
struct grHitCircleListNode {
    grHitCircleListNode<T>* m_prev;
    grHitCircleListNode<T>* m_next;
    T m_data;

    grHitCircleListNode() : m_prev(NULL), m_next(NULL) { }
    ~grHitCircleListNode() { }
};

class grHitCircleObserverTeam {
public:
    grHitCircleObserverTeam(int team)
        : m_start(NULL), m_end(NULL), m_current(NULL), m_count(0), m_team(team), m_isEnable(true) { }
    virtual ~grHitCircleObserverTeam();

    void enableHitCircle(bool isEnable, u32 id);
    void enableHitCircle(bool isEnable);
    void setPos(Vec2f* pos, u32 id);

    // HYPOTHESIS: inlined node-list cleanup used by ~grHitCircleObserverTeam.
    void clear() {
        m_current = m_start;
        for (u32 i = 0, count = m_count; i < count; i++) {
            grHitCircleListNode<grHitCircleData*>* node = m_current;
            m_current = node->m_next;
            delete node;
        }
        m_start = NULL;
        m_end = NULL;
        m_current = NULL;
        m_count = 0;
    }

    grHitCircleListNode<grHitCircleData*>* m_start;
    grHitCircleListNode<grHitCircleData*>* m_end;
    grHitCircleListNode<grHitCircleData*>* m_current;
    u32 m_count;
    int m_team;
    bool m_isEnable;
};
static_assert(sizeof(grHitCircleObserverTeam) == 0x1C, "Class is wrong size!");

class grHitCircleObserverGroup {
public:
    grHitCircleObserverGroup()
        : m_team0(0), m_team1(1), m_start(NULL), m_end(NULL), m_current(NULL), m_count(0), m_groupIndex(0),
          m_isDisable(false) { }
    virtual ~grHitCircleObserverGroup() {
        m_groupIndex = 0;
    }

    void update();
    void addHitCircle(grHitCircleData* data); // HYPOTHESIS: parameter type
    void enableHitCircle(bool isEnable, int team, u32 id); // HYPOTHESIS: parameter types
    void enableHitCircle(bool isEnable); // HYPOTHESIS: parameter type
    void setPos(Vec2f* pos, int team, u32 id); // HYPOTHESIS: parameter types

    grHitCircleObserverTeam m_team0;
    grHitCircleObserverTeam m_team1;
    grHitCircleListNode<grHitCircleData*>* m_start;
    grHitCircleListNode<grHitCircleData*>* m_end;
    grHitCircleListNode<grHitCircleData*>* m_current;
    u32 m_count;
    u8 m_groupIndex;
    bool m_isDisable;
};
static_assert(sizeof(grHitCircleObserverGroup) == 0x50, "Class is wrong size!");

class grHitCircleObserver : public gfTask {
public:
    grHitCircleListNode<grHitCircleObserverGroup*>* m_start;
    grHitCircleListNode<grHitCircleObserverGroup*>* m_end;
    grHitCircleListNode<grHitCircleObserverGroup*>* m_current;
    u32 m_count;
    u32 m_nextId;

    virtual void processUpdate();
    virtual ~grHitCircleObserver();

    // HYPOTHESIS: inlined node-list cleanup used by ~grHitCircleObserver.
    void clear() {
        m_current = m_start;
        for (u32 i = 0, count = m_count; i < count; i++) {
            grHitCircleListNode<grHitCircleObserverGroup*>* node = m_current;
            m_current = node->m_next;
            delete node;
        }
        m_start = NULL;
        m_end = NULL;
        m_current = NULL;
        m_count = 0;
    }

    // HYPOTHESIS: inlined group lookup used by addHitCircle.
    grHitCircleObserverGroup* getGroup(u8 groupIndex) {
        m_current = m_start;
        for (u32 i = 0, count = m_count; i < count; i++) {
            grHitCircleObserverGroup* group = m_current->m_data;
            if (groupIndex == group->m_groupIndex) {
                return group;
            }
            m_current = m_current->m_next;
        }
        return NULL;
    }

    u32 addHitCircle(grHitCircleData* data); // HYPOTHESIS: parameter and return types
    void enableHitCircle(bool isEnable, u8 groupIndex, int team, u32 id); // HYPOTHESIS: parameter types
    void enableHitCircle(bool isEnable, u8 groupIndex); // HYPOTHESIS: parameter types
    void setPos(Vec2f* pos, u8 groupIndex, int team, u32 id); // HYPOTHESIS: parameter types
#ifdef ST_ADVENTURE2_IMPL
    static void remove();
#endif
};
static_assert(sizeof(grHitCircleObserver) == 0x54, "Class is wrong size!");
#ifdef ST_ADVENTURE2_IMPL
extern grHitCircleObserver* lbl_27_bss_6148;
inline void grHitCircleObserver::remove() {
    lbl_27_bss_6148->exit();
    lbl_27_bss_6148 = NULL;
}
#endif
