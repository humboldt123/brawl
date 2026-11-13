// stAdventureInfoEmZako: the Subspace Emissary health display for ordinary (non-boss) enemies. It has two slots, which
// init fills (by index) for any enemy that isBoss would not claim. update refreshes each slot's icon parameters, forwards
// them with damage and death events to the adventure HUD through stAdventureInfoContact, and drops the slot 300 frames
// after it was shown or when its enemy dies.
#include <em/em_create.h>
#include <em/enemy.h>
#include <sora_adv_stage/st_adventure_info_emzako.h>

void stAdventureInfoEmZako::init(int createId, int index) {
    if (isZako(createId)) {
        Slot* slot = &m_slots[index];
        slot->m_createId = createId;
        slot->m_isDisp = true;
        slot->m_isDamage = true;
        slot->m_dispFrame = 0;
        slot->m_param.m_isValid = false;
        slot->m_param.unk15 = false;
        slot->m_param.m_faceTexData = NULL;
        m_isActive = true;
    }
}

// HYPOTHESIS: an inlined per-slot refresh, the zako counterpart of stAdventureInfoEmBoss::setInfoParam (no symbol of
// its own survives).
inline void stAdventureInfoEmZako::setInfoParam(int createId, int index) {
    Enemy* enemy = getEnemy(createId);
    if (enemy != NULL) {
        m_slots[index].m_param = getIconParamForEnemy(createId, enemy->getTaskId(), index, &m_slots[index].m_param);
    }
}

void stAdventureInfoEmZako::update() {
    // BYTES: an s32 (long) counter handed to the int parameter of setInfoParam; the conversion keeps the inlined
    // slot accesses on their own loop pointers, as in the original (with an int counter they merge, 93.05%).
    for (s32 i = 0; i < 2; i++) {
        if (m_slots[i].m_isDisp) {
            setInfoParam(m_slots[i].m_createId, i);
            sendToZakoInfo(m_slots[i].m_param);
            if (m_slots[i].m_isDamage) {
                sendToDamageZakoInfo(i);
                m_slots[i].m_isDamage = false;
            }
            if (m_slots[i].m_isDead) {
                sendToDestroyZakoInfo(i);
                m_slots[i].m_isDisp = false;
                m_slots[i].m_isDead = false;
                m_slots[i].m_createId = -1;
            }
            if (m_slots[i].m_dispFrame > 300) {
                sendToEraseZakoInfo(i);
                m_slots[i].m_isDisp = false;
            } else {
                m_slots[i].m_dispFrame++;
            }
        }
    }
}

bool stAdventureInfoEmZako::isZako(int createId) {
    // The boss kinds (the same list as stAdventureInfoEmBoss::isBoss); here a local .rodata template.
    int kinds[10] = {
        Enemy_Boss_BossPackun, Enemy_Boss_Galleom,    Enemy_Boss_Ridley,     Enemy_Boss_Rayquaza,
        Enemy_Boss_Duon,       Enemy_Boss_Porky,      Enemy_Boss_MetaRidley, Enemy_Boss_Tabuu,
        Enemy_Boss_MasterHand, Enemy_Boss_CrazyHand,
    };
    Enemy* enemy = getEnemy(createId);
    for (int i = 0; i < 10; i++) {
        if (enemy != NULL && kinds[i] == enemy->getEnemyKind()) {
            return false;
        }
    }
    return true;
}

void stAdventureInfoEmZako::notifyDead(u32 createId) {
    for (int i = 0; i < 2; i++) {
        if (createId == m_slots[i].m_createId) {
            m_slots[i].m_isDead = true;
        }
    }
}

void stAdventureInfoEmZako::remove() {
    m_isActive = false;
    sendToRemoveZakoInfo();
}
