#include <gf/gf_camera.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <if/if_player.h>
#include <math.h>
#include <ms/ms_resfont.h>
#include <ms/ms_tex_obj.h>
#include <mu/menu.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <sora/ms_message.h>
#include <stdio.h>
#include <types.h>

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group (same prefix as in if_player.cpp); mpScnMdl is the model the
// objects are attached to.
class ScnMdlExpand : public ScnGroup {
public:
    bool PushBack(ScnObj* object, u32 nodeId);
    ScnMdl* mpScnMdl; // 0xE8
};
}}

// HYPOTHESIS: data that lives in the first unit of the split (if_player.cpp / cluster A) or in a unit before it.
extern nw4r::g3d::ScnMdlExpand* lbl_805A02DC; // the player locator group
extern const char* lbl_80459CC0[8];           // "pos00" .. "pos07", the locator node names

extern u8 lbl_8049E01C[]; // the font manager (see mu_msg.cpp)
extern "C" u8 fn_800B8648(void* mgr, u32 fontSetting, int p2, int p3);
extern "C" void fn_800B881C(void* mgr, u8 fontId);

// HYPOTHESIS: map names IfPlayer::setHeadCursorDamageHP
extern "C" void fn_800E5318(IfPlayer* player, u32 damage);
// HYPOTHESIS: map names muMenu::exchangeStockchkind2Something
extern "C" int fn_800AF6F0(int stock);

struct IfPlayerStockFaceFunc {
    void (*m_init)(int index, IfPlayer* player);
    int (*m_main)(int index, IfPlayer* player);
};
// the table in if_player.cpp (static there)
extern const IfPlayerStockFaceFunc s_stockFaceFuncTbl[4];

// HYPOTHESIS: damage state table (init, main, post per state); defined in the data of if_player.cpp.
struct IfPlayerDamageFunc {
    void (*m_init)(IfPlayer* player, int prevState);
    int (*m_main)(IfPlayer* player);
    void (*m_post)(IfPlayer* player);
};
extern const IfPlayerDamageFunc s_damageFuncTbl[];

// HYPOTHESIS: .sdata2 tables that if_player.cpp owns; repeated here so that this unit compiles on its own.
// Team colour numbers by team, then by whether the player is a computer.
static const u8 s_teamColorTbl[3][2] = { { 0, 5 }, { 1, 6 }, { 3, 8 } };
// Stock mark colour frames (index 0 is not used).
static const u8 s_stockMarkTbl[4] = { 0xFF, 0x14, 0x15, 0x16 };

// HYPOTHESIS: the body of IfPlayer::setDamageState (cluster D owns the out-of-line copy), which the original inlines
// into appear: leave the current damage state through its post function, then enter the new one.
static inline void changeDamageState(IfPlayer* player, int state) {
    if (s_damageFuncTbl[player->m_damageState].m_post != NULL) {
        s_damageFuncTbl[player->m_damageState].m_post(player);
    }
    int prevState = player->m_damageState;
    player->m_damageState = state;
    if (s_damageFuncTbl[state].m_init != NULL) {
        s_damageFuncTbl[state].m_init(player, prevState);
    }
}

void IfPlayer::appear(IfPlayerAppearInfo* info, u8 unused) {
    bool isStamina = info->m_isStamina;
    s8 slot = info->m_locatorSlot;
    unk0_2 = true;
    m_0x0_0 = isStamina;
    m_scoreDisplay = info->m_scoreDisplay;
    unk0_3 = info->unk3C;
    if (slot >= 0) {
        unk3 = slot;
    } else {
        unk3 = unk1;
    }
    nw4r::g3d::ResMdl mdl(reinterpret_cast<void*>(*reinterpret_cast<u32*>(reinterpret_cast<u8*>(lbl_805A02DC->mpScnMdl) + 0xE8)));
    nw4r::g3d::ResNode node = mdl.GetResNode(lbl_80459CC0[unk3 + 1]);
    unk4 = node.IsValid() ? node->m_nodeIndex : 0;
    lbl_805A02DC->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[1]), unk4);
    MuObject* tag = m_objects[35];
    lbl_805A02DC->Remove(tag->m_sceneModel);
    if (unk0_2) {
        lbl_805A02DC->PushBack(tag->m_sceneModel, unk4);
    }
    MuObject* plate = m_objects[36];
    lbl_805A02DC->Remove(plate->m_sceneModel);
    if (info->m_type != 2 && unk0_2) {
        lbl_805A02DC->PushBack(plate->m_sceneModel, unk4);
    }
    plate->m_modelAnim->setUpdateRate(0.0f);
    int useSlot1 = info->m_useSlot1;
    u32 playerNo = info->m_playerNo;
    m_portraitRectPending = 1;
    m_loupeId = playerNo;
    unk34C = playerNo;
    if (useSlot1 == 1) {
        unk2 = unk1;
    } else {
        unk2 = playerNo;
    }
    m_portraitColorNo = info->m_colorNo;
    m_teamNo = info->m_teamNo;
    setStockFace(info->m_type, info->m_characterKind);
    if (m_teamBattle == 0) {
        if (info->m_isCpu == 0) {
            m_teamColor = unk2;
        } else {
            m_teamColor = 4;
        }
    } else {
        m_teamColor = s_teamColorTbl[info->m_teamNo][info->m_isCpu != 0];
    }
    setStockMarkColor(m_teamColor, info->m_markColor);
    switch (m_scoreMode) {
    case 1:
        updateStock(info->m_stockCount, 0);
        break;
    case 2: {
        updateCoinScoreSub(0);
        MuObject* counter = m_objects[24];
        counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        counter->setFrameVisible(0.0f);
        break;
    }
    case 0:
        if (m_scoreDisplay) {
            updateCoinScoreSub(0);
            MuObject* counter = m_objects[24];
            float frame = 1.0f;
            counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            counter->setFrameVisible(frame);
        } else {
            MuObject* counter = m_objects[24];
            counter->setFrameVisible(1.0f);
            counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        }
        break;
    }
    MuObject* badge = m_objects[40];
    float badgeFrame;
    if (info->m_tagSpecial != 0) {
        badgeFrame = 10.0f;
    } else if (info->m_isCpu == 0) {
        badgeFrame = (float)(unk2 + 1);
    } else {
        badgeFrame = 9.0f;
    }
    badge->setFrameTex(badgeFrame);
    badge->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    badge->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    lbl_805A02DC->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[0]));
    if (unk0_2) {
        lbl_805A02DC->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[0]), unk4);
    }
    u32 damage = info->m_damage;
    MuObject* a = m_objects[2];
    MuObject* d = m_objects[5];
    MuObject* c = m_objects[4];
    MuObject* b = m_objects[3];
    m_digitPos[0][0] = a->m_trans.m_x;
    m_digitPos[0][1] = a->m_trans.m_y;
    m_digitPos[0][2] = a->m_trans.m_z;
    m_digitPos[1][0] = b->m_trans.m_x;
    m_digitPos[1][1] = b->m_trans.m_y;
    m_digitPos[1][2] = b->m_trans.m_z;
    m_digitPos[2][0] = c->m_trans.m_x;
    m_digitPos[2][1] = c->m_trans.m_y;
    m_digitPos[2][2] = c->m_trans.m_z;
    m_digitPos[3][0] = d->m_trans.m_x;
    m_digitPos[3][1] = d->m_trans.m_y;
    m_digitPos[3][2] = d->m_trans.m_z;
    if (unk18 == unk1C) {
        dispDamageHP(damage, 1, 0);
    }
    unk18 = damage;
    changeDamageState(this, 0);
    if (info->m_name != NULL) {
        if (reinterpret_cast<msTexObj*>(m_texObj)->m_buffer == NULL) {
            makeTexture(info);
        }
        m_objects[40]->changeMaterialTex("PlyName_Dammy_Mat", &reinterpret_cast<msTexObj*>(m_texObj)->m_texObj);
        unk3A0 = 1;
    } else {
        unk3A0 = 0;
    }
    unk554 = 0;
}

void IfPlayer::makeTexture(IfPlayerAppearInfo* info) {
    if (reinterpret_cast<msTexObj*>(m_texObj)->m_buffer == NULL) {
        int useSlot1 = info->m_useSlot1;
        u32 playerNo = info->m_playerNo;
        m_loupeId = playerNo;
        if (useSlot1 == 1) {
            unk2 = unk1;
        } else {
            unk2 = playerNo;
        }
        if (m_teamBattle == 0) {
            if (info->m_isCpu == 0) {
                m_teamColor = unk2;
            } else {
                m_teamColor = 4;
            }
        } else {
            m_teamColor = s_teamColorTbl[info->m_teamNo][info->m_isCpu != 0];
        }
        const wchar_t* name = info->m_name;
        if (name != NULL) {
            // HYPOTHESIS: white text on a black box; the original keeps both in .sdata and makes extra copies of the
            // text colour (not reproduced exactly, see the stack layout diff).
            static GXColor colors[2] = { { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF } };
            GXColor white = colors[0];
            makeStringTex(m_texObj, name, white, colors[1], 0, 100, 20, GX_TF_I4, 20);
            int length = 0;
            while (*name != 0) {
                name++;
                length++;
            }
            m_nameLength = length;
        }
        for (int i = 0; i < 4; i++) {
            if (info->m_comments[i] != NULL) {
                m_apealComments[i].unk44 = makeApealCommentTex(&m_apealComments[i], info->m_comments[i]);
            } else {
                m_apealComments[i].unk44 = 0;
            }
        }
    }
}

int IfPlayer::makeApealCommentTex(ApealComment* comment, const wchar_t* text) {
    wchar_t line[30];
    static const GXColor colors[2] = { { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF } };
    GXColor white = colors[0];
    GXColor black = colors[1];
    wchar_t* first = line;
    int length = 0;
    // the comment is wrapped after ten characters
    for (; length < 10 && *text != 0; text++) {
        *first = *text;
        first++;
        length++;
    }
    if (*text != 0) {
        line[length] = '\n';
        length++;
        wchar_t* second = line + length;
        for (int i = 0; i < 10 && *text != 0; text++) {
            *second = *text;
            second++;
            length++;
            i++;
        }
    }
    line[length] = 0;
    makeStringTex(comment, line, white, black, 0, 200, 48, GX_TF_I4, 20);
    return length;
}

int IfPlayer::makeStringTex(void* texObj, const wchar_t* text, GXColor textColor, GXColor boxColor, int outline,
                            int width, int height, GXTexFmt format, int fontSize) {
    gfCamera camera;
    Message message(0x80, Heaps::MenuInstance);
    camera.init();
    camera.m_projectionType = gfCamera::Projection_Orthogonal;
    camera.m_viewportX = 0.0f;
    camera.m_viewportY = 0.0f;
    camera.m_viewportWidth = 640.0f;
    camera.m_viewportHeight = 480.0f;
    camera.m_viewportNear = 0.0f;
    camera.m_viewportFar = 0.99999f;
    camera.m_scissorX = 0;
    camera.m_scissorY = 0;
    camera.m_scissorWidth = 640;
    camera.m_scissorHeight = 480;
    camera.setGX();
    Message::drawBoxLine(*reinterpret_cast<s32*>(&boxColor), 0, 1, 0.0f, 0.0f, (float)width, (float)height, 0.0f);
    u8 font = fn_800B8648(lbl_8049E01C, 2, 0xFF, 0);
    const wchar_t* p = text;
    int length = 0;
    for (; *p != 0; p++) {
        length++;
    }
    char* utf8 = (char*)gfHeapManager::alloc(Heaps::MenuInstance, length * 4 + 1);
    Message::utf16to8(utf8, text);
    message.changeMsgBuf(-1);
    message.clearMsgBuf();
    message.setCallProjection(0);
    message.setWindow(0.0f, 0.0f, (float)width, (float)height);
    if (outline != 0) {
        message.setDrawFlag(0x100, 0x300);
        message.setDrawFlag(1, 3);
    } else {
        message.setDrawFlag(0, 0x300);
        message.setDrawFlag(0, 3);
    }
    message.setScale(0.03125f * (float)fontSize);
    message.setFixedWidth(-1.0f);
    message.setWidthModeAuto(1);
    message.setColor(textColor);
    message.setFace(2);
    message.printf(utf8);
    gfHeapManager::free(utf8);
    message.printMsgBuf(FontData::FONT_HIRAGANA);
    reinterpret_cast<msTexObj*>(texObj)->copyTexObjFromEFB(0.0f, 0.0f, (float)width, (float)height, format,
                                                            Heaps::MenuInstance, true);
    fn_800B881C(lbl_8049E01C, font);
    return length;
}

void IfPlayer::disappear() {
    int i;
    nw4r::g3d::ScnMdlExpand* group = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_groups[0]);
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove(group->sceneItemsCount - 1);
        }
    }
    for (i = 0; i < 3; i++) {
        if (m_groups[i] != NULL) {
            lbl_805A02DC->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[i]));
        }
    }
    for (i = 0; i < 50; i++) {
        if (m_objects[i] != NULL) {
            lbl_805A02DC->Remove(m_objects[i]->m_sceneModel);
        }
    }
    reinterpret_cast<msTexObj*>(m_texObj)->freeTexObj();
    unk3A0 = 0;
    for (i = 0; i < 4; i++) {
        reinterpret_cast<msTexObj*>(m_apealComments[i].m_texObj)->freeTexObj();
        m_apealComments[i].unk44 = 0;
    }
    unk0_2 = false;
}

void IfPlayer::updateDamageHP(u32 damage, int animate) {
    int i;
    bool changed = unk18 != unk1C;
    if (animate) {
        if (damage != unk18) {
            for (i = 2; i < 6; i++) {
                m_objects[i]->setFrameNode(50.0f);
            }
            for (i = 2; i < 6; i++) {
                m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            }
        }
    } else if (!changed) {
        dispDamageHP(damage, 1, 0);
    }
    unk18 = damage;
}

void IfPlayer::dispDamageHP(u32 damage, int setColor, int alt) {
    int perDigit;
    unk1C = damage;
    if (m_headCursorMode == 2) {
        fn_800E5318(this, damage);
    }
    if ((m_damageState == 2 || m_damageState == 4) && alt == 0) {
        return;
    }
    int i;
    for (i = 2; i < 6; i++) {
        lbl_805A02DC->Remove(m_objects[i]->m_sceneModel);
    }
    for (i = 6; i < 24; i++) {
        lbl_805A02DC->Remove(m_objects[i]->m_sceneModel);
    }
    perDigit = 1;
    if (alt != 0) {
        perDigit = 6;
    }
    u32 value = damage;
    if (value > 999) {
        value = 999;
    }
    // object numbers of the digit models: ones, tens, hundreds; plain digits first, alternate set second
    static const u8 digitTbl[3][2] = { { 3, 6 }, { 4, 12 }, { 5, 18 } };
    for (int digit = 0; digit < 3 && (value != 0 || digit <= 0); digit++) {
        u8 first;
        if (alt != 0) {
            first = digitTbl[digit][1];
        } else {
            first = digitTbl[digit][0];
        }
        for (int k = 0; k < perDigit; k++) {
            MuObject* obj = m_objects[first + k];
            if (unk0_2) {
                lbl_805A02DC->PushBack(obj->m_sceneModel, unk4);
            }
            obj->setFrameTex((float)(value % 10));
            obj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            if (alt == 0) {
                obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
                obj->setFrameVisible(1.0f);
            }
        }
        value /= 10;
    }
    float frame;
    if (m_0x0_0) {
        frame = 2.0f;
    } else {
        frame = 1.0f;
    }
    MuObject* percent = m_objects[2];
    if (unk0_2) {
        lbl_805A02DC->PushBack(percent->m_sceneModel, unk4);
    }
    percent->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    percent->setFrameVisible(frame);
    Vec3f pos;
    pos = percent->m_trans;
    pos.m_x = 0.0f;
    pos.m_y = 0.0f;
    percent->setTrans(&pos);
    if (setColor != 0) {
        setDamageNumColor(damage, alt);
    }
}

void IfPlayer::setDamageNumColor(u32 damage, int alt) {
    int perDigit;
    float color;
    if (m_0x0_0) {
        // stamina rule: the digits go from white to red as the health runs out
        color = 100.0f - (float)damage;
        if (color > 100.0f) {
            color = 100.0f;
        }
    } else {
        color = (float)damage / 10.0f;
        if (color > 100.0f) {
            color = 100.0f;
        }
    }
    perDigit = 1;
    if (alt != 0) {
        perDigit = 6;
    }
    static const u8 digitTbl[3][2] = { { 3, 6 }, { 4, 12 }, { 5, 18 } };
    if (alt == 0) {
        for (int digit = 0; digit < 3; digit++) {
            MuObject* obj = m_objects[digitTbl[digit][0]];
            obj->setFrameMatCol(color);
            obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    } else {
        for (int digit = 0; digit < 3; digit++) {
            u8 first = digitTbl[digit][1];
            for (int k = 0; k < perDigit; k++) {
                MuObject* obj = m_objects[first + k];
                obj->setFrameMatCol(0.0f);
                obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            }
        }
    }
    MuObject* percent = m_objects[2];
    percent->setFrameMatCol(color);
    percent->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void IfPlayer::updateStockCoin(int amount) {
    switch (m_scoreMode) {
    case 2: {
        updateCoinScoreSub(amount);
        MuObject* counter = m_objects[24];
        counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        counter->setFrameVisible(0.0f);
        break;
    }
    case 1:
        updateStock(amount, 1);
        break;
    case 0:
        if (m_scoreDisplay) {
            bool negative;
            if (amount < 0) {
                amount = -amount;
                negative = true;
            } else {
                negative = false;
            }
            updateCoinScoreSub(amount);
            float frame;
            if (negative) {
                frame = 2.0f;
            } else {
                frame = 1.0f;
            }
            MuObject* counter = m_objects[24];
            counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            counter->setFrameVisible(frame);
        }
        break;
    }
}

// Stock face state change shared by updateStock: run the init function of the state, then store the state.
static inline void setStockFaceState(IfPlayer* player, int index, int state) {
    if (s_stockFaceFuncTbl[state].m_init != NULL) {
        s_stockFaceFuncTbl[state].m_init(index, player);
    }
    player->m_stockFaceStates[index] = state;
}

void IfPlayer::updateStock(u32 count, int animate) {
    nw4r::g3d::ScnMdlExpand* group;
    u32 shown = count;
    if (shown > 99) {
        shown = 99;
    }
    if (unk0_3) {
        group = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_groups[0]);
        while (group->sceneItemsCount != 0) {
            if (group->sceneItemsCount != 0) {
                group->Remove(group->sceneItemsCount - 1);
            }
        }
        float frame;
        if (shown <= 5) {
            u32 stocks = m_stockCount;
            if (stocks > 5) {
                stocks = 5;
            }
            for (int i = 0; i < 5; i++) {
                int state;
                if ((u32)i < shown) {
                    state = 2;
                } else if ((u32)i < stocks && animate != 0) {
                    state = 1;
                } else {
                    state = 0;
                }
                setStockFaceState(this, i, state);
            }
            frame = 5.0f;
        } else {
            setStockFaceState(this, 0, 2);
            for (int i = 1; i < 5; i++) {
                setStockFaceState(this, i, 0);
            }
            int digit;
            for (digit = 0; digit < 2 && (shown != 0 || digit <= 0); digit++) {
                MuObject* obj = m_objects[30 + digit];
                float digitFrame = (float)(shown % 10);
                obj->m_modelAnim->setUpdateRate(0.0f);
                obj->setFrameTex(digitFrame);
                group->PushBack(obj->m_sceneModel, m_posNodeIds[digit]);
                shown /= 10;
            }
            frame = 6.0f + (float)digit;
        }
        MuObject* counter = m_objects[24];
        counter->m_modelAnim->setUpdateRate(0.0f);
        counter->setFrameVisible(frame);
    }
    m_stockCount = count;
}

void IfPlayer::receiveShareStock(Vec3f* pos) {
    if (m_stockCount < 5) {
        // MATCH-ONLY: component copy; the Vec3f assignment schedules its loads differently (still 88%).
        Vec3f* dst = &m_stockFacePos[m_stockCount];
        dst->m_x = pos->m_x;
        dst->m_y = pos->m_y;
        dst->m_z = pos->m_z;
        u32 index = m_stockCount;
        if (s_stockFaceFuncTbl[3].m_init != NULL) {
            s_stockFaceFuncTbl[3].m_init(index, this);
        }
        m_stockFaceStates[index] = 3;
        m_stockCount++;
    } else {
        updateStock(m_stockCount + 1, 0);
    }
}

bool IfPlayer::giveShareStock(Vec3f* pos) {
    if (m_stockCount == 0) {
        return false;
    }
    char name[8];
    sprintf(name, "pos%02d", (m_stockCount > 5 ? 0 : m_stockCount - 1) + 2);
    *pos = m_objects[24]->getGlobalPosition(name);
    updateStock(m_stockCount - 1, 0);
    return true;
}

void IfPlayer::setStockFace(int type, int characterKind) {
    MuObject* mini;
    int face;
    bool request = false;
    int mark;
    if (type == 1) {
        face = 0x259;
        mark = 0x13;
    } else {
        int stock = muMenu::exchangeGmCharacterKind2MuStockchkind(characterKind);
        int number = muMenu::exchangeGmCharacterKind2Something(stock);
        face = (int)(float)(number * 10 + 1);
        if (type == 2) {
            mark = 0x13;
        } else {
            mark = fn_800AF6F0(muMenu::exchangeGmCharacterKind2MuStockchkind(characterKind));
        }
    }
    int colorNo;
    if (type == 1) {
        colorNo = 0;
    } else {
        colorNo = m_portraitColorNo;
    }
    face += colorNo;
    mini = m_objects[35];
    float markFrame = 1.0f + (float)mark;
    mini->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    mini->setFrameTex(markFrame);
    m_objects[36]->setFrameTex((float)face);
    switch (m_portraitState) {
    case 0:
        request = true;
        break;
    case 1:
        if (m_portraitFaceId != face) {
            m_objects[36]->setFrameVisible(1.0f);
            request = true;
        }
        break;
    case 2:
        m_portraitReqId = face;
        m_portraitState = 3;
        break;
    case 3:
        m_portraitReqId = face;
        break;
    }
    if (request) {
        char path[64];
        sprintf(path, "/info/portrite/InfFace%03d.brres", face);
        reinterpret_cast<gfFileIOHandle*>(m_portraitFile)->readRequestNoSync(path, m_portraitBuffer, 0, 0);
        m_portraitFaceId = face;
        m_portraitState = 2;
    }
    m_stockFaceMode = type;
    m_stockFaceChar = characterKind;
}

void IfPlayer::setStockMarkColor(int color, int markColor) {
    int i;
    MuObject* obj;
    float frame;
    if (color < 4) {
        frame = 1.0f + (float)color;
    } else {
        frame = 9.0f + (float)(color - 4);
    }
    if (m_scoreMode != 2) {
        for (i = 0; i < 5; i++) {
            obj = m_objects[25 + i];
            obj->m_modelAnim->setUpdateRate(0.0f);
            obj->setFrameTex((float)color);
        }
    }
    obj = m_objects[35];
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    obj->setFrameMatCol(frame);
    float plateFrame = frame;
    if (markColor != 0) {
        plateFrame = (float)s_stockMarkTbl[markColor];
    }
    m_objects[36]->setFrameMatCol(plateFrame);
    obj = m_objects[37];
    obj->setFrameMatCol(frame);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    obj = m_objects[39];
    obj->setFrameMatCol(frame);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    obj = m_objects[40];
    obj->setFrameMatCol(frame);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void IfPlayer::updateCoinScoreSub(u32 amount) {
    MuObject* obj;
    nw4r::g3d::ScnMdlExpand* group = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_groups[0]);
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove(group->sceneItemsCount - 1);
        }
    }
    if (amount > 99999) {
        amount = 99999;
    }
    int digit;
    for (digit = 0; digit < 5 && (amount != 0 || digit <= 0); digit++) {
        obj = m_objects[30 + digit];
        float digitFrame = (float)(amount % 10);
        obj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        obj->setFrameTex(digitFrame);
        group->PushBack(obj->m_sceneModel, m_posNodeIds[digit]);
        amount /= 10;
    }
    MuObject* counter = m_objects[24];
    float digitCount = (float)digit;
    counter->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    counter->setFrameNode(digitCount);
    counter->calcMatrix();
    for (int i = 0; i < digit; i++) {
        m_objects[30 + i]->calcMatrix();
    }
}
