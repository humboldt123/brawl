#pragma once

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// HYPOTHESIS: local name. The display block that the info records pass by value to IfAdvMngr::dispBoss,
// IfAdvMngr::dispZako and IfMngr::dispBoss (main.dol 0x80107848, 0x801077D8, 0x800DCEA8).
struct stAdventureInfoDisp {
    int m_index;                    // 0x00
    float m_hp;                     // 0x04
    float m_maxHp;                  // 0x08
    int m_nameMsgId;                // 0x0C
    // HYPOTHESIS: the face texture file, kept as raw data (the copies are plain word copies).
    nw4r::g3d::ResFileData* m_faceTex;  // 0x10
};
static_assert(sizeof(stAdventureInfoDisp) == 0x14, "Class is wrong size!");

// HYPOTHESIS: local name. The per-enemy HUD record that stAdventureInfoBasis::getIconParamForEnemy fills.
struct stAdventureInfoIconParam {
    stAdventureInfoDisp m_disp;                 // 0x00
    bool m_isValid;                             // 0x14
    bool unk15;                                 // 0x15
    nw4r::g3d::ResFileData* m_faceTexData;      // 0x18

    stAdventureInfoIconParam() {
        m_disp.m_faceTex = NULL;
        m_isValid = false;
        unk15 = false;
        m_faceTexData = NULL;
    }
};
static_assert(sizeof(stAdventureInfoIconParam) == 0x1C, "Class is wrong size!");

// The Subspace Emissary HUD messenger: forwards boss, zako and corps state to IfAdvMngr and IfMngr.
class stAdventureInfoContact {
public:
    virtual ~stAdventureInfoContact() { }
    virtual void update() { }

    bool m_isBossAppear[2];     // 0x04
    bool m_isActive;            // 0x06

    stAdventureInfoContact() {
        m_isBossAppear[0] = false;
        m_isBossAppear[1] = false;
        m_isActive = false;
    }

    // HYPOTHESIS: parameter types from the register use and the callers. The records are taken by value: the
    // display block is then copied whole to the stack, as in the original.
    void sendToBossInfo(stAdventureInfoIconParam param, bool isStamina);
    void sendToDestroyBossInfo(int index, bool isStamina);
    void sendToDamageBossInfo(int index, bool isStamina);
    void sendToRemoveBossInfo();
    void sendToZakoInfo(stAdventureInfoIconParam param);
    void sendToDestroyZakoInfo(int index);
    void sendToDamageZakoInfo(int index);
    void sendToEraseZakoInfo(int index);
    void sendToRemoveZakoInfo();
    void addZakoCorps(u32 count);
    void removeZakoCorps();
    void sendToRemoveCarpsInfo();
};
static_assert(sizeof(stAdventureInfoContact) == 0x8, "Class is wrong size!");
