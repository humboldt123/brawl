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

void stAdventureInfoEmZako::update() {
    for (int i = 0; i < 2; i++) {
        Slot* slot = &m_slots[i];
        if (slot->m_isDisp) {
            int createId = slot->m_createId;
            Enemy* enemy = getEnemy(createId);
            if (enemy != NULL) {
                slot->m_param = getIconParamForEnemy(createId, enemy->getTaskId(), i, &slot->m_param);
            }
            sendToZakoInfo(slot->m_param);
            if (slot->m_isDamage) {
                sendToDamageZakoInfo(i);
                slot->m_isDamage = false;
            }
            if (slot->m_isDead) {
                sendToDestroyZakoInfo(i);
                slot->m_isDisp = false;
                slot->m_isDead = false;
                slot->m_createId = -1;
            }
            if (slot->m_dispFrame > 300) {
                sendToEraseZakoInfo(i);
                slot->m_isDisp = false;
            } else {
                slot->m_dispFrame++;
            }
        }
    }
}

bool stAdventureInfoEmZako::isZako(int createId) {
    int kinds[10] = {0x2E, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x3A, 0x3B, 0x3C};
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
