#include <memory.h>
#include <mu/selchar/mu_selchar_player_area.h>
#include <types.h>

// The menu helpers take three integers; this caller passes only the first one (see TIPS.md).
extern "C" int getNumCharColor__6muMenuFiii(int charKind);
extern "C" void startRumbleController__6muMenuFiii(int controller, int strength, int nameId);

// The animation of the colors of each team color.
static const u8 sTeamColorAnim[] = { 0, 1, 3 };

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
