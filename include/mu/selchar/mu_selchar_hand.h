#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it names the members
// of the hand from its constructor and its functions.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <mu/selchar/mu_selchar_coin.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

class muSelCharPlayerArea;

// The hand (the cursor) of a player on the character select screen. It carries the coin of its player to a character.
// HYPOTHESIS: the names of the fields, from how the functions use them.
class muSelCharHand : public gfTask {
public:
    enum {
        CharAnim_Num = 11,      // the poses of the hand (open, pointing, holding a coin ...)
    };

    // What the hand is doing.
    enum Mode {
        Mode_Normal = 0,        // the hand moves with the stick
        Mode_Catch = 1,         // the hand catches the coin
        Mode_Take = 2,          // the hand carries the coin back to the area of its player
        Mode_Put = 3,           // the hand puts the coin down (the animation plays to its end)
    };

    nw4r::g3d::ScnGroup* m_scnGroup;                    // 0x40
    nw4r::g3d::ScnMdl* m_scnMdl;                        // 0x44 the model of the hand
    nw4r::g3d::AnmObjTexPatRes* m_texPatAnm;            // 0x48 the number of the player on the hand
    nw4r::g3d::AnmObjMatClrRes* m_clrAnm;               // 0x4C the color of the player
    nw4r::g3d::AnmObjChrRes* m_chrAnms[CharAnim_Num];   // 0x50 the poses of the hand
    nw4r::g3d::G3dObj* m_chrBlend;                      // 0x7C the blend of the poses
    int m_charAnimId;                                   // 0x80 the pose that the hand blends to
    Vec3f m_startPos;                                   // 0x84 where the hand started
    Vec3f m_pos;                                        // 0x90 where the hand is on the screen
    muSelCharPlayerArea* m_selCharPlayerArea;           // 0x9C
    muSelCharCoin* m_selCharCoin;                       // 0xA0 the coin that the hand holds
    int m_mode;                                         // 0xA4
    int m_modeArg;                                      // 0xA8 what the mode works on (an area for a list, for example)
    int m_collKind;                                     // 0xAC what is under the hand (the last time it was saved)
    int m_collArg;                                      // 0xB0
    float m_collRect[4];                                // 0xB4
    bool m_isStopped;                                   // 0xC4 the stick does not move the hand
    int m_bButtonCnt;                                   // 0xC8 how long the B button was held
    float m_scale;                                      // 0xCC
    bool m_inScene;                                     // 0xD0 the hand is visible (it is in the group of the scene)
    int m_unkD4;                                        // 0xD4
    int m_unkD8;                                        // 0xD8
    bool m_banVisible;                                  // 0xDC the hand is kept hidden

    // What the select screen gives to create a hand.
    // HYPOTHESIS: the names, from how create uses the fields.
    struct InitData {
        nw4r::g3d::ResFile* m_resFile;          // 0x00 the file of the model
        nw4r::g3d::ScnGroup* m_scnGroup;        // 0x04
        int m_drawPrio;                         // 0x08
        Vec2f m_pos;                            // 0x0C where the hand starts
        muSelCharPlayerArea* m_playerArea;      // 0x14
    };

    static muSelCharHand* create(InitData* data);
    muSelCharHand();
    void createHandMdlAnim(nw4r::g3d::ResFile* resFile, int drawPrio);

    // HYPOTHESIS: an inline accessor; the coin copies the position of the hand through it.
    Vec2f getPos() { return Vec2f(m_pos.m_x, m_pos.m_y); }

    int getPlayerNo();
    void updatePlayerNo();
    void updateColorNo();
    void updateControllerNo();
    void setBanVisible(bool ban);
    void updateMtx();
    void setCharAnimID(int id, bool force);
    void updateMeleeKind();
    void updateTeamColor();
    virtual void processDefault();
    void setMode(int mode, int arg);
    void setForceNomalMode();
    void catchCoin(muSelCharCoin* coin);
    void takeCoin(muSelCharCoin* coin);
    void putCoin();
    void main();
    void moveProc();
    int getCollKind(int* arg, float* rect);
    void saveCollKind();
    int loadCollKind(int* arg, float* rect);
    void setCharAnimByCondition(int area, int collKind);
    void coinTakeProc();
    int getScreenArea();
    void bButtonHoldProc() __attribute__((never_inline)); // BYTES: main calls it out of line, MWCC would inline it
    void resetBButtonCnt();
    virtual ~muSelCharHand();

private:
    // The hand is shown in the modes that move it, while its player has a controller.
    bool isVisibleMode(int mode);

    // Puts the hand in the group of the scene or takes it out of it (it stays out while it is banned).
    // HYPOTHESIS: an inline helper; the name is not known.
    void setVisible(bool visible) {
        m_scnGroup->Remove(m_scnMdl);
        if (visible && !m_banVisible) {
            m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_scnMdl);
        }
        m_inScene = visible;
    }
};
static_assert(sizeof(muSelCharHand) == 0xe0, "Class is wrong size!");
