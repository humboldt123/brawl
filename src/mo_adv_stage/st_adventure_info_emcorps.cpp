// stAdventureInfoEmCorps: the Subspace Emissary enemy-group counter. When a group of enemies pops (setEmPopCount) the
// adventure HUD gets one corps marker per enemy, each death removes one, and remove clears what is left. The HUD calls
// go through stAdventureInfoContact.
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
