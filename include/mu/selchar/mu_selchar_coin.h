#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): the shared header only
// names the hand and the area of the coin.

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

class muSelCharPlayerArea;
class muSelCharHand;

// The coin of a player on the character select screen: the hand carries it to the picture of a character, and where it is
// put down picks the character. A cursor model and an effect model go with it.
class muSelCharCoin {
public:
    // What the select screen gives to create a coin.
    // HYPOTHESIS: the names, from how initProc uses the fields.
    struct InitData {
        nw4r::g3d::ResFile* m_resFile;      // 0x00 the file of the models
        nw4r::g3d::ScnGroup* m_scnGroup;    // 0x04 the group of the scene that the coin is drawn in
        Vec2f m_pos;                        // 0x08 where the coin starts
        muSelCharPlayerArea* m_playerArea;  // 0x10
        int m_nodeFrame;                    // 0x14 the frame of the node animations of the coin and of the cursor
        muSelCharHand* m_hand;              // 0x18
        int m_coinDrawPrio;                 // 0x1C the priority of the coin (the number of the area is added)
        int m_effDrawPrio;                  // 0x20
        int m_cursorDrawPrio;               // 0x24
    };

    enum Obj {
        Obj_Coin = 0,
        Obj_Effect = 1,     // the flash when the coin is put down
        Obj_Cursor = 2,     // the cursor that shows the player number on the coin
        Obj_Num = 3,
    };

    enum Mode {
        Mode_Free = 0,      // the coin follows the hand when it has one
        Mode_Jump = 1,      // the coin moves to the hand (jumpMove)
        Mode_Held = 2,      // the coin stays where it is until the hand is no longer in mode 2
    };

    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x00
    MuObject* m_objs[Obj_Num];              // 0x04
    muSelCharHand* m_hand;                  // 0x10 the hand the coin is on (none when it is put down)
    muSelCharPlayerArea* m_playerArea;      // 0x14 the area of the player of the coin
    int m_mode;                             // 0x18
    bool m_inScene;                         // 0x1C the coin is visible (it is in the group of the scene)
    bool m_banVisible;                      // 0x1D the coin is kept hidden

    muSelCharCoin() {
        m_scnGroup = nullptr;
        m_hand = nullptr;
        m_playerArea = nullptr;
        m_mode = Mode_Free;
        m_inScene = false;
        m_banVisible = false;
        for (int i = 0; i < Obj_Num; i++) {
            m_objs[i] = nullptr;
        }
    }
    ~muSelCharCoin();

    static muSelCharCoin* create(InitData* data);
    void initProc(InitData* data);
    int getPlayerKind();
    void setPos(const Vec2f& pos);
    Vec2f getPos();
    void updatePlayerKind();
    int getCoinNo();
    float getRadius();
    float getScaleRadius();
    void updateMeleeKind();
    void updateTeamColor();
    void main();
    void jumpMove() __attribute__((never_inline)); // BYTES: main calls it out of line, MWCC would inline it
    void dispEffect();
    MuObject* getCursorObj();
    void setHand(muSelCharHand* hand);
    void setMode(int mode, muSelCharHand* hand);
    int getScreenArea();
    u8 getCoinDrawPrio();
    void setBanVisible(bool ban);

private:
    // Puts the coin in the group of the scene or takes it out of it (it stays out while it is banned).
    // HYPOTHESIS: an inline helper; the name is not known.
    void setVisible(bool visible) {
        m_scnGroup->Remove(m_objs[Obj_Coin]->m_sceneModel);
        if (visible && !m_banVisible) {
            m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_objs[Obj_Coin]->m_sceneModel);
        }
        m_inScene = visible;
    }
};
static_assert(sizeof(muSelCharCoin) == 0x20, "Class is wrong size!");
