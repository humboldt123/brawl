#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<23grAdventureElevatorData, ...>); its layout is not
// described yet.
struct grAdventureElevatorData;

class grAdventureElevator : public grYakumono {
protected:
    grAdventureElevatorData* m_data;  // 0x150
    u8 unk154[4];
    u8 m_state;                       // 0x158, HYPOTHESIS: 0 stopped, 1 moving, 2 arrived
    float m_timer;                    // 0x15C
    Vec3f* m_floors;                  // 0x160
    u32 m_floorCount;                 // 0x164
    u32 m_floorIndex;                 // 0x168
    u32 m_targetFloorIndex;           // 0x16C
    u8 unk170[0x14];
    float unk184;                     // 0x184
    u8 unk188[0x30];
    u32 unk1B8;                       // 0x1B8
    u32 unk1BC;                       // 0x1BC
    u8 unk1C0[4];

public:
    grAdventureElevator(const char* taskName) : grYakumono(taskName) {
        m_state = 0;
        m_floors = NULL;
        m_floorCount = 0;
        m_floorIndex = 0;
        unk184 = 0.0f;
        unk1B8 = 0;
        unk1BC = 0;
    }
    virtual void processFixPosition();
    virtual ~grAdventureElevator();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);

    static grAdventureElevator* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureElevator) == 0x1C4, "Class is wrong size!");
