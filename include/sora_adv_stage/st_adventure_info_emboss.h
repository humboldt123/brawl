#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/st_adventure_info_basis.h>
#include <types.h>

// The Subspace boss HUD record (one boss, or a boss pair).
class stAdventureInfoEmBoss : public stAdventureInfoBasis {
public:
    int m_createId;                         // 0x08
    u32 unkC;                               // 0x0C
    // HYPOTHESIS: the create ids of the two Pack'n Wn halves (setBossPacknWnLeft/Right).
    u32 m_packnWnLeft;                      // 0x10
    u32 m_packnWnRight;                     // 0x14
    int m_subCreateId;                      // 0x18
    bool m_isDead[2];                       // 0x1C
    bool m_isDamage[2];                     // 0x1E
    bool m_isPair;                          // 0x20
    stAdventureInfoIconParam m_params[2];   // 0x24

    stAdventureInfoEmBoss() : m_createId(-1), unkC(0), m_packnWnLeft(0), m_packnWnRight(0), m_isPair(false) {
        m_isDead[0] = false;
        m_isDead[1] = false;
        m_isDamage[0] = false;
        m_isDamage[1] = false;
    }
    virtual ~stAdventureInfoEmBoss() { }
    virtual void update();

    // HYPOTHESIS: parameter types from the register use.
    void init(int createId);
    void setInfoParam(int createId);
    bool isBoss(int createId);
    void setBossPacknWnLeft(u32 createId);
    void setBossPacknWnRight(u32 createId);
    void notifyDead(u32 createId);
    void notifyDamage(u32 createId);
    void remove();
};
static_assert(sizeof(stAdventureInfoEmBoss) == 0x5C, "Class is wrong size!");
