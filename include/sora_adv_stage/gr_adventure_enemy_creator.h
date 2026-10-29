#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<27grAdventureEnemyCreatorData, ...>); its layout is
// not described yet.
struct grAdventureEnemyCreatorData;

class grAdventureEnemyCreator : public grYakumono {
protected:
    u8 m_state;                           // 0x150, HYPOTHESIS, 5 while invalid
    float unk154;                         // 0x154
    float unk158;                         // 0x158
    u16 unk15C;                           // 0x15C
    grAdventureEnemyCreatorData* m_data;  // 0x160
    u8 unk164[0x10];
    stTrigger* unk174;                    // 0x174
    u8 unk178[0x28];
    u32 unk1A0[5];                        // 0x1A0

public:
    grAdventureEnemyCreator(const char* taskName) : grYakumono(taskName) {
        m_state = 0;
        unk154 = 0.0f;
        unk158 = 0.0f;
        unk15C = 0;
        m_data = NULL;
        unk174 = NULL;
        unk1A0[0] = 0;
        unk1A0[1] = 0;
        unk1A0[2] = 0;
        unk1A0[3] = 0;
        unk1A0[4] = 0;
    }
    virtual ~grAdventureEnemyCreator();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setValid(u32 unk1);
    virtual void setValidAttachedEffect(u32 unk1);
    virtual void setInitializeFlag();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);

    static grAdventureEnemyCreator* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureEnemyCreator) == 0x1B4, "Class is wrong size!");
