#include <if/if_vs_result_task.h>

#include <gf/gf_3d_scene.h>
#include <gf/gf_camera.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gm/gm_global.h>
#include <if/if_snap_save.h>
#include <gm/gm_result_info.h>
#include <mu/menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

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
class muWifiCntWndTask : public gfTask {};

// HYPOTHESIS: the heap object of the replay save, only its virtual destructor (vptr at 0x8) is used here
class nteFileDataReplayBase {
public:
    u8 unk0[0x8];
};

class nteFileDataReplay : public nteFileDataReplayBase {
public:
    virtual ~nteFileDataReplay();
    u8 unkC[0x8];
};

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
extern MuAnimNameData lbl_8045A530[6][9];
// HYPOTHESIS: the two animations of the controller icon (0x8045A980)
extern MuAnimNameData lbl_8045A980[2];

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
            m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[i + 2][7], true);
            if (flag) {
                m_objects[Obj_PlayerPanel + i]->setFrameFlg(9.0f, lbl_8045A530[i + 2][7].m_flags);
                m_objects[Obj_PlayerPanel + i]->setUpdateRateFlg(0.0f, lbl_8045A530[i + 2][7].m_flags);
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
                m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[i + 2][7], true);
                m_objects[Obj_PlayerPanel + i]->setFrameFlg(9.0f, lbl_8045A530[i + 2][7].m_flags);
                m_objects[Obj_PlayerPanel + i]->setUpdateRateFlg(0.0f, lbl_8045A530[i + 2][7].m_flags);
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
        m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[player + 2][4], true);
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
                    m_objects[Obj_PlayerPanel + i]->setAnimName(&lbl_8045A530[i + 2][4], true);
                    if (((1 << i) & (m_unkC2 | m_unkC3)) && m_unk48[i] != 1) {
                        m_objects[Obj_PlayerPanel + i]->setFrameFlg(9.0f, lbl_8045A530[i + 2][4].m_flags);
                        m_objects[Obj_PlayerPanel + i]->setUpdateRateFlg(0.0f, lbl_8045A530[i + 2][4].m_flags);
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
        m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[player + 2][6], true);
        break;
    case 2:
        m_objects[Obj_PlayerPanel + player]->setAnimName(&lbl_8045A530[player + 2][8], true);
        break;
    }
    if (speedUp) {
        m_objects[Obj_PlayerPanel + player]->setFrameFlg(9.0f, lbl_8045A530[player + 2][4].m_flags);
        m_objects[Obj_PlayerPanel + player]->setUpdateRateFlg(0.0f, lbl_8045A530[player + 2][4].m_flags);
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
