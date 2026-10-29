#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <sora_adv_stage/gr_gimmick_block.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<17grGimmickMineData, ...>); its layout is not
// described yet.
struct grGimmickMineData;

class grAdventureMine : public grGimmickBlock {
protected:
    u32 unk15C;        // 0x15C
    u8 m_isUpdate;     // 0x160, set again each frame by the mine manager (HYPOTHESIS)
    u32 unk164;        // 0x164
    float unk168;      // 0x168
    u8 m_isWarning;    // 0x16C

public:
    virtual void processUpdate();
    virtual ~grAdventureMine();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureMine* create(int mdlIndex, const char* taskName);

    void callWarning();
    void callCancelWarning();
    void callBreak();
};
static_assert(sizeof(grAdventureMine) == 0x170, "Class is wrong size!");
