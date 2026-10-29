#include <sora_adv_stage/gr_gimmick_block.h>

// HYPOTHESIS: messageBreak/[grAdventureBlockMng] (sora_adv_stage map), not decompiled yet.
extern "C" void fn_40_1D200(void* blockMng, grGimmickBlock* block);

grGimmickBlock* grGimmickBlock::create(int mdlIndex, const char* taskName) {
    grGimmickBlock* ground = new (Heaps::StageInstance) grGimmickBlock(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grGimmickBlock::grGimmickBlock(const char* taskName) : grGimmick(taskName) {
    unk14C = 0;
    m_isBroken = 0;
    m_hp = 0.0f;
    m_blockData = NULL;
    m_blockMng = NULL;
}

grGimmickBlock::~grGimmickBlock() { }

void grGimmickBlock::update(float deltaFrame) {
    m_hasUpdatedG3dCalcWorld = true;
}

void grGimmickBlock::requestBreak(float damage) {
    if (!m_isBroken) {
        m_hp -= 20.0f * damage;
        if (m_hp <= 0.0f) {
            breakBlock();
            startGimmickSE(0);
            startGimmickEffect(0);
        } else {
            startGimmickSE(1);
            startGimmickEffect(1);
        }
    }
}

void grGimmickBlock::breakBlock() {
    setVisibility(0);
    setEnableCollisionStatus(false);
    m_isBroken = 1;
}

void grGimmickBlock::invalidatedByCameraClipping() { }

void grGimmickBlock::dbDispInvalidatedByCameraClippingSphere() { }

void grGimmickBlock::receiveCollMsg_Attack(grCollStatus* collStatus, grCollisionJoint* collisionJoint) {
    requestBreak(50.0f);
    fn_40_1D200(m_blockMng, this);
}
