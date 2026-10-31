#define MT_VEC3F_SUB_COMPACT
#include <ai/ai_enemy.h>
#include <ai/ai_mgr.h>
#include <em/em_manager.h>
#include <gf/gf_memory_util.h>
#include <gm/gm_lib.h>
#include <so/so_external_value_accesser.h>
#include <st/st_enemy_id_manager.h>
#include <st_madein/st_madein.h>

extern "C" int fn_27_8CC2C(StageObject* obj, int kind);  // soExternalValueAccesser::isGroundTouch

aiEnemyMgr::aiEnemyMgr() {
    gfMemFill(this, 0, sizeof(aiEnemyMgr));
    for (int i = 0; i < 32; i++) {
        m_enemies[i].m_index = i;
    }
}

aiEnemyMgr::~aiEnemyMgr() { }

void aiEnemyMgr::reset() {
    gfMemFill(this, 0, sizeof(aiEnemyMgr));
    for (int i = 0; i < 32; i++) {
        m_enemies[i].m_index = i;
    }
}

void aiEnemyMgr::update() {
    if (g_aiMgr->m_stageKind != Stages::Madein &&
        g_stEnemyIdManager == NULL) {
        return;
    }

    // Retire first, recycle next frame: existing target references must be
    // removed before a new object can take the same CPU target slot.
    aiEnemy* current = m_enemies;
    for (int i = 0; i < 32; i++, current++) {
        if ((current->m_flags & aiEnemy::Retired) != 0) {
            current->m_flags = 0;
        }
    }
    bool missing[32];
    for (int i = 0; i < 32; i++) {
        missing[i] = m_enemies[i].m_flags & aiEnemy::Active;
    }

    if (g_aiMgr->m_stageKind == Stages::Madein) {
        stMadeinAiData* data = reinterpret_cast<stMadeinAiData*>(aiMgr::getMadeinAiData(false));
        if (data != NULL) {
            switch (data->m_game) {
            case 6: // Narase: the three party crackers.
            case 7: { // Kudake: twelve breakable targets.
                int count = data->m_game == 6 ? 3 : 12;
                for (int target = 0; target < count; target++) {
                    u32 objId = target + 1;
                    Vec3f pos;
                    switch (data->m_game) {
                    case 6:
                        if (!data->m_narase.m_flag[target]) {
                            continue;
                        }
                        pos.m_x = data->m_narase.m_pos[target].m_x;
                        pos.m_y = data->m_narase.m_pos[target].m_y;
                        break;
                    case 7:
                        if (!data->m_kudake.m_flag[target]) {
                            continue;
                        }
                        pos.m_x = data->m_kudake.m_pos[target].m_x;
                        pos.m_y = data->m_kudake.m_pos[target].m_y;
                        break;
                    }
                    pos.m_z = 0.0f;

                    int slot;
                    aiEnemy* enemy = m_enemies;
                    for (slot = 0; slot < 32; slot++, enemy++) {
                        if (missing[slot] && enemy->m_objId == objId) {
                            break;
                        }
                    }
                    if (slot >= 32) {
                        enemy = m_enemies;
                        for (slot = 0; slot < 32; slot++, enemy++) {
                            if ((enemy->m_flags & aiEnemy::Active) == 0) {
                                break;
                            }
                        }
                        if (slot < 32) {
                            enemy->init(objId);
                            enemy->update(&pos);
                            m_count++;
                        }
                    } else {
                        enemy->update(&pos);
                        missing[slot] = false;
                    }
                }
                break;
            }
            }
        }
    } else if (g_stEnemyIdManager != NULL) {
        int count = g_stEnemyIdManager->getNumEnemies();
        for (int i = 0; i < count; i++) {
            u32 objId = g_stEnemyIdManager->getEnemyId(i);
            int slot;
            aiEnemy* enemy = m_enemies;
            for (slot = 0; slot < 32; slot++, enemy++) {
                if (missing[slot] && enemy->m_objId == objId) {
                    break;
                }
            }
            if (slot >= 32) {
                enemy = m_enemies;
                for (slot = 0; slot < 32; slot++, enemy++) {
                    if ((enemy->m_flags & aiEnemy::Active) == 0) {
                        break;
                    }
                }
                if (slot < 32) {
                    enemy->init(objId);
                    enemy->update();
                    m_count++;
                }
            } else {
                enemy->update();
                missing[slot] = false;
            }
        }
    }

    for (int i = 0; i < 32; i++) {
        if (missing[i]) {
            // Fighter and partner slots occupy 0..13; enemies begin at 14.
            g_aiMgr->removeTgtRefer(i + 14, 0);
            m_enemies[i].m_flags |= aiEnemy::Retired;
            m_count--;
        }
    }
}

aiEnemy* aiEnemyMgr::getEnemy(int index) {
    if (index >= 0 && index < 32) {
        aiEnemy* enemy = &m_enemies[index];
        if ((enemy->m_flags & aiEnemy::Active) != 0) {
            return enemy;
        }
    }
    return NULL;
}

// Retired slots remain addressable for one frame, but cannot be picked as a
// new target. Stop after examining the manager's count of live enemies.
aiEnemy* aiEnemyMgr::getEnemyNear(Vec3f* pos, float* distanceSq) {
    int i;
    aiEnemy* enemy;
    int remaining = m_count;
    if (remaining > 0) {
        float best = -1.0f;
        enemy = m_enemies;
        int bestIndex = -1;
        for (i = 0; i < 32; i++, enemy++) {
            if ((enemy->m_flags & aiEnemy::Active) != 0 && (enemy->m_flags & aiEnemy::Retired) == 0) {
                Vec3f diff;
                Vec3fSub(&diff, &enemy->m_pos, pos);
                float dist = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
                if (bestIndex < 0 || best > dist) {
                    best = dist;
                    bestIndex = i;
                }
                if (--remaining <= 0) {
                    break;
                }
            }
        }
        if (bestIndex >= 0) {
            if (distanceSq != NULL) {
                *distanceSq = best;
            }
            return &m_enemies[bestIndex];
        }
    }
    return NULL;
}

void aiEnemy::update() {
    StageObject* obj = emManager::getInstance()->getEnemyRefFromId(m_objId);
    m_flags = Active;
    m_status = soExternalValueAccesser::getStatusKind(obj);
    if (soExternalValueAccesser::getSituationKind(obj) != Situation_Outfield) {
        Vec2f move;
        move.m_x = m_pos.m_x;
        move.m_y = m_pos.m_y;
        m_pos = soExternalValueAccesser::getPos(obj);
        move.m_x = m_pos.m_x - move.m_x;
        move.m_y = m_pos.m_y - move.m_y;
        m_move = move;
    }
    m_lr = soExternalValueAccesser::getLr(obj);
    if ((u32)fn_27_8CC2C(obj, 8) == 1) {
        m_flags |= GroundTouch;
    }
}
