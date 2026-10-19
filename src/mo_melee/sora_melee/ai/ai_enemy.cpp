#include <ai/ai_enemy.h>
#include <em/em_manager.h>
#include <gf/gf_memory_util.h>
#include <so/so_external_value_accesser.h>

// emManager::getEnemyRefFromId (not declared by BrawlHeaders); returns the enemy as a StageObject.
extern "C" StageObject* fn_41_CECC(emManager* manager, int createId);
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

aiEnemy* aiEnemyMgr::getEnemy(int index) {
    if (index >= 0 && index < 32) {
        aiEnemy* enemy = &m_enemies[index];
        if ((enemy->m_flags & 1) != 0) {
            return enemy;
        }
    }
    return NULL;
}

aiEnemy* aiEnemyMgr::getEnemyNear(Vec3f* pos, float* distanceSq) {
    int remaining = m_count;
    if (remaining > 0) {
        float best = -1.0f;
        int bestIndex = -1;
        for (int i = 0; i < 32; i++) {
            aiEnemy* enemy = &m_enemies[i];
            if ((enemy->m_flags & 1) != 0 && (enemy->m_flags & 4) == 0) {
                Vec3f diff = enemy->m_pos - *pos;
                float dist = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
                if (bestIndex < 0 || best > dist) {
                    bestIndex = i;
                    best = dist;
                }
                remaining--;
                if (remaining < 1) {
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
    StageObject* obj = fn_41_CECC(emManager::getInstance(), m_objId);
    m_flags = 1;
    m_status = soExternalValueAccesser::getStatusKind(obj);
    if (soExternalValueAccesser::getSituationKind(obj) != 5) {
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
        m_flags |= 2;
    }
}
