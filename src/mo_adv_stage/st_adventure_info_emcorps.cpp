#include <sora_adv_stage/st_adventure_info_emcorps.h>

void stAdventureInfoEmCorps::init() { }

void stAdventureInfoEmCorps::update() { }

void stAdventureInfoEmCorps::setEmPopCount(u32 count) {
    addZakoCorps(count);
    m_isActive = true;
}

void stAdventureInfoEmCorps::notifyDead() {
    if (m_isActive) {
        removeZakoCorps();
    }
}

void stAdventureInfoEmCorps::remove() {
    m_isActive = false;
    sendToRemoveCarpsInfo();
}
