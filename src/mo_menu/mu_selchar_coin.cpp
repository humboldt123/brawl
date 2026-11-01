#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/mu_object.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_selchar_coin.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>

muSelCharCoin* muSelCharCoin::create(InitData* data) {
    muSelCharCoin* coin = new (Heaps::MenuInstance) muSelCharCoin();
    coin->initProc(data);
    return coin;
}

muSelCharCoin::~muSelCharCoin() {
    for (int i = 0; i < Obj_Num; i++) {
        if (m_objs[i]) {
            delete m_objs[i];
        }
    }
}

void muSelCharCoin::initProc(InitData* data) {
    m_scnGroup = data->m_scnGroup;
    m_playerArea = data->m_playerArea;
    m_hand = data->m_hand;

    MuObject* cursor;
    MuObject* coin = MuObject::create(data->m_resFile, "MenSelchrCoin_TopN", data->m_coinDrawPrio + m_playerArea->m_areaIdx, nullptr,
                                      Heaps::MenuInstance);
    m_objs[Obj_Coin] = coin;
    coin->changeAnimN(0, 7, false, false);
    coin->setFrameTex(m_playerArea->m_areaIdx + 1);
    coin->setFrameNode(data->m_nodeFrame + 1);

    m_objs[Obj_Effect] = MuObject::create(data->m_resFile, "MenSelchrCoineff_TopN", data->m_effDrawPrio, nullptr, Heaps::MenuInstance);
    m_objs[Obj_Effect]->changeAnimN(0, 0x13, false, false);

    cursor = MuObject::create(data->m_resFile, "MenSelchrCursorB_TopN", data->m_cursorDrawPrio, nullptr, Heaps::MenuInstance);
    m_objs[Obj_Cursor] = cursor;
    cursor->changeAnimN(0, 1, false, false);
    cursor->setFrameNode(data->m_nodeFrame);
    cursor->changeAnimN(0, 0x10, false, false);
    cursor->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);

    updatePlayerKind();
    setPos(data->m_pos);
}

int muSelCharCoin::getPlayerKind() {
    return m_playerArea->m_playerKind;
}

void muSelCharCoin::setPos(const Vec2f& pos) {
    Vec3f trans(pos.m_x, pos.m_y, 1.0f);
    m_objs[Obj_Coin]->setTrans(&trans);
}

Vec2f muSelCharCoin::getPos() {
    Vec3f trans;
    trans = m_objs[Obj_Coin]->m_trans;
    return Vec2f(trans.m_x, trans.m_y);
}

// Shows the coin for the kind of its player (none, a player or a computer) with the colors of the player.
void muSelCharCoin::updatePlayerKind() {
    MuObject* coin = m_objs[Obj_Coin];
    int kind = m_playerArea->m_playerKind;
    m_scnGroup->Remove(coin->m_sceneModel);
    m_inScene = false;
    if (kind != 0) {
        coin->setFrameVisible(kind == 1 ? 1.0f : 2.0f);
        updateMeleeKind();
    }
}

int muSelCharCoin::getCoinNo() {
    return m_playerArea->m_areaIdx;
}

float muSelCharCoin::getRadius() {
    Vec3f scale;
    scale = m_objs[Obj_Coin]->getScale("scaleN");
    return 3.25f * scale.m_x;
}

float muSelCharCoin::getScaleRadius() {
    Vec3f objScale;
    objScale = m_objs[Obj_Coin]->getObjScale();
    return objScale.m_x * getRadius();
}

// The colors of the coin and of its cursor: the color of the player for kind 1, the plain ones for the others.
void muSelCharCoin::updateMeleeKind() {
    m_objs[Obj_Coin]->changeAnimN(m_playerArea->m_playerKind == 1 ? m_playerArea->getColorNo(true) - 1 : 0, 0x10, true, false);
    m_objs[Obj_Cursor]->changeAnimN(m_playerArea->m_playerKind == 1 ? m_playerArea->getColorNo(true) - 1 : 4, 0x10, true, false);
}

void muSelCharCoin::updateTeamColor() {
    m_objs[Obj_Coin]->changeAnimN(m_playerArea->m_playerKind == 1 ? m_playerArea->getColorNo(true) - 1 : 0, 0x10, true, false);
    m_objs[Obj_Cursor]->changeAnimN(m_playerArea->m_playerKind == 1 ? m_playerArea->getColorNo(true) - 1 : 4, 0x10, true, false);
}

void muSelCharCoin::main() {
    if (m_playerArea->_0 != 0) {
        return;
    }

    switch (m_mode) {
    case Mode_Free:
        if (m_hand) {
            Vec2f pos = m_hand->getPos();
            setPos(pos);
        }
        break;
    case Mode_Jump:
        jumpMove();
        break;
    case Mode_Held:
        if (m_hand->m_mode != 2) {
            m_mode = Mode_Free;
        }
        break;
    }

    // The coin is shown while it is over the area of the characters.
    bool visible;
    if (m_playerArea->_0 == 1) {
        visible = false;
    } else if (m_playerArea->m_playerKind == 0) {
        visible = false;
    } else {
        Vec2f pos = getPos();
        visible = muSelCharTask::getScreenArea(pos) == 1;
    }
    setVisible(visible);
}

// Moves the coin to the hand, 8 units a frame at most; it is free again when it gets there.
void muSelCharCoin::jumpMove() {
    Vec2f dir = Vec2f(m_hand->m_pos.m_x, m_hand->m_pos.m_y) - getPos();
    float len = dir.length();
    bool arrived;
    if (len > 8.0f) {
        dir *= 8.0f / len;
        arrived = false;
    } else {
        arrived = true;
    }
    Vec2f pos = getPos();
    Vec3f trans(pos.m_x + dir.m_x, pos.m_y + dir.m_y, 1.0f);
    m_objs[Obj_Coin]->setTrans(&trans);
    if (arrived) {
        m_mode = Mode_Free;
    }
}

// Starts the flash of the coin where the coin is.
void muSelCharCoin::dispEffect() {
    MuObject* effect = m_objs[Obj_Effect];
    m_scnGroup->Remove(effect->m_sceneModel);
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, effect->m_sceneModel);
    Vec3f trans;
    trans = m_objs[Obj_Coin]->m_trans;
    effect->setTrans(&trans);
    effect->setFrame(0.0f);
    effect->m_modelAnim->setUpdateRate(1.0f);
}

MuObject* muSelCharCoin::getCursorObj() {
    return m_objs[Obj_Cursor];
}

// Puts the coin on a hand (it is drawn at full size) or takes it off (smaller, lying on the picture).
void muSelCharCoin::setHand(muSelCharHand* hand) {
    float scale;
    bool onHand;
    if (hand) {
        scale = 1.0f;
        onHand = true;
    } else {
        scale = 0.6f;
        onHand = false;
    }
    Vec3f objScale(scale, scale, scale);
    m_objs[Obj_Coin]->setObjScale(&objScale);
    m_hand = hand;
    m_playerArea->setCharPicInter(onHand);
}

void muSelCharCoin::setMode(int mode, muSelCharHand* hand) {
    if (mode == Mode_Jump || mode == Mode_Held) {
        setHand(hand);
    }
    m_mode = mode;
}

int muSelCharCoin::getScreenArea() {
    return muSelCharTask::getScreenArea(getPos());
}

void muSelCharCoin::setBanVisible(bool ban) {
    m_banVisible = ban;
    bool visible = m_inScene;
    setVisible(visible);
}
