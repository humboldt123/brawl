#include <ai/ai_weapon.h>
#include <gf/gf_memory_util.h>

int aiWeapon::getWnKind() {
    if (m_type != 0) {
        return -1;
    }
    return m_kind;
}

int aiWeapon::getItKind() {
    if (m_type != 1 && m_type != 4 && m_type != 5) {
        return -1;
    }
    return m_kind;
}

aiWeaponMgr::aiWeaponMgr() {
    gfMemFill(this, 0, sizeof(aiWeaponMgr));
}

aiWeaponMgr::~aiWeaponMgr() { }

void aiWeaponMgr::reset() {
    gfMemFill(this, 0, sizeof(aiWeaponMgr));
}

aiWeapon* aiWeaponMgr::enum_searched() {
    if (m_enumCount < m_searchedCount) {
        for (int i = m_enumCursor; i < 8; i++) {
            if (m_searchedBits[i] != 0) {
                for (int bit = 0; bit < 8; bit++) {
                    if ((1 << bit & m_searchedBits[i]) != 0) {
                        m_searchedBits[i] &= ~(1 << bit);
                        m_enumCount++;
                        return &m_weapons[bit + i * 8];
                    }
                }
            }
            m_enumCursor++;
        }
    }
    return NULL;
}

aiWeapon* aiWeaponMgr::near_searched(Vec2f* pos) {
    aiWeapon* nearest = NULL;
    float nearestDist = 9999979520.0f;
    for (;;) {
        aiWeapon* weapon = enum_searched();
        if (weapon == NULL) {
            break;
        }
        Vec2f diff;
        diff.m_x = pos->m_x - weapon->m_pos.m_x;
        diff.m_y = pos->m_y - weapon->m_pos.m_y;
        float dist = diff.m_x * diff.m_x + diff.m_y * diff.m_y;
        if (nearestDist > dist) {
            nearest = weapon;
            nearestDist = dist;
        }
    }
    return nearest;
}

aiWeapon* aiWeaponMgr::getWeapon(u32 objId) {
    if (m_count > 0) {
        for (int i = 0; i < 64; i++) {
            aiWeapon* weapon = &m_weapons[i];
            if ((weapon->m_flags & 1) != 0 && weapon->m_objId == objId) {
                return weapon;
            }
        }
    }
    return NULL;
}
