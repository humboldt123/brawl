#include <memory.h>
#include <gm/gm_global.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>

// The menu helpers take three integers; this caller passes only the first one (see TIPS.md).
extern "C" int getNumCharColor__6muMenuFiii(int charKind);
extern "C" void startRumbleController__6muMenuFiii(int controller, int strength, int nameId);

// The animation of the colors of each team color.
static const u8 sTeamColorAnim[] = { 0, 1, 3 };

// The two forms of Zelda and of Samus, and the Pokemon Trainer with his three Pokemon.
static const u8 sZeldaKinds[] = { 0x0E, 0x0F };
static const u8 sSamusKinds[] = { 0x03, 0x04 };
static const u8 sPokeKinds[] = { 0x1B, 0x1C, 0x1D, 0x1E };

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
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_chrList.open(MuSelctChrList::Kind_CpLevel, m_cpLevel, &pos);
}

void muSelCharPlayerArea::openNameList() {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_chrList.open(MuSelctChrList::Kind_Name, m_nameId, &pos);
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
            frame = 1.0f + sTeamColorAnim[teamColor];
        } else {
            frame = 1.0f + m_areaIdx;
        }
        break;
    }
    m_objs[Obj_FranchiseIcon]->setFrameMatCol(frame);
}

// The kind of the controller of the player (and its number, for a Wii Remote) on the button of the player kind.
void muSelCharPlayerArea::dispControllerKind(int playerKind, int controllerKind, int controllerNo) {
    // The frame of the button of each kind of controller.
    static const u8 kindFrame[] = { 2, 1, 3, 4, 0 };
    MuObject* obj = m_objs[Obj_Nameplate];
    int number = 0;
    if (controllerKind != 4) {
        number = controllerNo % 4;
    }
    obj->setFrameTex(number);
    float frame = kindFrame[controllerKind];
    if (playerKind == 2) {
        frame += 5.0f;
    }
    obj->setFrameVisible(frame);
}

u8 muSelCharPlayerArea::exchangePoke3ToGmCharKind(int poke) {
    return sPokeKinds[poke];
}

// The character that is picked: the form of Zelda or Samus, or the Pokemon, that the area has chosen.
u8 muSelCharPlayerArea::exchangeCharKindDetail(MuSelchkind charKind) {
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
            frame = 1.0f + sTeamColorAnim[teamColor];
        } else {
            frame = 1.0f + m_areaIdx;
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

// How the handicap of the area is shown: 0 not at all (the solo modes), 1 or 2 (the setting of the rules).
// HYPOTHESIS: the groups of the kinds of the select screen and the meaning of the values.
int muSelCharPlayerArea::getHandiDispMode() {
    int type = g_GameGlobal->m_selCharData->m_selCharType;
    int group;
    if (isSelCharTypeIn(type, 0, 4)) {
        group = 0;
    } else if (isSelCharTypeIn(type, 4, 12)) {
        group = 1;
    } else if (isSelCharTypeIn(type, 12, 16)) {
        group = 2;
    } else {
        group = 3;
    }
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
