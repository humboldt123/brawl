// stAdventureInfoEmBoss: the Subspace Emissary boss health display. A boss enemy that spawns registers here (init), and
// every frame update reads its icon and health through the base's getIconParamForEnemy and forwards them, with damage and
// death events, through stAdventureInfoContact to the adventure HUD (IfAdvMngr) and the player HUD (IfMngr); the
// contact's isStamina parameter is fed from the same adventure data flag as m_isPair (see the HYPOTHESIS on update). It tracks two slots: Petey Piranha fills both with his two cages (the task ids set by
// setBossPacknWnLeft/Right), and the Master Hand and Crazy Hand pair uses slot 0 and slot 1 (m_isPair, from the
// adventure data flags); every other boss uses slot 0 only.
#include <em/em_create.h>
#include <em/enemy.h>
#include <gm/gm_global.h>
#include <sora_adv_stage/st_adventure_info_emboss.h>

// BYTES: the boss enemy kinds are a writable file-level table that isBoss copies to its stack (the original keeps it
// in .data before the vtable); a plain local array initializer would be a .rodata template instead.
struct stAdventureInfoBossKinds {
    int m_kinds[10];
};
static stAdventureInfoBossKinds s_bossKinds = {{
    Enemy_Boss_BossPackun, Enemy_Boss_Galleom,    Enemy_Boss_Ridley,     Enemy_Boss_Rayquaza,
    Enemy_Boss_Duon,       Enemy_Boss_Porky,      Enemy_Boss_MetaRidley, Enemy_Boss_Tabuu,
    Enemy_Boss_MasterHand, Enemy_Boss_CrazyHand,
}};

void stAdventureInfoEmBoss::init(int createId) {
    if (isBoss(createId)) {
        m_isPair = g_GameGlobal->m_advData->m_flags.m_unk1;
        if (m_isPair) {
            Enemy* enemy = getEnemy(createId);
            bool isSub;
            if (enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_CrazyHand) {
                isSub = true;
            } else {
                isSub = false;
            }
            if (isSub) {
                m_subCreateId = createId;
                return;
            }
        }
        m_createId = createId;
        m_isActive = true;
    }
}

// HYPOTHESIS: the flag passed on as the stamina parameter is the same advData flag that init stores in m_isPair.
void stAdventureInfoEmBoss::update() {
    if (m_isActive) {
        setInfoParam(m_createId);
        for (int i = 0; i < 2; i++) {
            if (m_params[i].m_isValid) {
                bool isPair = g_GameGlobal->m_advData->m_flags.m_unk1 != 0;
                if (!m_isDead[i]) {
                    sendToBossInfo(m_params[i], isPair);
                    if (m_isDamage[i]) {
                        sendToDamageBossInfo(i, isPair);
                        m_isDamage[i] = false;
                    }
                } else {
                    sendToBossInfo(m_params[i], isPair);
                    sendToDestroyBossInfo(i, isPair);
                    m_params[i].m_isValid = false;
                }
            }
        }
        if (!m_params[0].m_isValid && !m_params[1].m_isValid) {
            m_isActive = false;
        }
    }
}

void stAdventureInfoEmBoss::setInfoParam(int createId) {
    Enemy* enemy = getEnemy(createId);
    if (enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_BossPackun) {
        stAdventureInfoIconParam param0 = getIconParamForEnemy(createId, m_packnWnLeft, 0, &m_params[0]);
        m_params[0] = param0;
        stAdventureInfoIconParam param1 = getIconParamForEnemy(createId, m_packnWnRight, 1, &m_params[1]);
        m_params[1] = param1;
    } else if (m_isPair) {
        if (enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_MasterHand) {
            stAdventureInfoIconParam param0 = getIconParamForEnemy(createId, enemy->getTaskId(), 0, &m_params[0]);
            m_params[0] = param0;
        }
        Enemy* subEnemy = getEnemy(m_subCreateId);
        if (subEnemy != NULL && subEnemy->getEnemyKind() == Enemy_Boss_CrazyHand) {
            stAdventureInfoIconParam param1 = getIconParamForEnemy(m_subCreateId, subEnemy->getTaskId(), 1, &m_params[1]);
            m_params[1] = param1;
        }
    } else if (enemy != NULL) {
        stAdventureInfoIconParam param0 = getIconParamForEnemy(createId, enemy->getTaskId(), 0, &m_params[0]);
        m_params[0] = param0;
    }
}

bool stAdventureInfoEmBoss::isBoss(int createId) {
    stAdventureInfoBossKinds bossKinds = s_bossKinds;
    int* kinds = bossKinds.m_kinds;
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

void stAdventureInfoEmBoss::notifyDead(u32 createId) {
    Enemy* enemy = getEnemy(createId);
    if (enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_BossPackun) {
        m_isDead[0] = true;
        m_isDead[1] = true;
    } else if (m_isPair && enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_MasterHand) {
        m_isDead[0] = true;
    } else if (m_isPair && enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_CrazyHand) {
        m_isDead[1] = true;
    } else if (isBoss(createId)) {
        m_isDead[0] = true;
    }
}

void stAdventureInfoEmBoss::notifyDamage(int createId, u32 taskId) {
    if (m_packnWnLeft == taskId) {
        m_isDamage[0] = true;
    }
    if (m_packnWnRight == taskId) {
        m_isDamage[1] = true;
    }
    if (isBoss(createId)) {
        Enemy* enemy = getEnemy(createId);
        if (enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_BossPackun) {
            m_isDamage[0] = true;
            m_isDamage[1] = true;
        } else if (m_isPair && enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_MasterHand) {
            m_isDamage[0] = true;
        } else if (m_isPair && enemy != NULL && enemy->getEnemyKind() == Enemy_Boss_CrazyHand) {
            m_isDamage[1] = true;
        } else {
            m_isDamage[0] = true;
        }
    }
}

void stAdventureInfoEmBoss::remove() {
    m_isActive = false;
    sendToRemoveBossInfo();
}
