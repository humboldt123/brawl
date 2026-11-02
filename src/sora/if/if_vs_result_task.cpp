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

// Unit a of the temporary split of if_vs_result_task (0x800E5858-0x800E7BD0): the constructor, loadResource, the three
// print helpers and processAnim. It owns the data of the whole range: the animation tables (.data), the buffers they
// point at (.bss) and the constants.

// ---- data ----

// HYPOTHESIS: the names of the animations that initMdlData formats with sprintf ("<node name>__<n>"). The animation
// tables below point at these 36 buffers (9 rows for each of the four players' rows, 0x20 bytes each); the splits
// give this unit only the first buffer, the other 35 are part of the same array in the original.
char lbl_8049E0D0[36][0x20] __attribute__((aligned(8)));

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
    {0.0f, 19.0f, 0, lbl_8049E0D0[0], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[1], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[2], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[3], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[4], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[5], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[6], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[7], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[8], 3},
    // row of player 1
    {0.0f, 19.0f, 0, lbl_8049E0D0[9], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[10], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[11], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[12], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[13], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[14], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[15], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[16], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[17], 3},
    // row of player 2
    {0.0f, 19.0f, 0, lbl_8049E0D0[18], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[19], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[20], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[21], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[22], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[23], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[24], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[25], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[26], 3},
    // row of player 3
    {0.0f, 19.0f, 0, lbl_8049E0D0[27], 0x17},
    {0.0f, 230.0f, 0, lbl_8049E0D0[28], 3},
    {0.0f, 29.0f, 0, lbl_8049E0D0[29], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[30], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[31], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[32], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[33], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[34], 3},
    {0.0f, 9.0f, 0, lbl_8049E0D0[35], 3},
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
