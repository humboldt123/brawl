#include <em/enemy.h>
#include <sora_adv_stage/st_adventure_info_emboss.h>

extern "C" void fn_40_15B48(stAdventureInfoContact* contact);

void stAdventureInfoEmBoss::setInfoParam(int createId) {
    Enemy* enemy = getEnemy(createId);
    if (enemy != NULL && enemy->getEnemyKind() == 0x2E) {
        m_params[0] = getIconParamForEnemy(createId, m_packnWnLeft, 0, &m_params[0]);
        m_params[1] = getIconParamForEnemy(createId, m_packnWnRight, 1, &m_params[1]);
    } else if (m_isPair) {
        if (enemy != NULL && enemy->getEnemyKind() == 0x3B) {
            m_params[0] = getIconParamForEnemy(createId, enemy->getTaskId(), 0, &m_params[0]);
        }
        Enemy* subEnemy = getEnemy(m_subCreateId);
        if (subEnemy != NULL && subEnemy->getEnemyKind() == 0x3C) {
            m_params[1] = getIconParamForEnemy(m_subCreateId, subEnemy->getTaskId(), 1, &m_params[1]);
        }
    } else if (enemy != NULL) {
        m_params[0] = getIconParamForEnemy(createId, enemy->getTaskId(), 0, &m_params[0]);
    }
}

bool stAdventureInfoEmBoss::isBoss(int createId) {
    int kinds[10] = {0x2E, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x3A, 0x3B, 0x3C};
    Enemy* enemy = getEnemy(createId);
    for (int i = 0; i < 10; i++) {
        if (enemy != NULL && kinds[i] == enemy->getEnemyKind()) {
            return true;
        }
    }
    return false;
}

void stAdventureInfoEmBoss::setBossPacknWnLeft(u32 createId) {
    m_packnWnLeft = createId;
}

void stAdventureInfoEmBoss::setBossPacknWnRight(u32 createId) {
    m_packnWnRight = createId;
}

void stAdventureInfoEmBoss::remove() {
    m_isActive = false;
    fn_40_15B48(this);
}
