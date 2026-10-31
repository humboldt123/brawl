// MATCH-ONLY: reuse the native vector-add register form used by the bike utilities.
#define MT_VEC3F_BIKE_UTILITY_OPERATORS
#define MT_VEC3F_SUB_COMPACT
#define MT_VEC3F_MUL_INLINE
#include <ai/ai_weapon.h>
#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>
#include <gf/gf_task_scheduler.h>
#include <it/it_manager.h>
#include <it/it_create.h>
#include <so/so_external_value_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <wn/weapon.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <so/so_module_hyp.h>
#include <so/team/so_team.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <gf/gf_memory_util.h>
#include <math.h>
#include <mt/mt_common.h>

namespace aiweapon {
// MATCH-ONLY: the prediction core calls the scalar solver out of line.
int calc_acc(float, float, float, float, float, float*, float*, float*, float*) __attribute__((never_inline));
}

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

// Keep the previous records pending until all live sources have been scanned.
// Newly freed slots are therefore available on the next refresh, not this one.
void aiWeaponMgr::update() {
    gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
    m_hasPickupItem = false;
    m_previousCount = m_count;
    u8 stale[64];
    if (m_count > 0) {
        aiWeapon* weapon = m_weapons;
        for (int i = 0; i < 64; i++, weapon++) {
            stale[i] = weapon->m_flags & 1;
        }
    }
    for (u32 i = 0;; i++) {
        gfTask* task = scheduler->getTask(gfTask::Category_Weapon, i);
        if (task == NULL) break;
        if (!task->isAlive()) continue;
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        StageObject* object = dynamic_cast<StageObject*>(task);
        if (weapon == NULL || object == NULL) continue;
        int kind = weapon->getSubKind();
        // HYPOTHESIS: the excluded article kinds' gameplay names are not yet verified.
        if (kind == 0x69 || kind == 0x50) continue;
        soModuleAccesser* accesser = soExternalValueAccesser::getModuleAccesser(object);
        int parent = -1;
        if (accesser->getLinkModule().isModelConstraint()) {
            parent = accesser->getLinkModule().getParentTaskId(3);
        }
        update_core(object, 0, kind, stale, task->m_taskId, parent);
    }
    for (u32 i = 0; i < itManager::getInstance()->m_itemArrayList.size(); i++) {
        BaseItem* item = itManager::getInstance()->getSafeItemFromIndex(i);
        int parent = -1;
        if (item == NULL) continue;
        if (item->isConstraint(false)) {
            // Gooey Bombs remain relevant after being attached to another object.
            if (item->m_kind != Item_GooeyBomb) continue;
            soModuleAccesser* accesser = soExternalValueAccesser::getModuleAccesser(item);
            if (accesser->getLinkModule().isModelConstraint()) {
                parent = accesser->getLinkModule().getParentTaskId(3);
            }
        }
        update_core(item, 1, item->m_kind, stale, item->m_taskId, parent);
    }
    for (u32 i = 0;; i++) {
        gfTask* task = scheduler->getTask(gfTask::Category_Ground, i);
        if (task == NULL) break;
        if (!task->isAlive()) continue;
        StageObject* object = dynamic_cast<StageObject*>(task);
        if (object != NULL) {
            update_core(object, 2, 0, stale, task->m_taskId, -1);
        }
    }
    for (u32 i = 0; i < 14; i++) {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* stat = aiMgr::get_aiStat(&idx);
        // The native flags select fighter states that also participate as threats.
        if ((stat->m_flags & 0x80000000) != 0 && (stat->m_flags & 0x10) != 0) {
            aiChrIdx playerIdx;
            playerIdx.m_idx = i;
            s8 player = aiChrIdx::GetPlayerNo(&playerIdx);
            update_core2(&stat->m_center, 10.0f, s8(stat->m_team), player,
                         3, 1, stat->m_chrKind, stale, stat->m_taskId, 0x38, -1, 0);
        }
    }
    if (m_previousCount > 0) {
        aiWeapon* weapon = m_weapons;
        for (int i = 0; i < 64; i++, weapon++) {
            if (stale[i] != 0) {
                weapon->m_flags = 0;
                m_count--;
            }
        }
    }
    if (m_count > 0) {
        aiWeapon* weapon = m_weapons;
        for (u32 i = 0; i < 64; i++, weapon++) {
            if ((weapon->m_flags & 1) != 0 && (weapon->m_flags & 8) == 0) {
                gfCamera* camera = CameraController::getInstance()->getCameraInternal();
                if (camera != NULL) {
                    gfCamera::ClipSphere sphere;
                    float clipRadius = 30.0f + weapon->m_radius;
                    sphere.center = Vec3f(weapon->m_pos.m_x, weapon->m_pos.m_y, 0.0f);
                    sphere.radius = clipRadius;
                    if (!camera->isClipIn(&sphere)) continue;
                }
                // Visibility admits a previously unmarked record; it does not
                // remove a mark already supplied by the classifier/update.
                weapon->m_flags |= 8;
            }
        }
    }
}

// Classify active collision volumes first; selected articles/items remain useful
// to the AI even while they have no attack, grab, or search volume.
void aiWeaponMgr::update_core(StageObject* object, s8 type, int kind, u8* stale,
                              u32 objId, int constraintParentId) {
    soModuleAccesser* accesser = soExternalValueAccesser::getModuleAccesser(object);
    float radius = 10.0f;
    s8 aiPriority = 1;
    u32 flags = 0x10;
    if (type == 0) {
        switch (kind) {
        case 0x5F: case 0x48: return;
        }
    }
    soCollisionAttackModule& attack = accesser->getCollisionAttackModule();
    soHyp::soCollisionCatchModuleHyp& catchModule = getCollisionCatchModule(accesser);
    soCollisionSearchModule& search = accesser->getCollisionSearchModule();
    Vec3f pos;
    if (attack.isAttack(0, false)) {
        pos = attack.getCenterPos(0, false);
        radius = attack.getSize(0) + 4.0f;
        soCollisionAttackData* data = attack.getData(0, false);
        if (data != NULL) {
            flags = (data->m_isAbsorbable ? 0x40 : 0) |
                    (data->m_isReflectable ? 0x20 : 0) |
                    (data->m_isShieldable ? 0x10 : 0);
        }
        if (type == 1 && kind == Item_Unira) radius += 15.0f;
    } else if (catchModule.isCatch(0)) {
        pos = catchModule.getCenterPos(0);
        radius = catchModule.getSize(0) + 4.0f;
    } else if (search.isSearch(0)) {
        pos = soExternalValueAccesser::getPrevRoughPos(object);
        radius = search.getSize(0) + 4.0f;
        if (type == 1) {
            switch (kind) {
            case Item_MotionSensorBomb: case Item_Bumper: radius += 15.0f; break;
            }
        } else if (type == 0 && kind == 0x16) {
            radius += 15.0f;
        }
    } else {
        bool hasPosition = false;
        // HYPOTHESIS: gameplay names for the numeric weapon-kind exceptions
        // remain unverified. Item kinds use the existing itKind declarations.
        switch (type) {
        case 0:
            switch (kind) {
            case 0x54: radius = 2.0f; break;
            case 0x4D: case 0x66: radius = 17.544f; break;
            case 0x29: case 0x2A: radius = 0.0f; break;
            case 0x18: radius = 17.0f; break;
            case 0x16: radius = 30.0f; break;
            case 0x1D: radius = 8.0f; break;
            case 0x8A: {
                radius = 20.0f;
                hasPosition = true;
                pos.m_x = soExternalValueAccesser::getWorkFloat(object, 0x11000001);
                pos.m_y = soExternalValueAccesser::getWorkFloat(object, 0x11000002);
                int depth = soExternalValueAccesser::getWorkInt(object, 0x10000005);
                if (depth <= 0) pos.m_z = 0.0f;
                else pos.m_z = radius * float(depth);
                break;
            }
            default: return;
            }
            break;
        case 1:
            switch (kind) {
            case Item_Link_Bomb: case Item_ToonLink_Bomb: radius = 8.0f; break;
            case Item_Snake_Grenade: case Item_Assist_GrayFox: case Item_Assist_SamuraiGoroh: case Item_Assist_Saki: case Item_Assist_Starfy:
                radius = 15.0f;
                break;
            case Item_Pokemon_Gardevoir: radius = 160.0f; type = 5; break;
            default: {
                BaseItem* item = dynamic_cast<BaseItem*>(object);
                bool eligible = false;
                if (item != NULL) {
                    u32 trait = item->getParam(itValueAccesser::Work_Param_Int_Trait);
                    switch (kind) {
                    case Item_DekuNut:
                        if (item->getParamInt(0x1000000B) > 180 ||
                            item->getParamInt(0x1000000B) < 0) eligible = true;
                        break;
                    case Item_BobOmb:
                        if (item->getParamInt(0x1000000B) > 240 ||
                            item->getParamInt(0x1000000B) < 0) eligible = true;
                        break;
                    default:
                        if (trait & 0x211) {
                            eligible = true;
                        } else if (item->isPickUp()) {
                            if (trait & 0x60) eligible = true;
                            else if (trait & 2) {
                                if (item->getBullet() > 0) eligible = true;
                            } else if (trait & (4 | 0x100000)) eligible = true;
                            else if (trait & 8) eligible = true;
                        }
                        break;
                    }
                    if (eligible) {
                        aiPriority = item->getParam(itValueAccesser::Specialized_Param_Int_AI_Pri);
                        if (aiPriority <= 0) return;
                        type = 4;
                        radius = item->getParam(itValueAccesser::Specialized_Param_Float_Pickup_Range_Size_H);
                        float width = item->getParam(itValueAccesser::Specialized_Param_Float_Pickup_Range_Size_W);
                        m_hasPickupItem = true;
                        radius = 0.5f * (width + radius);
                    } else {
                        switch (kind) {
                        case Item_MotionSensorBomb: radius = 15.0f; eligible = true; break;
                        case Item_GooeyBomb: radius = 10.0f; eligible = true; break;
                        }
                    }
                }
                if (!eligible) return;
                break;
            }
            }
            break;
        default: return;
        }
        if (!hasPosition) pos = soExternalValueAccesser::getPrevRoughPos(object);
    }
    if (__fpclassifyf(pos.m_x) <= 2 && object != NULL) return;
    if (type != 4) flags |= 8;
    if (type == 2) {
        char* madein = aiMgr::getMadeinAiData(false);
        if (madein != NULL) {
            if (madein[2] == 3) return;
            flags &= ~0x10;
        }
    }
    s8 team = accesser->getTeamModule().getTeam()->getNo();
    s8 ownerPlayer = -1;
    gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
    u32 founderId = accesser->getWorkManageModule().getInt(0x10000000);
    gfTask* founder = scheduler->getTaskById(gfTask::Category_Fighter, founderId);
    if (founder == NULL) {
        int remaining = 3;
        do {
            StageObject* parent = dynamic_cast<StageObject*>(
                scheduler->getTaskById(gfTask::Category_Weapon, founderId));
            if (parent == NULL) {
                founder = NULL;
                break;
            }
            soModuleAccesser* parentAccesser = soExternalValueAccesser::getModuleAccesser(parent);
            founderId = parentAccesser->getWorkManageModule().getInt(0x10000000);
            founder = scheduler->getTaskById(gfTask::Category_Fighter, founderId);
            if (founder != NULL) break;
        } while (--remaining > 0);
    }
    if (founder != NULL && founder->isAlive()) {
        Fighter* fighter = dynamic_cast<Fighter*>(founder);
        if (fighter != NULL) ownerPlayer = g_ftManager->getPlayerNo(fighter->m_entryId);
    }
    switch (type) {
    case 0:
        if (kind == 0x16) team = -1;
        break;
    case 1:
        switch (kind) {
        case Item_Link_Bomb: case Item_Snake_Grenade:
        case Item_ToonLink_Bomb: case Item_MotionSensorBomb: team = -1; break;
        }
        break;
    }
    u32 itemFlags = 0;
    switch (type) {
    case 1: case 4: case 5: {
        BaseItem* item = dynamic_cast<BaseItem*>(object);
        if (item != NULL) itemFlags = item->getParam(itValueAccesser::Work_Param_Int_Trait);
        break;
    }
    }
    update_core2(&pos, radius, team, ownerPlayer, type, aiPriority, kind,
                 stale, objId, flags, constraintParentId, itemFlags);
}

// Reuse only a previously occupied, still-pending record with this object ID.
// Otherwise allocate an inactive slot and seed both samples at the spawn point.
void aiWeaponMgr::update_core2(Vec3f* pos, float radius, s8 team, s8 ownerPlayer,
                               s8 type, s8 aiPriority, int kind, u8* stale, u32 objId,
                               u32 flags, int constraintParentId, u32 itemFlags) {
    aiWeapon* weapon = m_weapons;
    int index = 0;
    if (m_previousCount > 0) {
        for (; index < 64; index++, weapon++) {
            if (stale[index] && weapon->m_objId == objId) break;
        }
    }
    if (m_previousCount <= 0 || index >= 64) {
        weapon = m_weapons;
        for (index = 0; index < 64; index++, weapon++) {
            if ((weapon->m_flags & 1) == 0) break;
        }
        if (index >= 64) return;
        weapon->m_objId = objId;
        weapon->m_flags = flags | 1;
        weapon->m_type = type;
        weapon->m_kind = kind;
        weapon->m_oldSamplePos = weapon->m_samplePos = weapon->m_pos = *pos;
        weapon->m_velocity = weapon->m_acceleration = Vec3f(0.0f, 0.0f, 0.0f);
        weapon->m_speed = 0.0f;
        weapon->m_team = team;
        weapon->m_updates = 0;
        weapon->m_sampleFrames = 16;
        weapon->m_radius = radius;
        weapon->m_ownerPlayer = ownerPlayer;
        weapon->m_floorBlk = NULL;
        m_count++;
    } else {
        CameraController* camera = CameraController::getInstance();
        Vec3f delta = *pos - weapon->m_pos;
        // Stop refreshing an object beyond the camera limit while it travels
        // farther out. Leaving it pending lets update() retire it this frame.
        if (pos->m_x + radius < camera->unk158) {
            if (delta.m_x < 0.0f) return;
        } else if (pos->m_x - radius > camera->unk15C && delta.m_x > 0.0f) {
            return;
        }
        if (pos->m_y + radius < camera->unk164) {
            if (delta.m_y < 0.0f) return;
        } else if (pos->m_y - radius > camera->unk160 && delta.m_y > 0.0f) {
            return;
        }
        if (type == 0) {
            // HYPOTHESIS: identities of these article kinds remain unresolved.
            // Preserve the near-stationary kind-0x29 expansion and kind-0x2A override.
            if (kind == 0x2A) {
                radius = 29.26f;
            } else if (kind == 0x29 && radius < 0.01f &&
                       delta.m_z * delta.m_z + (delta.m_x * delta.m_x + delta.m_y * delta.m_y) < 0.1f) {
                radius = 29.26f;
            }
        }
        weapon->update(type, pos, radius, team, flags);
        stale[index] = 0;
    }
    weapon->m_constraintParentId = constraintParentId;
    weapon->m_itemTrait = itemFlags;
    weapon->m_aiPriority = aiPriority;
}

// Refresh a CPU's remembered weapons by object ID, keeping each mark's delay
// across pointer refreshes. Unseen marks become available on the next refresh.
void aiWeaponMgr::mark_wpn_update(aiWeaponMark* marks, int count, u8* seen, int frames,
                                  Vec3f* pos, float radius, s8 excludedTeam) {
    for (int i = 0; i < count; i++) {
        seen[i] = 0;
    }
    if (search(pos, radius, 0, excludedTeam) > 0) {
        for (;;) {
            aiWeapon* weapon = enum_searched();
            if (weapon == NULL) {
                break;
            }
            int i;
            aiWeaponMark* mark = marks;
            for (i = 0; i < count; mark++, i++) {
                if (mark->m_weapon != NULL && weapon->m_objId == mark->m_objId) {
                    marks[i].m_weapon = weapon;
                    if (marks[i].m_frames > 0) {
                        marks[i].m_frames--;
                    }
                    seen[i] = 1;
                    break;
                }
            }
            if (i >= count) {
                mark = marks;
                for (i = 0; i < count; mark++, i++) {
                    if (mark->m_weapon == NULL) {
                        marks[i].m_objId = weapon->m_objId;
                        marks[i].m_frames = frames;
                        marks[i].m_weapon = weapon;
                        if (marks[i].m_frames > 0) {
                            marks[i].m_frames--;
                        }
                        seen[i] = 1;
                        break;
                    }
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        if (!seen[i] && marks[i].m_weapon != NULL) {
            marks[i].m_weapon = NULL;
        }
    }
}

int aiWeaponMgr::search(Vec3f* pos, float radius, int mode, s8 excludedTeam) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    float radiusSq = radius * radius;
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) == 0) {
            continue;
        }
        if (mode != 0) {
            if (weapon->m_type != 4) {
                continue;
            }
            if ((weapon->m_flags & 8) == 0) {
                continue;
            }
        } else {
            if (weapon->m_type == 4 || weapon->m_type == 5) {
                continue;
            }
            if (excludedTeam >= 0 && weapon->m_team == excludedTeam) {
                continue;
            }
        }
        Vec3f diff;
        Vec3fSub(&diff, pos, &weapon->m_pos);
        float distanceSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
        // Preserve the native comparison: unordered distances also pass here.
        if (distanceSq > radiusSq) {
            continue;
        }
        count++;
        m_searchedBits[i / 8] |= 1 << (i % 8);
    }
    m_searchedCount = count;
    return count;
}

// Used by Snake's C4 decision among other callers: a negative owner accepts
// every player, and weapon kind -1 accepts every weapon kind.
int aiWeaponMgr::search1(s8 ownerPlayer, int wnKind) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) == 0 || weapon->m_type != 0) {
            continue;
        }
        if (ownerPlayer >= 0 && ownerPlayer != weapon->m_ownerPlayer) {
            continue;
        }
        if (wnKind != -1 && wnKind != weapon->m_kind) {
            continue;
        }
        count++;
        m_searchedBits[i / 8] |= 1 << (i % 8);
    }
    m_searchedCount = count;
    return count;
}

int aiWeaponMgr::search_gimmic(Vec3f* pos, float radius) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    float radiusSq = radius * radius;
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) == 0 || weapon->m_type != 2) {
            continue;
        }
        Vec3f diff;
        Vec3fSub(&diff, pos, &weapon->m_pos);
        float distanceSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
        if (distanceSq > radiusSq) {
            continue;
        }
        count++;
        m_searchedBits[i / 8] |= 1 << (i % 8);
    }
    m_searchedCount = count;
    return count;
}

// A requested kind of -1 also matches the non-item sentinel from getItKind().
int aiWeaponMgr::search_item(int itKind) {
    m_enumCount = 0;
    m_enumCursor = 0;
    if (m_count <= 0) {
        m_searchedCount = 0;
        return 0;
    }
    gfMemFill(m_searchedBits, 0, sizeof(m_searchedBits));
    int i;
    aiWeapon* weapon = m_weapons;
    int count = 0;
    for (i = 0; i < 64; i++, weapon++) {
        if ((weapon->m_flags & 1) != 0 && itKind == weapon->getItKind()) {
            count++;
            m_searchedBits[i / 8] |= 1 << (i % 8);
        }
    }
    m_searchedCount = count;
    return count;
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
        aiWeapon* weapon = m_weapons;
        for (int i = 0; i < 64; i++, weapon++) {
            if ((weapon->m_flags & 1) != 0 && weapon->m_objId == objId) {
                return weapon;
            }
        }
    }
    return NULL;
}

// Motion samples are taken every 16 updates. Stable acceleration uses the
// per-frame model; irregular motion falls back to the longer sampled average.
void aiWeapon::update(int type, Vec3f* pos, float radius, s8 team, u16 flags) {
    Vec3f displacement = *pos - m_pos;
    // Saved here; below this becomes the previous-minus-current acceleration.
    Vec3f accelerationChange = m_acceleration;
    m_acceleration = m_velocity;
    m_velocity = displacement;
    m_acceleration = m_velocity - m_acceleration;
    m_type = type;
    m_radius = radius;
    m_team = team;
    m_pos = *pos;
    float distanceSq = displacement.m_z * displacement.m_z +
                       (displacement.m_x * displacement.m_x + displacement.m_y * displacement.m_y);
    double absolute = __fabs(distanceSq);
    float magnitude = absolute;
    float speed;
    if (magnitude <= 1.17549435e-38f) {
        speed = 0.0f;
    } else {
        speed = distanceSq * rsqrtf(distanceSq);
    }
    Vec3fSub(&accelerationChange, &accelerationChange, &m_acceleration);
    m_speed = speed;
    m_flags = (m_flags & ~0x76) | flags;
    m_floorBlk = 0;
    float changeSq = accelerationChange.m_z * accelerationChange.m_z +
                     (accelerationChange.m_x * accelerationChange.m_x + accelerationChange.m_y * accelerationChange.m_y);
    if (changeSq < 0.001f) {
        m_flags |= 2;
    } else {
        Vec2f direction(m_velocity.m_x, m_velocity.m_y);
        direction.normalize();
        if (direction.m_y < 0.01f && accelerationChange.m_x < 0.01f) {
            m_flags |= 2;
        } else if (direction.m_x < 0.01f && accelerationChange.m_y < 0.01f) {
            m_flags |= 2;
        }
    }
    // HYPOTHESIS: this weapon kind is deliberately forced to sampled motion;
    // its gameplay identity has not yet been corroborated here.
    if (type == 0 && m_kind == 0x63) {
        m_flags &= ~2;
    }
    if (--m_sampleFrames <= 0) {
        m_oldSamplePos = m_samplePos;
        m_samplePos = *pos;
        m_sampleFrames = 16;
    }
    m_updates++;
}

int aiWeapon::predict(Vec3f* pos, float radius, int frames) {
    Vec3f queryPos = *pos;
    int time = predict_core(&queryPos, radius, frames);
    float absZ = __fabsf(m_pos.m_z);
    if (time >= 0 && absZ > m_radius) {
        float absSpeedZ = __fabsf(m_velocity.m_z);
        if (absSpeedZ < 0.01f ||
            (m_velocity.m_z > 0.0f && m_pos.m_z > 0.0f) ||
            (m_velocity.m_z < 0.0f && m_pos.m_z < 0.0f)) {
            time = -1;
        } else {
            int depthTime = int(absZ / absSpeedZ);
            if (time < depthTime) {
                time = depthTime;
            }
        }
    }
    return time;
}

// The rectangular broad phase includes an acceleration turning point. The
// narrow phase intersects the two candidate contact intervals on each axis.
// Keep the native half-frame integration and asymmetric axis extents: these
// queries are the CPU's approximation, rather than the collision engine.
int aiWeapon::predict_core(Vec3f* pos, float radius, int frames) {
    int motion = 0;
    float radiusX = radius;
    if (m_velocity.m_x * (pos->m_x - m_pos.m_x) <= 0.0f) {
        radiusX = radius * 0.3f;
    }
    float radiusY = radius;
    if (m_velocity.m_y * (pos->m_y - m_pos.m_y) <= 0.0f) {
        radiusY = radius * 0.3f;
    }
    float weaponRadius = m_radius * 0.7f;
    radiusX += weaponRadius;
    radius += weaponRadius;
    radiusY += weaponRadius;
    if (pos->m_x - radiusX <= m_pos.m_x && m_pos.m_x <= pos->m_x + radiusX &&
        pos->m_y - radiusY <= m_pos.m_y && m_pos.m_y <= pos->m_y + radiusY) {
        return 0;
    }

    Vec2f velocity;
    Vec2f start;
    Vec2f end;
    if ((m_flags & 2) != 0) {
        velocity.m_x = m_velocity.m_x;
        velocity.m_y = m_velocity.m_y;
        float halfFrameSq = (float(frames) + 0.5f) * (float(frames) + 0.5f);
        float accelSq = m_acceleration.m_z * m_acceleration.m_z +
                        (m_acceleration.m_x * m_acceleration.m_x + m_acceleration.m_y * m_acceleration.m_y);
        // The native inverse branch also selects this path for unordered input.
        if (!(accelSq < 0.00001f)) {
            motion = 1;
            float accel = m_acceleration.m_x;
            float speed = m_velocity.m_x;
            float x = m_pos.m_x;
            if (speed * accel >= 0.0f) {
                start.m_x = x;
                end.m_x = x + speed * float(frames) + (0.5f * (accel * halfFrameSq) - 0.125f * accel);
            } else {
                float turn = __fabsf(speed / accel);
                start.m_x = x + speed * turn + (0.5f * ((turn + 0.5f) * (accel * (turn + 0.5f))) - 0.125f * accel);
                if (turn < float(frames) * 0.5f) {
                    end.m_x = x + speed * float(frames) + (0.5f * (accel * halfFrameSq) - 0.125f * accel);
                } else {
                    end.m_x = x;
                }
                motion = 2;
            }
            accel = m_acceleration.m_y;
            speed = m_velocity.m_y;
            float y = m_pos.m_y;
            if (speed * accel >= 0.0f) {
                start.m_y = y;
                end.m_y = y + speed * float(frames) + (0.5f * (accel * halfFrameSq) - 0.125f * accel);
            } else {
                float turn = __fabsf(speed / accel);
                start.m_y = y + speed * turn + (0.5f * ((turn + 0.5f) * (accel * (turn + 0.5f))) - 0.125f * accel);
                if (turn < float(frames) * 0.5f) {
                    end.m_y = y + speed * float(frames) + (0.5f * (accel * halfFrameSq) - 0.125f * accel);
                } else {
                    end.m_y = y;
                }
                motion = 2;
            }
        } else {
            start.m_x = m_pos.m_x;
            start.m_y = m_pos.m_y;
            Vec3f travel;
            Vec3fScale(&travel, &m_velocity, float(frames));
            Vec3f future;
            Vec3fAdd(&future, &m_pos, &travel);
            end.m_x = future.m_x;
            end.m_y = future.m_y;
        }
    } else {
        if (m_updates == 0) {
            return -1;
        }
        start.m_x = m_pos.m_x;
        start.m_y = m_pos.m_y;
        Vec3f displacement;
        Vec3fSub(&displacement, &m_pos, &m_oldSamplePos);
        int sampleSpan = (m_updates < 16 ? 16 : 32) - m_sampleFrames;
        Vec3f average;
        Vec3fScale(&average, &displacement, 1.0f / float(sampleSpan));
        Vec2f travel(average.m_x * float(frames), average.m_y * float(frames));
        end.m_x = start.m_x + travel.m_x;
        end.m_y = start.m_y + travel.m_y;
        velocity.m_x = average.m_x;
        velocity.m_y = average.m_y;
    }

    // Reject only when both endpoints lie strictly outside the same edge.
    if (((pos->m_x - radiusX > start.m_x) && (pos->m_x - radiusX > end.m_x)) ||
        ((pos->m_x + radiusX < start.m_x) && (pos->m_x + radiusX < end.m_x)) ||
        ((pos->m_y - radiusY > start.m_y) && (pos->m_y - radiusY > end.m_y)) ||
        ((pos->m_y + radiusY < start.m_y) && (pos->m_y + radiusY < end.m_y))) {
        return -1;
    }
    if (motion >= 1 && motion < 3) {
        float xEntry0, xExit0, xEntry1, xExit1;
        float yEntry0, yExit0, yEntry1, yExit1;
        if (!aiweapon::calc_acc(pos->m_x, velocity.m_x, m_acceleration.m_x, radius, m_pos.m_x,
                                &xEntry0, &xExit0, &xEntry1, &xExit1)) {
            if (!(pos->m_x - radiusX <= m_pos.m_x && m_pos.m_x <= pos->m_x + radiusX)) {
                return -1;
            }
            xEntry0 = 0.0f;
            xExit0 = float(frames);
            xExit1 = 99999.9f;
            xEntry1 = 99999.9f;
        }
        if (!aiweapon::calc_acc(pos->m_y, velocity.m_y, m_acceleration.m_y, radius, m_pos.m_y,
                                &yEntry0, &yExit0, &yEntry1, &yExit1)) {
            if (!(pos->m_y - radiusY <= m_pos.m_y && m_pos.m_y <= pos->m_y + radiusY)) {
                return -1;
            }
            yEntry0 = 0.0f;
            yExit0 = float(frames);
            yExit1 = 99999.9f;
            yEntry1 = 99999.9f;
        }
        if (yEntry0 <= float(frames) && yExit0 >= 0.0f) {
            if (yEntry0 < 0.0f) yEntry0 = 0.0f;
        } else {
            yExit0 = -1.0f;
        }
        if (yEntry1 <= float(frames) && yExit1 >= 0.0f) {
            if (yEntry1 < 0.0f) yEntry1 = 0.0f;
        } else {
            yExit1 = -1.0f;
        }
        if (xEntry0 <= float(frames) && xExit0 >= 0.0f) {
            if (xEntry0 < 0.0f) xEntry0 = 0.0f;
            if (yEntry0 <= xExit0 && xEntry0 <= yExit0) {
                return int(xEntry0 - yEntry0 >= 0.0f ? xEntry0 : yEntry0);
            }
            if (yEntry1 <= xExit0 && xEntry0 <= yExit1) {
                return int(xEntry0 - yEntry1 >= 0.0f ? xEntry0 : yEntry1);
            }
        }
        if (xEntry1 <= float(frames) && xExit1 >= 0.0f) {
            if (xEntry1 < 0.0f) xEntry1 = 0.0f;
            if (yEntry0 <= xExit1 && xEntry1 <= yExit0) {
                return int(xEntry1 - yEntry0 >= 0.0f ? xEntry1 : yEntry0);
            }
            if (yEntry1 <= xExit1 && xEntry1 <= yExit1) {
                return int(xEntry1 - yEntry1 >= 0.0f ? xEntry1 : yEntry1);
            }
        }
        return -1;
    }

    Vec3fSub(pos, pos, &m_pos);
    if (__fabsf(velocity.m_x) > 0.001f) {
        if (velocity.m_x < 0.0f) {
            velocity.m_x *= -1.0f;
            pos->m_x *= -1.0f;
        }
        float time = (pos->m_x - radiusX) / velocity.m_x;
        float y = velocity.m_y * time;
        // Native X entry test uses the X extent on both coordinates.
        if (pos->m_y - radiusX <= y && y <= pos->m_y + radiusX) {
            return int(time);
        }
    }
    if (__fabsf(velocity.m_y) > 0.001f) {
        if (velocity.m_y < 0.0f) {
            velocity.m_y *= -1.0f;
            pos->m_y *= -1.0f;
        }
        float time = (pos->m_y - radiusY) / velocity.m_y;
        float x = velocity.m_x * time;
        // The corresponding Y entry test uses the Y extent on both coordinates.
        if (pos->m_x - radiusY <= x && x <= pos->m_x + radiusY) {
            return int(time);
        }
    }
    return -1;
}

// HYPOTHESIS: get_under_floor's other output remains opaque. Its register ABI
// is verified here; the complete collision-query signature is not recovered.
class aiMgr;
extern aiMgr* g_aiMgr;

AiClBlock* aiWeapon::getFloorBlk() {
    if ((m_flags & 4) == 0) {
        Vec3f above = m_pos + Vec3f(0.0f, 5.0f, 0.0f);
        g_aiMgr->get_under_floor(&above, &m_floorBlk, NULL, -1, -9999.9f, 0.0f, 0.0f);
        m_flags |= 4;
    }
    return m_floorBlk;
}

// The prediction core applies this independently to the X and Y axes, then
// intersects the candidate time intervals. Its half-frame terms and sentinels
// are part of the native discrete-motion calculation.
// HYPOTHESIS: aiweapon is a namespace rather than a class with static helpers;
// the map spelling and the absence of a receiver establish the calling ABI.
namespace aiweapon {
int calc_acc(float target, float velocity, float acceleration, float radius, float position,
             float* entry0, float* exit0, float* entry1, float* exit1) {
    float distance = target - position;
    float absAcceleration = __fabsf(acceleration);
    if (absAcceleration > 0.001f) {
        float turn = -(velocity / acceleration);
        if (acceleration < 0.0f) {
            distance *= -1.0f;
            velocity *= -1.0f;
            acceleration *= -1.0f;
        }
        float h = 0.5f + turn;
        float offset = velocity * turn + (0.5f * (h * (acceleration * h)) - 0.125f * acceleration);
        float nearValue = 0.125f * acceleration + ((distance - radius) - offset);
        float nearRoot;
        if (nearValue < 0.0f) {
            nearRoot = -999999.0f;
        } else {
            nearRoot = mtSqrtf(nearValue / (0.5f * acceleration)) - 0.5f;
        }
        float farValue = 0.125f * acceleration + ((distance + radius) - offset);
        float farRoot;
        if (farValue < 0.0f) {
            farRoot = -999999.0f;
        } else {
            farRoot = mtSqrtf(farValue / (0.5f * acceleration)) - 0.5f;
        }
        *entry0 = turn - farRoot;
        *exit0 = turn - nearRoot;
        *entry1 = turn + nearRoot;
        *exit1 = turn + farRoot;
        return 1;
    }
    float absVelocity = __fabsf(velocity);
    if (absVelocity > 0.001f) {
        if (velocity < 0.0f) {
            distance *= -1.0f;
            velocity *= -1.0f;
        }
        *entry0 = (distance - radius) / velocity;
        *exit0 = (distance + radius) / velocity;
        *entry1 = 99999.0f;
        *exit1 = 99999.0f;
        return 1;
    }
    return 0;
}
}
