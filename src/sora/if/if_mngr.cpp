#include <gf/gf_resource_loader.h>
#include <gf/gf_task.h>
#include <if/if_center.h>
#include <if/if_minigame_homerun.h>
#include <if/if_minigame_training.h>
#include <if/if_mngr.h>
#include <ip/ip_switch.h>
#include <ms/ms_message.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <mu/mu_notice_wnd.h>
#include <nt/nt_etc_file.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <if/if_wifi_intr_menu.h>

namespace nw4r {
namespace g3d {
class ScnChoice : public G3dObj {
public:
    bool SetChoice(int idx);
};
} // namespace g3d
} // namespace nw4r

// HYPOTHESIS: field names unknown; offsets from the accesses of IfMngr.
struct IfPhoto {
    u8 unk0[0x70];
    int unk70;
    void exitPause();
    void main();
};

struct IfAdvBoss {
    u8 unk0[0x32];
    bool unk32[3];
    bool unk35[2];
    bool unk37[2];
    bool unk39;
    u8 unk3A[2];
    int unk3C;
    int unk40;
};

// HYPOTHESIS: the third display flag of IfPlayer, which the BrawlHeaders class does not declare.
struct IfPlayerFlags {
    u8 unk7 : 1;
    u8 unk6 : 1;
    u8 unk5 : 1;
};

class ifTask : public gfTask {
public:
    virtual void processDefault();
    virtual void renderPre();
};

class ifAnimTask : public gfTask {
public:
    virtual void processDefault();
};

// HYPOTHESIS: map names IfMngr::realizeResource, freeResource
extern "C" void fn_800DBB10(IfMngr* mngr);
extern "C" void fn_800DBD3C(IfMngr* mngr);
extern "C" void fn_800E244C(IfPlayer* player);
extern "C" void fn_80108E5C(IfAdvBoss* boss);
extern "C" int fn_800FBFDC(void* menu);
// HYPOTHESIS: map names IfPlayer::receiveShareStock, addDragoonParts
extern "C" void fn_800E1D78(IfPlayer* player, Vec3f* pos);
// HYPOTHESIS: map names IfPhoto::startPause
extern "C" void fn_800F00D4(void* photo, int arg1, int arg2, int playerNo, bool arg4);
// HYPOTHESIS: map names IfCenter::dispKumiteKills
extern "C" void fn_800DF4AC(void* center, int kills);
// HYPOTHESIS: map names muNoticeWndTask::dispSubMsg
extern "C" void fn_800BF928(muNoticeWndTask* wnd, const char* text);
// HYPOTHESIS: map names IfAdvBoss::eraseBoss
extern "C" void fn_80109540(IfAdvBoss* boss, int index);
// HYPOTHESIS: map names IfSnapSaveTask::create, getState
extern "C" gfTask* fn_800F18C4(nw4r::g3d::ResFile* arg1, nw4r::g3d::ResFile* resFile, void* msgData, u32 scnRoot, int arg5,
                               nteFileDataReplay* data, int arg7, int heap, int heap2, int heap3);
extern "C" int fn_800F2894(gfTask* task);

IfMngr* IfMngr::create() {
    return new (Heaps::System) IfMngr();
}

void IfMngr::destroy() {
    IfMngr* mngr = g_IfMngr;
    if (mngr != NULL) {
        fn_800DBD3C(mngr);
        if (mngr->unk18 != NULL) {
            mngr->unk18->Destroy();
            mngr->unk18 = NULL;
        }
        delete mngr;
    }
    g_IfMngr = NULL;
}

IfMngr::IfMngr() {
    unk0_0 = true;
    unk0_1 = false;
    unk4 = 0;
    unk8 = 0;
    unkC = 0;
    unk10 = 0;
    m_game2DGroup = NULL;
    unk18 = NULL;
    m_ifCenter = NULL;
    unk48 = 0;
    m_ifPhoto = NULL;
    m_ifMinigameHomerun = NULL;
    m_ifMinigameTraining = NULL;
    unk7C = 0;
    unk80 = 0;
    unk84 = 0;
    unk88 = 0;
    m_ifWifiIntrMenu = NULL;
    unk90 = NULL;
    m_ifAdvBoss = NULL;
    unk98 = 0;
    unk9C = NULL;
    unkA0 = 0;
    unkA4 = 0;
    unkA8 = 0;
    m_isPauseMenuActive = false;
    unkAC = 0;
    unkB0 = NULL;
    unkB4 = 0;
    unkB8 = NULL;
    unkBC = 0;
    unkC0 = NULL;
    unkC4 = NULL;
    for (int i = 0; i < 10; i++) {
        unk1C[i] = NULL;
    }
    memset(m_ifPlayers, 0, sizeof(m_ifPlayers));
    memset(unk68, 0, MAX_PLAYERS);
    g_IfMngr = this;
}

bool IfMngr::isLoadResourceFinished() {
    if (unk0_1 == true) {
        return true;
    }
    if (!unkC4->isLoaded()) {
        return false;
    }
    fn_800DBB10(this);
    unk0_1 = true;
    return true;
}

void IfMngr::main() {
    bool paused;
    if (!m_isPauseMenuActive) {
        paused = false;
    } else if (m_ifPhoto == NULL) {
        paused = false;
    } else {
        paused = static_cast<IfPhoto*>(m_ifPhoto)->unk70 == 1;
    }
    if (!paused) {
        reinterpret_cast<IfCenter*>(m_ifCenter)->main();
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (m_ifPlayers[i] != NULL) {
                fn_800E244C(m_ifPlayers[i]);
            }
        }
        IfPlayer::arrowMain(m_ifPlayers, MAX_PLAYERS);
    }
    if (m_ifMinigameHomerun != NULL) {
        static_cast<IfMinigameHomerun*>(m_ifMinigameHomerun)->main();
    }
    if (m_ifMinigameTraining != NULL) {
        static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->main();
    }
    if (m_ifAdvBoss != NULL) {
        fn_80108E5C(static_cast<IfAdvBoss*>(m_ifAdvBoss));
    }
    if (m_isPauseMenuActive) {
        reinterpret_cast<IfPhoto*>(m_ifPhoto)->main();
    }
    if (getIntrMenuState() == 1) {
        switch (fn_800FBFDC(m_ifWifiIntrMenu)) {
        case 0:
        case 2:
            unkAC = 0;
            break;
        case 3:
            unkAC = 2;
            break;
        case 4:
            unkAC = 3;
            break;
        }
    }
    if (unk18 != NULL) {
        unk18->G3dProc(8, 0, 0);
    }
}

void IfMngr::startPauseMenu(int arg1, int arg2, int playerNo, bool arg4) {
    fn_800F00D4(m_ifPhoto, arg1, arg2, playerNo >= 0 ? unk68[playerNo] : -1, arg4);
    unk18->SetChoice(1);
    m_isPauseMenuActive = true;
}

void IfMngr::exitPauseMenu() {
    static_cast<IfPhoto*>(m_ifPhoto)->exitPause();
    unk18->SetChoice(0);
    m_isPauseMenuActive = false;
}

void IfMngr::startIntrMenu(int padNo, int kind) {
    static_cast<IfWifiIntrMenu*>(m_ifWifiIntrMenu)->startMenu(padNo, kind);
    unkAC = 1;
}

void IfMngr::exitIntrMenu() {
    static_cast<IfWifiIntrMenu*>(m_ifWifiIntrMenu)->endMenu();
    unkAC = 0;
}

int IfMngr::getIntrMenuState() {
    if (m_ifWifiIntrMenu == NULL) {
        return 0;
    }
    return unkAC;
}

void IfMngr::openDlg(int arg1, int msgId, int arg3) {
    int kind = arg3 != 0;
    void* msgData = unk90;
    if (unkB0 != NULL) {
        unkB0->deleteWnd();
        unkB0->exit();
        unkB0 = NULL;
    }
    unkB0 = fn_800BEEB8(kind, msgData, msgId, reinterpret_cast<nw4r::g3d::ResFile*>(&unk1C[8]), reinterpret_cast<void*>(unk8), 0x60, 0, Heaps::MenuInstance,
                        Heaps::MenuInstance, 1, 1, 0xF0);
}

void IfMngr::closeDlg() {
    if (unkB0 != NULL) {
        unkB0->deleteWnd();
        unkB0->exit();
        unkB0 = NULL;
    }
}

int IfMngr::getDlgState() {
    muNoticeWndTask* wnd = unkB0;
    if (wnd == NULL) {
        return 0;
    }
    if (wnd->m_step != 3) {
        return 1;
    }
    int state = 3;
    if (wnd->m_selected != 0) {
        state = 2;
    }
    return state;
}

void IfMngr::setDlgDWCErrorCode(int code) {
    u32 size;
    char* text;
    char buf[128];
    Message::getPrintIndexData(unk90, 0x13, &text, &size);
    memcpy(buf, text, size);
    size += sprintf(buf + size, "%05d", code);
    unkB0->dispSubMsg(buf);
}

bool IfMngr::sharePlayerStock(int giver, int receiver) {
    Vec3f stock;
    if (!m_ifPlayers[giver]->giveShareStock(&stock)) {
        return false;
    }
    fn_800E1D78(m_ifPlayers[receiver], &stock);
    return true;
}

void IfMngr::addPlayerDragoonParts(int playerNo, int part) {
    m_ifPlayers[playerNo]->addDragoonParts(part);
}

void IfMngr::deletePlayerDragoonParts(int playerNo, int part) {
    m_ifPlayers[playerNo]->deleteDragoonParts(part);
}

void IfMngr::setPlayerDragoonPartsComplete(int playerNo, int arg2) {
    m_ifPlayers[playerNo]->setDragoonPartsComplete(arg2);
}

void IfMngr::resetPlayerDragoonParts(int playerNo) {
    m_ifPlayers[playerNo]->resetDragoonParts();
}

void IfMngr::dispHomerunStart(int arg1, int arg2) {
    static_cast<IfMinigameHomerun*>(m_ifMinigameHomerun)->dispStart(arg1, arg2);
}

bool IfMngr::isFinishedHomerunStartAnim() {
    return static_cast<IfMinigameHomerun*>(m_ifMinigameHomerun)->unk115 == 0;
}

void IfMngr::eraseCorps() {
    static_cast<IfCenter*>(m_ifCenter)->eraseCorps();
}

void IfMngr::dispKumiteKills(int kills) {
    reinterpret_cast<IfCenter*>(m_ifCenter)->dispKumiteKills(kills);
}

void IfMngr::closeTrainingMenu() {
    static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->closeMenu();
}

int IfMngr::getTrainingSpeed() {
    return static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->m_speed;
}

int IfMngr::getTrainingGenerateItemKind() {
    return static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->m_generateItemKind;
}

int IfMngr::getTrainingNumCP() {
    return static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->m_numCP;
}

int IfMngr::getTrainingCPAI() {
    return static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->m_cpAI;
}

int IfMngr::getTrainingCPDamage() {
    return static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->m_cpDamage;
}

bool IfMngr::isTrainingQuit() {
    return static_cast<IfMinigameTraining*>(m_ifMinigameTraining)->m_quit;
}

void IfMngr::setTrainingStatusDamage(int damage) {
    IfMinigameTraining* training = static_cast<IfMinigameTraining*>(m_ifMinigameTraining);
    training->m_damage = damage;
    if (training->m_statusVisible) {
        training->printStatusDamage(damage);
    }
}

void IfMngr::setTrainingStatusCombo(int combo) {
    IfMinigameTraining* training = static_cast<IfMinigameTraining*>(m_ifMinigameTraining);
    training->m_combo = combo;
    if (training->m_statusVisible) {
        training->printStatusCombo(combo);
    }
}

void IfMngr::setTrainingStatusTotalDamage(int damage) {
    IfMinigameTraining* training = static_cast<IfMinigameTraining*>(m_ifMinigameTraining);
    training->m_totalDamage = damage;
    if (training->m_statusVisible) {
        training->printStatusTotalDamage(damage);
    }
}

void IfMngr::eraseBoss(int index) {
    IfAdvBoss* boss = static_cast<IfAdvBoss*>(m_ifAdvBoss);
    if (boss != NULL) {
        fn_80109540(boss, index);
    }
}

void IfMngr::setBossDamageDisp(int index) {
    IfAdvBoss* boss = static_cast<IfAdvBoss*>(m_ifAdvBoss);
    if (boss != NULL) {
        boss->unk35[index] = true;
        boss->unk3C = index;
    }
}

void IfMngr::setBossDeathDisp(int index) {
    IfAdvBoss* boss = static_cast<IfAdvBoss*>(m_ifAdvBoss);
    if (boss != NULL) {
        boss->unk37[index] = true;
        boss->unk32[index] = false;
        boss->unk40 = index;
    }
}

void IfMngr::setBossMoveDispFlag(bool flag) {
    IfAdvBoss* boss = static_cast<IfAdvBoss*>(m_ifAdvBoss);
    if (boss != NULL) {
        boss->unk39 = flag;
    }
}

bool IfMngr::saveReplay() {
    if (unk9C != NULL) {
        return false;
    }
    unkB8 = new (Heaps::MenuInstance) nteFileDataReplay(Heaps::MenuInstance);
    void* buffer;
    u32 size;
    ipSwitch::getInstance()->m_keyRecorder->getBufferData(&buffer, &size);
    unkB8->setData(buffer, size);
    unk9C = fn_800F18C4(reinterpret_cast<nw4r::g3d::ResFile*>(&unk1C[5]), reinterpret_cast<nw4r::g3d::ResFile*>(&unk1C[8]), unk90, unk8, 0x60, unkB8, 0, Heaps::MenuInstance, Heaps::MenuInstance,
                        Heaps::MenuInstance);
    return true;
}

bool IfMngr::isSaveReplayFinished(int* state) {
    if (unk9C == NULL) {
        return true;
    }
    *state = fn_800F2894(unk9C);
    if (*state != 0) {
        unk9C->exit();
        unk9C = NULL;
        delete unkB8;
        unkB8 = NULL;
        return true;
    }
    return false;
}

void ifAnimTask::processDefault() {
    nw4r::g3d::ScnGroup* group = g_IfMngr->m_game2DGroup;
    if (group != NULL) {
        group->G3dProc(8, 0, 0);
    }
}

void ifTask::renderPre() {
    int i;
    IfMngr* mngr = g_IfMngr;
    int count;
    u8 work[0x78];
    IfPlayer::portriteRenderMainFastInit(reinterpret_cast<Rect2D*>(work), &count, MAX_PLAYERS);
    IfPlayer::loupeRenderPreInit();
    for (i = 0; i < MAX_PLAYERS; i++) {
        IfPlayer* player = mngr->m_ifPlayers[i];
        if (player != NULL && reinterpret_cast<IfPlayerFlags*>(player)->unk5) {
            player->portriteRenderMainFast(reinterpret_cast<Rect2D*>(work), count);
            mngr->m_ifPlayers[i]->loupeRenderPre();
        }
    }
}

void ifTask::processDefault() {
    g_IfMngr->main();
}

void IfMngr::setHomerunDistance(float distance) {
    static_cast<IfMinigameHomerun*>(m_ifMinigameHomerun)->setDistance(distance);
}

void IfMngr::dispHomerunResult(bool newRecord) {
    static_cast<IfMinigameHomerun*>(m_ifMinigameHomerun)->dispResult(newRecord);
}

void IfMngr::dispHomerunVersusResult(float distance0, float distance1, float distance2) {
    static_cast<IfMinigameHomerun*>(m_ifMinigameHomerun)->dispVersusResult(distance0, distance1, distance2);
}

void IfMngr::doHeartSwap(int areaNo1, int areaNo2) {
    u8 playerNo = unk68[areaNo1];
    unk68[areaNo1] = unk68[areaNo2];
    unk68[areaNo2] = playerNo;
    IfPlayer* player1 = m_ifPlayers[areaNo1];
    u32 loupeId = player1->m_loupeId;
    player1->m_loupeId = m_ifPlayers[areaNo2]->m_loupeId;
    player1->setLoupeVisible(player1->m_isLoupeVisible);
    IfPlayer* player2 = m_ifPlayers[areaNo2];
    player2->m_loupeId = loupeId;
    player2->setLoupeVisible(player2->m_isLoupeVisible);
    u32 damage = m_ifPlayers[areaNo1]->unk18;
    m_ifPlayers[areaNo1]->updateDamageHP(m_ifPlayers[areaNo2]->unk18, 0);
    m_ifPlayers[areaNo2]->updateDamageHP(damage, 0);
    int colorNo = m_ifPlayers[areaNo1]->m_portraitColorNo;
    m_ifPlayers[areaNo1]->m_portraitColorNo = m_ifPlayers[areaNo2]->m_portraitColorNo;
    m_ifPlayers[areaNo2]->m_portraitColorNo = colorNo;
    int teamNo = m_ifPlayers[areaNo1]->m_teamNo;
    m_ifPlayers[areaNo1]->m_teamNo = m_ifPlayers[areaNo2]->m_teamNo;
    m_ifPlayers[areaNo2]->m_teamNo = teamNo;
    int characterKind1;
    int type1 = m_ifPlayers[areaNo1]->getStockFace(&characterKind1);
    int characterKind2;
    int type2 = m_ifPlayers[areaNo2]->getStockFace(&characterKind2);
    m_ifPlayers[areaNo1]->setStockFace(type2, characterKind2);
    m_ifPlayers[areaNo2]->setStockFace(type1, characterKind1);
}

void IfMngr::initHeartSwap() {
    int areaNo;
    int playerNo;
    for (areaNo = 0; areaNo < MAX_PLAYERS; areaNo++) {
        playerNo = unk68[areaNo];
        if (areaNo != playerNo) {
            break;
        }
    }
    if (areaNo < MAX_PLAYERS) {
        u32 damage = m_ifPlayers[areaNo]->unk18;
        m_ifPlayers[areaNo]->updateDamageHP(m_ifPlayers[playerNo]->unk18, 0);
        m_ifPlayers[playerNo]->updateDamageHP(damage, 0);
        int colorNo = m_ifPlayers[areaNo]->m_portraitColorNo;
        m_ifPlayers[areaNo]->m_portraitColorNo = m_ifPlayers[playerNo]->m_portraitColorNo;
        m_ifPlayers[playerNo]->m_portraitColorNo = colorNo;
        int teamNo = m_ifPlayers[areaNo]->m_teamNo;
        m_ifPlayers[areaNo]->m_teamNo = m_ifPlayers[playerNo]->m_teamNo;
        m_ifPlayers[playerNo]->m_teamNo = teamNo;
        int characterKind1;
        int type1 = m_ifPlayers[areaNo]->getStockFace(&characterKind1);
        int characterKind2;
        int type2 = m_ifPlayers[playerNo]->getStockFace(&characterKind2);
        m_ifPlayers[areaNo]->setStockFace(type2, characterKind2);
        m_ifPlayers[playerNo]->setStockFace(type1, characterKind1);
        for (u8 index = 0; index < MAX_PLAYERS; index++) {
            if (index != unk68[index]) {
                unk68[index] = index;
                IfPlayer* player = m_ifPlayers[index];
                player->m_loupeId = player->unk34C;
                player->setLoupeVisible(player->m_isLoupeVisible);
            }
        }
    }
}
