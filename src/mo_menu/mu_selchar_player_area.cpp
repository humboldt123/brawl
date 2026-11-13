#include <memory.h>
#include <gm/gm_global.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <mu/mu_msg.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <stdio.h>
#include <string.h>
#include <gf/gf_archive.h>
#include <if/if_wifipr_player.h>
#include <revolution/CX/CXCompression.h>
#include <revolution/OS/OSCache.h>
#include <snd/snd_system.h>

// The menu helpers take three integers; this caller passes only the first one (see TIPS.md).
extern "C" int getNumCharColor__6muMenuFiii(int charKind);
extern "C" void startRumbleController__6muMenuFiii(int controller, int strength, int nameId);

// The pool ahead of the form tables. The form lookup addresses that pool and then these tables.
// HYPOTHESIS: these leading bytes are the constants the earlier missing functions emit; kept here so the
// form tables stay at the original offsets from the start of the pool.
// HYPOTHESIS: these leading bytes belong to functions that are still missing. They keep the form
// tables at the original offsets from the start of the pool.
#pragma force_active on
extern const u8 sFormPoolHead[0x18] = {
    0x00, 0x00, 0x00, 0x00, 0x3D, 0x80, 0x00, 0x00, 0x41, 0xE0, 0x00, 0x00, 0x01, 0x02, 0x05, 0x06,
    0x3F, 0x80, 0x00, 0x00, 0x01, 0x02, 0x05, 0x06,
};
#pragma force_active reset

// The two forms of Zelda and of Samus, and the Pokemon Trainer with his three Pokemon.
static const u8 sZeldaKinds[] = { 0x0E, 0x0F };
static const u8 sSamusKinds[] = { 0x03, 0x04 };
static const u8 sPokeKinds[] = { 0x1B, 0x1C, 0x1D, 0x1E };

// The animation of the colors of each team color.
static const u8 sTeamColorAnim[] = { 0, 1, 3 };

#pragma dont_inline on
// Reads the pad again and, after a short wait, shows the controller that is actually plugged in.
void muSelCharPlayerArea::updateController() {
    int read = 0;
    int nextKind = 4;
    int updated = 0;
    if (m_controllerNo >= 0) {
        gfPadStatus pad;
        g_gfPadSystem->getSysPadStatus(m_controllerNo, &pad);
        if (pad.m_error == gfPadError::NONE) {
            nextKind = pad.m_controllerType;
            read = 1;
        }
    }
    if (getControllerKind() != 4) {
        if (read == 0) {
            if (--unk1E4 <= 0) {
                m_controllerKind = static_cast<gfPadType::PadType>(4);
                // The pad-read flag is finished, so the player kind stays in that register across the call.
                read = *(volatile int*)&m_playerKind;
                dispControllerKind(read, getControllerKind(), m_controllerNo);
            }
            updated = 1;
        }
    }
    if (updated == 0) {
        if (m_controllerKind != nextKind) {
            m_controllerKind = static_cast<gfPadType::PadType>(nextKind);
            read = *(volatile int*)&m_playerKind;
            dispControllerKind(read, getControllerKind(), m_controllerNo);
        }
        unk1E4 = 0xF;
    }
}
#pragma dont_inline reset

int muSelCharPlayerArea::getNumCharColor(int charKind) {
    return getNumCharColor__6muMenuFiii(charKind);
}

// Rumbles the controller of a player (with the rumble setting of the name of the player).
void muSelCharPlayerArea::rumbleController() {
    if (m_controllerNo < 0) {
        return;
    }
    if (m_playerKind != 1) {
        return;
    }
    startRumbleController__6muMenuFiii(m_controllerNo, 10, m_nameId);
}

void muSelCharPlayerArea::addObj(int obj) {
    m_scnGroup->Insert(m_scnGroup->sceneItemsCount, m_objs[obj]->m_sceneModel);
}

void muSelCharPlayerArea::removeObj(int obj) {
    m_scnGroup->Remove(m_objs[obj]->m_sceneModel);
}

gfPadType::PadType muSelCharPlayerArea::getControllerKind() {
    return m_controllerKind;
}

void muSelCharPlayerArea::removeFromBaseMdl(int obj) {
    m_baseMdlGroup->Remove(m_objs[obj]->m_sceneModel);
}

void muSelCharPlayerArea::closeHandiList() {
    m_chrList.close();
}

void muSelCharPlayerArea::closeCpLevelList() {
    m_chrList.close();
}

void muSelCharPlayerArea::addDisableNameList(int name) {
    m_chrList.addDisableName(name);
}

void muSelCharPlayerArea::delDisableNameList(int name) {
    m_chrList.delDisableName(name);
}

void muSelCharPlayerArea::clearDisableNameList() {
    m_chrList.clearDisableName();
}

// Opens the keyboard to type a new name, with the pad of the player.
void muSelCharPlayerArea::openNameEntry() {
    m_nameEntry.m_padNo = m_controllerNo;
    m_nameEntry.open(nullptr, nullptr, 5);
}

void muSelCharPlayerArea::closeNameEntry() {
    m_nameEntry.close();
}

// Lights up the picture of the character (unless it keeps its color).
void muSelCharPlayerArea::setCharPicInter(bool inter) {
    MuObject* charPic = m_objs[Obj_CharPic];
    if (!m_unk1F9) {
        charPic->changeAnimN(0, 0x10, false, false);
        float frame = inter ? 1.0f : 0.0f;
        charPic->setFrameMatCol(frame);
    }
    m_charPicInter = inter;
}

void muSelCharPlayerArea::addToBaseMdl(int obj) {
    m_baseMdlGroup->Remove(m_objs[obj]->m_sceneModel);
    m_baseMdlGroup->PushBack(m_objs[obj]->m_sceneModel, "pos00");
}

// The level of the computer on its button.
void muSelCharPlayerArea::dispCpLevel(int level) {
    m_objs[Obj_ComputerLvl]->setFrameTex(1.0f + level);
}

// The color of the button of the name: the color of the team or of the player.
void muSelCharPlayerArea::dispNameEntryButtonColor(bool isTeamBattle, int teamColor) {
    m_objs[Obj_TagBtn]->changeAnimN(isTeamBattle ? sTeamColorAnim[teamColor] : m_areaIdx, 0x10, false, false);
}

void muSelCharPlayerArea::openCpLevelList() {
    int level = m_cpLevel;
    MuSelctChrList* list = &m_chrList;
    Vec3f pos;
    pos.m_x = pos.m_y = pos.m_z = 0.0f;
    list->open(MuSelctChrList::Kind_CpLevel, level, &pos);
}

void muSelCharPlayerArea::openNameList() {
    int name = m_nameId;
    MuSelctChrList* list = &m_chrList;
    Vec3f pos;
    pos.m_x = pos.m_y = pos.m_z = 0.0f;
    list->open(MuSelctChrList::Kind_Name, name, &pos);
}

void muSelCharPlayerArea::closeNameList() {
    m_chrList.clearDisableName();
    m_chrList.close();
}

// The number of the player on the base of the area: none, the number of the area or the mark of the computer.
void muSelCharPlayerArea::dispPlayerNum(int playerKind) {
    float frame;
    switch (playerKind) {
    case 0:
        frame = 0.0f;
        break;
    case 2:
        frame = 9.0f;
        break;
    case 1:
        frame = 1.0f + m_areaIdx;
        break;
    }
    m_objs[Obj_PlayerNum]->setFrameTex(frame);
}

void muSelCharPlayerArea::dispPlayerKindButtonColor(int playerKind, bool isTeamBattle, int teamColor) {
    int anim;
    switch (playerKind) {
    case 0:
        anim = 5;
        break;
    case 2:
        anim = 4;
        break;
    case 1:
        anim = isTeamBattle ? sTeamColorAnim[teamColor] : m_areaIdx;
        break;
    }
    m_objs[Obj_Nameplate]->changeAnimN(anim, 0x10, false, false);
}

// The button of the level of the computer is only on the area of a computer.
void muSelCharPlayerArea::dispCpLevelVisible(int playerKind) {
    if (playerKind == 2) {
        addToBaseMdl(Obj_ComputerLvl);
        m_objs[Obj_ComputerLvl]->setFrameVisible(1.0f);
    } else {
        removeFromBaseMdl(Obj_ComputerLvl);
    }
}

// The button of the name is only on the area of a player.
void muSelCharPlayerArea::dispNameEntryButtonVisible(int playerKind, int unk) {
    if (playerKind == 1 && unk != 1) {
        addToBaseMdl(Obj_TagBtn);
    } else {
        removeFromBaseMdl(Obj_TagBtn);
    }
}

// The handicap on its button.
void muSelCharPlayerArea::dispHandi(int handi) {
    MuObject* obj = m_objs[Obj_Handicap];
    obj->setFrameTex(handi);
    obj->setFrameNode(handi);
    obj->setFrameMatCol(handi);
}

void muSelCharPlayerArea::setHandiVisible(bool visible) {
    removeFromBaseMdl(Obj_Handicap);
    if (visible) {
        addToBaseMdl(Obj_Handicap);
    }
}

// Widens an index and adds one. Both branches of the color switches share this conversion.
static inline float onePlusIndex(int index) {
    return 1.0f + index;
}

// The color of the mark of the series of the character.
void muSelCharPlayerArea::dispMarkColor(int playerKind, bool isTeamBattle, int teamColor) {
    float frame;
    switch (playerKind) {
    case 0:
        frame = 0.0f;
        break;
    case 2:
        frame = 9.0f;
        break;
    case 1:
        if (isTeamBattle) {
            frame = onePlusIndex(sTeamColorAnim[teamColor]);
        } else {
            frame = onePlusIndex(m_areaIdx);
        }
        break;
    }
    m_objs[Obj_FranchiseIcon]->setFrameMatCol(frame);
}

// The kind of the controller of the player (and its number, for a Wii Remote) on the button of the player kind.
void muSelCharPlayerArea::dispControllerKind(int playerKind, int controllerKind, int controllerNo) {
    // The frame of the button of each kind of controller.
    static const u8 sControllerKindFrame[] = { 2, 1, 3, 4, 0 };
    MuObject* obj = m_objs[Obj_Nameplate];
    int number = 0;
    if (controllerKind != 4) {
        number = controllerNo % 4;
    }
    obj->setFrameTex(number);
    float frame = sControllerKindFrame[controllerKind];
    if (playerKind == 2) {
        frame += 5.0f;
    }
    obj->setFrameVisible(frame);
}

u8 muSelCharPlayerArea::exchangePoke3ToGmCharKind(int poke) {
    return sPokeKinds[poke];
}

// The two functions below keep the call to exchangeCharKindDetail out of line (the original has them in another object).
extern "C" int exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(muSelCharPlayerArea* self, int charKind);

// Whether another area with a player on it already uses this color of this character.
static inline int isCharColorUsed(muSelCharPlayerArea* self, muSelCharTask* task, int charKind, int color) {
    muSelCharPlayerArea* other;
    for (int i = 0; i < task->m_numPlayerAreas; i++) {
        if (i == self->m_areaIdx) {
            continue;
        }
        other = task->m_selCharPlayerAreas[i];
        if (other->m_playerKind != 0 && charKind == other->m_charKind && color == other->m_charColorNo) {
            return 1;
        }
    }
    return 0;
}

// Steps to the next color of the character that no other area already uses.
void muSelCharPlayerArea::incCharColorNo() {
    int charKind = m_charKind;
    if ((unsigned int)(charKind - 0x28) > 1) {
        int teamSet;
        int teamColor;
        int playerKind;
        bool isTeamBattle;
        int color;
        int numColor = getNumCharColor(charKind);
        muSelCharTask* task = m_selCharTask;
        color = m_charColorNo;
        do {
            color++;
            if (color >= numColor) {
                color = 0;
            }
        } while (isCharColorUsed(this, task, charKind, color));
        isTeamBattle = task->m_isTeamBattle;
        if (!isTeamBattle) {
            teamSet = m_teamSet;
            teamColor = m_teamColor;
            playerKind = m_playerKind;
            int kind = exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(this, m_charKind);
            setCharPic(kind, playerKind, color, isTeamBattle, teamColor, teamSet);
        }
        m_charColorNo = color;
    }
}

// Steps to the previous color of the character that no other area already uses.
void muSelCharPlayerArea::decCharColorNo() {
    int charKind = m_charKind;
    if ((unsigned int)(charKind - 0x28) > 1) {
        int teamSet;
        int teamColor;
        int playerKind;
        bool isTeamBattle;
        int color;
        int numColor = getNumCharColor(charKind);
        muSelCharTask* task = m_selCharTask;
        color = m_charColorNo;
        do {
            color--;
            if (color < 0) {
                color = numColor - 1;
            }
        } while (isCharColorUsed(this, task, charKind, color));
        isTeamBattle = task->m_isTeamBattle;
        if (!isTeamBattle) {
            teamSet = m_teamSet;
            teamColor = m_teamColor;
            playerKind = m_playerKind;
            int kind = exchangeCharKindDetail__19muSelCharPlayerAreaF11MuSelchkind(this, m_charKind);
            setCharPic(kind, playerKind, color, isTeamBattle, teamColor, teamSet);
        }
        m_charColorNo = color;
    }
}

// The character that is picked: the form of Zelda or Samus, or the Pokemon, that the area has chosen.
MuSelchkind muSelCharPlayerArea::exchangeCharKindDetail(MuSelchkind charKind) {
    switch (charKind) {
    case 14:
        charKind = static_cast<MuSelchkind>(sZeldaKinds[m_selectedZelda]);
        break;
    case 3:
        charKind = static_cast<MuSelchkind>(sSamusKinds[m_selectedSamus]);
        break;
    case 27:
        charKind = static_cast<MuSelchkind>(sPokeKinds[m_selectedPoke]);
        break;
    }
    return charKind;
}

// The color of the base of the area: none, the color of the team or of the player, or the color of the computers.
void muSelCharPlayerArea::dispBaseMdlColor(int playerKind, bool isTeamBattle, int teamColor) {
    float frame;
    switch (playerKind) {
    case 0:
        frame = 0.0f;
        break;
    case 2:
        frame = 9.0f;
        break;
    case 1:
        if (isTeamBattle) {
            frame = onePlusIndex(sTeamColorAnim[teamColor]);
        } else {
            frame = onePlusIndex(m_areaIdx);
        }
        break;
    }
    m_objs[Obj_Backplate]->setFrameMatCol(frame);
}

// The animation of the base of the area for the kind of its player.
void muSelCharPlayerArea::dispBaseMdlAnim(int playerKind) {
    float frame;
    float rate;
    switch (playerKind) {
    case 0:
        frame = 0.0f;
        rate = 0.0f;
        break;
    case 1:
        frame = 0.0f;
        rate = 1.0f;
        break;
    case 2:
        frame = 15.0f;
        rate = 0.0f;
        break;
    }
    MuObject* base = m_objs[Obj_Backplate];
    base->setFrameNode(frame);
    base->m_modelAnim->m_anmObjChrRes->SetUpdateRate(rate);
    base->setFrameVisible(frame);
    base->m_modelAnim->m_anmObjVisRes->SetUpdateRate(rate);
}

// Picks the form of Zelda (Zelda or Sheik): the picture changes when the area has Zelda.
void muSelCharPlayerArea::setZeldas(int zelda) {
    int color;
    int playerKind;
    int charKind = m_charKind;
    m_selectedZelda = zelda;
    if (charKind == 14) {
        charKind = exchangeCharKindDetail(static_cast<MuSelchkind>(charKind));
        color = m_charColorNo;
        playerKind = m_playerKind;
        setCharPic(charKind, playerKind, color, isTeamBattle(), m_teamColor, m_teamSet);
    }
}

// Picks the Pokemon of the Pokemon Trainer (or the trainer himself).
void muSelCharPlayerArea::setPoke3(int poke) {
    int color;
    int playerKind;
    int charKind = m_charKind;
    m_selectedPoke = poke;
    if (charKind == 27) {
        charKind = exchangeCharKindDetail(static_cast<MuSelchkind>(charKind));
        color = m_charColorNo;
        playerKind = m_playerKind;
        setCharPic(charKind, playerKind, color, isTeamBattle(), m_teamColor, m_teamSet);
    }
}

static inline bool isSelCharTypeIn(int type, int first, int end) {
    bool in;
    if (type >= first && type < end) {
        in = true;
    } else {
        in = false;
    }
    return in;
}

// muMenuController::init (the map name); include/mu/mu_menu_controller.h calls it by this stand-in name.
extern "C" void fn_800B96E8(muMenuController* controller, int pad);

muSelCharPlayerArea::muSelCharPlayerArea()
    : _0(0), m_scnGroup(NULL), m_objProcTask(NULL), m_selCharHand(NULL), m_selCharCoin(NULL),
      m_areaIdx(0), m_playerKind(0), m_charKind(static_cast<MuSelchkind>(0)), m_charColorNo(0),
      m_teamColor(0), m_teamSet(0), m_nameId(-1), unk1CC(0), m_handi(0), m_cpLevel(0), unk1D8(0),
      m_controllerKind(static_cast<gfPadType::PadType>(4)), unk1E4(0), m_selectedZelda(0),
      m_selectedSamus(0), m_selectedPoke(0), m_selCharTask(NULL), m_charPicInter(false), m_unk1F9(false),
      unk40CPad(NULL), unk410Value(0), unk414(0), unk418(1), unk41CPad(NULL), unk420Value(0),
      unk424(false), unk425Pad(0), unk426Pad(0), unk428(0), unk42CPad(0), unk430Value(0),
      unk434(0xFF), unk435(0xFF), m_charPicData(NULL), m_charPicRes(), unk440(0),
      unk444(0xFF), unk445(0xFF), unk446(0xFF), unk447(0xFF) {
    m_baseMdlGroup = NULL;
    unk408Group = NULL;
    for (int index = 0; index < Obj_Num; ++index) m_objs[index] = NULL;
    fn_800B96E8(&m_menuController, 0);
}



// How the handicap of the area is shown: 0 not at all (the solo modes), 1 or 2 (the setting of the rules).
// HYPOTHESIS: the groups of the kinds of the select screen and the meaning of the values.
int muSelCharPlayerArea::getHandiDispMode() {
    int type = g_GameGlobal->m_selCharData->m_selCharType;
    int group;
    if (isSelCharTypeIn(type, 0, 4)) { group = 0; goto groupSelected; }
    if (isSelCharTypeIn(type, 4, 12)) { group = 1; goto groupSelected; }
    {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (!inGroup) goto groupSelected;
        group = 2;
    }
    groupSelected:
    if (group == 1) {
        return 0;
    }
    if (m_selCharTask->m_unk5D0) {
        return 2;
    }
    return m_selCharTask->m_handiMode == 2;
}

bool muSelCharPlayerArea::isTeamBattle() {
    return m_selCharTask->m_isTeamBattle;
}

extern "C" int fn_800AF6F0(int stock);
extern void* lbl_10_bss_8[2];
extern const u8 lbl_10_rodata_648[0x40];
extern const u8 lbl_10_rodata_688[0x8];

void muSelCharPlayerArea::clearCharPicTexResArchive() {
    lbl_10_bss_8[0] = nullptr;
}

void muSelCharPlayerArea::setCharPicTexResArchive(void* archive) {
    lbl_10_bss_8[0] = archive;
}

// Leaves the entry mode: the area gets the pad that pressed, and the hand goes back to where it started.
void muSelCharPlayerArea::changeEntryModeToNormalMode(int controllerNo) {
    removeObj(0x23);
    setControllerNo(controllerNo);
    m_menuController.setControllerID((u8)controllerNo);
    initDispSelect();
    muSelCharHand* hand = m_selCharHand;
    hand->m_pos.m_x = hand->m_startPos.m_x;
    hand->m_pos.m_y = hand->m_startPos.m_y;
    hand->m_pos.m_z = hand->m_startPos.m_z;
    _0 = 0;
}

// The number of the color of the coin and of the hand (see the header).
int muSelCharPlayerArea::getColorNo(bool asKind1) {
    int kind = asKind1 ? 1 : m_playerKind;
    switch (kind) {
    case 0:
        return 6;
    case 2:
        return 5;
    }
    if (m_selCharTask->m_isTeamBattle) {
        return sTeamColorAnim[m_teamColor] + 1;
    }
    return m_areaIdx + 1;
}

// The mark of the character on the base of the area: the two special marks, or the number of the stock kind.
void muSelCharPlayerArea::dispMarkKind(MuSelchkind charKind) {
    float frame;
    switch (charKind) {
    case 0x28:
        frame = 0.0f;
        break;
    case 0x29:
        frame = 30.0f;
        break;
    default:
        frame = (float)(fn_800AF6F0(muMenu::exchangeMuSelchkind2MuStockchkind(charKind)) + 1);
        break;
    }
    m_objs[Obj_FranchiseIcon]->setFrameTex(frame);
}

void muSelCharPlayerArea::submitFriendCertify() {
    g_muWifiInterfaceTask->requestExchangeFriendKey(m_areaIdx);
}

void muSelCharPlayerArea::createNameModel(nw4r::g3d::ResFile* resFile, int priority) {
    createObjResFile(lbl_10_rodata_648, 8, resFile, priority);
}

void muSelCharPlayerArea::createSingleModel(nw4r::g3d::ResFile* resFile, int priority) {
    createObjResFile(lbl_10_rodata_688, 1, resFile, priority);
}

void muSelCharPlayerArea::submitFriendApply() {
    g_muWifiInterfaceTask->requestExchangeFriendKey(m_areaIdx);
    unk425 = true;
}

void muSelCharPlayerArea::dispWifiFriendProfileName(const wchar_t* name) {
    char nameUtf8[0x20];
    Message::utf16to8(nameUtf8, name);
    unk40C->printf(0x1D, nameUtf8);
}

void muSelCharPlayerArea::dispWifiFriendProfileNameBaseColor() {
    m_objs[37]->setFrameMatCol(m_areaIdx);
}

bool muSelCharPlayerArea::getWifiFriendProfileNameVisible() {
    return 0.0f != m_objs[37]->m_modelAnim->m_anmObjVisRes->GetFrame();
}

void muSelCharPlayerArea::openHandiList() {
    int kind;
    int handi;
    int type = g_GameGlobal->m_selCharData->m_selCharType;
    int group;
    if (isSelCharTypeIn(type, 0, 4)) { group = 0; goto groupSelected; }
    if (isSelCharTypeIn(type, 4, 12)) { group = 1; goto groupSelected; }
    {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (!inGroup) goto groupSelected;
        group = 2;
    }
    groupSelected:
    int mode;
    if (group == 1) mode = 0;
    else if (m_selCharTask->m_unk5D0) mode = 2;
    else mode = m_selCharTask->m_handiMode == 2;
    switch (mode) {
    case 1:
        kind = MuSelctChrList::Kind_Handi;
        handi = m_handi;
        break;
    case 2:
        kind = MuSelctChrList::Kind_Hp;
        handi = m_handi;
        break;
    }
    Vec3f pos;
    pos.m_x = pos.m_y = pos.m_z = 0.0f;
    m_chrList.open(kind, handi, &pos);
}

void muSelCharPlayerArea::setNameNo(int name) {
    dispName(name, m_playerKind, m_selCharTask->m_isTeamBattle, m_teamColor);
    m_nameId = name;
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    if (wifi) {
        wchar_t text[6];
        if (name >= 0) {
            int type = g_GameGlobal->m_selCharData->m_selCharType;
            int group;
            if (isSelCharTypeIn(type, 0, 4)) { group = 0; goto groupSelected; }
            if (isSelCharTypeIn(type, 4, 12)) { group = 1; goto groupSelected; }
            {
                bool inGroup = isSelCharTypeIn(type, 12, 16);
                group = 3;
                if (!inGroup) goto groupSelected;
                group = 2;
            }
            groupSelected:
            if (group == 2) {
                g_GameGlobal->getNameUTF16(name, text);
                goto sendName;
            }
        }
        text[0] = 0;
    sendName:
        wifi->setLocalPlayersOnamae(m_areaIdx, text, name < 0 ? 120 : static_cast<u8>(name));
    }
}


void muSelCharPlayerArea::setControllerNo(int controller) {
    m_controllerNo = controller;
    if ((controller >= 0) & (controller < 8)) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(controller, &status);
        m_controllerKind = status.m_controllerType;
    } else {
        m_controllerKind = static_cast<gfPadType::PadType>(4);
    }
    dispControllerKind(m_playerKind, m_controllerKind, m_controllerNo);
    if (controller < 0) forceCloseList();
    if (controller >= 0) m_menuController.setControllerID(static_cast<u8>(controller));
    m_selCharHand->updateControllerNo();
}

extern const u8 lbl_10_rodata_744[6][2];
extern char lbl_10_data_B08[0x18];

void muSelCharPlayerArea::colorChangePointed(int pointed) {
    if (!m_selCharTask->m_isTeamBattle) return;
    int anim;
    int teamColor = m_teamColor;
    if (pointed >= 0) anim = lbl_10_rodata_744[teamColor][1];
    else anim = lbl_10_rodata_744[teamColor][0];
    const char* name = lbl_10_data_B08;
    static MuAnimNameData animations[] = {
        {0.0f, 0.0f, 0, name, 0x10},
        {30.0f, 30.0f, 0, name, 0x10},
        {60.0f, 60.0f, 0, name, 0x10},
        {0.0f, 29.0f, 1, name, 0x10},
        {30.0f, 59.0f, 1, name, 0x10},
        {60.0f, 89.0f, 1, name, 0x10}
    };
    m_objs[Obj_Frag]->setAnimName(&animations[anim], false);
}

void muSelCharPlayerArea::initCharKind(MuSelchkind charKind) {
    switch (charKind) {
    case Selch_Samus: m_selectedSamus = 0; charKind = Selch_Samus; break;
    case Selch_SZeroSuit: m_selectedSamus = 0; charKind = Selch_Samus; break;
    case Selch_Zelda: m_selectedZelda = 0; charKind = Selch_Zelda; break;
    case Selch_Sheik: m_selectedZelda = 1; charKind = Selch_Zelda; break;
    case Selch_PokeTrainer: m_selectedPoke = 0; charKind = Selch_PokeTrainer; break;
    case Selch_PokeLizardon: m_selectedPoke = 1; charKind = Selch_PokeTrainer; break;
    case Selch_PokeZenigame: m_selectedPoke = 2; charKind = Selch_PokeTrainer; break;
    case Selch_PokeFushigisou: m_selectedPoke = 3; charKind = Selch_PokeTrainer; break;
    }
    m_charKind = charKind;
}

extern const GXColor lbl_10_rodata_750[6];

void muSelCharPlayerArea::dispWifiFriendName(const wchar_t* name, bool isTeamBattle, int teamColor) {
    GXColor color = lbl_10_rodata_750[isTeamBattle ? sTeamColorAnim[teamColor] : m_areaIdx];
    unk40C->setFontColor(0x1E, color.r, color.g, color.b, 255);
    char nameUtf8[0x14];
    Message::utf16to8(nameUtf8, name);
    unk40C->printf(0x1E, nameUtf8);
}

extern const u8 lbl_10_rodata_6AC[8];
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 op);

void muSelCharPlayerArea::bindCalcWorldFlag() {
    nw4r::g3d::ScnMdl* model;
    for (int index = 0; index < 7; ++index) {
        int obj;
        if (index >= 0 && index < 7) obj = lbl_10_rodata_6AC[index];
        else obj = Obj_Num;
        MuObject* object = m_objs[obj];
        if (object) {
            model = object->getSceneModel();
            m_calcWorldCBs[index].m_next = model->m_callback;
            model->m_callback = &m_calcWorldCBs[index];
            ScnObj_EnableCallbackExecOp(model, 4);
            ScnObj_EnableCallbackTiming(model, 1);
        }
    }
}

void muSelCharPlayerArea::getDefName(char* name) {
    int nameNumber = unk1CC;
    int length;
    const char* message = unk40C->getMsgData(nameNumber > 0, &length);
    memcpy(name, message, length);
    char number[16];
    if (nameNumber > 0) sprintf(number, "%02d", nameNumber);
    else sprintf(number, "%2d", m_areaIdx + 1);
    strcpy(name + length, number);
}

void muSelCharPlayerArea::nameEntryButtonPointed(int pointed) {
    MuObject* object = m_objs[Obj_TagBtn];
    float rate = object->m_modelAnim->m_anmObjMatClrRes->GetUpdateRate();
    if (pointed >= 0) {
        if (0.0f == rate) object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    } else {
        if (0.0f != rate) {
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
}

u8 muSelCharPlayerArea::getWifiAID(u8* localPlayer) {
    u8 player;
    u8 aid = g_muWifiInterfaceTask->getPlayerArea(m_areaIdx, &player);
    if (aid == 0xFF) {
        player = 0xFF;
    }
    *localPlayer = player;
    return aid;
}

muSelCharPlayerArea* muSelCharPlayerArea::create(void* data) {
    muSelCharPlayerArea* area = new (Heaps::MenuInstance) muSelCharPlayerArea;
    area->initProc(data);
    return area;
}

void muSelCharPlayerArea::wifiRequestPlayerEntry(int entry) {
    if (g_muWifiInterfaceTask->requestApproval(m_areaIdx)) {
        dispWifiDispState(2);
        unk42C = entry;
        unk426Flag = true;
    }
}

nw4r::g3d::ResFile* muSelCharPlayerArea::getCharPicTexResFile(int charKind) {
    int stockKind = muMenu::exchangeMuSelchkind2MuStockchkind(charKind);
    int fileIndex = muMenu::exchangeGmCharacterKind2Something(stockKind);
    void* compressed = static_cast<gfArchive*>(lbl_10_bss_8[0])->getData(Data_Type_Misc, fileIndex, 0xFFFE);
    CXUncompressLZ(compressed, m_charPicData);
    DCFlushRange(m_charPicData, 0x40000);
    m_charPicRes = nw4r::g3d::ResFile(m_charPicData);
    nw4r::g3d::ResFile::Init(&m_charPicRes);
    return &m_charPicRes;
}


void muSelCharPlayerArea::dispCharPicEffect() {
    m_objs[Obj_CharPic]->changeAnimN(1, 0x10, true, false);
    m_objs[Obj_Unk5]->setFrameMatCol(1.0f);
    m_objs[Obj_Unk5]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_unk1F9 = true;
}

extern char lbl_10_data_BA8[8];

void muSelCharPlayerArea::setWifiFriendProfileNameVisible(bool visible) {
    m_objs[37]->setFrameVisible(visible ? 1.0f : 0.0f);
    if (!visible) {
        unk40C->printf(0x1D, lbl_10_data_BA8);
        unk40C->printf(0x1E, lbl_10_data_BA8);
    }
}

extern const u8 lbl_10_rodata_690[0x18];

void muSelCharPlayerArea::createWifiModel(nw4r::g3d::ResFile* resFile, int priority) {
    createObjResFile(lbl_10_rodata_690, 3, resFile, priority);
    int playerNo = m_areaIdx;
    unk41C = new (Heaps::MenuInstance) IfWifiprPlayer(playerNo, 1);
    unk41C->createModel(resFile, priority);
}

void muSelCharPlayerArea::playerKindButtonPointed(int hand) {
    MuObject* object = m_objs[Obj_Nameplate];
    float rate = object->m_modelAnim->m_anmObjMatClrRes->GetUpdateRate();
    if (hand >= 0) {
        if (0.0f == rate) {
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
    } else {
        if (0.0f != rate) {
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
}

void muSelCharPlayerArea::cpLevelButtonPointed(int hand) {
    MuObject* object = m_objs[Obj_ComputerLvl];
    float rate = object->m_modelAnim->m_anmObjMatClrRes->GetUpdateRate();
    if (hand >= 0) {
        if (0.0f == rate) {
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
    } else {
        if (0.0f != rate) {
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
}

void muSelCharPlayerArea::wifiFriendCommandButtonPointed(int hand) {
    MuObject* object = m_objs[38];
    float rate = object->m_modelAnim->m_anmObjMatClrRes->GetUpdateRate();
    if (hand >= 0) {
        if (0.0f == rate) {
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        }
    } else {
        if (0.0f != rate) {
            object->setFrameMatCol(0.0f);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
}

void muSelCharPlayerArea::handiButtonPointed(int hand) {
    MuObject* object = m_objs[Obj_Handicap];
    if (object) {
        float rate = object->m_modelAnim->m_anmObjMatClrRes->GetUpdateRate();
        if (hand >= 0) {
            if (0.0f == rate) {
                object->changeAnimN(1, 0x10, true, false);
            }
        } else {
            if (0.0f != rate) {
                object->changeAnimN(0, 0x10, false, false);
                object->setFrameMatCol(m_handi);
            }
        }
    }
}

void muSelCharPlayerArea::createObjResFile(const void* descriptors, int count, nw4r::g3d::ResFile* resFile, int priority) {
    int num;
    int index;
    int instance;
    // HYPOTHESIS: eight-byte descriptors contain a model name, index range and priority.
    const struct {
        const char* unk0;
        u8 unk4;
        u8 unk5;
        u8 unk6;
        u8 unk7;
    }* model = static_cast<const __typeof__(*model)*>(descriptors);
    for (index = 0; index < count; index++, model++) {
        num = 1;
        if (model->unk4 < model->unk5) {
            num = model->unk5 - model->unk4;
        }
        for (instance = 0; instance < num; instance++) {
            MuObject* object = MuObject::create(resFile, model->unk0, model->unk6 + priority, NULL, Heaps::MenuInstance);
            m_objs[model->unk4 + instance] = object;
            char animName[0x40];
            strcpy(animName, model->unk0);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

extern const u32 lbl_10_rodata_788[4];

// HYPOTHESIS: an inline accessor returns the area controller status by value.

inline gfPadStatus muSelCharPlayerArea::getPadStatus() {
    gfPadStatus pad;
    if (m_controllerNo >= 0) {
        g_gfPadSystem->getSysPadStatus(m_controllerNo, &pad);
    } else {
        memset(&pad, 0, sizeof(pad));
    }
    return pad;
}

bool muSelCharPlayerArea::isColorIncButtonPushed() {
    gfPadStatus pad = getPadStatus();
    return (pad.m_buttonsPressedThisFrame.bits & lbl_10_rodata_788[pad.m_controllerType]) != 0;
}

extern const int lbl_10_rodata_590[12];

void muSelCharPlayerArea::wifiFriendMain() {
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    if (unk428 > 0 && --unk428 > 0) {
        return;
    }
    if (unk426Flag) {
        int permitted = wifi->isPermittedEntry(m_areaIdx);
        if (permitted == 1 || permitted == 2) {
            if (permitted == 1) {
                int entry = unk42C;
                unk41C->setWifiDispState(0);
                unk420Value = 0;
                removeObj(0x25);
                removeObj(0x26);
                removeObj(0x27);
                m_controllerNo = entry;
                if (entry >= 0) {
                    m_menuController.setControllerID(static_cast<u8>(entry));
                }
                initDispSelect();
                if (entry >= 0) {
                    muSelCharHand* hand = m_selCharHand;
                    hand->m_pos = hand->m_startPos;
                }
                _0 = 0;
                setPlayerKind(1);
                unk1D8 = 0;
                dispNumStar(0, m_playerKind);
                g_sndSystem->playSE(static_cast<SndID>(0x26), -1, 0, 0, -1);
            } else {
                dispWifiDispState(3);
                unk428 = 0x78;
                wifi->exitNodePlayer(m_areaIdx);
            }
            unk426Flag = false;
        }
    } else {
        int state = lbl_10_rodata_590[getWifiStateByWifiCond()];
        if (state == 0) {
            int type = g_GameGlobal->m_selCharData->m_selCharType;
            if (static_cast<u32>(type - 13) > 2 && m_selCharTask->isWifiGameMaster()) {
                state = 1;
            }
        }
        if (state != unk430Value) {
            dispWifiDispState(state);
            if (state == 1) {
                g_sndSystem->playSE(static_cast<SndID>(0x2052), -1, 0, 0, -1);
            }
        }
    }
}

extern const u32 lbl_10_rodata_798[8];

bool muSelCharPlayerArea::isColorDecButtonPushed() {
    gfPadStatus pad = getPadStatus();
    return (pad.m_buttonsPressedThisFrame.bits & lbl_10_rodata_798[pad.m_controllerType]) != 0;
}

// A computer takes the area over wifi: the entry models go away and the area becomes a computer.
void muSelCharPlayerArea::wifiCPUPlayerEntry() {
    g_muWifiInterfaceTask->setCpuPlayerArea(m_areaIdx, 1);
    unk41C->setWifiDispState(0);
    unk420Value = 0;
    removeObj(0x25);
    removeObj(0x26);
    removeObj(0x27);
    m_controllerNo = -1;
    initDispSelect();
    _0 = 0;
    setPlayerKind(2);
}

muSelCharPlayerArea::~muSelCharPlayerArea() {
    if (m_selCharCoin) {
        delete m_selCharCoin;
    }
    if (m_selCharHand) {
        m_selCharHand->exit();
    }
    if (m_objProcTask) {
        static_cast<gfTask*>(m_objProcTask)->exit();
        m_objProcTask = NULL;
    }
    for (int index = 0; index < 2; ++index) {
        nw4r::g3d::ScnMdlExpand* group = (&m_baseMdlGroup)[index];
        if (group) {
            group->Destroy();
        }
    }
    for (int index = 0; index < Obj_Num; ++index) {
        if (m_objs[index]) {
            delete m_objs[index];
        }
    }
    if (unk41C) {
        delete unk41C;
    }
    if (unk40C) {
        delete unk40C;
    }
    if (m_charPicData) {
        gfHeapManager::free(m_charPicData);
        m_charPicData = NULL;
    }
}

// The hand stores the mode through a pointer. Defined after the caller so the check stays.
static int readHandMode(muSelCharHand* hand, int* arg);

// Closes a list the hand of this area still has open, then puts that hand back to moving.
void muSelCharPlayerArea::forceCloseList() {
    int modeArg;
    int closed = 0;
    for (int index = 0; index < m_selCharTask->m_numPlayerAreas; ++index) {
        muSelCharHand* hand = m_selCharTask->getHand(index);
        int mode = readHandMode(hand, &modeArg);
        switch (mode) {
        case 5:
            if (modeArg == m_areaIdx) {
                m_chrList.close();
                closed = 1;
            }
            break;
        case 6:
            if (modeArg == m_areaIdx) {
                m_chrList.close();
                closed = 1;
            }
            break;
        case 7:
            if (modeArg == m_areaIdx) {
                m_chrList.clearDisableName();
                m_chrList.close();
                closed = 1;
            }
            break;
        case 8:
            if (modeArg == m_areaIdx) {
                m_nameEntry.close();
                closed = 1;
            }
            break;
        }
        if (closed) {
            hand->setMode(0, 0);
            break;
        }
    }
}

// The picture flash reads 0, 1 and 20 from one pool. The leading bytes are the common-model table.
struct PicPool {
    u8 head[0xE8];
    float zero;
    u8 gap[0xC];
    float one;
    u8 gap2[0x84];
    float twenty;
};
static const PicPool sPicPool = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00 },
    0.0f,
    {},
    1.0f,
    {},
    20.0f,
};

// Finishes the flash of the character picture once its color animation has played out.
void muSelCharPlayerArea::charPicEffectMain() {
    const PicPool* pool = &sPicPool;
    MuObject* charPic = m_objs[Obj_CharPic];
    if (!m_unk1F9) {
        return;
    }
    float frame = m_objs[Obj_Unk5]->m_modelAnim->m_anmObjMatClrRes->GetFrame();
    if (frame >= pool->twenty) {
        charPic->changeAnimN(0, 0x10, false, false);
        float col = m_charPicInter ? pool->one : pool->zero;
        charPic->setFrameMatCol(col);
        m_objs[Obj_Unk5]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(pool->zero);
        m_unk1F9 = false;
    }
}

static int readHandMode(muSelCharHand* hand, int* arg) {
    if (arg) {
        *arg = hand->m_modeArg;
    }
    return hand->m_mode;
}

// HYPOTHESIS: only the count of the friends held by the matching manager is used here.
struct SelCharMatchingHyp {
    char _pad[0x3408];
    int m_friendCount;
};
extern SelCharMatchingHyp* lbl_805A0434;
extern "C" int fn_800C9F14(muWifiInterfaceTask* self, int index);

// The state that the wifi conditions give to the area (0 when the select screen is not the wifi one).
// HYPOTHESIS: the values are the states that dispWifiDispState shows.
int muSelCharPlayerArea::getWifiStateByWifiCond() {
    int type = g_GameGlobal->m_selCharData->m_selCharType;
    int group;
    if (isSelCharTypeIn(type, 0, 4)) { group = 0; goto groupSelected; }
    if (isSelCharTypeIn(type, 4, 12)) { group = 1; goto groupSelected; }
    {
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        group = 3;
        if (!inGroup) goto groupSelected;
        group = 2;
    }
    groupSelected:
    if (group != 2) {
        return 0;
    }
    if (unk428 > 0) {
        return 2;
    }
    if (unk426Flag) {
        return 1;
    }
    muWifiInterfaceTask* wifi = g_muWifiInterfaceTask;
    if (!fn_800C9F14(wifi, m_areaIdx)) {
        return 0;
    }
    if (wifi->isExchangingFriendKey(m_areaIdx)) {
        return 5;
    }
    if (wifi->getFriendAIDIndex(m_areaIdx) >= 0) {
        return 5;
    }
    if (unk425) {
        return 4;
    }
    if (wifi->isRequestedExchangeFriendKey(m_areaIdx)) {
        return 7;
    }
    if (!wifi->isEnoughRoomForFriendList(m_areaIdx) || lbl_805A0434->m_friendCount >= 0x40) {
        return 6;
    }
    return 3;
}
