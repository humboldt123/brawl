#pragma once

#include <StaticAssert.h>
#include <ec/ec_mgr.h>
#include <gr/gr_gimmick.h>
#include <sora_adv_stage/gr_adventure_move_ground.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<29grGimmickMoveAttackGroundData, ...>); only the
// attack that the ground sets is described.
struct grGimmickMoveAttackGroundData {
#ifdef ST_ADVENTURE2_IMPL
    u8 unk0[0x44];
    u8 m_mdlIndex;
    u8 unk45[0x1A7];
#else
    u8 unk0[0x1EC];
#endif
    grGimmick::AttackData m_attackData; // 0x1EC
};

// The moving ground that attacks what it touches (sora_adv_stage gr_adventure_move_attack_ground.o).
class grAdventureMoveAttackGround : public grAdventureMoveGround {
protected:
    u32 m_effectHandle;     // 0x154, from setEffect
    void* m_moveAttackData; // 0x158
    u8 unk15C[8];
    EfID m_effectId;        // 0x164

public:
    grAdventureMoveAttackGround(const char* taskName);
    virtual ~grAdventureMoveAttackGround();
    virtual void update(float deltaFrame);
    virtual void setVisibilityByClipping(u32 unk1);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setValid(u32 unk1);
    virtual void setValidAttachedEffect(u32 unk1);
    virtual void setAttackMoveGround();

    static grAdventureMoveAttackGround* create(int mdlIndex, const char* taskName);

    void setEffect(EfID effectId);
};
static_assert(sizeof(grAdventureMoveAttackGround) == 0x168, "Class is wrong size!");
