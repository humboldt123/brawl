#include <gf/gf_pad_system.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_selchar_coin.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>

void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* mdl, u32 nodeId, const Matrix* mtx);

muSelCharHand* muSelCharHand::create(InitData* data) {
    muSelCharHand* hand = new (Heaps::MenuInstance) muSelCharHand();
    hand->m_scnGroup = data->m_scnGroup;
    hand->m_selCharPlayerArea = data->m_playerArea;
    hand->createHandMdlAnim(data->m_resFile, data->m_drawPrio);
    hand->setCharAnimID(1, false);
    float x = data->m_pos.m_x;
    float y = data->m_pos.m_y;
    hand->m_startPos.m_x = x;
    hand->m_startPos.m_y = y;
    hand->m_pos.m_x = x;
    hand->m_pos.m_y = y;
    hand->updateMtx();
    return hand;
}

muSelCharHand::muSelCharHand() : gfTask("muSelCharHandTask", Category_Menu, 0xF, 8, true) {
    m_scnGroup = nullptr;
    m_scnMdl = nullptr;
    m_texPatAnm = nullptr;
    m_clrAnm = nullptr;
    m_chrBlend = nullptr;
    m_charAnimId = 0;
    m_selCharPlayerArea = nullptr;
    m_selCharCoin = nullptr;
    m_mode = Mode_Normal;
    m_modeArg = 0;
    m_collKind = 0;
    m_collArg = 0;
    m_isStopped = false;
    m_bButtonCnt = 0;
    m_scale = 0.75f;
    m_inScene = false;
    m_unkD4 = 0;
    m_unkD8 = 0;
    m_banVisible = false;
    unk2C_b1 = false;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.16f;
    m_startPos.m_x = 0.0f;
    m_startPos.m_y = 0.0f;
    m_startPos.m_z = 0.16f;
    m_collRect[0] = 0.0f;
    m_collRect[1] = 0.0f;
    m_collRect[2] = 0.0f;
    m_collRect[3] = 0.0f;
    for (int i = 0; i < CharAnim_Num; i++) {
        m_chrAnms[i] = nullptr;
    }
}

muSelCharHand::~muSelCharHand() {
    if (m_scnMdl) {
        m_scnMdl->Destroy();
    }
    if (m_texPatAnm) {
        m_texPatAnm->Destroy();
    }
    if (m_clrAnm) {
        m_clrAnm->Destroy();
    }
    if (m_chrBlend) {
        m_chrBlend->Destroy();
    }
    for (int i = 0; i < CharAnim_Num; i++) {
        if (m_chrAnms[i]) {
            m_chrAnms[i]->Destroy();
        }
    }
}

inline bool muSelCharHand::isVisibleMode(int mode) {
    // BYTES: an unsigned compare of the mode (one cmplwi, no test for a negative mode)
    if ((u32)mode <= Mode_Put && m_selCharPlayerArea->m_controllerNo >= 0) {
        return true;
    }
    return false;
}

int muSelCharHand::getPlayerNo() {
    return m_selCharPlayerArea->m_areaIdx;
}

// The number of the player on the hand.
void muSelCharHand::updatePlayerNo() {
    m_texPatAnm->SetFrame(m_selCharPlayerArea->m_areaIdx + 1);
}

void muSelCharHand::updateColorNo() {
    m_clrAnm->SetFrame(m_selCharPlayerArea->getColorNo(true));
}

// The hand is hidden while its player has no controller.
void muSelCharHand::updateControllerNo() {
    bool visible;
    if (m_selCharPlayerArea->m_controllerNo >= 0) {
        visible = isVisibleMode(m_mode);
    } else {
        setForceNomalMode();
        visible = false;
    }
    setVisible(visible);
}

void muSelCharHand::setBanVisible(bool ban) {
    m_banVisible = ban;
    bool visible = m_inScene;
    setVisible(visible);
}

// Puts the model of the hand where the hand is.
void muSelCharHand::updateMtx() {
    Vec3f scale(m_scale, m_scale, 1.0f);
    Vec3f rot(0.0f, 0.0f, 0.0f);
    Matrix mtx;
    mtx.setSRT(scale, rot, m_pos);
    ScnMdl_SetNodeMtx(m_scnMdl, 0, &mtx);
}

// The frame of the color animation of each team color.
static const u8 sTeamColorFrame[] = { 1, 2, 4 };

void muSelCharHand::updateMeleeKind() {
    float frame;
    if (m_selCharPlayerArea->isTeamBattle()) {
        frame = sTeamColorFrame[m_selCharPlayerArea->m_teamColor];
    } else {
        frame = m_selCharPlayerArea->m_areaIdx;
        frame = 1.0f + frame;
    }
    m_clrAnm->SetFrame(frame);
}

void muSelCharHand::updateTeamColor() {
    float frame;
    if (m_selCharPlayerArea->isTeamBattle()) {
        frame = sTeamColorFrame[m_selCharPlayerArea->m_teamColor];
    } else {
        frame = m_selCharPlayerArea->m_areaIdx;
        frame = 1.0f + frame;
    }
    m_clrAnm->SetFrame(frame);
}

// The hand blends to its pose: the weight of the pose grows and the weights of the others fall, a quarter a frame.
void muSelCharHand::processDefault() {
    for (int i = 0; i < CharAnim_Num; i++) {
        float weight = m_chrBlend->GetWeight(i);
        if (i == m_charAnimId) {
            if (weight < 1.0f) {
                weight += 0.25f;
                if (weight > 1.0f) {
                    weight = 1.0f;
                }
            }
        } else if (weight > 0.0f) {
            weight -= 0.25f;
            if (weight < 0.0f) {
                weight = 0.0f;
            }
        }
        m_chrBlend->SetWeight(i, weight);
    }
}

void muSelCharHand::setMode(int mode, int arg) {
    bool visible = isVisibleMode(mode);
    setVisible(visible);
    m_mode = mode;
    m_modeArg = arg;
}

// The hand takes the coin from the picture of a character: it points at it from its side of the coin.
void muSelCharHand::catchCoin(muSelCharCoin* coin) {
    Vec2f coinPos = coin->getPos();
    Vec2f pos = getPos();
    setCharAnimID(coinPos.m_x > pos.m_x ? 9 : 10, false);
    setMode(Mode_Catch, 0);
    m_selCharCoin = coin;
    if (!coin) {
        m_scale = 0.75f;
        updateMtx();
    } else {
        m_scale = 1.0f;
        updateMtx();
    }
}

void muSelCharHand::takeCoin(muSelCharCoin* coin) {
    setCharAnimID(2, false);
    setMode(Mode_Take, 0);
    m_selCharCoin = coin;
    if (!coin) {
        m_scale = 0.75f;
        updateMtx();
    } else {
        m_scale = 1.0f;
        updateMtx();
    }
}

void muSelCharHand::putCoin() {
    setCharAnimID(8, false);
    setMode(Mode_Put, 0);
    m_selCharCoin = nullptr;
    m_scale = 0.75f;
    updateMtx();
}

void muSelCharHand::main() {
    if (m_selCharPlayerArea->_0 == 1) {
        return;
    }
    switch (m_mode) {
    case Mode_Normal:
        moveProc();
        break;
    case Mode_Catch:
        // The hand waits for the coin to reach it.
        if (m_selCharCoin->m_mode != muSelCharCoin::Mode_Jump) {
            setMode(Mode_Normal, 0);
        }
        break;
    case Mode_Take:
        coinTakeProc();
        break;
    case Mode_Put: {
        // The hand is normal again once the animation of putting the coin down has ended.
        nw4r::g3d::AnmObjChrRes* anim = m_chrAnms[8];
        int length = anim->m_anmChrFile->m_animLength;
        if (anim->GetFrame() >= length - 1.0f) {
            setMode(Mode_Normal, 0);
        }
        break;
    }
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        break;
    }
    bButtonHoldProc();
}

// The pose of the hand over the things that are not characters: what is under the hand, and the pose.
struct muSelCharHandCollAnim {
    u8 m_collKind;
    u8 m_animId;
};
static const muSelCharHandCollAnim sCollAnim[] = { { 2, 4 }, { 4, 4 }, { 0, 0 } };

// Picks the pose of the hand from where it is and what is under it.
void muSelCharHand::setCharAnimByCondition(int area, int collKind) {
    int id;
    if (collKind == 8) {
        id = 5;
    } else if (area == 1) {
        if (m_selCharCoin) {
            id = 7;
        } else if (collKind == 1) {
            id = 3;
        } else {
            id = 2;
        }
    } else {
        id = 1;
        for (int i = 0; i < 3; i++) {
            if (collKind == sCollAnim[i].m_collKind) {
                id = sCollAnim[i].m_animId;
                break;
            }
        }
    }
    if (id != m_charAnimId) {
        setCharAnimID(id, false);
    }
}

int muSelCharHand::getScreenArea() {
    return muSelCharTask::getScreenArea(getPos());
}

// Counts the frames that the B button is held (the hand goes back when it is held long enough).
void muSelCharHand::bButtonHoldProc() {
    if (m_selCharPlayerArea->m_controllerNo < 0) {
        m_bButtonCnt = 0;
        return;
    }
    // HYPOTHESIS: modes 4 to 8 are the lists and the name entry, where B closes the window instead.
    // BYTES: one unsigned compare of the mode minus 4.
    if ((u32)(m_mode - 4) <= 4) {
        m_bButtonCnt = 0;
        return;
    }
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(m_selCharPlayerArea->m_controllerNo, &pad);
    if (pad.m_buttonsCurrentFrame2.bits & gfPadButtons::B) {
        m_bButtonCnt++;
    } else {
        m_bButtonCnt = 0;
    }
}

void muSelCharHand::resetBButtonCnt() {
    m_bButtonCnt = 0;
}

void muSelCharHand::saveCollKind() {
    m_collKind = getCollKind(&m_collArg, m_collRect);
}

int muSelCharHand::loadCollKind(int* arg, float* rect) {
    if (arg) {
        *arg = m_collArg;
    }
    if (rect) {
        rect[0] = m_collRect[0];
        rect[1] = m_collRect[1];
        rect[2] = m_collRect[2];
        rect[3] = m_collRect[3];
    }
    return m_collKind;
}
