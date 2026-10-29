#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/st_adventure_info_basis.h>
#include <types.h>

// The two Subspace zako (common enemy) HUD slots.
class stAdventureInfoEmZako : public stAdventureInfoBasis {
public:
    // HYPOTHESIS: local name and field names from the use in init, update and notifyDead.
    struct Slot {
        int m_createId;                     // 0x00
        bool m_isDamage;                    // 0x04
        bool m_isDead;                      // 0x05
        bool m_isDisp;                      // 0x06
        u32 m_dispFrame;                    // 0x08
        stAdventureInfoIconParam m_param;   // 0x0C

        Slot() {
            m_createId = -1;
            m_isDamage = false;
            m_isDead = false;
            m_isDisp = false;
            m_dispFrame = 0;
        }
    };

    Slot m_slots[2];    // 0x08

    virtual ~stAdventureInfoEmZako() { }
    virtual void update();

    // HYPOTHESIS: parameter types from the register use.
    void init(int createId, int index);
    bool isZako(int createId);
    void notifyDead(u32 createId);
    void remove();
};
static_assert(sizeof(stAdventureInfoEmZako) == 0x58, "Class is wrong size!");
