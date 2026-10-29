#include <sora_adv_stage/gr_adventure_motion_attack_ground.h>

grAdventureMotionAttackGround* grAdventureMotionAttackGround::create(int mdlIndex, const char* taskName) {
    grAdventureMotionAttackGround* ground = new (Heaps::StageInstance) grAdventureMotionAttackGround(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMotionAttackGround::grAdventureMotionAttackGround(const char* taskName) : grAdventureMotionGround(taskName) {
    m_attackGroundData = NULL;
    unk158 = 0;
    unk15C = 0;
}

grAdventureMotionAttackGround::~grAdventureMotionAttackGround() { }

void grAdventureMotionAttackGround::setMotionAttack() {
    grYakumono::setAttackGimmick(0, 0, 0, &m_attackGroundData->m_attackData, NULL);
}

void grAdventureMotionAttackGround::setValid(u32 unk1) {
    grYakumono::setValid(unk1);
    if (unk1 == 1) {
        grYakumono::setAttackGimmick(0, 0, 0, &m_attackGroundData->m_attackData, NULL);
    }
}

void grAdventureMotionAttackGround::setValidAttachedEffect(u32 unk1) {
    grYakumono::setValidAttachedEffect(unk1);
    if (unk1 == 1) {
        grYakumono::setAttackGimmick(0, 0, 0, &m_attackGroundData->m_attackData, NULL);
    }
}
