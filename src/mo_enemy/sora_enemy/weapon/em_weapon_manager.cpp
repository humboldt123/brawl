// SearchNearestFighterPosition's inline Vec2f::length squares both components in one expression (as the item and
// fighter kinetic energy units do); the two-statement form swaps its float registers.
#define MT_VEC2F_LENGTH_SQ_SINGLE_EXPR
#include <em/em_area_module_impl.h>
#include <em/em_weapon_manager.h>
#include <ft/ft_external_value_accesser.h>
#include <memory.h>
#include <so/so_module_accesser.h>
#include <sr/sr_common.h>


// emWeaponManager, the pool of the enemies' wnemSimple weapon articles (39 of them, heap Heaps::WeaponInstance), with
// the table of the weapon procs EnemyWeaponRegister fills and the list of weapons that other enemies look up by task
// id. The pool's members and the inline helpers of the classes it touches (utList::init, wnemSimple::getOwnerID /
// setDeactivate, Weapon::getTaskId) are weak copies in this unit, in the order they are first needed.

inline void utList::init() {
    m_tail = NULL;
    m_head = NULL;
    m_length = 0;
}

inline u32 wnemSimple::getOwnerID() {
    return m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
}

inline void wnemSimple::setDeactivate(bool x) {
    deactivate(x);
}

// HYPOTHESIS: the name (the instance pointer of the unit's .bss; getInstance only returns it).
emWeaponManager* g_emWeaponManager;

void emWeaponManager::remove() {
    if (g_emWeaponManager != NULL) {
        delete g_emWeaponManager;
        g_emWeaponManager = NULL;
    }
}

emWeaponManager* emWeaponManager::getInstance() {
    return g_emWeaponManager;
}

void emWeaponManager::create() {
    if (g_emWeaponManager == NULL) {
        g_emWeaponManager = new (Heaps::WeaponInstance) emWeaponManager();
        if (g_emWeaponManager != NULL) {
            EnemyWeaponRegister();
        }
    }
}

emWeaponManager::emWeaponManager() : m_procFncs(70, 0), m_managedWeapons(10, 0) {
    resetProcFncList();
    resetManageWeaponList();
}

emWeaponManager::~emWeaponManager() { }

void emWeaponManager::activateObject(wnemSimple* wnem, float lr, float speed, int founderTaskId, int ownerId,
                                     wnemSimple_Data* work, int p7, Vec3f* pos, int team, int p10, bool flag, int life,
                                     s16 p13) {
    if (work != NULL) {
        fn_41_3091C(wnem, work, founderTaskId, ownerId, p7, flag, pos, lr, team, speed, p10, life, p13);
    }
}

void emWeaponManager::deactivateObject(wnemSimple* wnem) {
    wnem->deactivate(false);
}

wnemSimple* emWeaponManager::findWeapon(int ownerId, u32 linkId, u32 index) {
    if (m_activeList.getLength() == 0) {
        return NULL;
    }
    wnemSimple* weapon = static_cast<wnemSimple*>(m_activeList.getNode(0));
    u32 count = 0;
    // The first node is not tested for NULL again: the list is not empty.
    do {
        if (ownerId == weapon->getOwnerID()) {
            if (linkId == 0xFFFF) {
                if (count == index) {
                    return weapon;
                }
                count++;
            }
            if ((s32)linkId == weapon->m_linkId) {
                if (count == index) {
                    return weapon;
                }
                count++;
            }
        }
        weapon = static_cast<wnemSimple*>(weapon->utListNode::m_next);
    } while (weapon != NULL);
    return NULL;
}

// MATCH-ONLY: the original unit has a function here that nothing calls and the REL linker strips (its body is
// unknown). It still decides where the weak copy of Weapon::getTaskId lands: right after getOwnerID, not after
// UnregisterManageWeapon. An unused static function that names getTaskId does the same and emits no code.
static bool isTaskIdOf(wnemSimple* weapon, u32 taskId) {
    return taskId == weapon->getTaskId();
}

void emWeaponManager::resetProcFncList() { }

void emWeaponManager::RegisterProcFnc(void* proc, s32 id) {
    for (s32 i = 0; i < m_procFncs.capacity(); i++) {
        wnemProcFncObj* entry = &m_procFncs.at(i);
        if (entry != NULL && entry->unk0 == 0) {
            entry->unk0 = (u32)proc; // the proc's address, 0 while the entry is free
            entry->unk4 = id;        // the proc number
            break;
        }
    }
}

void* emWeaponManager::GetProcFncPtr(s32 id) {
    if (m_procFncs.isEmpty()) {
        return NULL;
    }
    for (s32 i = 0; i < m_procFncs.capacity(); i++) {
        wnemProcFncObj* entry = &m_procFncs.at(i);
        if (entry != NULL && (s32)entry->unk4 == id) {
            return (void*)entry->unk0;
        }
    }
    return NULL;
}

void emWeaponManager::resetManageWeaponList() {
    for (s32 i = 0; i < m_managedWeapons.capacity(); i++) {
        m_managedWeapons.at(i) = NULL;
    }
}

void emWeaponManager::RegisterManageWeapon(wnemSimple* weapon) {
    for (s32 i = 0; i < m_managedWeapons.capacity(); i++) {
        if (m_managedWeapons.at(i) == NULL) {
            m_managedWeapons.at(i) = weapon;
            weapon->m_isManaged = true;
            break;
        }
    }
}

void emWeaponManager::UnregisterManageWeapon(u32 taskId) {
    if (!m_managedWeapons.isEmpty()) {
        for (s32 i = 0; i < m_managedWeapons.capacity(); i++) {
            m_managedWeapons.at(i); // MATCH-ONLY: the original evaluates the slot once before the null test.
            if (m_managedWeapons.at(i) != NULL && taskId == m_managedWeapons.at(i)->getTaskId()) {
                m_managedWeapons.at(i) = NULL;
                break;
            }
        }
    }
}

wnemSimple* emWeaponManager::GetManagedWeaponFromTaskID(u32 taskId) {
    if (m_managedWeapons.isEmpty()) {
        return NULL;
    }
    wnemSimple* weapon;
    for (s32 i = 0; i < m_managedWeapons.capacity(); i++) {
        weapon = m_managedWeapons.at(i);
        if (weapon != NULL && taskId == weapon->getTaskId()) {
            return weapon;
        }
    }
    return NULL;
}

wnemSimple* emWeaponManager::GetManagedWeapon(u32 ownerId, u32 linkId, u32 index) {
    if (m_managedWeapons.isEmpty()) {
        return NULL;
    }
    wnemSimple* weapon;
    u32 count = 0;
    for (s32 i = 0; i < m_managedWeapons.capacity(); i++) {
        weapon = m_managedWeapons.at(i);
        if (weapon != NULL && ownerId == weapon->getOwnerID()) {
            if (linkId == 0xFFFF) {
                if (count == index) {
                    return weapon;
                }
                count++;
            } else if ((s32)linkId == weapon->m_linkId) {
                if (count == index) {
                    return weapon;
                }
                count++;
            }
        }
    }
    return NULL;
}

Fighter* emWeaponManager::SearchNearestFighterPosition(const Vec2f* pos, float maxDistance) {
    // The loop bound is a const local: MWCC folds its uses to an immediate but still emits the object at the end of
    // the unit's .rodata, which is the int 4 there that no code reads.
    const int slotCount = 4;
    // HYPOTHESIS: the player slots of the fighters, as the table in .rodata lists them.
    const int slots[4] = { 0, 1, 2, 3 };
    float nearestDistance = 3.4028235e+38f;
    Fighter* nearest = NULL;
    Vec2f target;
    for (int i = 0; i < slotCount; i++) {
        Fighter* fighter = emAreaModuleImpl::getTargetFighterPtr(slots[i]);
        if (fighter != NULL) {
            Vec3f hip = ftExternalValueAccesser::getHipPos(fighter);
            Vec2f fighterPos = hip;
            target = *pos;
            Vec2f offset;
            target.sub(&fighterPos, &offset);
            float distance = offset.length();
            if (!(maxDistance < distance) && nearestDistance > distance) {
                nearest = fighter;
                nearestDistance = distance;
            }
        }
    }
    return nearest;
}

// The pool's members that the class does not define.
template <typename T, int C>
utListNode* utStageObjectPool<T, C>::getObject(bool deactivateOldest) {
    if (m_isEmpty) {
        return NULL;
    }
    if (m_freeList.getLength() == 0) {
        if (deactivateOldest && m_activeList.getLength() != 0) {
            static_cast<T*>(m_activeList.getNode(0))->setDeactivate(1);
        } else {
            return NULL;
        }
    }
    utListNode* object = m_freeList.getNode(0);
    m_freeList.remove(object);
    m_activeList.addTail(object);
    return object;
}

template <typename T, int C>
void utStageObjectPool<T, C>::releaseObject(utListNode* object) {
    if (!m_isEmpty && m_activeList.isExistNode(object) != -1) {
        m_activeList.remove(object);
        m_freeList.addTail(object);
    }
}

