#include <OS/OSTime.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_manager.h>
#include <gr/collision/gr_collision_status.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>

extern "C" void fn_80110A40(grCollision* collision);
extern "C" void fn_80110EA0(grCollision* collision, clAABBox2D* range);
extern "C" void fn_80110F7C(grCollision* collision);
extern "C" void fn_8011135C(grCollision* collision, grCollision* target);
extern "C" void fn_801127D0(grCollisionManager* manager);

// HYPOTHESIS: g_grCollisionList links grCollision through m_next (0x30); g_grCollStatusList at 0x8049E594.
typedef nw4r::ut::LinkList<grCollision, 0x30> grCollisionNodeList;
typedef nw4r::ut::LinkList<grCollStatus, 0> grCollStatusNodeList;
extern grCollStatusNodeList lbl_8049E594;
extern grCollisionManager* lbl_805A0360;
extern bool lbl_805A0358;

grCollisionManager::grCollisionManager() :
    gfTask("GroundCollision", Category_GrCollision, 4, 13, true), unk41(0), m_nextIndex(0), unk44(0) {
    m_indexBits.initilize(0x100);
    lbl_805A0360 = this;
    for (int i = 0; i < 256; i++) {
        m_collisions[i] = NULL;
    }
    for (int i = 0; i < 8; i++) {
        m_cliffHangInfos[i].m_status = NULL;
        m_cliffHangInfos[i].m_lineId = 0xFFFF;
        m_cliffHangInfos[i].m_team = 0;
    }
    unk4F0 = 0;
    unk4F4 = 0;
}

grCollisionManager::~grCollisionManager() {
    lbl_805A0360 = NULL;
}

void grCollisionManager::collisionUpdate(bool dynamic) {
    grCollisionNodeList& list = (grCollisionNodeList&)g_grCollisionList;
    clAABBox2D range;
    if (m_flags.m_useClipRange) {
        // MATCH-ONLY: the bottom and top edges are read first, as the original loads them.
        float down = m_clipRange.m_down;
        range.m_unk1 = m_clipRange.m_left;
        range.m_unk2 = down;
        float up = m_clipRange.m_up;
        range.m_unk3 = m_clipRange.m_right;
        range.m_unk4 = up;
    }
    for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* collision = &*it;
        if (collision->m_isEnabled && dynamic == collision->m_isDynamic) {
            collision->update();
            if (m_flags.m_useClipRange) {
                fn_80110EA0(collision, &range);
            }
            fn_80110F7C(collision);
        }
    }
    for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* collision = &*it;
        u8 flags = collision->_spacer[0];
        if (((flags >> 6) & 1) && dynamic == collision->m_isDynamic && ((flags >> 5) & 1)) {
            grCollisionNodeList::Iterator jt = it;
            for (jt++; jt != list.GetEndIter(); jt++) {
                grCollision* target = &*jt;
                if (target->m_isEnabled && dynamic == collision->m_isDynamic && (((u8)collision->_spacer[0] >> 6) & 1) &&
                    (((u8)target->_spacer[0] >> 5) & 1))
                {
                    fn_8011135C(collision, target);
                }
            }
        }
    }
}

void grCollisionManager::processUpdate() {
    m_flags.m_isActive = true;
    m_flags.m_isGameFrame = false;
    if (g_GameGlobal->isPrevJustGameFrame()) {
        m_flags.m_isGameFrame = true;
    }
    if (m_flags.m_isActive) {
        // MATCH-ONLY: reading the flag through a const reference reloads the flag byte like the original.
        const Flags& flags = m_flags;
        lbl_805A0358 = flags.unk_b5;
        OSGetTime();
        collisionUpdate(false);
        fn_801127D0(this);
        OSGetTime();
    }
}

void grCollisionManager::processPreMapCorrection() {
    if (m_flags.m_isActive && unk41 == 0) {
        for (grCollStatusNodeList::Iterator it = lbl_8049E594.GetBeginIter(); it != lbl_8049E594.GetEndIter(); it++) {
            grCollStatus* status = &*it;
            if (status->m_enabled && status->m_unk124 == 0) {
                status->test();
                grCollision* collision = status->m_unk104;
                if (collision != NULL) {
                    collision->update();
                    fn_80110F7C(collision);
                }
            }
        }
    }
}

void grCollisionManager::processMapCorrection() {
    if (m_flags.m_isActive && unk41 == 0) {
        collisionUpdate(true);
        for (grCollStatusNodeList::Iterator it = lbl_8049E594.GetBeginIter(); it != lbl_8049E594.GetEndIter(); it++) {
            grCollStatus* status = &*it;
            if (status->m_enabled && status->m_unk124 == 1) {
                status->test();
            }
        }
    }
}

void grCollisionManager::processFixPosition() {
    if (m_flags.m_isGameFrame && unk41 == 0) {
        grCollisionNodeList& list = (grCollisionNodeList&)g_grCollisionList;
        for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
            if (it->m_disablePending) {
                fn_80110A40(&*it);
                it->m_disablePending = false;
            }
        }
    }
}

void grCollisionManager::processDebug() { }

void grCollisionManager::renderDebug() { }

void grCollisionManager::regist(grCollision* collision) {
    u8 next = m_nextIndex;
    u16 index = next;
    u32 mask = 1 << (index & 31);
    if (!(m_indexBits.m_bits[(int)index >> 5] & mask)) {
        m_indexBits.m_bits[next >> 5] |= mask;
        m_nextIndex++;
    } else {
        index = m_indexBits.getUnsetBit();
        m_indexBits.m_bits[index >> 5] |= 1 << (index & 31);
        m_nextIndex = index + 1;
    }
    collision->m_index = (u8)index;
    m_collisions[index] = collision;
}

// HYPOTHESIS: grCollisionManager::release in the map; it keeps the fn_ name that gr_collision.cpp calls.
extern "C" void fn_80112D64(grCollisionManager* manager, grCollision* collision) {
    u16 index = collision->m_index;
    manager->m_collisions[index] = NULL;
    manager->m_indexBits.m_bits[(int)index >> 5] &= ~(1 << (index & 31));
    collision->m_index = 0xFFFF;
}

grCollision* grCollisionManager::getCollision(int index) {
    return m_collisions[index];
}

bool grCollisionManager::checkCliffOverlap(grCollStatus* status, const u32& lineId) {
    CliffHangInfo* info = m_cliffHangInfos;
    u8 team = status->m_cliffTeam;
    bool result = false;
    for (int i = 0; i < 8; i++, info++) {
        if (info->m_status != NULL && info->m_lineId == lineId) {
            if (info->m_team == team) {
                result = false;
            } else {
                result = true;
            }
            break;
        }
    }
    return result;
}

void grCollisionManager::addCliffHangInfo(grCollStatus* status, const u32& lineId) {
    CliffHangInfo* info = m_cliffHangInfos;
    u8 team = status->m_cliffTeam;
    for (int i = 0; i < 8; i++, info++) {
        if (info->m_status == NULL) {
            info->m_status = status;
            info->m_lineId = lineId;
            info->m_team = team;
            return;
        }
    }
}

void grCollisionManager::removeCliffHangInfo(grCollStatus* status) {
    for (int i = 0; i < 8; i++) {
        CliffHangInfo* info = &m_cliffHangInfos[i];
        if (info->m_status == status) {
            info->m_status = NULL;
            info->m_lineId = 0xFFFF;
            info->m_team = 0;
            return;
        }
    }
}
