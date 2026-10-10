#include <memory.h>
#include <stdio.h>
#include <mt/mt_prng.h>
#include <mu/menu.h>
#include <types.h>

#include <st_pictchat/gr_pictchat.h>

// GameGlobal (main, unnamed): the two checks that say which language the font of the names is for
extern "C" int fn_8004D9E8(GameGlobal*);
extern "C" int fn_8004D9FC(GameGlobal*);

grPictchat::grPictchat(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grPictchat::~grPictchat() {
}

grPictchatBg* grPictchatBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatBg* ground = new (Heaps::StageInstance) grPictchatBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatBg::~grPictchatBg() {
    if (m_message != NULL) {
        delete m_message;
        m_message = NULL;
    }
}

void grPictchatBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateMessage(deltaFrame);
    }
}

// The floor of the model (a joint of the collision that belongs to this ground) is turned on and off: the flag 0x2000 is
// set on it while the last picture (0x13) is drawn.
void grPictchatBg::updateJoint(float deltaFrame) {
    if (m_joint != NULL) {
        int flag = 0;
        if (*m_pictIDWork == 0x13) {
            flag |= 0x2000;
        }
        m_joint->m_0x52 = flag;
    } else {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u16 count = collision->m_jointLen;
            u32 index = 0;
            grCollisionJoint* joint = reinterpret_cast<grCollisionJoint*>(this);
            while (index != count) {
                joint = collision->getJoint(index);
                if (joint != NULL && reinterpret_cast<Ground*>(joint->m_ground) == this) {
                    break;
                }
                index++;
            }
            if (index != count) {
                m_joint = joint;
            }
        }
    }
}

// The name of a fighter who is playing is shown on the board: the first time a player is picked at random from those that are
// not out of the game (the state 3), and the name is written with the name of the fighter in the font of the stage.
void grPictchatBg::updateMessage(float deltaFrame) {
    switch (m_state) {
    case 0:
        m_message = new (Heaps::StageInstance) Message(10, Heaps::StageInstance);
        if (m_message == NULL) {
            return;
        }
        m_message->allocMsgBuf(0x400, 1, Heaps::StageInstance);
        m_message->attachMsgBuf(0, m_sceneModels[0], "pictchat_font", 1, 3, 0.0625f);
        m_state = 1;
        break;
    case 1:
        break;
    default:
        return;
    }
    m_message->changeMsgBuf(0);
    m_message->clearMsgBuf();
    m_message->setWindow(-1700.0f, 0.0f, 1700.0f, 570.0f);
    m_message->setFace(4);
    m_message->setFixedWidth(-1.0f);
    m_message->setColor(-1);
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee == NULL) {
        return;
    }
    u32 player = m_charIndex;
    if (player == 0xFF) {
        u8 players[8];
        u8 count = 0;
        u8 i = 0;
        for (int left = 7; left != 0; left--) {
            players[i] = 0xFF;
            if (melee->m_playersInitData[i].m_state != 3) {
                u8 slot = count;
                count++;
                players[slot] = i;
            }
            i++;
        }
        if (count == 0) {
            return;
        }
        int picked = static_cast<int>(static_cast<float>(count) * randf());
        u8 last = count - 1;
        m_charIndex = picked;
        u32 index = picked & 0xFF;
        index = (index > 0) ? index : 0;
        if (index < last) {
            last = index;
        }
        player = players[last];
        m_charIndex = player;
        if (melee->m_playersInitData[player].m_state == 3) {
            return;
        }
    }
    int kind = muMenu::exchangeGmCharacterKind2MuStockchkind(melee->m_playersInitData[player].m_characterKind);
    if (kind == 0x17) {
        kind = 3;
    }
    float cursorX = 0.0f;
    float scale = 20.0f;
    if (fn_8004D9E8(g_GameGlobal) == 1) {
        cursorX = 800.0f;
    } else if (fn_8004D9FC(g_GameGlobal) == 1) {
        cursorX = 1200.0f;
    }
    float cursorY = 200.0f;
    m_message->setScale(scale, scale);
    m_message->setCursorX(cursorX);
    m_message->setCursorY(cursorY);
    if (GameGlobal::getLanguage() == 0) {
        char text[0x80];
        char suffix[8];
        char* str;
        u32 len;
        memset(text, 0, sizeof(text));
        memset(suffix, 0, sizeof(suffix));
        Message::getPrintIndexData(m_msgData, 0xD, &str, &len);
        if (static_cast<int>(len) > 7) {
            len = 7;
        }
        memcpy(suffix, str, len);
        sprintf(text, "%s%s", muMenu::exchangeMuStockchkind2MuCharName(kind), suffix);
        m_message->printf(text);
    } else {
        m_message->printf(muMenu::exchangeMuStockchkind2MuCharName(kind));
    }
}
