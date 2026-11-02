#include <if/if_vs_result_task.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <gm/gm_result_info.h>
#include <if/if_snap_save.h>
#include <ip/ip_switch.h>
#include <mu/menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <gf/gf_camera.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <mu/mu_notice_wnd.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <gf/gf_scene.h>
#include <nw4r/g3d/g3d_obj.h>

// ---- 0x800E5858-0x800E7BD0 ----

// Unit a of the temporary split of if_vs_result_task (0x800E5858-0x800E7BD0): the constructor, loadResource, the three
// print helpers and processAnim. It owns the data of the whole range: the animation tables (.data), the buffers they
// point at (.bss) and the constants.

// ---- data ----

// HYPOTHESIS: the names of the animations that initMdlData formats with sprintf ("<node name>__<n>"). The animation
// tables below point at these 36 buffers (9 rows for each of the four players' rows, 0x20 bytes each); the splits
// give this unit only the first buffer, the other 35 are part of the same array in the original.
char lbl_8049E0D0[4][9][0x20] __attribute__((aligned(8)));

// HYPOTHESIS: the tables of animations that MuObject::setAnimName gets. m_frame is where the animation starts, unk4
// the frame it ends at (19: the panel opens, 230: it plays through, 29: it closes, 9: spare), m_flags which kinds of
// animation are driven (see MuObject::setFrameFlg: 1 chr, 2 vis, 4 tex pattern, 8 srt, 0x10 color). They are not
// const: initMdlData patches the flags of the player rows.
// Panel (rows 0..8) | HitNum (9..17) | the four player rows (18..53)
MuAnimNameData lbl_8045A530[54] = {
    {0.0f, 19.0f, 0, "InfResultPanel_TopN__0", 0x1B},
    {0.0f, 230.0f, 0, "InfResultPanel_TopN__1", 0x1B},
    {0.0f, 29.0f, 0, "InfResultPanel_TopN__2", 0x1B},
    {0.0f, 9.0f, 0, NULL, 0},
    {0.0f, 9.0f, 0, NULL, 0},
    {0.0f, 9.0f, 0, NULL, 0},
    {0.0f, 9.0f, 0, NULL, 0},
    {0.0f, 9.0f, 0, NULL, 0},
    {0.0f, 9.0f, 0, NULL, 0},
    {0.0f, 0.0f, 0, "InfResultHitnum_TopN__0", 4},
    {0.0f, 230.0f, 0, NULL, 0},
    {0.0f, 29.0f, 0, "InfResultHitnum_TopN__2", 0x13},
    {0.0f, 9.0f, 0, "InfResultHitnum_TopN__3", 0x13},
    {0.0f, 9.0f, 0, "InfResultHitnum_TopN__4", 0x13},
    {0.0f, 9.0f, 0, "InfResultHitnum_TopN__5", 0x13},
    {0.0f, 9.0f, 0, "InfResultHitnum_TopN__6", 0x13},
    {0.0f, 9.0f, 0, "InfResultHitnum_TopN__7", 0x13},
    {0.0f, 9.0f, 0, "InfResultHitnum_TopN__8", 0x13},
    // row of player 0
    {0.0f, 19.0f, 0, lbl_8049E0D0[0][0], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[0][1], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[0][2], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[0][3], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[0][4], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[0][5], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[0][6], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[0][7], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[0][8], 3},
    // row of player 1
    {0.0f, 19.0f, 0, lbl_8049E0D0[1][0], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[1][1], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[1][2], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[1][3], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[1][4], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[1][5], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[1][6], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[1][7], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[1][8], 3},
    // row of player 2
    {0.0f, 19.0f, 0, lbl_8049E0D0[2][0], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[2][1], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[2][2], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[2][3], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[2][4], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[2][5], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[2][6], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[2][7], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[2][8], 3},
    // row of player 3
    {0.0f, 19.0f, 0, lbl_8049E0D0[3][0], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[3][1], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[3][2], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3][3], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3][4], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3][5], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3][6], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3][7], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3][8], 3},
};

// The controller icon of a player: the animation plays a minute at frame 59, then stops at frame 0.
MuAnimNameData lbl_8045A980[2] = {
    {0.0f, 59.0f, 1, "InfResultContl_TopN__0", 0x10},
    {0.0f, 0.0f, 0, "InfResultContl_TopN__0", 0x14},
};

// The animation of the nodes of the table of the player (initMdlData gives it to the "InfStockface_TopN" objects).
MuAnimNameData lbl_8045A9C0[1] = {
    {0.0f, 0.0f, 0, "InfStockface_TopN__0", 4},
};

// ---- code ----

// MATCH-ONLY: the shadow header declares the rule as an enum bit field, the code reads the plain byte.
static inline u8 getGameRule(const gmResultInfo* info) {
    return reinterpret_cast<const u8*>(info)[1];
}

// MATCH-ONLY: the shadow header declares the character as an enum bit field, the code reads the plain byte.
static inline u8 getCharacterKind(const gmPlayerResultInfo& info) {
    return *reinterpret_cast<const u8*>(&info);
}

// HYPOTHESIS: the table of the strings that the numbers are printed from (digits 0-9, then "+", "-", ... ":" and a
// blank for a missing leading digit); it lives in the range of ifSimpleResultTask.
extern const char* lbl_8045B674[];

// HYPOTHESIS: bit views of bytes of gmResultInfo that the shadow header does not name yet.
struct ResultPlayerFlags {   // byte 7 of gmPlayerResultInfo
    u8 : 1;
    u8 : 1;
    u8 : 1;
    u8 hasController : 1;    // 0x10
    u8 : 4;
};
struct ResultInfoFlags {     // byte 0xE of gmResultInfo
    u8 tournament : 1;       // 0x80
    u8 : 7;
};

ifVsResultTask::ifVsResultTask()
    : gfTask("ifVsResultTask", Category_Info, 14, 6, true),
      m_state(0),
      m_unk44(0),
      m_unk6C(0),
      m_unk70(0),
      m_unk74(0),
      m_winnerPlayer(0),
      m_unkC0(0),
      m_unkC1(0),
      m_unkC2(0),
      m_unkC3(0),
      m_bgmHandle(-1),
      m_unkDC(0),
      m_unkE0(0),
      m_soundStep(3),
      m_noticeGroup(NULL),
      m_noticeWnd(NULL),
      m_cntWnd(NULL),
      m_timer(900),
      m_unk668(1),
      m_unk669(1),
      m_unk66A(0),
      m_unk66C(0),
      m_msgData(NULL),
      m_wifiMsgData(NULL),
      m_replaySaveTask(NULL),
      m_replayData(NULL) {
    unk2C_b1 = false;

    adjustResultinfo();
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    muObjProcessTask* objProcessTask = muObjProcessTask::create((HeapType)0x2A);
    m_objProcessTask = objProcessTask;
    objProcessTask->m_objs = m_objects;
    objProcessTask->m_count = Obj_Count;

    // HYPOTHESIS: the masks are by player: m_unkC0 every human, m_unkC1 the humans with a controller, m_unkC2 those
    // without, m_unkC3 the CPUs.
    for (int i = 0; i < PlayerMax; i++) {
        gmPlayerResultInfo* player = &resultInfo->m_playersResultInfo[i];
        int state = player->m_state;
        m_playerState[i] = state;
        if (state == 0) {
            m_unkC0 |= 1 << i;
            if (reinterpret_cast<ResultPlayerFlags*>(&player->_7[0])->hasController) {
                m_unkC1 |= 1 << i;
            } else {
                m_unkC2 |= 1 << i;
            }
        } else if (state == 1) {
            m_unkC3 |= 1 << i;
        }
        m_unk58[i] = 0;
        m_unk48[i] = 0;
        m_unk78[i] = 0;
        m_unk88[i] = 0;
        m_unk98[i] = 0;
        m_unk68[i] = 0;
        m_unkC4[i] = 0;
    }

    // HYPOTHESIS: gmResultInfo has no names for these bytes yet: the winner (0x1F) and the winning team (0x1E)
    m_winnerPlayer = reinterpret_cast<u8*>(resultInfo)[0x1F];
    m_winnerTeam = reinterpret_cast<u8*>(resultInfo)[0x1E];
    bool tournamentLike = false;
    if (reinterpret_cast<ResultInfoFlags*>(&reinterpret_cast<u8*>(resultInfo)[0xE])->tournament) {
        if (g_GameGlobal->m_setRule->m_spMeleeSetting1 == 2) {
            tournamentLike = true;
        }
    }
    m_unkD4 = tournamentLike;

    for (int i = 0; i < Obj_Count; i++) {
        m_objects[i] = NULL;
    }
    for (int i = 0; i < 0x35; i++) {
        m_expands[i] = NULL;
    }
    m_resFiles.m_result = nw4r::g3d::ResFile((void*)NULL);
    m_resFiles.m_stockFace = nw4r::g3d::ResFile((void*)NULL);
    m_resFiles.m_camera = nw4r::g3d::ResFile((void*)NULL);
    m_resFiles.m_noticeWnd = nw4r::g3d::ResFile((void*)NULL);
    m_resFiles.m_wifiCount = nw4r::g3d::ResFile((void*)NULL);
    m_resFiles.m_snapSave = nw4r::g3d::ResFile((void*)NULL);
    for (int i = 0; i < 4; i++) {
        m_resFiles.m_busts[i] = nw4r::g3d::ResFile((void*)NULL);
    }
    for (int i = 0; i < FileHandleCount; i++) {
        m_fileLoaded[i] = 0;
    }

    // The replay can be saved when the recording is short enough (frames <= 0x2B98) and did not overflow.
    bool canSaveReplay = false;
    if (ipSwitch::getInstance()->m_keyRecorder->m_buffer->frames <= 0x2B98) {
        if (!ipSwitch::getInstance()->m_keyRecorder->m_overflow) {
            canSaveReplay = true;
        }
    }
    m_unk444C = canSaveReplay;
}

void ifVsResultTask::loadResource(gfArchive* archive) {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    m_archive = archive;
    if (archive == NULL) {
        if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            m_fileHandles[0].readRequest("/menu/defaultcamera/CharacterSelectWide.brres", (HeapType)0x2B, 0, 0);
        } else {
            m_fileHandles[0].readRequest("/menu/defaultcamera/CharacterSelect.brres", (HeapType)0x2B, 0, 0);
        }
        m_fileHandles[1].readRequest("/menu/result/Result.brres", (HeapType)0x2B, 0, 0);
        m_fileHandles[2].readRequest("/menu/common/StockFaceTex.brres", (HeapType)0x2B, 0, 0);
        m_fileHandles[3].readRequest("/menu/cmnwindow/wifi_notice_wnd.brres", (HeapType)0x2B, 0, 0);
        m_fileHandles[4].readRequest("/menu/wifi/wifi_count.brres", (HeapType)0x2B, 0, 0);
        m_fileHandles[5].readRequest("/info/snap_save.brres", (HeapType)0x2B, 0, 0);
        m_fileHandles[6].readRequest("/menu/msg/vsres.msbin", (HeapType)0x2B, 0, 0);
        m_fileHandles[7].readRequest("/menu/msg/wifi_common.msbin", (HeapType)0x2B, 0, 0);
    } else {
        // the archive already holds the files
        for (int i = 0; i < 8; i++) {
            m_fileLoaded[i] = 1;
        }
    }

    // The bust of each character: a player that plays the same character as an earlier one shares its file.
    for (u32 i = 0; i < PlayerMax; i++) {
        bool unique = true;
        if (m_playerState[i] != 3) {
            for (u32 j = 0; j < i; j++) {
                if (m_playerState[j] != 3 && getCharacterKind(resultInfo->m_playersResultInfo[i]) == getCharacterKind(resultInfo->m_playersResultInfo[j])) {
                    m_bustHandle[i] = j;
                    unique = false;
                    break;
                }
            }
            if (unique) {
                char path[0x40];
                int stock = muMenu::exchangeGmCharacterKind2MuStockchkind(getCharacterKind(resultInfo->m_playersResultInfo[i]));
                // HYPOTHESIS: the number of the bust file (MenSelchrFaceB%03d.brres)
                sprintf(path, "/menu/common/char_bust_tex/MenSelchrFaceB%03d.brres",
                        (u32)((float)(muMenu::exchangeGmCharacterKind2Something(stock) * 10 + 1) - 1.0f));
                m_fileHandles[8 + i].readRequest(path, (HeapType)0x2A, 0, 0);
                m_bustHandle[i] = i;
            } else {
                m_fileLoaded[8 + i] = 1;
            }
        } else {
            m_fileLoaded[8 + i] = 1;
            m_bustHandle[i] = 0;
        }
    }
    muMenu::loadMenuSound();
}

void ifVsResultTask::init() {}

// The score rows are the numbers of the panel of a player. Without the tournament-like result, they are the KOs, the
// falls and the self-destructs (or the coins), and the sum.
void ifVsResultTask::printHitPanel(int player) {
    int slot = player * 4;
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    if (m_playerState[player] == 3) {
        return;
    }
    gmPlayerResultInfo* info = &resultInfo->m_playersResultInfo[player];
    u32 kos;
    u32 deaths;
    if (m_unkD4) {
        printHitNum(slot + 3, info->m_koCount, true);
        printHitNum(slot + 4, info->m_deadCount, false);
        deaths = info->m_deadCount;
        kos = info->m_koCount;
        if (kos < deaths) {
            m_msg->setFontColor(slot + 6, 0xD2, 0x9F, 0x9F, 0xFF);
            printHitNum(slot + 6, deaths - kos, false);
        } else {
            m_msg->setFontColor(slot + 6, 0x90, 0xC9, 0xEB, 0xFF);
            printHitNum(slot + 6, kos - deaths, true);
        }
    } else if (getGameRule(resultInfo) == 2) {
        // coins
        printHitNum(slot + 3, info->m_pickupCoins, true);
        printHitNum(slot + 4, info->m_pickupCoins - info->m_coins, false);
        m_msg->setFontColor(slot + 6, 0x90, 0xC9, 0xEB, 0xFF);
        printHitNum(slot + 6, info->m_coins, true);
    } else {
        s32 selfDestructPenalty = reinterpret_cast<s8*>(resultInfo)[3];
        u32 fallsOnly = info->m_deadCount - info->m_suicideCount;
        s32 selfDestructs = -(info->m_suicideCount * selfDestructPenalty);
        printHitNum(slot + 3, info->m_koCount, true);
        printHitNum(slot + 4, fallsOnly, false);
        printHitNum(slot + 5, selfDestructs, false);
        if (getGameRule(resultInfo) == 0) {
            kos = info->m_koCount;
            deaths = fallsOnly + selfDestructs;
            if (kos < deaths) {
                m_msg->setFontColor(slot + 6, 0xD2, 0x9F, 0x9F, 0xFF);
                printHitNum(slot + 6, deaths - kos, false);
            } else {
                m_msg->setFontColor(slot + 6, 0x90, 0xC9, 0xEB, 0xFF);
                printHitNum(slot + 6, kos - fallsOnly - selfDestructs, true);
            }
        } else if (static_cast<s8>(info->m_startStockCount) > 0 || resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
            printTime(slot + 6, 0, false);
        } else {
            // HYPOTHESIS: the frame the player was eliminated at (not named in gmPlayerResultInfo yet)
            printTime(slot + 6, *reinterpret_cast<u32*>(reinterpret_cast<u8*>(info) + 0x254), true);
        }
    }
}

// A number of at most five digits with its sign, printed into a message of m_msg.
void ifVsResultTask::printHitNum(int msgSlot, u32 value, bool plus) {
    if (value > 99999) {
        value = 99999;
    }
    char str[32];
    for (int i = 0; i < 32; i++) {
        str[i] = 0;
    }
    if (value != 0) {
        if (plus) {
            sprintf(str, "%s", lbl_8045B674[10]);
        } else {
            sprintf(str, "%s", lbl_8045B674[11]);
        }
    }
    if (value >= 10000) {
        sprintf(str, "%s%s", str, lbl_8045B674[value / 10000]);
    }
    if (value >= 1000) {
        sprintf(str, "%s%s", str, lbl_8045B674[value % 10000 / 1000]);
    }
    if (value >= 100) {
        sprintf(str, "%s%s", str, lbl_8045B674[value % 1000 / 100]);
    }
    if (value >= 10) {
        sprintf(str, "%s%s", str, lbl_8045B674[value % 100 / 10]);
    }
    sprintf(str, "%s%s", str, lbl_8045B674[value % 10]);
    m_msg->printf(msgSlot, str);
}

// A time of at most 99:59.99 as "MM:SS" (the missing minutes tens digit is blank), or "--:--" when no time is shown.
void ifVsResultTask::printTime(int msgSlot, u32 frames, bool format) {
    const u8 none[5] = {11, 11, 17, 11, 11};
    char str[24];
    if (format) {
        if (frames >= 0x57E40) {
            frames = 0x57E3F;
        }
        u32 minutes = frames / 3600;
        u32 seconds = frames / 60 % 60;
        u32 tens = minutes / 10;
        if (tens == 0) {
            tens = 18;
        }
        strcpy(str, lbl_8045B674[tens]);
        strcat(str, lbl_8045B674[minutes % 10]);
        strcat(str, lbl_8045B674[17]);
        strcat(str, lbl_8045B674[seconds / 10]);
        strcat(str, lbl_8045B674[seconds % 10]);
    } else {
        str[0] = 0;
        for (int i = 0; i < 5; i++) {
            strcat(str, lbl_8045B674[none[i]]);
        }
    }
    m_msg->printf(msgSlot, str);
}

// ---- processAnim ----

extern "C" void OSReport(const char* msg, ...);

// HYPOTHESIS: the Wifi counter window is a task whose class is not modeled here (only exit() of gfTask is used).
class muWifiCntWndTask : public gfTask {};

// HYPOTHESIS: map names muWifiCntWndTask::create / setCounter, muWifiInterfaceTask::setContinueNextGame /
// isReadyNextGame, IfSnapSaveTask::getState and gfSceneRoot::layerUpdateFrame.
extern "C" muWifiCntWndTask* fn_800DA15C(int kind, nw4r::g3d::ResFile* resFile, nw4r::g3d::ScnMdlExpand* expand, const char* node,
                                        int drawPriority, int heap, int heap2);
extern "C" void fn_800DA6EC(muWifiCntWndTask* task, int seconds, int unk);
extern "C" void fn_800D1ADC(muWifiInterfaceTask* task, bool next);
extern "C" bool fn_800D1BC8(muWifiInterfaceTask* task);
extern "C" int fn_800F2894(IfSnapSaveTask* task);
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);

// HYPOTHESIS: the heap object of the replay save, only its virtual destructor (vptr at 0x8) is used here.
class nteFileDataReplayBase {
public:
    u8 unk0[0x8];
};
class nteFileDataReplay : public nteFileDataReplayBase {
public:
    virtual ~nteFileDataReplay();
    u8 unkC[0x8];
};

// The frame of the animation that colors the mark of a player's row: the number of the player for a human, 9 for a CPU,
// the team color in the team battles (+10 for a CPU).
static inline float getRowColorFrame(const ifVsResultTask* task, int player) {
    float frame;
    int state = task->m_playerState[player];
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    if (state == 3) {
        frame = 0.0f;
    } else if (!resultInfo->m_isTeams) {
        if (state == 0) {
            frame = 1.0f + static_cast<float>(static_cast<u32>(player));
        } else {
            frame = 9.0f;
        }
    } else {
        const float teamFrames[3] = {1.0f, 2.0f, 4.0f};
        frame = teamFrames[resultInfo->m_playersResultInfo[player].m_teamNo];
        if (state != 0) {
            frame += 10.0f;
        }
    }
    return frame;
}

// The frame of the icon of the controller type of a pad (GameCube, Classic, Wii Remote, Nunchuk, other).
static inline float getControllerIconFrame(const gfPadStatus& pad) {
    float frame;
    if (pad.m_error != 0) {
        frame = 0.0f;
    } else {
        switch (pad.m_controllerType) {
        case gfPadType::GCC:
            frame = 2.0f;
            break;
        case gfPadType::WII_CLASSIC:
            frame = 1.0f;
            break;
        case gfPadType::WIIMOTE:
            frame = 3.0f;
            break;
        case gfPadType::NUNCHUK:
            frame = 4.0f;
            break;
        default:
            frame = 5.0f;
            break;
        }
    }
    return frame;
}

// The color animation of a row stops at the frame (it is picked, not played).
#define SET_ROW_COLOR(task, index, frame)                                                   \
    do {                                                                                      \
        (task)->m_objects[index]->setFrameMatCol(frame);                                      \
        (task)->m_objects[index]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);        \
    } while (0)

// MATCH-ONLY: the controller number (+1, 0 for none) and the rumble setting of a player of the result are read as bytes.
static inline u8 getResultPadNo(const gmResultInfo* info, int player) {
    return reinterpret_cast<const u8*>(info)[0x28 + player * 0x2AC];
}
static inline u8 getResultRumble(const gmResultInfo* info, int player) {
    return reinterpret_cast<const u8*>(info)[0x29 + player * 0x2AC];
}

// The part of the Wifi states that lets the players use the screen: it counts up a few frames, and tells whether every
// player is past the step 2 of subPlayerControl.
static inline bool controlWifiPlayers(ifVsResultTask* task) {
    task->m_unk66A = 1;
    task->subPanelControl();
    if (task->m_unk66C < 10) {
        task->m_unk66C++;
    }
    bool ready = true;
    for (int i = 0; i < ifVsResultTask::PlayerMax; i++) {
        task->subPlayerControl(i);
        ready = ready & (task->m_unk48[i] >= 3);
    }
    bool done = false;
    if (ready && task->m_unk66C >= 10) {
        done = true;
    }
    return done;
}

void ifVsResultTask::processAnim() {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(GF_PAD_SYSTEM_GET_ALL_PADS, &pad);
    pad.clamp();

    switch (m_state) {
    case State_Load: {
        soundProcess();
        for (int i = 0; i < FileHandleCount; i++) {
            if (m_fileLoaded[i] == 0 && m_fileHandles[i].m_request != NULL) {
                if (!m_fileHandles[i].isReady()) {
                    return;
                }
                m_fileLoaded[i] = 1;
            }
        }
        if (!muMenu::isLoadFinishMenuSound()) {
            return;
        }
        if (m_archive == NULL) {
            m_resFiles.m_camera = nw4r::g3d::ResFile(m_fileHandles[0].getBuffer());
            m_resFiles.m_result = nw4r::g3d::ResFile(m_fileHandles[1].getBuffer());
            m_resFiles.m_stockFace = nw4r::g3d::ResFile(m_fileHandles[2].getBuffer());
            m_resFiles.m_noticeWnd = nw4r::g3d::ResFile(m_fileHandles[3].getBuffer());
            m_resFiles.m_wifiCount = nw4r::g3d::ResFile(m_fileHandles[4].getBuffer());
            m_resFiles.m_snapSave = nw4r::g3d::ResFile(m_fileHandles[5].getBuffer());
            m_msgData = m_fileHandles[6].getBuffer();
            m_wifiMsgData = m_fileHandles[7].getBuffer();
        } else {
            // the archive: 100 / 101 the camera (wide), then the Result, StockFaceTex ... files by their numbers
            if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
                m_resFiles.m_camera = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 0x65, 0xFFFE));
            } else {
                m_resFiles.m_camera = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 100, 0xFFFE));
            }
            m_resFiles.m_result = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 0x6E, 0xFFFE));
            m_resFiles.m_stockFace = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 0x78, 0xFFFE));
            m_resFiles.m_noticeWnd = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 0x82, 0xFFFE));
            m_resFiles.m_wifiCount = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 0x8C, 0xFFFE));
            m_resFiles.m_snapSave = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Misc, 0xB4, 0xFFFE));
            m_msgData = m_archive->getData(Data_Type_Misc, 0xA0, 0xFFFE);
            m_wifiMsgData = m_archive->getData(Data_Type_Misc, 0xAA, 0xFFFE);
        }
        nw4r::g3d::ResFile::Init(&m_resFiles.m_camera);
        nw4r::g3d::ResFile::Init(&m_resFiles.m_result);
        nw4r::g3d::ResFile::Init(&m_resFiles.m_stockFace);
        m_resFiles.m_result.Bind(m_resFiles.m_stockFace);
        nw4r::g3d::ResFile::Init(&m_resFiles.m_noticeWnd);
        nw4r::g3d::ResFile::Init(&m_resFiles.m_wifiCount);
        nw4r::g3d::ResFile::Init(&m_resFiles.m_snapSave);
        for (int i = 0; i < PlayerMax; i++) {
            if (m_playerState[i] != 3) {
                m_resFiles.m_busts[i] = nw4r::g3d::ResFile(m_fileHandles[8 + m_bustHandle[i]].getBuffer());
                nw4r::g3d::ResFile::Init(&m_resFiles.m_busts[i]);
            }
        }
        initMdlData();
        m_state = State_PanelOpen;
        m_unk44 = 1;
        if (g_muWifiInterfaceTask != NULL) {
            m_cntWnd = fn_800DA15C(2, &m_resFiles.m_wifiCount, m_expands[0], "pos05", 0x11, 0x2A, 0x2A);
            fn_800DA6EC(m_cntWnd, (m_timer + 59) / 60, 0);
        }
        if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
            m_soundStep = 2;
            break;
        }
        m_soundStep = 0;
        const int bgmIds[] = {
            0x283D, 0x283E, 0x283F, 0x2840, 0x2854, 0x2841, 0x2842, 0x2843, 0x2844, 0x2845, 0x2846, 0x2847, 0x2848,
            0x2849, 0x284A, 0x284B, 0x284C, 0x284C, 0x284C, 0x284D, 0x284E, 0x284F, 0x2850, 0x2851, 0x2852, 0x2853,
            0x2855, 0x2856, 0x2857, 0x2858, 0x2858, 0x2858, 0x2858, 0x2858, 0x2858, 0x2859, 0x285A, 0x285B, 0x285C,
            0x285D, 0x2860, 0x2843, 0x2861, 0x2862, 0x2848, 0x2851, 0,      0,      0,      0,      0,
        };
        gmResultInfo* resultInfo2 = g_GameGlobal->m_resultInfo;
        if (resultInfo2->m_decisionKind == gmResultInfo::Decision_NoContest) {
            m_bgmHandle = -1;
            m_soundStep = 2;
            g_sndSystem->playSE(static_cast<SndID>(0x20), -1, 0, 0, -1);
        } else {
            // the music of the character that won
            m_bgmHandle = g_sndSystem->playBGM(bgmIds[getCharacterKind(resultInfo2->m_playersResultInfo[m_winnerPlayer])], 0, true);
            g_sndSystem->playSE(static_cast<SndID>(0x1E), -1, 0, 0, -1);
        }
        // the winner's controller rumbles
        if (m_unkC1 & (1 << m_winnerPlayer)) {
            int padNo = getResultPadNo(resultInfo, m_winnerPlayer);
            if (padNo > 0) {
                int rumble = getResultRumble(resultInfo, m_winnerPlayer);
                if (rumble == 0x78) {
                    rumble = -1;
                }
                muMenu::startRumbleController(padNo - 1, 0x14, rumble);
            }
        }
        break;
    }
    case State_PanelOpen: {
        soundProcess();
        if (m_objects[Obj_Panel]->m_modelAnim->getFrame() == 19.0f) {
            m_state = State_Intro;
            m_unk70 = 1;
            m_objects[Obj_Panel]->setAnimName(&lbl_8045A530[1], true);
            if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
                m_objects[Obj_Panel]->changeTexPatAnimN("InfResultPanel_TopN__0");
            }
            for (int i = 0; i < PlayerMax; i++) {
                m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[9 * (i + 2) + 1], true);
                switch (m_playerState[i]) {
                case 0:
                    SET_ROW_COLOR(this, Obj_PlayerPanel + i, getRowColorFrame(this, i));
                    break;
                case 1:
                    SET_ROW_COLOR(this, Obj_PlayerPanel + i, getRowColorFrame(this, i));
                    break;
                }
            }
        }
        break;
    }
    case State_Intro: {
        soundProcess();
        if (m_objects[Obj_Panel]->m_modelAnim->getFrame() == 106.0f) {
            setWinnerName();
        }
        if (((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) || (pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_START)) &&
            m_objects[Obj_Panel]->m_modelAnim->getFrame() < 221.0f) {
            // A or Start skips the introduction to its last frame
            if (m_objects[Obj_Panel]->m_modelAnim->getFrame() < 106.0f) {
                setWinnerName();
            }
            m_state = State_Wait;
            m_objects[Obj_Panel]->setFrameFlg(230.0f, lbl_8045A530[1].m_flags);
            for (int i = 0; i < PlayerMax; i++) {
                m_objects[Obj_PlayerPanel + i]->setFrameFlg(230.0f, lbl_8045A530[9 * (i + 2) + 1].m_flags);
            }
        } else if (m_objects[Obj_Panel]->m_modelAnim->getFrame() == 230.0f) {
            m_state = State_Wait;
        }
        break;
    }
    case State_Wait: {
        soundProcess();
        m_unk74++;
        if (m_unk74 >= 0x78) {
            for (int i = 0; i < PlayerMax; i++) {
                int padNo = getResultPadNo(resultInfo, i);
                if ((m_unkC1 & (1 << i)) && padNo > 0) {
                    gfPadStatus padStatus;
                    g_gfPadSystem->getSysPadStatus(padNo - 1, &padStatus);
                    float frame = getControllerIconFrame(padStatus);
                    if (frame >= 0.0f) {
                        m_objects[Obj_Contl + i]->setFrameFlg(frame, 4);
                    }
                }
            }
            if (m_unk74 == 0x78) {
                for (int i = 0; i < PlayerMax; i++) {
                    if (m_unkC1 & (1 << i)) {
                        m_objects[Obj_Contl + i]->setAnimName(&lbl_8045A980[0], false);
                    }
                }
            }
        }
        if ((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) || (pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_START)) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            m_state = State_Main;
            m_objects[Obj_Panel]->setAnimName(&lbl_8045A530[2], true);
            m_objects[Obj_HitNum]->changeAnimN(0, 0x13, true, false);
            for (int i = 0; i < PlayerMax; i++) {
                printHitPanel(i);
                m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[9 * (i + 2) + 2], true);
                switch (m_playerState[i]) {
                case 0:
                    SET_ROW_COLOR(this, Obj_PlayerPanel + i, getRowColorFrame(this, i));
                    break;
                case 1:
                    SET_ROW_COLOR(this, Obj_PlayerPanel + i, getRowColorFrame(this, i));
                    break;
                }
                if (m_unkC1 & (1 << i)) {
                    m_objects[Obj_Contl + i]->setAnimName(&lbl_8045A980[1], false);
                }
                int replayPadNo = getResultPadNo(resultInfo, i);
                if (m_unk444C && (m_unkC1 & (1 << i)) && replayPadNo > 0) {
                    // the replay save icon of the player's controller
                    m_expands[1 + i]->PushBack(m_objects[Obj_Replay + i]->getSceneModel(), "pos02");
                    m_objects[Obj_Replay + i]->changeAnimN(0, 4, false, false);
                    gfPadStatus padStatus;
                    g_gfPadSystem->getSysPadStatus(replayPadNo - 1, &padStatus);
                    float frame = getControllerIconFrame(padStatus);
                    if (frame >= 0.0f) {
                        m_objects[Obj_Replay + i]->setFrameFlg(frame, 4);
                    }
                }
            }
        }
        break;
    }
    case State_Main: {
        soundProcess();
        subPanelControl();
        for (int i = 0; i < PlayerMax; i++) {
            subPlayerControl(i);
        }
        if (m_cntWnd == NULL) {
            u8 done = 0;
            for (int i = 0; i < PlayerMax; i++) {
                if (m_unk48[i] == 3 && m_unk78[i] > 9) {
                    done |= 1 << i;
                }
            }
            if (done == 0xF) {
                m_state = State_Closing;
                m_unk74 = 0;
                g_sndSystem->stopBGM(0x3C);
            }
        }
        break;
    }
    case State_Unk7: {
        if (controlWifiPlayers(this) && m_unk668) {
            fn_800D1ADC(g_muWifiInterfaceTask, true);
            m_unk74 = 0;
            m_state = State_Unk8;
        }
        break;
    }
    case State_Unk8: {
        if (fn_800D1BC8(g_muWifiInterfaceTask)) {
            m_state = State_Closing;
            m_unk74 = 0;
            g_sndSystem->stopBGM(0x3C);
            fn_800DA6EC(m_cntWnd, 0, 0);
            m_timer = -1;
        } else if (m_timer < 0) {
            m_unk74++;
            if (m_unk74 > 600) {
                OSReport("ifVsResult: isReadyNextGame(true) time out!!!\n");
                m_state = State_Unk9;
            }
        }
        break;
    }
    case State_Unk9: {
        if (controlWifiPlayers(this) && m_unk668) {
            fn_800D1ADC(g_muWifiInterfaceTask, false);
            m_unk669 = 0;
            m_unk74 = 0;
            m_state = State_Unk10;
        }
        break;
    }
    case State_Unk10: {
        if (controlWifiPlayers(this) && m_unk668) {
            if (g_muWifiInterfaceTask->isEndMatchingEnd()) {
                m_state = State_Closing;
                m_unk74 = 0;
                g_sndSystem->stopBGM(0x3C);
                fn_800DA6EC(m_cntWnd, 0, 0);
                m_timer = -1;
            } else {
                m_unk74++;
                if (m_unk74 > 600) {
                    OSReport("ifVsResult: isReadyNextGame(false) time out!!!\n");
                    m_state = State_End;
                    g_sndSystem->stopBGM(0x3C);
                }
            }
        }
        break;
    }
    case State_ReplaySave: {
        soundProcess();
        if (fn_800F2894(m_replaySaveTask) != 0) {
            if (m_noticeGroup != NULL) {
                m_noticeGroup->Destroy();
                m_noticeGroup = NULL;
            }
            if (m_replayData != NULL) {
                delete m_replayData;
                m_replayData = NULL;
            }
            if (m_replaySaveTask != NULL) {
                m_replaySaveTask->exit();
                m_replaySaveTask = NULL;
            }
            m_state = State_Main;
        }
        break;
    }
    case State_Unk6: {
        if (controlWifiPlayers(this) && m_unk668) {
            if (g_muWifiInterfaceTask->isEndMatchingEnd()) {
                m_state = State_End;
                g_sndSystem->stopBGM(0x3C);
            } else {
                m_unk74++;
                if (m_unk74 > 600) {
                    OSReport("ifVsResult: isReadyNextGame(false) time out!!!\n");
                    m_state = State_End;
                    g_sndSystem->stopBGM(0x3C);
                }
            }
        }
        break;
    }
    case State_Closing: {
        m_unk74++;
        if (m_unk74 >= 10) {
            m_state = State_End;
        }
        break;
    }
    case State_End:
        break;
    }

    // The counter of the Wifi window counts down the 15 seconds; when it runs out the screen goes on as if the
    // players had answered no.
    int state = m_state;
    if (((state >= 1 && state <= 12) || state <= 5) && m_cntWnd != NULL) {
        int timer = m_timer;
        if (timer > 0) {
            timer = timer - 1;
            m_timer = timer;
            fn_800DA6EC(m_cntWnd, (timer + 0x3B) / 0x3C, 0);
            if (timer < 0x12D && timer == timer / 0x3C * 0x3C) {
                g_sndSystem->playSE(static_cast<SndID>(10), -1, 0, 0, -1);
            }
        }
        if (m_timer == 0) {
            int step = m_state;
            m_timer = -1;
            if (step >= 3 && step <= 4) {
                if (m_unk668) {
                    OSReport("ResultTimeOut NextStep:if_ResCom_WifiNextPre\n");
                    m_state = State_Unk7;
                } else {
                    OSReport("ResultTimeOut NextStep:if_ResCom_WifiExit\n");
                    fn_800D1ADC(g_muWifiInterfaceTask, false);
                    m_unk669 = 0;
                    m_unk74 = 0;
                    m_state = State_Unk10;
                }
            } else if (step == 5) {
                OSReport("ResultTimeOut on ReplaySave\n");
                fn_800D1ADC(g_muWifiInterfaceTask, false);
                m_unk669 = 0;
            } else if (step == 7 && m_unk668 == 0) {
                OSReport("ResultTimeOut at Wait UploadComplete...\n");
                fn_800D1ADC(g_muWifiInterfaceTask, false);
                m_unk669 = 0;
                m_unk74 = 0;
                m_state = State_Unk10;
            }
        }
    }
    fn_8000DB40(g_gfSceneRoot, 8);
}

// ---- 0x800E7BD0-0x800E9878 ----

// Unit b of the temporary split of if_vs_result_task: setWinnerName .. storeMessageData.

// HYPOTHESIS: the team name table (one 8 byte entry per team: name, ?) and the "no contest" text are defined in the
// range of ifSimpleResultTask.
struct ifVsResultTeamName {
    const char* m_name;
    int unk4;
};
extern ifVsResultTeamName lbl_8045B650[];
extern const char* lbl_8059E348;

// the name of a character (defined in sora/mu/mu_menu.cpp, still unnamed there)
extern "C" const char* fn_800AFA8C(int idx);
// HYPOTHESIS: gfSceneRoot::setClearDepthBufferState (name from the Ghidra symbol pass; layer 8 is the Info layer)
extern "C" void fn_8000E590(gfSceneRoot* root, int layer, bool clear);

// HYPOTHESIS: the Wifi counter window; only gfTask::exit is used here

// HYPOTHESIS: the heap object of the replay save, only its virtual destructor (vptr at 0x8) is used here
// HYPOTHESIS: the list at gfSceneRoot+0x450 (a pointer to an object that counts its entries at +0xE4 and removes one
// through its 13th virtual function); the destructor drains it.
struct gfSceneRootEntryList {
    virtual void unk0();
    virtual void unk1();
    virtual void unk2();
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void unk6();
    virtual void unk7();
    virtual void unk8();
    virtual void unk9();
    virtual void unk10();
    virtual void unk11();
    virtual void removeEntry(int index);
    char _4[0xE4 - 4];  // HYPOTHESIS: filler up to the count
    int m_count;

    void removeLast() {
        int count = m_count;
        if (count != 0) {
            removeEntry(count - 1);
        }
    }

    void removeAll() {
        while (m_count != 0) {
            removeLast();
        }
    }
};

// Plays the voice of the result headline and prints the name of the winner (the character, the team or "no contest").
void ifVsResultTask::setWinnerName() {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    g_sndSystem->playSE(static_cast<SndID>(0x1EF1), -1, 0, 0, -1);
    if (resultInfo->m_decisionKind == 9) {
        m_msgName->printf(0, lbl_8059E348);
    } else if (!resultInfo->m_isTeams) {
        // MATCH-ONLY: the byte is read directly (the bitfield member is loaded as a word and shifted)
        int character = *reinterpret_cast<const u8*>(&resultInfo->m_playersResultInfo[m_winnerPlayer]);
        const char* name = fn_800AFA8C(muMenu::exchangeGmCharacterKind2MuStockchkind(character));
        m_msgName->printf(0, name);
    } else {
        m_msgName->printf(0, lbl_8045B650[m_winnerTeam].m_name);
    }
}

// Releases the file that the handle read: its buffer goes back to the heap.
static void freeFile(gfFileIOHandle* handle) {
    gfHeapManager::free(handle->getBuffer());
    handle->release();
}

ifVsResultTask::~ifVsResultTask() {
    g_gfSceneRoot->removeInfoCamResAnm();
    gfSceneRootEntryList* list = *reinterpret_cast<gfSceneRootEntryList**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x450);
    list->removeAll();
    fn_8000E590(g_gfSceneRoot, gfSceneRoot::Layer_Info, false);
    if (m_noticeGroup) {
        m_noticeGroup->Destroy();
        m_noticeGroup = NULL;
    }
    if (m_replayData) {
        delete m_replayData;
        m_replayData = NULL;
    }
    if (m_replaySaveTask) {
        m_replaySaveTask->exit();
        m_replaySaveTask = NULL;
    }
    if (m_noticeGroup) {
        m_noticeGroup->Destroy();
        m_noticeGroup = NULL;
    }
    if (m_noticeWnd) {
        m_noticeWnd->exit();
        m_noticeWnd = NULL;
    }
    for (u32 i = 0; i < 0x35; i++) {
        if (m_expands[i]) {
            m_expands[i]->Destroy();
            m_expands[i] = NULL;
        }
    }
    for (u32 i = 0; i < Obj_Count; i++) {
        if (m_objects[i]) {
            delete m_objects[i];
            m_objects[i] = NULL;
        }
    }
    if (!m_archive) {
        // the files that were read themselves (with an archive the archive owns them)
        if (m_resFiles.m_camera.IsValid()) {
            freeFile(&m_fileHandles[0]);
            m_resFiles.m_camera = nw4r::g3d::ResFile();
        }
        if (m_resFiles.m_result.IsValid()) {
            freeFile(&m_fileHandles[1]);
            m_resFiles.m_result = nw4r::g3d::ResFile();
        }
        if (m_resFiles.m_stockFace.IsValid()) {
            freeFile(&m_fileHandles[2]);
            m_resFiles.m_stockFace = nw4r::g3d::ResFile();
        }
        if (m_resFiles.m_noticeWnd.IsValid()) {
            freeFile(&m_fileHandles[3]);
            m_resFiles.m_noticeWnd = nw4r::g3d::ResFile();
        }
        if (m_resFiles.m_wifiCount.IsValid()) {
            freeFile(&m_fileHandles[4]);
            m_resFiles.m_wifiCount = nw4r::g3d::ResFile();
        }
        if (m_resFiles.m_snapSave.IsValid()) {
            freeFile(&m_fileHandles[5]);
            m_resFiles.m_snapSave = nw4r::g3d::ResFile();
        }
        freeFile(&m_fileHandles[7]);
        freeFile(&m_fileHandles[6]);
    }
    for (u32 i = 0; i < PlayerMax; i++) {
        if (m_fileHandles[8 + i].m_request) {
            freeFile(&m_fileHandles[8 + i]);
        }
        m_resFiles.m_busts[i] = nw4r::g3d::ResFile();
    }
    delete m_msg;
    delete m_msgName;
    m_objProcessTask->exit();
    m_objProcessTask = NULL;
    if (m_cntWnd) {
        m_cntWnd->exit();
        m_cntWnd = NULL;
    }
}

// HYPOTHESIS: two counters of the result info of a player at +0x1C4 and +0x1C8 (inside gmPlayerResultInfo::_26), the
// numbers of entries of the two lists that the table of the player shows (5 entries per line, at most 100 of each).
static inline u32 playerInfoWord(const gmPlayerResultInfo& info, int offset) {
    return *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&info) + offset);
}

// The number of lines of the table of a player: both lists (an empty list still takes one line) and four fixed lines.
int ifVsResultTask::lineCalcExt1(int player) {
    const gmPlayerResultInfo& info = g_GameGlobal->m_resultInfo->m_playersResultInfo[player];
    u32 first = playerInfoWord(info, 0x1C4);
    u32 second = playerInfoWord(info, 0x1C8);
    int lines;
    if (first > 100) {
        first = 100;
    }
    if (first % 5 == 0) {
        if (first == 0) {
            lines = 1;
        } else {
            lines = first / 5;
        }
    } else {
        lines = first / 5 + 1;
    }
    if (second > 100) {
        second = 100;
    }
    if (second % 5 == 0) {
        if (second == 0) {
            lines += 1;
        } else {
            lines += second / 5;
        }
    } else {
        lines += second / 5 + 1;
    }
    return lines + 4;
}

// The speed that the stick (or else the d-pad) scrolls the table with, as the rows move opposite to the input.
int ifVsResultTask::calcScrollLv(const gfPadStatus* pad) {
    s8 stick = pad->m_stickY;
    int level;
    if (stick < 0) {
        level = (stick < -75 ? -75 : stick) * 60 / 75;
    } else if (stick > 0) {
        level = (stick > 75 ? 75 : stick) * 60 / 75;
    } else if (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_DPAD_UP) {
        level = 60;
    } else {
        level = (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_DPAD_DOWN) ? -60 : 0;
    }
    return -level;
}

// The frame of the animation that colors the mark of the player: the player number (1..4) when it is a human, 9 for
// the others, the team color in the team battles; plus 10 for the second set of colors.
float ifVsResultTask::getMarkColAnimFrame(int player, bool plus) {
    float frame;
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    int state = m_playerState[player];
    if (state == 3) {
        return 0.0f;
    }
    if (!resultInfo->m_isTeams) {
        if (state == 0) {
            frame = 1.0f + static_cast<float>(static_cast<u32>(player));
        } else {
            frame = 9.0f;
        }
    } else {
        u32 team = resultInfo->m_playersResultInfo[player].m_teamNo;
        if (team == 2) {
            frame = 4.0f;
        } else {
            frame = 1.0f + static_cast<float>(team);
        }
    }
    if (plus) {
        frame += 10.0f;
    }
    return frame;
}

// Writes a text (message data of size bytes) into the message buffer at pos, for the table that storeTexExt2 draws.
void ifVsResultTask::storeMessageData(float scale, Message* message, const void* data, int size, const float* pos, bool shadow) {
    message->changeMsgBuf(-1);
    message->clearMsgBuf();
    message->setCallProjection(0);
    message->setWindow(pos[0], pos[1] - 16.0f, 128.0f + pos[0], 16.0f + pos[1]);
    message->setFixedWidth(-1.0f);
    if (shadow) {
        message->setDrawFlag(2, 3);
    } else {
        message->setDrawFlag(0, 0x303);
    }
    message->setWidthModeAuto(1);
    message->setScale(scale);
    message->setColor(-1);
    message->setColor2(-1);
    message->write(data, size);
    message->printMsgBuf(static_cast<FontData::FONT_RESOURCE>(9));
}

// HYPOTHESIS: the animations of the result screen (the object lbl_8045A530[54] of if_vs_result_task.cpp, seen here as
// six groups of nine rows because the code indexes it by group): group 0 is the panel, 1 the hit numbers and
// 2 + player the rows of the four players.
// HYPOTHESIS: the two animations of the controller icon (0x8045A980)

// Puts the rows of the player (and of the players that have no turn) back into the standby position: the scroll
// state is cleared, the row plays its standby animation, the numbers are blanked and the textures are reset.
void ifVsResultTask::jumpStandby(int player, bool flag) {
    m_unk78[player] = 0;
    m_unk88[player] = 0;
    m_unk98[player] = 0;
    m_unk68[player] = 0;
    if (m_unkC1 & (1 << player)) {
        m_objects[Obj_Contl + player]->setAnimName(&lbl_8045A980[1], false);
    }
    u8 done = 0;
    for (int i = 0; i < PlayerMax; i++) {
        if (i == player || (m_playerState[i] == 3 && m_unk48[i] != 3)) {
            m_unk78[i] = 0;
            m_unk88[i] = 0;
            m_unk98[i] = 0;
            m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[9 * (i + 2) + 7], true);
            if (flag) {
                m_objects[Obj_PlayerPanel + i]->setFrameFlg(9.0f, lbl_8045A530[9 * (i + 2) + 7].m_flags);
                m_objects[Obj_PlayerPanel + i]->setUpdateRateFlg(0.0f, lbl_8045A530[9 * (i + 2) + 7].m_flags);
            }
            m_unk48[i] = 3;
            for (int k = 0; k < 4; k++) {
                m_msg->printf(3 + 4 * i + k, "");
            }
        }
        if ((m_unkC1 & (1 << i)) && m_unk48[i] == 3) {
            done |= 1 << i;
        }
    }
    if (m_unkC1 == done) {
        for (int i = 0; i < PlayerMax; i++) {
            if ((1 << i) & (m_unkC2 | m_unkC3)) {
                m_unk78[i] = 0;
                m_unk88[i] = 0;
                m_unk98[i] = 0;
                m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[9 * (i + 2) + 7], true);
                m_objects[Obj_PlayerPanel + i]->setFrameFlg(9.0f, lbl_8045A530[9 * (i + 2) + 7].m_flags);
                m_objects[Obj_PlayerPanel + i]->setUpdateRateFlg(0.0f, lbl_8045A530[9 * (i + 2) + 7].m_flags);
                m_unk48[i] = 3;
                for (int k = 0; k < 4; k++) {
                    m_msg->printf(3 + 4 * i + k, "");
                }
                // the rows go back to the dummy texture
                int block = Obj_PlayerBlock + Obj_PlayerBlockSize * i;
                for (int j = 0; j < 11; j++) {
                    MuObject* spos = m_objects[block + j];
                    spos->changeMaterialTex(spos->m_resMdl.GetResMat("dammyMat")->m_id, "InfResultDammytex", NULL);
                    for (u32 k = 0; k < 5; k++) {
                        m_objects[block + 11 + 5 * j + k]->setFrameTex(0.0f);
                    }
                }
                // and are lined up again below each other
                for (u32 j = 0; j < 11; j++) {
                    Vec3f pos;
                    m_objects[block + j]->getPos(&pos, "InfResultSpos_TopN");
                    pos.m_y = -2.5f * static_cast<float>(j) + 0.0f;
                    m_objects[block + j]->setTrans(&pos);
                }
            }
        }
    }
}

// Steps the rows of the player back to the step before (m_unk58 counts down and m_unk48 takes the old value); going
// back from step 0 (the first detail view) to the overview also resets the rows of the players that have no controller.
void ifVsResultTask::backStep(int player) {
    bool speedUp = false;
    if (m_unk48[player] == 3 && m_unk58[player] != 2) {
        speedUp = true;
    }
    switch (m_unk58[player]) {
    case 0: {
        m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[9 * (player + 2) + 4], true);
        u8 ready = 1 << player;
        u8 controllers = m_unkC1;
        for (int i = 0; i < PlayerMax; i++) {
            if ((controllers & (1 << i)) && m_unk48[i] == 0) {
                ready |= 1 << i;
            }
        }
        if (controllers == ready) {
            for (int i = 0; i < PlayerMax; i++) {
                if (!(m_unkC1 & (1 << i)) && m_unk48[i] != 0) {
                    if ((1 << i) & (m_unkC2 | m_unkC3)) {
                        int block = Obj_PlayerBlock + Obj_PlayerBlockSize * i;
                        for (int j = 0; j < 11; j++) {
                            MuObject* spos = m_objects[block + j];
                            spos->changeMaterialTex(spos->m_resMdl.GetResMat("dammyMat")->m_id, "InfResultDammytex", NULL);
                            for (u32 k = 0; k < 5; k++) {
                                m_objects[block + 11 + 5 * j + k]->setFrameTex(0.0f);
                            }
                        }
                        for (u32 j = 0; j < 11; j++) {
                            Vec3f pos;
                            m_objects[block + j]->getPos(&pos, "InfResultSpos_TopN");
                            pos.m_y = -2.5f * static_cast<float>(j) + 0.0f;
                            m_objects[block + j]->setTrans(&pos);
                        }
                    }
                    m_unk48[i] = 0;
                    m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[9 * (i + 2) + 4], true);
                    if (((1 << i) & (m_unkC2 | m_unkC3)) && m_unk48[i] != 1) {
                        m_objects[Obj_PlayerPanel + i]->setFrameFlg(9.0f, lbl_8045A530[9 * (i + 2) + 4].m_flags);
                        m_objects[Obj_PlayerPanel + i]->setUpdateRateFlg(0.0f, lbl_8045A530[9 * (i + 2) + 4].m_flags);
                    }
                    m_unk78[i] = 0;
                    m_unk88[i] = 0;
                    m_unk98[i] = 0;
                    m_unk68[i] = 0;
                    m_unkC4[i] = 0;
                }
            }
        }
        break;
    }
    case 1:
        m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[9 * (player + 2) + 6], true);
        break;
    case 2:
        m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[9 * (player + 2) + 8], true);
        break;
    }
    if (speedUp) {
        m_objects[Obj_PlayerPanel + player]->setFrameFlg(9.0f, lbl_8045A530[9 * (player + 2) + 4].m_flags);
        m_objects[Obj_PlayerPanel + player]->setUpdateRateFlg(0.0f, lbl_8045A530[9 * (player + 2) + 4].m_flags);
    }
    if (m_unkC1 & (1 << player)) {
        m_objects[Obj_Contl + player]->setAnimName(&lbl_8045A980[1], false);
    }
    m_unk48[player] = m_unk58[player];
    m_unk58[player]--;
    m_unk78[player] = 0;
    m_unk88[player] = 0;
    m_unk98[player] = 0;
    m_unk68[player] = 0;
    // MATCH-ONLY: the highest step of the players that are in the game is computed but not used
    int highest = 0;
    for (int i = 0; i < PlayerMax; i++) {
        if (m_playerState[i] != 3 && m_unk48[i] != 3 && highest < m_unk48[i]) {
            highest = m_unk48[i];
        }
    }
}

// HYPOTHESIS: a byte of the per-player lists that the table shows: the character (+0x34, +0xFC) and the color
// (+0x98, +0x160) of up to 100 fighters in each of two lists, counted by the words at +0x1C4 and +0x1C8.
static inline u8 playerInfoByte(const gmPlayerResultInfo& info, int offset) {
    return *(reinterpret_cast<const u8*>(&info) + offset);
}

// The number of lines that a list of count (at most 100) fighters takes in the table (five per line, an empty list takes one).
static inline u32 listLineCount(u32 count) {
    u32 lines;
    if (count % 5 == 0) {
        if (count == 0) {
            lines = 1;
        } else {
            lines = count / 5;
        }
    } else {
        lines = count / 5 + 1;
    }
    return lines;
}

// The frame of the stock icon texture of a fighter of a list: ten per character, then the color.
static inline float stockIconFrame(const gmPlayerResultInfo& info, int characterOffset, int colorOffset, u32 index) {
    int icon = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(playerInfoByte(info, characterOffset + index)));
    u32 first = static_cast<u32>(static_cast<float>(icon * 10 + 1));
    return static_cast<float>(static_cast<u32>(playerInfoByte(info, colorOffset + index)) + first);
}

// Shows eleven lines of the table of the player from the line `line` on: the headings, the two lists of fighters
// (the icons go into the Stockface nodes) and the texts that storeTexExt2 drew into m_texObjs.
void ifVsResultTask::drawExt1(int player, int line) {
    const gmPlayerResultInfo& info = g_GameGlobal->m_resultInfo->m_playersResultInfo[player];
    u32 first = playerInfoWord(info, 0x1C4);
    u32 second = playerInfoWord(info, 0x1C8);
    if (first > 100) {
        first = 100;
    }
    u32 firstLines = listLineCount(first);
    if (second > 100) {
        second = 100;
    }
    u32 secondLines = listLineCount(second);
    u32 secondEnd = firstLines + secondLines + 2;
    msTexObj* texObjs = &m_texObjs[player * TexObjsPerPlayer];
    int block = Obj_PlayerBlock + Obj_PlayerBlockSize * player;
    int lineFirst = line * 5;
    for (int i = 0; i < 11; i++, line++, lineFirst += 5) {
        MuObject* spos = m_objects[block + i];
        for (u32 k = 0; k < 5; k++) {
            m_objects[block + 11 + 5 * i + k]->setFrameTex(0.0f);
        }
        if (line == 0) {
            m_objects[block]->changeMaterialTex("dammyMat", &texObjs[0].m_texObj);
        } else if (line >= 1 && line < firstLines + 1) {
            if (first == 0) {
                spos->changeMaterialTex("dammyMat", &texObjs[1].m_texObj);
            } else {
                spos->changeMaterialTex(spos->m_resMdl.GetResMat("dammyMat")->m_id, "InfResultDammytex", NULL);
                u32 index = lineFirst - 5;
                for (u32 k = 0; k < 5; k++, index++) {
                    if (index >= first || index > 99) {
                        break;
                    }
                    m_objects[block + 11 + 5 * i + k]->setFrameTex(stockIconFrame(info, 0x34, 0x98, index));
                }
            }
        } else if (line == firstLines + 1) {
            spos->changeMaterialTex("dammyMat", &texObjs[2].m_texObj);
        } else if (line >= firstLines + 2 && line < secondEnd) {
            if (second == 0) {
                spos->changeMaterialTex("dammyMat", &texObjs[3].m_texObj);
            } else {
                spos->changeMaterialTex(spos->m_resMdl.GetResMat("dammyMat")->m_id, "InfResultDammytex", NULL);
                u32 index = (line - (firstLines + 2)) * 5;
                for (u32 k = 0; k < 5; k++, index++) {
                    if (index >= second || index > 99) {
                        break;
                    }
                    m_objects[block + 11 + 5 * i + k]->setFrameTex(stockIconFrame(info, 0xFC, 0x160, index));
                }
            }
        } else if (line == secondEnd) {
            spos->changeMaterialTex("dammyMat", &texObjs[4].m_texObj);
        } else if (line == secondEnd + 1) {
            spos->changeMaterialTex("dammyMat", &texObjs[5].m_texObj);
        } else {
            spos->changeMaterialTex(spos->m_resMdl.GetResMat("dammyMat")->m_id, "InfResultDammytex", NULL);
        }
    }
}

// Writes the heading text with the number index of the message file into the buffer of the message, at pos.
static inline void printHeading(Message& message, void* msgData, u32 index, const float* pos) {
    message.changeMsgBuf(-1);
    message.clearMsgBuf();
    message.setCallProjection(0);
    message.setWindow(pos[0], pos[1] - 16.0f, 128.0f + pos[0], 16.0f + pos[1]);
    message.setFixedWidth(-1.0f);
    message.setDrawFlag(0, 3);
    message.setWidthModeAuto(1);
    message.setScale(0.6666667f);
    message.setColor(-1);
    message.writeIndexData(msgData, index);
    message.printMsgBuf(static_cast<FontData::FONT_RESOURCE>(9));
}

// Draws the texts of the table of the player (headings at the even positions, the value of a statistic with its
// unit after each) into the embedded frame buffer, one 128 x 32 text after the other. With store the texts are copied
// from the frame buffer into the textures m_texObjs of the player. Returns how many texts there are.
u32 ifVsResultTask::storeTexExt2(int player, bool store) {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    msTexObj* texObj;
    const gmPlayerResultInfo& info = resultInfo->m_playersResultInfo[player];
    Message message(0x80, static_cast<HeapType>(0x2A));
    gfCamera camera;
    char text[128];
    // the message of the unit after the value of each statistic (-1: none)
    const int unitMessages[29] = {-1, -1, -1, 0x26, 0x26, 0x26, 0x26, 0x27, 0x28, 0x28, 0x28, 0x26, 0x29, 0x29, 0x29,
                                  0x29, 0x29, 0x29, 0x29, 8, 0x2A, 0x2A, 0x28, 0x28, 0x29, -1, -1, -1, -1};
    float pos[2];
    pos[1] = 0.0f;
    pos[0] = 0.0f;
    u32 count = 0x32;
    if (*(reinterpret_cast<const u8*>(resultInfo) + 1) == 2) {
        count = 0x38;
    }
    if (resultInfo->m_isTeams == 1) {
        count += 2;
    }
    texObj = &m_texObjs[player * TexObjsPerPlayer];
    for (int i = 0; static_cast<u32>(i) < count; texObj++, i++) {
        if (i % 2 == 0) {
            if (i >= 0x32 && resultInfo->m_isTeams != 1) {
                printHeading(message, m_msg->getBinary(), (i - 0x32) / 2 + 0x23, pos);
            } else if (m_unkD4) {
                if (i == 0) {
                    printHeading(message, m_msg->getBinary(), 0x32, pos);
                } else if (i == 2) {
                    printHeading(message, m_msg->getBinary(), 0x33, pos);
                } else {
                    printHeading(message, m_msg->getBinary(), i / 2 + 9, pos);
                }
            } else {
                printHeading(message, m_msg->getBinary(), i / 2 + 9, pos);
            }
        } else {
            int statistic = (i - 1) / 2;
            u32 value = 0;
            if (statistic == 0) {
                value = playerInfoWord(info, 0x1C4);
            } else if (statistic == 1) {
                value = playerInfoWord(info, 0x1C8);
            } else if (statistic == 2) {
                value = info.m_suicideCount;
            } else if (statistic < 0xB) {
                value = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&info) + statistic * 4 + 0x1C0);
            } else if (statistic == 0xB) {
                value = playerInfoByte(info, 0x1EC);
            } else if (statistic == 0xC) {
                value = playerInfoWord(info, 0x1F0);
            } else if (statistic == 0xD) {
                value = playerInfoWord(info, 0x1F4);
            } else if (statistic < 0x19) {
                value = *reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(&info) + statistic * 4 + 0x1C0);
            } else {
                int index = statistic;
                if (resultInfo->m_isTeams == 1) {
                    if (statistic == 0x19) {
                        value = playerInfoWord(info, 0x224);
                    }
                    index = statistic - 1;
                }
                switch (index) {
                case 0x19:
                    value = playerInfoWord(info, 0x2C);
                    break;
                case 0x1A:
                    value = playerInfoWord(info, 0x228);
                    break;
                case 0x1B:
                    value = playerInfoWord(info, 0x30);
                    break;
                }
            }
            int unit = unitMessages[statistic];
            int length;
            int unitLength;
            switch (unit) {
            case -1:
                length = sprintf(text, "%d", value);
                break;
            case 0x26:
                if (statistic < 7 && m_unkD4) {
                    length = sprintf(text, "%d", value);
                } else {
                    length = sprintf(text, "%d", value);
                    memcpy(text + length, m_msg->getMsgData(unit, &unitLength), unitLength);
                    length += unitLength;
                }
                break;
            case 0x2A:
                length = sprintf(text, "%d", value);
                memcpy(text + length, m_msg->getMsgData(unit, &unitLength), unitLength);
                length += unitLength;
                break;
            default:
                length = sprintf(text, "%d", value);
                memcpy(text + length, m_msg->getMsgData(unit, &unitLength), unitLength);
                length += unitLength;
                break;
            }
            storeMessageData(0.61538464f, &message, text, length, pos, true);
        }
        if (store) {
            texObj->copyTexObjFromEFB(pos[0], pos[1], 128.0f + pos[0], 32.0f + pos[1], GX_TF_I4, static_cast<Heaps::HeapType>(0x2A), true);
        }
        pos[1] += 32.0f;
        if (pos[1] >= 480.0f) {
            pos[0] += 128.0f;
            if (pos[0] >= 640.0f) {
                pos[0] = 0.0f;
                // the frame buffer is full: clear it for the next page of texts
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
                Message::drawBoxLine(0xFF, 0, 0, 0.0f, 0.0f, 640.0f, 480.0f, 0.0f);
                GXFlush();
                GXDrawDone();
            }
            pos[1] = 0.0f;
        }
    }
    return count;
}

// ---- 0x800E9878-0x800EB0E4 ----

// Unit c of the temporary split of if_vs_result_task: initMdlData (0x800E9878).

extern "C" void GXFlush(void);
extern "C" void GXDrawDone(void);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);

// HYPOTHESIS: unnamed helper of muMenu (also declared in mu_tour_entry.cpp): the number of the stock mark of a stock
// character kind; InfResultMark%02d_TopN holds one model per mark (counted from 1).
extern "C" int fn_800AF6F0(int stock);
// HYPOTHESIS: map name gfSceneRoot::setClearDepthBufferState

// The tables of the first unit (ifVsResultTask's data): the animation names of the objects of the screen, 9 rows per
// group: 0 the panel, 1 the hit numbers, 2..5 the four player rows. The flag bytes of some rows are patched here.
// The animations of the controller icon (row 1 is the one that is set) and of the text lines of the table.
// HYPOTHESIS: 4 players x 9 rows x 0x20 bytes: the names of the animations of the player rows ("<node>__<row>").
// The strings of the full width digits and symbols (0..9, then the name and no-limit marks).

// The bytes of gmResultInfo that the header hides.
struct VsResultInfoView {
    u8 m_gameMode;          // 0x00 GameMode: the tournament and the Wifi for-fun mode have their own title
    u8 m_gameRule;          // 0x01 GameRule: time, stock or coin
    u8 m_isTeams;           // 0x02
    u8 _3;
    u32 m_timeOrCoins;      // 0x04 the limit of the time and coin rules
    u32 m_stockCount;       // 0x08 the limit of the stock rule
    s8 _C[2];
    u8 m_flag0E_7 : 1;      // 0x0E bit 7. HYPOTHESIS: unknown flag; it picks the third title and, with the set rule, ifVsResultTask::m_unkD4
    u8 : 7;
};

// HYPOTHESIS: the frame of the color animation of a player's row: 1..4 for the player number (free for all), 9 for a cpu,
// the team color 1, 2 or 4 (+10 for a cpu) in a team match.
static inline float getPanelColorFrame(gmResultInfo* info, int state, u32 playerNo) {
    if (state == 3) {
        return 0.0f;
    }
    float frame;
    if (info->m_isTeams == 0) {
        if (state == 0) {
            frame = 1.0f + (float)playerNo;
        } else {
            frame = 9.0f;
        }
    } else {
        float teamFrame[3] = { 1.0f, 2.0f, 4.0f };
        frame = teamFrame[reinterpret_cast<u8*>(info)[0x2A + playerNo * 0x2AC]];
        if (state != 0) {
            frame += 10.0f;
        }
    }
    return frame;
}

static inline void copySequenceName(char* dst, const char* src) {
    dst[0] = 0;
    strcat(dst, src);
}

// Whether the sequence that is running is sqQuMelee.
static inline bool isQuMeleeSequence() {
    char sequence[0x44];
    copySequenceName(sequence, gfSceneManager::getInstance()->m_currentSequence->m_sequenceName);
    return strcmp(sequence, "sqQuMelee") == 0;
}

// Builds everything the result screen shows, once its files are bound: the model nodes of the panel, the winner mark, the
// hit numbers, the MVP and replay icons and, for every player that takes part, the row (busts, stock marks, controller
// icons, masks and the 11 x 5 text lines of the table); the groups that hang these models on the bones pos00.. of the
// panel; the camera of the screen; the texts (title, rule and limit, the names of the players and the counters); and
// finally the tables of the players, drawn into textures through storeTexExt2 with a screen sized camera.
void ifVsResultTask::initMdlData() {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    VsResultInfoView* view = reinterpret_cast<VsResultInfoView*>(resultInfo);
    u8* resultBytes = reinterpret_cast<u8*>(resultInfo);

    // HYPOTHESIS: the rank of each team, read from the team records of gmResultInfo (0x30 bytes each, the header hides them).
    u8 teamRank[3];
    if (resultInfo->m_isTeams == 1) {
        teamRank[0] = resultBytes[0x1304];
        teamRank[1] = resultBytes[0x1334];
        teamRank[2] = resultBytes[0x1364];
    }

    // The objects of the screen: the panel, the mark of the winner, the hit numbers and the MVP.
    int firstController = -1;
    char nodeName[0x40];
    char pos[0x20];
    m_objects[Obj_Panel] = MuObject::create(&m_resFiles[0], "InfResultPanel_TopN", 0x10, NULL, Heaps::MenuInstance);
    if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
        m_objects[Obj_Mark] = MuObject::create(&m_resFiles[0], "InfResultMarkNocon_TopN", 0x11, NULL, Heaps::MenuInstance);
    } else {
        sprintf(nodeName, "InfResultMark%02d_TopN", fn_800AF6F0(muMenu::exchangeGmCharacterKind2MuStockchkind(*reinterpret_cast<u8*>(&resultInfo->m_playersResultInfo[m_winnerPlayer]))) + 1);
        m_objects[Obj_Mark] = MuObject::create(&m_resFiles[0], nodeName, 0x11, NULL, Heaps::MenuInstance);
    }
    m_objects[Obj_HitNum] = MuObject::create(&m_resFiles[0], "InfResultHitnum_TopN", 0x17, NULL, Heaps::MenuInstance);
    m_objects[Obj_Mvp] = MuObject::create(&m_resFiles[0], "InfResultMvp_TopN", 0x1C, NULL, Heaps::MenuInstance);
    if (m_unk444C) {
        for (int i = 0; i < PlayerMax; i++) {
            if (m_unkC1 & (1 << i)) {
                m_objects[Obj_Replay + i] = MuObject::create(&m_resFiles[0], "InfResultReplay_TopN", 0x1D, NULL, Heaps::MenuInstance);
                Vec3f pos;
                pos.m_x = 2.0f;
                pos.m_y = 0.0f;
                pos.m_z = 0.0f;
                m_objects[Obj_Replay + i]->setTrans(&pos);
            }
        }
    }

    // The row of every player (a player that is not there gets the empty row), with the bust, the stock mark, the controller
    // icon and the nodes of the table.
    MuAnimNameData* group = lbl_8045A530;
    for (int i = 0; i < PlayerMax; i++, group += 9) {
        if (m_playerState[i] == 3) {
            sprintf(nodeName, "InfResultRank5_TopN");
            group[18].m_flags = 5;
            group[19].m_flags = 1;
            group[20].m_flags = 1;
            group[21].m_flags = 1;
            group[22].m_flags = 1;
            group[23].m_flags = 1;
            group[24].m_flags = 1;
            group[25].m_flags = 1;
            group[26].m_flags = 1;
        } else {
            gmPlayerResultInfo* player = &resultInfo->m_playersResultInfo[i];
            u8* playerBytes = reinterpret_cast<u8*>(player);
            sprintf(nodeName, "InfResultMark%02d_TopN", fn_800AF6F0(muMenu::exchangeGmCharacterKind2MuStockchkind(playerBytes[0])) + 1);
            m_objects[Obj_PlayerMark + i] = MuObject::create(&m_resFiles[0], nodeName, 0x12, NULL, Heaps::MenuInstance);
            if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
                sprintf(nodeName, "InfResultRank6_TopN");
            } else if (resultInfo->m_isTeams == 0) {
                sprintf(nodeName, "InfResultRank%d_TopN", player->m_rank + 1);
            } else {
                sprintf(nodeName, "InfResultRank%d_TopN", teamRank[player->m_teamNo] + 1);
            }
            group[18].m_flags = 0x17;
            group[19].m_flags = 3;
            group[20].m_flags = 3;
            group[21].m_flags = 3;
            group[22].m_flags = 3;
            group[23].m_flags = 3;
            group[24].m_flags = 3;
            group[25].m_flags = 3;
            group[26].m_flags = 3;
            m_objects[Obj_Charb + i] = MuObject::create(&m_resFiles[0], "InfResultCharb_TopN", 0x18, NULL, Heaps::MenuInstance);
            // The bust of the character: charBM00 and charBM01 take the texture of the file of the player.
            char bust[0x20];
            sprintf(bust, "MenSelchrFaceB.%03d", playerBytes[3] + (int)(float)(muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(playerBytes[0])) * 10 + 1));
            MuObject* charb = m_objects[Obj_Charb + i];
            charb->changeMaterialTex(charb->m_resMdl.GetResMat("charBM00")->m_id, bust, &m_resFiles[1] + i);
            charb = m_objects[Obj_Charb + i];
            charb->changeMaterialTex(charb->m_resMdl.GetResMat("charBM01")->m_id, bust, &m_resFiles[1] + i);
            if (m_unkC1 & (1 << i)) {
                if (firstController >= 0) {
                    m_objects[Obj_Contl + i] = m_objects[Obj_Contl + firstController]->copy(0x1D, 0, Heaps::MenuInstance);
                } else {
                    m_objects[Obj_Contl + i] = MuObject::create(&m_resFiles[0], "InfResultContl_TopN", 0x1D, NULL, Heaps::MenuInstance);
                    firstController = i;
                }
            }
            m_objects[Obj_Mask + i] = MuObject::create(&m_resFiles[0], "InfResultMask_TopN", 0x19, NULL, Heaps::MenuInstance);
            for (int k = 0; k < 11; k++) {
                m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + k] = MuObject::create(&m_resFiles[0], "InfResultSpos_TopN", 0x1A, NULL, Heaps::MenuInstance);
            }
            for (int k = 0; k < 55; k++) {
                m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + 11 + k] = MuObject::create(&m_resFiles[0], "InfStockface_TopN", 0x1B, &m_resFiles[5], Heaps::MenuInstance);
            }
        }
        m_objects[Obj_PlayerPanel + i] = MuObject::create(&m_resFiles[0], nodeName, 0x16 - i, NULL, Heaps::MenuInstance);
        for (u32 r = 0; r < 9; r++) {
            sprintf(lbl_8049E0D0[i][r], "%s__%d", nodeName, r);
        }
        if (m_playerState[i] == 3) {
            sprintf(lbl_8049E0D0[i][7], "%s__3", nodeName);
            sprintf(lbl_8049E0D0[i][8], "%s__4", nodeName);
        }
    }

    // The camera of the screen.
    nw4r::g3d::ResAnmScn cameraAnm;
    cameraAnm = m_resFiles[6].GetResAnmScn(0);
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&cameraAnm, cameraAnm);
    g_gfSceneRoot->setInfoCamResAnm(&cameraAnm);

    // The groups of the model nodes: m_expands[0] holds the panel, [1..4] the rows of the players, [5 + 11 * n + k] the
    // positions of the table of the player n.
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    u32 groupSize;
    // HYPOTHESIS: local tables; the originals are const data that the function copies to the stack where it uses them.
    const int constructFlags[5] = { 0x36, 5, 5, 5, 5 };   // the capacity of the groups (the panel, the four player rows)
    const int groupObject[5] = { 0, 8, 9, 10, 11 };       // the object of m_objects that each group is made for
    for (u32 g = 0; g <= 4; g++) {
        m_expands[g] = nw4r::g3d::ScnMdlExpand::Construct(allocator, &groupSize, constructFlags[g], m_objects[groupObject[g]]->m_scnMdl);
    }
    for (u32 i = 0; i < PlayerMax; i++) {
        if (m_playerState[i] != 3) {
            for (u32 k = 0; k < 11; k++) {
                m_expands[5 + 11 * i + k] = nw4r::g3d::ScnMdlExpand::Construct(allocator, &groupSize, 5, m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + k]->m_scnMdl);
            }
        } else {
            for (u32 k = 0; k < 11; k++) {
                m_expands[5 + 11 * i + k] = NULL;
            }
        }
    }

    // The nodes the models hang on (the bones pos00 ..).
    m_expands[0]->PushBack(m_objects[Obj_Mark]->m_scnMdl, "pos00");
    for (u32 i = 0; i < PlayerMax; i++) {
        sprintf(pos, "pos%02d", i + 1);
        m_expands[0]->PushBack(m_expands[1 + i], pos);
        if (m_playerState[i] != 3) {
            m_expands[0]->PushBack(m_objects[Obj_Mask + i]->m_scnMdl, pos);
            for (u32 k = 0; k < 11; k++) {
                m_expands[0]->PushBack(m_expands[5 + 11 * i + k], pos);
            }
        }
    }
    for (int i = 0; i < PlayerMax; i++) {
        if (m_playerState[i] != 3) {
            m_expands[1 + i]->PushBack(m_objects[Obj_Charb + i]->m_scnMdl, "pos00");
            m_expands[1 + i]->PushBack(m_objects[Obj_PlayerMark + i]->m_scnMdl, "pos01");
            if (m_unkC1 & (1 << i)) {
                m_expands[1 + i]->PushBack(m_objects[Obj_Contl + i]->m_scnMdl, "pos02");
            }
            if (resultBytes[0x1F] == i) {
                m_expands[1 + i]->PushBack(m_objects[Obj_Mvp]->m_scnMdl, "pos03");
            }
        }
    }
    for (u32 i = 0; i < PlayerMax; i++) {
        if (m_playerState[i] != 3) {
            for (u32 k = 0; k < 11; k++) {
                for (u32 j = 0; j < 5; j++) {
                    sprintf(pos, "pos%02d", j);
                    m_expands[5 + 11 * i + k]->PushBack(m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + 11 + 5 * k + j]->m_scnMdl, pos);
                }
                Vec3f spos;
                m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + k]->getPos(&spos, "InfResultSpos_TopN");
                spos.m_y = -2.5f * (float)k;
                m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + k]->setTrans(&spos);
                m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + k]->getPos(&spos, "textN0");
            }
        }
    }

    gfSceneRoot* sceneRoot = g_gfSceneRoot;
    sceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_expands[0]));
    sceneRoot->add(gfSceneRoot::Layer_Info, m_objects[Obj_HitNum]->m_scnMdl);
    fn_8000E590(sceneRoot, gfSceneRoot::Layer_Info, true);

    // Everything starts stopped on its first frame.
    for (u32 i = 0; i < Obj_Count; i++) {
        if (m_objects[i]) {
            m_objects[i]->m_modelAnim->setUpdateRate(0.0f);
            m_objects[i]->setFrame(0.0f);
        }
    }

    // HYPOTHESIS: the frame of the texture animation that picks the title of the hit numbers (per rule) and of the player rows.
    float hitFrame = 0.0f;
    float rowFrame = hitFrame;
    if (view->m_gameRule == Game_Rule_Coin) {
        hitFrame = 1.0f;
        rowFrame = hitFrame;
    }
    if (view->m_gameRule == Game_Rule_Stock) {
        hitFrame = 3.0f;
    }
    if (m_unkD4) {
        hitFrame = 2.0f;
        rowFrame = hitFrame;
    }
    m_objects[Obj_Panel]->setAnimName(&lbl_8045A530[0], true);
    if (resultInfo->m_decisionKind == gmResultInfo::Decision_NoContest) {
        m_objects[Obj_Mark]->setFrameMatCol(9.0f);
    } else {
        m_objects[Obj_Mark]->setFrameMatCol(getMarkColAnimFrame(m_winnerPlayer, false));
    }
    m_objects[Obj_HitNum]->changeAnimN(3, 4, false, false);
    m_objects[Obj_HitNum]->setFrameTex(hitFrame);

    for (u32 i = 0; i < PlayerMax; i++) {
        m_objects[Obj_PlayerPanel + i]->setAnimName(&(&lbl_8045A530[9 * i])[18], true);
        m_objects[Obj_PlayerPanel + i]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        m_objects[Obj_PlayerPanel + i]->setFrameTex(rowFrame);
        float markFrame = getMarkColAnimFrame(i, false);
        switch (m_playerState[i]) {
        case 0:
            m_objects[Obj_PlayerMark + i]->setFrameMatCol(markFrame);
            m_objects[Obj_PlayerPanel + i]->setFrameMatCol(getPanelColorFrame(g_GameGlobal->m_resultInfo, m_playerState[i], i));
            m_objects[Obj_PlayerPanel + i]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
            if (m_unkC1 & (1 << i)) {
                m_objects[Obj_Contl + i]->setAnimName(&lbl_8045A980[1], true);
            }
            break;
        case 1:
            m_objects[Obj_PlayerMark + i]->setFrameMatCol(markFrame);
            m_objects[Obj_PlayerPanel + i]->setFrameMatCol(getPanelColorFrame(g_GameGlobal->m_resultInfo, m_playerState[i], i));
            m_objects[Obj_PlayerPanel + i]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
            break;
        case 2:
            break;
        }
        if (m_playerState[i] != 3) {
            for (int k = 0; k < 55; k++) {
                m_objects[Obj_PlayerBlock + i * Obj_PlayerBlockSize + 11 + k]->setAnimName(&lbl_8045A9C0[0], true);
            }
        }
    }
    m_objects[Obj_Mvp]->changeAnimN(0, 0x13, true, false);

    // The names of the players and of the winner.
    const u8 fontColor[15] = { 0xFF, 0xA9, 0xA7, 0xA2, 0xB4, 0xFF, 0xFF, 0xEA, 0x81, 0xA0, 0xE5, 0x8B, 0xBB, 0xBB, 0xBB };
    m_msgName = MuMsg::create(2, Heaps::MenuInstance, 0x2B);
    m_msgName->allocMsgBuf(0x80, 6);
    m_msgName->attachScnMdlSimple(0, m_objects[Obj_Panel]->m_sceneModel, 0, 1.0f / 6.0f);
    m_msgName->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 1, 0.0625f);
    for (int c = 0; c <= 1; c++) {
        m_msgName->setAlignMode(c, MuMsg::Align_Center);
        m_msgName->setFontColor(c, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msgName->setFontType(c, 1);
        m_msgName->setFontWidthModeAuto(c);
    }
    for (int i = 0; i < PlayerMax; i++) {
        if (m_playerState[i] != 3) {
            m_msgName->attachScnMdlSimple(2 + i, m_objects[Obj_PlayerPanel + i]->m_sceneModel, 0, 0.0625f);
            m_msgName->setAlignMode(2 + i, MuMsg::Align_Center);
            m_msgName->setEdgeEnable(2 + i, true);
            m_msgName->setEdge(2 + i, 0x2D, 0x2D, 0x2D, 2.0f);
            m_msgName->setFontType(2 + i, 1);
            if (m_playerState[i] == 0) {
                gmPlayerResultInfo* player = &g_GameGlobal->m_resultInfo->m_playersResultInfo[i];
                if (resultInfo->m_isTeams == 0) {
                    const u8* rgb = &fontColor[3 * i];
                    m_msgName->setFontColor(2 + i, rgb[0], rgb[1], rgb[2], 0xFF);
                } else {
                    const int teamColor[3] = { 0, 1, 3 };
                    int color = teamColor[player->m_teamNo] * 3;
                    m_msgName->setFontColor(2 + i, fontColor[color], fontColor[color + 1], fontColor[color + 2], 0xFF);
                }
                // The name that the player typed in: the player of the match whose number is this player's.
                const gmPlayerInitData* init = NULL;
                gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
                for (int k = 0; k < 4; k++) {
                    if (melee->m_playersInitData[k].m_state != 3 && melee->m_playersInitData[k].m_playerNo == i) {
                        init = &melee->m_playersInitData[k];
                        break;
                    }
                }
                if (init != NULL && init->m_name[0] != 0) {
                    char utf8[0x18];
                    Message::utf16to8(utf8, init->m_name);
                    m_msgName->printData(2 + i, utf8, strlen(utf8));
                } else {
                    m_msgName->printf(2 + i, "%s%s", lbl_8045B674[13], lbl_8045B674[1 + i]);
                }
            } else {
                m_msgName->setFontColor(2 + i, fontColor[12], fontColor[13], fontColor[14], 0xFF);
                m_msgName->printf(2 + i, lbl_8045B674[14]);
                m_msgName->setWindowRectVisible(2 + i, true);
            }
        }
    }

    // The title, the rule and the limit.
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, 0x13);
    m_msg->setMsgData(m_msgData);
    for (int c = 0; c <= 2; c++) {
        m_msg->setAlignMode(c, MuMsg::Align_Center);
        m_msg->setFontColor(c, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msg->setFontWidthModeAuto(c);
    }
    m_msg->attachScnMdlSimple(0, m_objects[Obj_Panel]->m_sceneModel, 2, 0.0625f);
    switch (view->m_gameMode) {
    case Game_Mode_Tournament:
        m_msg->printIndex(0, 0x30);
        break;
    case Game_Mode_Net_Okiraku:
        m_msg->printIndex(0, 0x31);
        break;
    default: {
        if (isQuMeleeSequence()) {
            // HYPOTHESIS: the title of the result of sqQuMelee depends on a flag (byte 2) of the tournament data.
            u32 index = 0x2E;
            if (reinterpret_cast<u8*>(g_GameGlobal->m_tournamentData)[2] == 0) {
                index = 0x2F;
            }
            m_msg->printIndex(0, index);
        } else if (view->m_isTeams == 1) {
            m_msg->printIndex(0, 1);
        } else if (view->m_flag0E_7) {
            m_msg->printIndex(0, 2);
        } else {
            m_msg->printIndex(0, 0);
        }
        break;
    }
    }
    u32 limit = 0;
    bool isStock = false;
    if (m_unkD4) {
        m_msg->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 3, 1.0f / 28.0f);
        m_msg->printIndex(1, 6);
    } else {
        switch (view->m_gameRule) {
        case Game_Rule_Time:
            m_msg->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 3, 1.0f / 28.0f);
            m_msg->printIndex(1, 3);
            limit = view->m_timeOrCoins;
            break;
        case Game_Rule_Stock:
            m_msg->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 3, 1.0f / 32.0f);
            m_msg->printIndex(1, 5);
            limit = view->m_stockCount;
            isStock = true;
            break;
        case Game_Rule_Coin:
            m_msg->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 3, 1.0f / 28.0f);
            m_msg->printIndex(1, 4);
            limit = view->m_timeOrCoins;
            break;
        }
        m_msg->attachScnMdlSimple(2, m_objects[Obj_Panel]->m_sceneModel, 4, 1.0f / 24.0f);
        if (!isStock && limit == 0) {
            m_msg->setHScale(2, 1.5f);
            m_msg->getHScale(2, 1.5f);
            m_msg->printf(2, lbl_8045B674[12]);
        } else {
            // The limit in full width digits followed by its unit.
            int length = 0;
            char text[0x20];
            // MATCH-ONLY: the original clears the buffer with 32 single byte stores (a plain initializer gives word stores).
            for (int j = 0; j < 0x20; j++) {
                text[j] = 0;
            }
            if (limit >= 10) {
                length = sprintf(text, "%s", lbl_8045B674[limit / 10]);
            }
            length += sprintf(text + length, "%s", lbl_8045B674[limit % 10]);
            int unitSize;
            const char* unit;
            if (isStock) {
                unit = m_msg->getMsgData(8, &unitSize);
            } else {
                unit = m_msg->getMsgData(7, &unitSize);
            }
            memcpy(text + length, unit, unitSize);
            m_msg->printData(2, text, length + unitSize);
        }
    }

    // The counters of the players: the knock outs, falls, ... and the points.
    for (int i = 0; i < PlayerMax; i++) {
        u32 channel = 3 + 4 * i;
        if (m_playerState[i] != 3) {
            if (view->m_gameRule == Game_Rule_Coin || m_unkD4) {
                m_msg->attachScnMdlSimple(channel, m_objects[Obj_HitNum]->m_sceneModel, 0x10 + i, 0.0625f);
                m_msg->setAlignMode(channel, MuMsg::Align_Right);
                m_msg->setFontColor(channel, 0x90, 0xC9, 0xEB, 0xFF);
                m_msg->attachScnMdlSimple(channel + 1, m_objects[Obj_HitNum]->m_sceneModel, 0x14 + i, 0.0625f);
                m_msg->setAlignMode(channel + 1, MuMsg::Align_Right);
                m_msg->setFontColor(channel + 1, 0xD2, 0x9F, 0x9F, 0xFF);
                m_msg->attachScnMdlSimple(channel + 3, m_objects[Obj_HitNum]->m_sceneModel, 0xC + i, 0.0625f);
                m_msg->setAlignMode(channel + 3, MuMsg::Align_Right);
                m_msg->setEdgeEnable(channel + 3, true);
                m_msg->setEdge(channel + 3, 0x14, 0x14, 0x14, 2.0f);
            } else {
                for (int k = 0; k < 4; k++) {
                    m_msg->attachScnMdlSimple(channel + k, m_objects[Obj_HitNum]->m_sceneModel, i + 4 * k, 0.0625f);
                    m_msg->setAlignMode(channel + k, MuMsg::Align_Right);
                }
                m_msg->setFontColor(channel, 0x90, 0xC9, 0xEB, 0xFF);
                m_msg->setFontColor(channel + 1, 0xD2, 0x9F, 0x9F, 0xFF);
                m_msg->setFontColor(channel + 2, 0xD2, 0x9F, 0x9F, 0xFF);
                m_msg->setEdgeEnable(channel + 3, true);
                m_msg->setEdge(channel + 3, 0x14, 0x14, 0x14, 2.0f);
            }
        }
    }

    // The tables of the players are drawn into the textures with a camera of the size of the screen; the screen is cleared before.
    {
        gfCamera camera;
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
        Message::drawBoxLine(0xFF, 0, 0, 0.0f, 0.0f, 640.0f, 480.0f, 0.0f);
        GXFlush();
        GXDrawDone();
    }
    for (u32 i = 0; i < PlayerMax; i++) {
        if (m_playerState[i] != 3) {
            m_texCount[i] = storeTexExt2(i, true);
            gfCamera camera;
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
            Message::drawBoxLine(0xFF, 0, 0, 0.0f, 0.0f, 640.0f, 480.0f, 0.0f);
            GXFlush();
            GXDrawDone();
        }
    }
}

// ---- 0x800EB0E4-0x800ED474 ----

extern "C" void OSReport(const char* msg, ...);

// HYPOTHESIS: the stand-ins of functions that are not named yet (the map names are in the comments).
// ScnGroup constructor (the group that the notice window and the replay save window are drawn in)
extern "C" nw4r::g3d::ScnGroup* fn_801AB6CC(MEMAllocator* allocator, u32* size, u32 capacity);
// nteFileDataReplay::__ct, setData
extern "C" nteFileDataReplay* fn_80152B5C(void* mem, Heaps::HeapType heap);
extern "C" void fn_80152C4C(nteFileDataReplay* data, void* buffer, u32 size);
// IfSnapSaveTask::create for a replay
extern "C" IfSnapSaveTask* fn_800F18C4(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* resFile2, void* msgData,
                                       nw4r::g3d::ScnGroup* scnGroup, int drawPriority, nteFileDataReplay* data,
                                       int unkA4, Heaps::HeapType heap, Heaps::HeapType heap2,
                                       Heaps::HeapType dataHeap);
// muWifiCntWndTask::setCounter(task, seconds, unk)



// the animation tables of cluster A (the rows of the four players start at 18, nine rows per group)
// the two animations of the controller icon (Obj_Contl)

// 0x800EB0E4: the music and the voices of the screen. From State_Intro on, the result music (0x2700) is started when
// nothing else plays (but not for a no contest). m_soundStep 0 (winner) / 2 (no contest) counts frames in m_unkE0 for
// the jingles; at step 0 the winner voice follows after 150 frames (m_unkDC): the voice of the winner's character, or
// of the winning team in the team battles.
void ifVsResultTask::soundProcess() {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;

    if (m_state >= State_Intro) {
        if (g_sndSystem->getBGMId() == -1) {
            if (m_bgmHandle < 0 || !g_sndSystem->isPlay(m_bgmHandle)) {
                if (resultInfo->m_decisionKind != gmResultInfo::Decision_NoContest) {
                    g_sndSystem->playBGM(0x2700, 0, true);
                }
            }
        }
    }

    // HYPOTHESIS: 0x2045 / 0x2041 / 0x20 are the jingles at the 30th (and 90th, no contest) frame of the screen
    if (m_soundStep == 0 || m_soundStep == 2) {
        if (m_unkE0 == 30) {
            if (m_soundStep == 0) {
                g_sndSystem->playSE((SndID)0x2045, -1, 0, 0, -1);
            } else {
                g_sndSystem->playSE((SndID)0x2041, -1, 0, 0, -1);
            }
        }
        if (m_unkE0 == 90 && m_soundStep == 2) {
            g_sndSystem->playSE((SndID)0x20, -1, 0, 0, -1);
        }
        m_unkE0++;
    }

    if (m_soundStep == 0) {
        if (m_unkDC >= 150) {
            m_soundStep = 1;
            if (!resultInfo->m_isTeams) {
                // HYPOTHESIS: indexed by the character kind of the winner
                const int voiceIds[] = {
                    0x2002, 0x2003, 0x2004, 0x2005, 0x2019, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x200B, 0x200C, 0x200D,
                    0x200E, 0x200F, 0x2010, 0x2011, 0x2011, 0x2011, 0x2012, 0x2013, 0x2014, 0x2015, 0x2016, 0x2017, 0x2018,
                    0x201A, 0x201B, 0x201C, 0x201E, 0x201E, 0x201F, 0x201F, 0x2020, 0x2020, 0x2021, 0x2022, 0x2023, 0x2024,
                    0x2025, 0x2026, 0x2027, 0x2028, 0x2029, 0x200D, 0x2016, 0x202A, 0x202A, 0x202A, 0x202A, 0,
                };
                // MATCH-ONLY: the kind is read as a plain byte (the shadow header declares it as an enum bit field)
                u8 characterKind = *(u8*)&resultInfo->m_playersResultInfo[m_winnerPlayer];
                g_sndSystem->playSE((SndID)voiceIds[characterKind], -1, 0, 0, -1);
            } else {
                // HYPOTHESIS: the voices of the three teams, indexed by m_winnerTeam
                const int teamVoiceIds[] = {0x2044, 0x2042, 0x2043};
                g_sndSystem->playSE((SndID)teamVoiceIds[m_winnerTeam], -1, 0, 0, -1);
            }
        }
        m_unkDC++;
    }
}

// 0x800EB300
bool ifVsResultTask::isLoadEnd() {
    return m_state != State_Load;
}

// 0x800EB314: the values that gmResultInfo keeps in game units are converted for the display. Per player that is not
// absent (m_state 3): the total fly distance (stored at 0x1DC, converted in place and kept as the display value at
// 0x1E0..) and the two maximum speeds (floats at 0x2A4 / 0x2A8).
// HYPOTHESIS: the field meanings come from the OSReport strings; the unit conversions (30.4788 cm per foot,
// 2011.68 = 1 mile in 0.1 m / 60 ...) are from the constants.
void ifVsResultTask::adjustResultinfo() {
    for (int i = 0; i < PlayerMax; i++) {
        // HYPOTHESIS: the stat fields are not declared in gmPlayerResultInfo yet; offsets are relative to the entry
        u8* player = (u8*)&g_GameGlobal->m_resultInfo->m_playersResultInfo[i];
        if (player[1] != 3) {
            OSReport("ifVsResultTask::adjustResultinfo normaldata[%d]\n", i);
            OSReport("    _flydisttotal:%dg\n", *(u32*)(player + 0x1DC));
            OSReport("    _damagespeedmax:%fg/f\n", *(float*)(player + 0x2A4));
            OSReport("    _attackspeedmax:%fg/f\n", *(float*)(player + 0x2A8));
            *(u32*)(player + 0x1DC) = (u32)(10.0f * *(u32*)(player + 0x1DC) / 30.4788f);
            *(float*)(player + 0x2A4) = (10.0f * *(float*)(player + 0x2A4) * 60.0f * 60.0f * 60.0f) / 2011.68f;
            *(float*)(player + 0x2A8) = (10.0f * *(float*)(player + 0x2A8) * 60.0f * 60.0f * 60.0f) / 2011.68f;
            OSReport("    _flydisttotal:%dft\n", *(u32*)(player + 0x1DC));
            OSReport("    _damagespeedmaxf:%fft/h\n", *(float*)(player + 0x2A4));
            OSReport("    _attackspeedmaxf:%fft/h\n", *(float*)(player + 0x2A8));
            *(u32*)(player + 0x210) = (u32)*(float*)(player + 0x2A4);
            *(u32*)(player + 0x214) = (u32)*(float*)(player + 0x2A8);
            *(u32*)(player + 0x1E0) = (u32)(*(u32*)(player + 0x1E0) / 60.0f);
            *(u32*)(player + 0x1E4) = (u32)(*(u32*)(player + 0x1E4) / 60.0f);
            *(u32*)(player + 0x1E8) = (u32)(*(u32*)(player + 0x1E8) / 60.0f);
            *(u32*)(player + 0x218) = (u32)(*(u32*)(player + 0x218) / 60.0f);
            *(u32*)(player + 0x21C) = (u32)(*(u32*)(player + 0x21C) / 60.0f);
        }
    }
}

// 0x800EB564: the step of the main panel (the HitNum object, the headline of the counters). 1: waits for its node
// animation to end (-> 3); 2: as soon as one player is still on page 0 it changes to the animation 2 (-> 3); 3: once
// every player is past page 0 (m_unk48 > 0) it changes to the animation 1 and goes back to 2.
void ifVsResultTask::subPanelControl() {
    switch (m_unk6C) {
    case 1:
        if (m_objects[Obj_HitNum]->isNodeAnimFinished()) {
            m_unk6C = 3;
        }
        break;
    case 2:
        for (int i = 0; i < PlayerMax; i++) {
            if (m_unk48[i] == 0) {
                m_objects[Obj_HitNum]->changeAnimN(2, 0x13, true, false);
                m_unk6C = 3;
                break;
            }
        }
        break;
    case 3: {
        int ready = 0;
        if (m_unk48[0] > 0) ready++;
        if (m_unk48[1] > 0) ready++;
        if (m_unk48[2] > 0) ready++;
        if (m_unk48[3] > 0) ready++;
        if (ready == 4) {
            m_objects[Obj_HitNum]->changeAnimN(1, 0x13, true, false);
            m_unk6C = 2;
        }
        break;
    }
    }
}


// ---- subPlayerControl ----

namespace {

struct PadWords {
    u32 m_words[sizeof(gfPadStatus) / 4];
};

inline bool hasBit(int mask, int index) {
    return (mask & (1 << index)) != 0;
}

// Shows the icon of the pad on the controller object.
inline void setControllerIcon(MuObject* object, const gfPadStatus& pad) {
    float frame = getControllerIconFrame(pad);
    if (frame >= 0.0f) {
        object->setFrameFlg(frame, 4);
    }
}

// HYPOTHESIS: the button that asks for the replay save: Z on GameCube and Nunchuk, ZL/ZR on the Classic Controller, the
// X-ish button (0x400) on the Wii remote.
inline u32 isReplaySaveButton(const gfPadStatus& pad) {
    u32 pressed;
    switch (pad.m_controllerType) {
    case gfPadType::GCC: pressed = (pad.m_buttonsPressedThisFrame.bits >> 4) & 1; break;
    case gfPadType::WII_CLASSIC: pressed = (pad.m_buttonsPressedThisFrame.bits & 0xC000) != 0; break;
    case gfPadType::WIIMOTE: pressed = (pad.m_buttonsPressedThisFrame.bits >> 22) & 1; break;
    case gfPadType::NUNCHUK: pressed = (pad.m_buttonsPressedThisFrame.bits >> 4) & 1; break;
    default: pressed = 0; break;
    }
    return pressed;
}

inline ifVsResultTask::PlayerNodes* getPlayerNodes(ifVsResultTask* task, int player) {
    return reinterpret_cast<ifVsResultTask::PlayerNodes*>(
        &task->m_objects[ifVsResultTask::Obj_PlayerBlock + ifVsResultTask::Obj_PlayerBlockSize * player]);
}

// Replaces the texture of a row by the dummy texture.
inline void clearRowTexture(MuObject* row) {
    row->changeMaterialTex(row->m_resMdl.GetResMat("dammyMat")->m_id, "InfResultDammytex", NULL);
}

// The first of the objects of the table of a player in m_objects: the 11 rows (Spos), then the 55 text lines (Stockface).
inline int getPlayerBlock(int player) {
    return ifVsResultTask::Obj_PlayerBlock + ifVsResultTask::Obj_PlayerBlockSize * player;
}

// Empties the rows of the table of a player (the dummy texture, and the text lines back to the first frame).
inline void clearRows(ifVsResultTask* task, int player) {
    int block = getPlayerBlock(player);
    int lines = ifVsResultTask::Obj_PlayerBlock + 11 + ifVsResultTask::Obj_PlayerBlockSize * player;
    for (int row = 0; row < 11; row++) {
        clearRowTexture(task->m_objects[block + row]);
        for (u32 line = 0; line < 5; line++) {
            task->m_objects[lines + row * 5 + line]->setFrameTex(0.0f);
        }
    }
}

// Puts the rows of the table under each other, 2.5 apart, and shifts them by offset.
inline void placeRows(ifVsResultTask* task, int player, float offset) {
    int block = getPlayerBlock(player);
    for (u32 row = 0; row < 11; row++) {
        Vec3f pos;
        task->m_objects[block + row]->getPos(&pos, "InfResultSpos_TopN");
        pos.m_y = -2.5f * (float)row + offset;
        task->m_objects[block + row]->setTrans(&pos);
    }
}

// The rows of the table with the textures of the first line down (the lines are stored in m_texObjs).
inline void showTexRows(ifVsResultTask* task, int player, u32 line) {
    typedef msTexObj TexObjRow[ifVsResultTask::TexObjsPerPlayer];
    TexObjRow* texObjs = reinterpret_cast<TexObjRow*>(task->m_texObjs);
    int block = getPlayerBlock(player);
    for (int row = 0; row < 11; row++) {
        if (line < task->m_texCount[player]) {
            task->m_objects[block + row]->changeMaterialTex("dammyMat", &texObjs[player][line].m_texObj);
            line++;
        } else {
            clearRowTexture(task->m_objects[block + row]);
        }
    }
}

}  // namespace

// 0x800EB684: input and animation of the rows of one player. m_unk48[player] is the page of the player: 0 the hit
// counters, 1 the statistics lines that drawExt1 draws, 2 the table with the textures of storeTexExt2, 3 for a player
// that is absent, 4 the question for the next Wifi game. B goes back a page (backStep), A and Start go on
// (jumpStandby). The stick or d-pad scrolls the lines (calcScrollLv).
void ifVsResultTask::subPlayerControl(int player) {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    u32 action = 2;  // 2: nothing happened, 0: A was pressed, 1: follow the other players

    int active0 = 0;
    if (m_playerState[0] != 0) {
        active0 = 1;
    }
    int active01 = 0;
    if (active0 && m_playerState[1] != 0) {
        active01 = 1;
    }
    int active012 = 0;
    if (active01 && m_playerState[2] != 0) {
        active012 = 1;
    }
    int allPlayers = 0;
    if (active012 && m_playerState[3] != 0) {
        allPlayers = 1;
    }

    int bit = 1 << player;
    int padIndex = -1;
    if (m_unkC1 & bit) {
        // HYPOTHESIS: the byte at +4 of the player is the number of the controller port + 1
        padIndex = reinterpret_cast<u8*>(&resultInfo->m_playersResultInfo[player])[4] - 1;
    } else if (m_playerState[player] == 1 && allPlayers) {
        padIndex = GF_PAD_SYSTEM_GET_ALL_PADS;
    }

    gfPadStatus pad;
    // MATCH-ONLY: word-wise copies of the pad for calcScrollLv (a gfPadStatus copy goes field by field)
    PadWords scrollPad1;
    PadWords scrollPad2;
    if (padIndex < 0) {
        memset(&pad, 0, sizeof(pad));
    } else {
        g_gfPadSystem->getSysPadStatus(padIndex, &pad);
        pad.clamp();
    }

    // the button that asks for the replay save: opens the save window
    if (!m_unk66A && m_state == State_Main && m_unk48[player] >= 0 && m_unk48[player] <= 2 && m_unk444C &&
        m_noticeGroup == NULL) {
        if (isReplaySaveButton(pad)) {
            u32 groupSize;
            m_noticeGroup = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::HeapType(0x2A)), &groupSize, 8);
            g_gfSceneRoot->add(gfSceneRoot::Layer_Info, (nw4r::g3d::ScnMdl*)m_noticeGroup);
            void* mem = operator new(0x14, Heaps::HeapType(0x2A));
            if (mem != NULL) {
                mem = fn_80152B5C(mem, Heaps::HeapType(0x2A));
            }
            m_replayData = (nteFileDataReplay*)mem;
            u32 size;
            void* buffer;
            ipSwitch::getInstance()->m_keyRecorder->getBufferData(&buffer, &size);
            fn_80152C4C(m_replayData, buffer, size);
            m_replaySaveTask = fn_800F18C4(&m_resFiles[9], &m_resFiles[7], m_wifiMsgData, m_noticeGroup, 0x1E,
                                           m_replayData, 0, Heaps::HeapType(0x2A), Heaps::HeapType(0x2A),
                                           Heaps::HeapType(0xFF));
            m_state = State_ReplaySave;
        }
    }

    switch (m_unk48[player]) {
    case 0: {
        if ((u32)m_playerState[player] > 1) {
            break;
        }
        m_unk78[player]++;
        if (m_unkC1 & bit) {
            if (m_unk78[player] >= 120) {
                MuObject** contl = &m_objects[Obj_Contl + player];
                setControllerIcon(*contl, pad);
                if (m_unk78[player] == 120) {
                    (*contl)->setAnimName(&lbl_8045A980[0], false);
                }
            }
            int port;
            if (m_unk444C && (m_unkC1 & bit) &&
                (port = reinterpret_cast<u8*>(&g_GameGlobal->m_resultInfo->m_playersResultInfo[player])[4]) > 0) {
                {
                    MuObject** replay = &m_objects[Obj_Replay + player];
                    m_expands[1 + player]->PushBack((*replay)->m_scnMdl, "pos02");
                    (*replay)->changeAnimN(0, 4, false, false);
                    gfPadStatus replayPad;
                    g_gfPadSystem->getSysPadStatus(port - 1, &replayPad);
                    setControllerIcon(*replay, replayPad);
                }
            }
        }

        if (m_objects[Obj_PlayerPanel + player]->m_modelAnim->getFrame() == 9.0f && !m_unk68[player]) {
            printHitPanel(player);
            m_unk68[player] = 1;
            m_objects[Obj_PlayerMark + player]->setFrameMatCol(getMarkColAnimFrame(player, false));
        }

        if (pad.m_buttonsPressedThisFrame.bits & 0x100) {
            m_unkC4[player] = 3;
            action = 0;
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
            m_objects[Obj_HitNum]->setFrameFlg(30.0f, 0x13);
            m_unk6C = 1;
        } else if ((bit & (m_unkC2 | m_unkC3)) && m_unkC4[player] == 0) {
            // the CPUs and the players without a controller follow the first player that went on
            for (int i = 0; i < PlayerMax; i++) {
                if ((m_unkC1 & (1 << i)) && m_unk48[i] != 0) {
                    action = 1;
                    break;
                }
            }
        }

        if (action != 2) {
            m_unk78[player] = 0;
            m_unk58[player] = 0;
            m_unk68[player] = 0;
            if (m_unkC1 & bit) {
                m_objects[Obj_Contl + player]->setAnimName(&lbl_8045A980[1], false);
            }
            m_unk48[player] = 1;
            m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[(player + 2) * 9 + 3], true);
            for (int i = 0; i < PlayerMax; i++) {
                m_msg->printf(3 + player * 4 + i, "");
            }
            for (int i = 0; i < PlayerMax; i++) {
                if (m_playerState[i] == 3 && m_unk48[i] != 3) {
                    m_unk68[i] = 0;
                    m_unk78[i] = 0;
                    m_unk48[i] = 3;
                    m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[(i + 2) * 9 + 7], true);
                }
            }
            m_objects[Obj_PlayerMark + player]->setFrameMatCol(getMarkColAnimFrame(player, true));
        
        } else {
            if (((m_unkC1 & bit) && pad.m_error != gfPadError::NONE) || (pad.m_buttonsPressedThisFrame.bits & 0x1000) ||
                m_unk66A) {
                g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
                jumpStandby(player, true);
                m_unk58[player] = 0;
                int ready = 0;
                if (m_unk48[0] > 0) ready++;
                if (m_unk48[1] > 0) ready++;
                if (m_unk48[2] > 0) ready++;
                if (m_unk48[3] > 0) ready++;
                if (ready == 4) {
                    m_objects[Obj_HitNum]->changeAnimN(1, 0x13, true, false);
                    m_unk6C = 2;
                }
            }
        }
        break;
    }
    case 1: {
        if ((u32)m_playerState[player] > 1) {
            break;
        }
        m_unk78[player]++;
        if (m_unkC1 & bit) {
            if (m_unk78[player] >= 120) {
                MuObject** contl = &m_objects[Obj_Contl + player];
                setControllerIcon(*contl, pad);
                if (m_unk78[player] == 120) {
                    (*contl)->setAnimName(&lbl_8045A980[0], false);
                }
            }
            int port;
            if (m_unk444C && (m_unkC1 & bit) &&
                (port = reinterpret_cast<u8*>(&g_GameGlobal->m_resultInfo->m_playersResultInfo[player])[4]) > 0) {
                {
                    MuObject** replay = &m_objects[Obj_Replay + player];
                    m_expands[1 + player]->PushBack((*replay)->m_scnMdl, "pos02");
                    (*replay)->changeAnimN(0, 4, false, false);
                    gfPadStatus replayPad;
                    g_gfPadSystem->getSysPadStatus(port - 1, &replayPad);
                    setControllerIcon(*replay, replayPad);
                }
            }
        }

        if (m_objects[Obj_PlayerPanel + player]->m_modelAnim->getFrame() == 9.0f && !m_unk68[player]) {
            m_unk68[player] = 1;
            drawExt1(player, 0);
        }

        scrollPad1 = *(const PadWords*)&pad;
        int scroll = calcScrollLv((const gfPadStatus*)&scrollPad1);
        if (scroll != 0) {
            m_unkC4[player] = 3;
        }
        switch (m_unkC4[player]) {
        case 0:
            if (m_unk78[player] >= 120) {
                m_unkC4[player] = 1;
            }
            break;
        case 1:
            scroll = 9;
            break;
        case 2:
            if (m_unk78[player] >= 120) {
                action = 1;
                m_unkC4[player] = 0;
            }
            break;
        }

        if (scroll != 0) {
            u32 oldPos = m_unk88[player];
            u32 newPos = scroll + oldPos;
            if ((int)newPos < 0 || (u32)lineCalcExt1(player) <= 10) {
                m_unk88[player] = 0;
            } else if (newPos > (u32)((lineCalcExt1(player) - 10) * 200)) {
                m_unk88[player] = (lineCalcExt1(player) - 10) * 200;
            } else {
                m_unk88[player] = newPos;
            }

            if (oldPos != m_unk88[player]) {
                int block = getPlayerBlock(player);
                u32 rem = m_unk88[player] % 200;
                for (u32 row = 0; row < 11; row++) {
                    Vec3f pos;
                    m_objects[block + row]->getPos(&pos, "InfResultSpos_TopN");
                    pos.m_y = -2.5f * (float)row + 2.5f * (float)rem / 200.0f;
                    m_objects[block + row]->setTrans(&pos);
                }
                if (scroll > 0 && oldPos % 200 > m_unk88[player] % 200) {
                    m_unk98[player]++;
                    drawExt1(player, m_unk98[player]);
                } else if (scroll < 0 && oldPos % 200 < m_unk88[player] % 200) {
                    m_unk98[player]--;
                    drawExt1(player, m_unk98[player]);
                }
            } else {
                if (m_unkC4[player] == 1) {
                    m_unkC4[player] = 2;
                    m_unk78[player] = 0;
                }
            }
        }

        if (pad.m_buttonsPressedThisFrame.bits & 0x100) {
            action = 0;
            m_unkC4[player] = 3;
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        }

        if (action != 2) {
            m_unk68[player] = 0;
            m_unk78[player] = 0;
            m_unk88[player] = 0;
            m_unk98[player] = 0;
            m_unk58[player] = 1;
            if (m_unkC1 & bit) {
                m_objects[Obj_Contl + player]->setAnimName(&lbl_8045A980[1], false);
            }
            m_unk48[player] = 2;
            m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[(player + 2) * 9 + 5], true);
            clearRows(this, player);
            placeRows(this, player, 0.0f);
        } else {
            if (pad.m_buttonsPressedThisFrame.bits & 0x200) {
                g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
                clearRows(this, player);
                placeRows(this, player, 0.0f);
                backStep(player);
            } else if (((m_unkC1 & bit) && pad.m_error != gfPadError::NONE) ||
                       (pad.m_buttonsPressedThisFrame.bits & 0x1000) || m_unk66A) {
                g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
                clearRows(this, player);
                placeRows(this, player, 0.0f);
                jumpStandby(player, true);
                m_unk58[player] = 1;
            }
        }
        break;
    }
    case 2: {
        if ((u32)m_playerState[player] > 1) {
            break;
        }
        m_unk78[player]++;
        if (m_unkC1 & bit) {
            if (m_unk78[player] >= 120) {
                MuObject** contl = &m_objects[Obj_Contl + player];
                setControllerIcon(*contl, pad);
                if (m_unk78[player] == 120) {
                    (*contl)->setAnimName(&lbl_8045A980[0], false);
                }
            }
            int port;
            if (m_unk444C && (m_unkC1 & bit) &&
                (port = reinterpret_cast<u8*>(&g_GameGlobal->m_resultInfo->m_playersResultInfo[player])[4]) > 0) {
                {
                    MuObject** replay = &m_objects[Obj_Replay + player];
                    m_expands[1 + player]->PushBack((*replay)->m_scnMdl, "pos02");
                    (*replay)->changeAnimN(0, 4, false, false);
                    gfPadStatus replayPad;
                    g_gfPadSystem->getSysPadStatus(port - 1, &replayPad);
                    setControllerIcon(*replay, replayPad);
                }
            }
        }

        if (m_objects[Obj_PlayerPanel + player]->m_modelAnim->getFrame() == 9.0f && !m_unk68[player]) {
            m_unk68[player] = 1;
            showTexRows(this, player, 0);
        }

        scrollPad2 = *(const PadWords*)&pad;
        int scroll = calcScrollLv((const gfPadStatus*)&scrollPad2);
        if (scroll != 0) {
            m_unkC4[player] = 3;
        }
        switch (m_unkC4[player]) {
        case 0:
            if (m_unk78[player] >= 120) {
                m_unkC4[player] = 1;
            }
            break;
        case 1:
            scroll = 9;
            break;
        case 2:
            if (m_unk78[player] >= 120) {
                action = 1;
                m_unkC4[player] = 0;
            }
            break;
        }

        if (scroll != 0) {
            u32 oldPos = m_unk88[player];
            u32 newPos = scroll + oldPos;
            if ((int)newPos < 0) {
                m_unk88[player] = 0;
            } else if (newPos > (u32)((m_texCount[player] - 9) * 200)) {
                m_unk88[player] = (m_texCount[player] - 9) * 200;
            } else {
                m_unk88[player] = newPos;
            }

            if (oldPos != m_unk88[player]) {
                int block = getPlayerBlock(player);
                u32 rem = m_unk88[player] % 200;
                for (u32 row = 0; row < 11; row++) {
                    Vec3f pos;
                    m_objects[block + row]->getPos(&pos, "InfResultSpos_TopN");
                    pos.m_y = -2.5f * (float)row + 2.5f * (float)rem / 200.0f;
                    m_objects[block + row]->setTrans(&pos);
                }
                if (scroll > 0 && oldPos % 200 > m_unk88[player] % 200) {
                    m_unk98[player]++;
                    showTexRows(this, player, m_unk98[player]);
                } else if (scroll < 0 && oldPos % 200 < m_unk88[player] % 200) {
                    m_unk98[player]--;
                    showTexRows(this, player, m_unk98[player]);
                }
            } else {
                if (m_unkC4[player] == 1) {
                    m_unkC4[player] = 2;
                    m_unk78[player] = 0;
                }
            }
        }

        if (pad.m_buttonsPressedThisFrame.bits & 0x100) {
            action = 0;
            m_unkC4[player] = 3;
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        }

        if (action != 2) {
            jumpStandby(player, false);
            m_unk58[player] = 2;
            clearRows(this, player);
            placeRows(this, player, 0.0f);
        } else {
            if (((m_unkC1 & bit) && pad.m_error != gfPadError::NONE) || (pad.m_buttonsPressedThisFrame.bits & 0x1000) ||
                m_unk66A) {
                g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
                clearRows(this, player);
                placeRows(this, player, 0.0f);
                jumpStandby(player, false);
                m_unk58[player] = 2;
            } else if (pad.m_buttonsPressedThisFrame.bits & 0x200) {
                g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
                clearRows(this, player);
                placeRows(this, player, 0.0f);
                backStep(player);
            }
        }
        break;
    }
    case 3: {
        if (m_unk66A) {
            break;
        }
        if (m_cntWnd != NULL && player == 0) {
            if (m_unk669) {
                if ((u32)(*(int*)((u8*)g_muWifiInterfaceTask + 0x6C) - 4) <= 3) {
                    // asks whether to play the next game
                    u32 groupSize;
                    u8 text[0x40];
                    memset(text, 0, sizeof(text));
                    int length;
                    const char* data = m_msg->getMsgData(0x2B, &length);
                    memcpy(text, data, length);
                    m_noticeGroup = fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::HeapType(0x2A)), &groupSize, 1);
                    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, (nw4r::g3d::ScnMdl*)m_noticeGroup);
                    m_noticeWnd = fn_800BF048(1, (const char*)text, length + 1, &m_resFiles[7], m_noticeGroup, 0x1E, 0, 0x2A,
                                              0x2A, 0, 0, 0xF0);
                    fn_800BF97C(m_noticeWnd, m_msg->getBinary(), 0x2C);
                    fn_800BFA04(m_noticeWnd, m_msg->getBinary(), 0x2D);
                    m_noticeWnd->m_unk120 = 1;
                    m_unk48[player] = 4;
                    m_unk66A = 1;
                } else {
                    m_state = State_Unk7;
                }
            } else {
                m_state = State_Unk6;
                m_unk74 = 0;
            }
        } else {
            m_unk78[player]++;
            if ((u32)m_playerState[player] <= 1 && (pad.m_buttonsPressedThisFrame.bits & 0x200)) {
                g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
                backStep(player);
            }
        }
        break;
    }
    case 4: {
        if (m_noticeWnd != NULL) {
            if (m_state != State_Main) {
                if (m_noticeGroup != NULL) {
                    m_noticeGroup->Destroy();
                    m_noticeGroup = NULL;
                }
                if (m_noticeWnd != NULL) {
                    m_noticeWnd->exit();
                    m_noticeWnd = NULL;
                }
            } else if (m_noticeWnd->m_step == 3) {
                m_unk669 = m_noticeWnd->m_selected;
                if (m_unk669) {
                    m_state = State_Unk7;
                } else {
                    m_state = State_Unk9;
                    m_timer = -1;
                    fn_800DA6EC(m_cntWnd, 0, 0);
                }
                if (m_noticeGroup != NULL) {
                    m_noticeGroup->Destroy();
                    m_noticeGroup = NULL;
                }
                if (m_noticeWnd != NULL) {
                    m_noticeWnd->exit();
                    m_noticeWnd = NULL;
                }
            }
        }
        break;
    }
    }
}

