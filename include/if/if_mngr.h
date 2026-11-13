#pragma once
// Local BrawlHeaders shadow: names the shared game-2D scene group, the HUD sub-managers and the recovered IfMngr operations.

#include <StaticAssert.h>
#include <if/if_player.h>
#include <gm/gm_lib.h>
#include <types.h>

namespace nw4r { namespace g3d { class ScnGroup; class ScnObj; class ScnChoice; } }

class gfArchive;
class gfResourceLoader;
class gfTask;
class muNoticeWndTask;
class nteFileDataReplay;

class IfMngr;
extern IfMngr* g_IfMngr;
class IfMngr {
public:
    u8 unk0_0 : 1;
    u8 unk0_1 : 1;
    u8 unk1[0x3];
    int unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    nw4r::g3d::ScnGroup* m_game2DGroup;
    nw4r::g3d::ScnChoice* unk18;
    // HYPOTHESIS: resource file data, passed to the HUD tasks as nw4r::g3d::ResFile.
    void* unk1C[10];
    void* m_ifCenter;
    u32 unk48;
    IfPlayer* m_ifPlayers[MAX_PLAYERS];
    u8 unk68[8];
    void* m_ifPhoto;
    void* m_ifMinigameHomerun;
    void* m_ifMinigameTraining;
    u32 unk7C;
    u32 unk80;
    u32 unk84;
    u32 unk88;
    void* m_ifWifiIntrMenu;
    void* unk90;
    void* m_ifAdvBoss;
    u32 unk98;
    gfTask* unk9C;
    u32 unkA0;
    u32 unkA4;
    u8 unkA8;
    bool m_isPauseMenuActive;
    int unkAC;
    muNoticeWndTask* unkB0;
    u32 unkB4;
    nteFileDataReplay* unkB8;
    u32 unkBC;
    gfArchive* unkC0;
    gfResourceLoader* unkC4;

    void addGame2DObj(nw4r::g3d::ScnObj* object);
    void removeGame2DObj(nw4r::g3d::ScnObj* object);

    // HYPOTHESIS: parameter types below come from register use and callers;
    // the map has the names only.
    static IfMngr* create();
    static void destroy();
    IfMngr();
    bool isLoadResourceFinished();
    void main();
    void startPauseMenu(int arg1, int arg2, int playerNo, bool arg4);
    void exitPauseMenu();
    void startIntrMenu(int padNo, int kind);
    void exitIntrMenu();
    int getIntrMenuState();
    void openDlg(int arg1, int msgId, int arg3);
    void closeDlg();
    int getDlgState();
    void setDlgDWCErrorCode(int code);
    bool sharePlayerStock(int giver, int receiver);
    void addPlayerDragoonParts(int playerNo, int part);
    void deletePlayerDragoonParts(int playerNo, int part);
    void setPlayerDragoonPartsComplete(int playerNo, int arg2);
    void resetPlayerDragoonParts(int playerNo);
    void dispHomerunStart(int arg1, int arg2);
    bool isFinishedHomerunStartAnim();
    void eraseCorps();
    // HYPOTHESIS: eight-byte entries contain a character kind and costume byte.
    void dispCorps(const void*, int);
    void dispKumiteKills(int kills);
    void closeTrainingMenu();
    int getTrainingSpeed();
    int getTrainingGenerateItemKind();
    int getTrainingNumCP();
    int getTrainingCPAI();
    int getTrainingCPDamage();
    bool isTrainingQuit();
    void setTrainingStatusDamage(int damage);
    void setTrainingStatusCombo(int combo);
    void setTrainingStatusTotalDamage(int damage);
    // The boss slot (0 or 1), passed on to IfAdvBoss::eraseBoss.
    void eraseBoss(int index);
    void setBossDamageDisp(int index);
    void setBossDeathDisp(int index);
    void setBossMoveDispFlag(bool flag);
    bool saveReplay();
    bool isSaveReplayFinished(int* state);

    void setHomerunDistance(float distance);
    void dispHomerunResult(bool newRecord);
    void dispHomerunVersusResult(float distance0, float distance1, float distance2);
    void doHeartSwap(int areaNo1, int areaNo2);
    void initHeartSwap();

    inline static IfMngr* getInstance() { return g_IfMngr; }
};
static_assert(sizeof(IfMngr) == 200, "Class is wrong size!");
