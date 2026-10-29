#include <sora_adv_stage/gr_adventure_move_attack_ground.h>

void grAdventureMoveAttackGround::setAttackMoveGround() {
    grGimmickMoveAttackGroundData* data = static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData);
    grYakumono::setAttackGimmick(0, 0, data->m_attackData.m_nodeIndex, &data->m_attackData, NULL);
}

void grAdventureMoveAttackGround::setEffect(EfID effectId) {
    m_effectId = effectId;
    m_effectHandle = g_ecMgr->setEffect(effectId);
}

void grAdventureMoveAttackGround::setValid(u32 unk1) {
    grYakumono::setValid(unk1);
    if (unk1 == 1) {
        grYakumono::disableAttack(0);
        grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->m_attackData,
                                     NULL);
    }
}

void grAdventureMoveAttackGround::setValidAttachedEffect(u32 unk1) {
    grYakumono::setValidAttachedEffect(unk1);
    if (unk1 == 1) {
        grYakumono::disableAttack(0);
        grYakumono::setAttackGimmick(0, 0, 0, &static_cast<grGimmickMoveAttackGroundData*>(m_moveAttackData)->m_attackData,
                                     NULL);
    }
}
