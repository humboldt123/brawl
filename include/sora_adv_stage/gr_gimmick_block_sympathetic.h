#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <types.h>

// HYPOTHESIS: The block that breaks together with its group (sora_adv_stage gr_gimmick_block_sympathetic.o); it is not a
// grGimmickBlock. Its destructor is inline: the kept copy is in gr_adventure_block_pos_sympathetic.o.
class grGimmickBlockSympathetic : public grGimmick {
protected:
    u32 unk14C; // 0x14C
    u8 unk150;  // 0x150
    u8 unk151[7];

public:
    grGimmickBlockSympathetic(const char* taskName) : grGimmick(taskName) {
        unk14C = 0;
        unk150 = 0;
    }
    virtual ~grGimmickBlockSympathetic() { }
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void invalidatedByCameraClipping();
    virtual void dbDispInvalidatedByCameraClippingSphere();

    static grGimmickBlockSympathetic* create(int mdlIndex, const char* taskName);

    void requestBreak();
};
static_assert(sizeof(grGimmickBlockSympathetic) == 0x158, "Class is wrong size!");
