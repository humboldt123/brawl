#include <sora_adv_stage/gr_adventure_mine.h>

void grAdventureMine::processUpdate() {
    if (m_isUpdate) {
        grGimmick::processUpdate();
    }
}

void grAdventureMine::update(float deltaFrame) {
    grGimmick::updateCallback(0);
    m_isUpdate = false;
}
