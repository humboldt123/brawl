#include <memory.h>
#include <sora_adv_stage/st_adventure_info_mediation.h>

static stAdventureInfoMediation* s_instance;

stAdventureInfoMediation* stAdventureInfoMediation::getInstance() {
    return s_instance;
}

void stAdventureInfoMediation::create() {
    s_instance = new (Heaps::StageInstance) stAdventureInfoMediation();
}

void stAdventureInfoMediation::remove() {
    s_instance->exit();
    s_instance = NULL;
}

stAdventureInfoMediation::~stAdventureInfoMediation() {
    m_emBoss.remove();
    m_emCorps.remove();
    m_emZako.remove();
}

stAdventureInfoEmBoss* stAdventureInfoMediation::getEmBoss() {
    return &m_emBoss;
}

stAdventureInfoEmCorps* stAdventureInfoMediation::getEmCorps() {
    return &m_emCorps;
}

stAdventureInfoEmZako* stAdventureInfoMediation::getEmZako() {
    return &m_emZako;
}

void stAdventureInfoMediation::notifyStepJump() {
    m_emBoss.remove();
    m_emCorps.remove();
    m_emZako.remove();
}

void stAdventureInfoMediation::processUpdate() { }

void stAdventureInfoMediation::processEnd() {
    if (m_emBoss.m_isActive) {
        m_emBoss.update();
    }
    if (m_emCorps.m_isActive) {
        m_emCorps.update();
    }
    if (m_emZako.m_isActive) {
        m_emZako.update();
    }
}
