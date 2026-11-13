// BYTES: this unit has its own copy of the soItemSearch destructor and expands the soItemSearchImpl one.
#define SO_ITEM_SEARCH_INLINE_DTORS
#define SO_NULLABLE_INLINE_CTOR
#include <ft/dedede/ft_dedede_waddledee_search_impl.h>
#include <gf/gf_task_scheduler.h>
#include <so/area/so_area_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/dedede/wn_dedede_follower.h>
#include <wn/weapon.h>
#include <types.h>

// sora_melee: soItemSearchImpl::search (the map name); it stays unnamed there.
extern "C" bool fn_27_BE550(soItemSearchImpl* search, int* areaId, u32 unk, soModuleAccesser* moduleAccesser);

// HYPOTHESIS: the area contact of an article (category 4, kind 0x1d) with its task id and its offset from the fighter.
struct ftDededeWaddledeeContact {
    int unk0;
    int unk4;
    int m_taskId;  // 0x8
    u8 m_category; // 0xc
    u8 m_kind;     // 0xd
    u8 unkE[2];
    Vec2f m_offset; // 0x10
};

static inline bool isPickableWaddledee(int kind, gfTask* task) {
    bool pickable = false;
    if (kind == 0x33 && dynamic_cast<wnDededeFollower*>(task)->isPickup()) {
        pickable = true;
    }
    return pickable;
}

bool ftDededeWaddledeeSearchImpl::search(int* areaId, u32 unk, soModuleAccesser* moduleAccesser) {
    return fn_27_BE550(this, areaId, unk, moduleAccesser);
}

// The closest Waddle Dee (article kind 0x33) in range that can be picked up: its kind and task id go to work ints 0x10000043
// and 0x10000044 (both -1 when there is none).
bool ftDededeWaddledeeSearchImpl::searchWaddledee(Result* result, soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getAreaModule().getAreaContactCount(-1);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000043);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000044);
    if (count > 0) {
        float best = 3.4028235e38f;
        const ftDededeWaddledeeContact* found = NULL;
        int foundKind = -1;
        for (int i = 0; i < count; i++) {
            const ftDededeWaddledeeContact* contact =
                reinterpret_cast<const ftDededeWaddledeeContact*>(moduleAccesser->getAreaModule().getAreaContactLog(-1, i));
            if (contact->m_category == 4 && contact->m_kind == 0x1d) {
                float distance = contact->m_offset.m_x * contact->m_offset.m_x + contact->m_offset.m_y * contact->m_offset.m_y;
                if (distance < best) {
                    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, contact->m_taskId);
                    if (task) {
                        int kind = dynamic_cast<Weapon*>(task)->soGetSubKind();
                        if (isPickableWaddledee(kind, task)) {
                            found = contact;
                            best = distance;
                            foundKind = kind;
                        }
                    }
                }
            }
        }
        if (found) {
            result->unk4 = 0;
            moduleAccesser->getWorkManageModule().setInt(foundKind, 0x10000043);
            moduleAccesser->getWorkManageModule().setInt(found->m_taskId, 0x10000044);
            return true;
        }
    }
    return false;
}

ftDededeWaddledeeSearchImpl g_ftDededeWaddledeeSearchImpl;
