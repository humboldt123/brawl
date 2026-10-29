#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <types.h>

// HYPOTHESIS: The breakable block of the Subspace stages (sora_adv_stage gr_gimmick_block.o). The block manager and the
// block record are not described yet.
class grGimmickBlock : public grGimmick {
protected:
    u8 unk14C;            // 0x14C
    u8 m_isBroken;        // 0x14D, set by breakBlock
    float m_hp;           // 0x150, HYPOTHESIS: the durability that requestBreak lowers
    void* m_blockData;    // 0x154, the gimmick record of the block
    void* m_blockMng;     // 0x158, HYPOTHESIS: the grAdventureBlockMng that is told about breaks

public:
    grGimmickBlock(const char* taskName);
    virtual ~grGimmickBlock();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Attack(grCollStatus* collStatus, grCollisionJoint* collisionJoint);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void invalidatedByCameraClipping();
    virtual void dbDispInvalidatedByCameraClippingSphere();
    virtual void requestBreak(float damage);
    virtual void breakBlock();

    static grGimmickBlock* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grGimmickBlock) == 0x15C, "Class is wrong size!");
