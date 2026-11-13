// Local BrawlHeaders shadow: adds typed initialization fields.
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
class MuMsg;
class IfWifiprPlayer;

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
    union { char _0x1CC[0x4]; int unk1CC; };
    int m_handi;                        // 0x1D0 the handicap (the line of the list)
    int m_cpLevel;                      // 0x1D4 the level of the computer (the line of the list)
    union { char _0x1D8[0x4]; int unk1D8; };
    int m_controllerNo;
    gfPadType::PadType m_controllerKind;
    union { char _0x1E4[0x4]; int unk1E4; };
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
    union {
        char _0x408[0x30];
        struct {
            char unk408[4];
            MuMsg* unk40C;
            char unk410[0x15];
            bool unk425;
            char unk426[0x12];
        };
        struct {
            char unk408Pad[0x14];
            IfWifiprPlayer* unk41C;
            char unk420[6];
            bool unk426Flag;
            char unk427[5];
            int unk42C;
            char unk430[8];
        };
        // HYPOTHESIS: field types follow constructor stores and model group creation.
        struct {
            nw4r::g3d::ScnMdlExpand* unk408Group;
            MuMsg* unk40CPad;
            int unk410Value;
            int unk414;
            int unk418;
            IfWifiprPlayer* unk41CPad;
            int unk420Value;
            bool unk424;
            u8 unk425Pad;
            u8 unk426Pad;
            u8 unk427Byte;
            int unk428;
            int unk42CPad;
            int unk430Value;
            u8 unk434;
            u8 unk435;
        };
    };
    nw4r::g3d::ResFileData* m_charPicData;
    nw4r::g3d::ResFile m_charPicRes;
    union { char _0x440[0x8]; struct { int unk440; u8 unk444; u8 unk445; u8 unk446; u8 unk447; }; };

    void main();
    void setCharPic(int charKind, int playerKind, int curCostume, bool isTeamBattle, int teamColor, int unk);
    nw4r::g3d::ResFile* getCharPicTexResFile(int charKind);
    bool isTeamBattle() __attribute__((never_inline)); // BYTES: the area calls it out of line, MWCC would inline it
    // The form is a full word: the original returns the table byte in r3 with no extra clear.
    MuSelchkind exchangeCharKindDetail(MuSelchkind selCharKind);
    void dispMarkKind(MuSelchkind charKind);
    void setCharKind(MuSelchkind charKind);
    MuSelchkind getCharKind();
    // The number of the color of the coin and of the hand: the number of the area, or the color of the team in a team battle,
    // counted from 1. Player kind 0 gets 6 and kind 2 gets 5; the flag asks for the color of kind 1 whatever the kind is.
    // HYPOTHESIS: the meaning of the flag.
    int getColorNo(bool asKind1);
    // Lights up the picture of the character while a coin is held over it (the coin is on the hand).
    // HYPOTHESIS: what the flag means, from the coin that passes whether it is on a hand.
    void setCharPicInter(bool inter);
    int getNumCharColor(int charKind) __attribute__((never_inline));
    void rumbleController();
    void addObj(int obj);
    void removeObj(int obj) __attribute__((never_inline));
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
    void incCharColorNo();
    void decCharColorNo();
    void dispCpLevelVisible(int playerKind);
    void dispNameEntryButtonVisible(int playerKind, int unk);
    void dispHandi(int handi);
    void setHandiVisible(bool visible);
    void dispMarkColor(int playerKind, bool isTeamBattle, int teamColor);
    void dispControllerKind(int playerKind, int controllerKind, int controllerNo);
    void setZeldas(int zelda);
    void setPoke3(int poke);
    int getHandiDispMode();
    void dispBaseMdlColor(int playerKind, bool isTeamBattle, int teamColor);
    void dispBaseMdlAnim(int playerKind);
    // The character kind of each of the three Pokemon of the Pokemon Trainer (and of the trainer).
    static u8 exchangePoke3ToGmCharKind(int poke);
    void closeNameEntry();
    void sendSystemCharKind();
    static void clearCharPicTexResArchive();
    static void setCharPicTexResArchive(void* archive); // HYPOTHESIS: the archive is an opaque pointer
    void submitFriendCertify();
    void submitFriendApply();
    void createNameModel(nw4r::g3d::ResFile* resFile, int priority);
    void createSingleModel(nw4r::g3d::ResFile* resFile, int priority);
    // HYPOTHESIS: the first parameter is an opaque array of eight-byte model descriptors.
    void createObjResFile(const void* descriptors, int count, nw4r::g3d::ResFile* resFile, int priority);
    void dispWifiFriendProfileName(const wchar_t* name);
    void dispWifiFriendProfileNameBaseColor();
    bool getWifiFriendProfileNameVisible();
    void colorChangePointed(int pointed);
    void initCharKind(MuSelchkind charKind);
    void dispWifiFriendName(const wchar_t* name, bool isTeamBattle, int teamColor);
    void bindCalcWorldFlag();
    void getDefName(char* name);
    void nameEntryButtonPointed(int pointed);
    void playerKindButtonPointed(int hand);
    void cpLevelButtonPointed(int hand);
    void wifiFriendCommandButtonPointed(int hand);
    void handiButtonPointed(int hand);
    bool isColorIncButtonPushed();
    bool isColorDecButtonPushed();
    // HYPOTHESIS: an inline accessor returns the area controller status by value.
    gfPadStatus getPadStatus();
    // The direction that the controller repeats (the trigger word of the menu controller).
    int getDirRepeat();
    int getCollKind(Vec2f* pos, int* arg, float* rect);
    void openHandiList();
    void forceCloseList();
    void charPicEffectMain();
    void setNameNo(int name);
    void setControllerNo(int controller) __attribute__((never_inline));
    void dispName(int name, int playerKind, bool teamBattle, int teamColor);
    u8 getWifiAID(u8* localPlayer);
    muSelCharPlayerArea();
    // HYPOTHESIS: initialization data is an opaque parameter block.
    static muSelCharPlayerArea* create(void* data);
    void initProc(void* data);
    void wifiRequestPlayerEntry(int entry);
    void dispWifiDispState(int state);
    void dispCharPicEffect();
    void setWifiFriendProfileNameVisible(bool visible);
    void createWifiModel(nw4r::g3d::ResFile* resFile, int priority);
    int getWifiStateByWifiCond();
    void wifiExitCpuPlayer();
    void setTeamColor(int color, int teamSet);
    void wifiFriendMain();
    void dispNumStar(int unk, int playerKind);
    void initDispSelect();
    void changeEntryModeToNormalMode(int controllerNo);
    void setPlayerKind(int playerKind);
    void wifiCPUPlayerEntry();
    void updateController();
    ~muSelCharPlayerArea();
};
static_assert(sizeof(muSelCharPlayerArea) == 0x448, "Wrong size for class!");
