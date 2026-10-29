#include <sora_adv_stage/gr_adventure_enemy_creator.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

grAdventureEnemyCreator* grAdventureEnemyCreator::create(int mdlIndex, const char* taskName) {
    grAdventureEnemyCreator* ground = new (Heaps::StageInstance) grAdventureEnemyCreator(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureEnemyCreator::~grAdventureEnemyCreator() { }

void grAdventureEnemyCreator::setValid(u32 unk1) {
    grYakumono::setValid(unk1);
    if (unk1 == 0) {
        m_state = 5;
    } else {
        m_state = 0;
    }
}

void grAdventureEnemyCreator::setValidAttachedEffect(u32 unk1) {
    grYakumono::setValidAttachedEffect(unk1);
    if (unk1 == 0) {
        m_state = 5;
    } else {
        m_state = 0;
    }
}
