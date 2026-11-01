#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it adds the functions
// of the area of a player that the hand and the coin call.

#include <StaticAssert.h>
#include <mu/menu.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_object.h>
#include <mu/selchar/mu_selchar.h>
#include <mu/selchar/mu_select_character_list.h>
#include <mu/selchar/mu_select_character_name_entry.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <mu/selchar/mu_selchar_hand.h>
#include <gf/gf_pad_status.h>
#include <gm/gm_lib.h>
#include <types.h>

class muSelCharCBCalcWorld;

class muSelCharPlayerArea {
protected:
public:
    int _0;                 // 0x00 HYPOTHESIS: a state of the area; the hand and the coin stop while it is 1, the coin while it is not 0
    muMenuController m_menuController;
    nw4r::g3d::ScnGroup* m_scnGroup;    // 0xAC the group of the scene that the models of the area are drawn in
    // The models of the area. The names of the first ones come from the shared headers.
    enum Obj {
        Obj_Backplate = 0,
        Obj_PlayerNum = 1,
        Obj_FranchiseIcon = 2,
        Obj_Frag = 3,
        Obj_CharPic = 4,
        Obj_Unk5 = 5,
        Obj_CharName = 6,
        Obj_TagBtn = 7,
        Obj_Nameplate = 8,
        Obj_Handicap = 9,
        Obj_ComputerLvl = 10,
        Obj_Num = 40,
    };
    MuObject* m_objs[Obj_Num];          // 0xB0
    void* m_objProcTask;
    muSelCharCBCalcWorld m_calcWorldCBs[MAX_PLAYERS];
    muSelCharHand* m_selCharHand;
    muSelCharCoin* m_selCharCoin;
    int m_areaIdx;
    int m_playerKind;
    MuSelchkind m_charKind;
    int m_charColorNo;
    int m_teamColor;
    u8 m_teamSet;
    char _0x1C5[3];
    int m_nameId;
    char _0x1CC[0x8];
    int m_cpLevel;                      // 0x1D4 the level of the computer (the line of the list)
    char _0x1D8[0x4];
    int m_controllerNo;
    gfPadType::PadType m_controllerKind;
    char _0x1E4[0x4];
    int m_selectedZelda;
    int m_selectedSamus;
    int m_selectedPoke;
    muSelCharTask* m_selCharTask;
    bool m_charPicInter;                // 0x1F8 the picture of the character is lit (a coin is held over it)
    bool m_unk1F9;                      // 0x1F9 HYPOTHESIS: the picture keeps its color while it is set
    char _0x1FA[0x2];
    MuSelctChrList m_chrList;   // 0x1FC
    MuSelctChrNameEntry m_nameEntry;
    nw4r::g3d::ScnMdlExpand* m_baseMdlGroup;    // 0x404 HYPOTHESIS: the models attached to the nodes of the base model of the area
    char _0x408[0x30];
    nw4r::g3d::ResFileData* m_charPicData;
    nw4r::g3d::ResFile m_charPicRes;
    char _0x440[0x8];

    void setCharPic(int charKind, int playerKind, int curCostume, bool isTeamBattle, int teamColor, int unk);
    nw4r::g3d::ResFile* getCharPicTexResFile(int charKind);
    bool isTeamBattle();
    u8 exchangeCharKindDetail(MuSelchkind selCharKind);
    void dispMarkKind(MuSelchkind charKind);
    void setCharKind(MuSelchkind charKind);
    // The number of the color of the coin and of the hand: the number of the area, or the color of the team in a team battle,
    // counted from 1. Player kind 0 gets 6 and kind 2 gets 5; the flag asks for the color of kind 1 whatever the kind is.
    // HYPOTHESIS: the meaning of the flag.
    int getColorNo(bool asKind1);
    // Lights up the picture of the character while a coin is held over it (the coin is on the hand).
    // HYPOTHESIS: what the flag means, from the coin that passes whether it is on a hand.
    void setCharPicInter(bool inter);
    int getNumCharColor(int charKind);
    void rumbleController();
    void addObj(int obj);
    void removeObj(int obj);
    gfPadType::PadType getControllerKind();
    void removeFromBaseMdl(int obj);
    void closeHandiList();
    void closeCpLevelList();
    void addDisableNameList(int name);
    void delDisableNameList(int name);
    void clearDisableNameList();
    void openNameEntry();
    void openCpLevelList();
    void openNameList();
    void closeNameList();
    void addToBaseMdl(int obj);
    void dispCpLevel(int level);
    void dispNameEntryButtonColor(bool isTeamBattle, int teamColor);
    void dispPlayerNum(int playerKind);
    void dispPlayerKindButtonColor(int playerKind, bool isTeamBattle, int teamColor);
    void dispCpLevelVisible(int playerKind);
    void dispNameEntryButtonVisible(int playerKind, int unk);
    void closeNameEntry();
};
static_assert(sizeof(muSelCharPlayerArea) == 0x448, "Wrong size for class!");
