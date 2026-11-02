#include <types.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <gm/gm_result_info.h>
#include <if/if_vs_result_task.h>
#include <ip/ip_switch.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_system.h>
#include <string.h>

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
extern "C" void fn_800DA6EC(muWifiCntWndTask* task, int seconds, int unk);



// the animation tables of cluster A (the rows of the four players start at 18, nine rows per group)
extern MuAnimNameData lbl_8045A530[];
// the two animations of the controller icon (Obj_Contl)
extern MuAnimNameData lbl_8045A980[2];

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

// The controller icon (Obj_Contl / Obj_Replay) shows the frame of the pad type: 2 GameCube, 1 Classic, 3 Wii remote,
// 4 Nunchuk, 5 anything else, and 0 when the pad is not connected.
// HYPOTHESIS: the numbers are the frames of the texture pattern animation, the order is by gfPadType.
inline float getControllerIconFrame(const gfPadStatus& pad) {
    float frame;
    if (pad.m_error != gfPadError::NONE) {
        frame = 0.0f;
    } else {
        switch (pad.m_controllerType) {
        case gfPadType::GCC: frame = 2.0f; break;
        case gfPadType::WII_CLASSIC: frame = 1.0f; break;
        case gfPadType::WIIMOTE: frame = 3.0f; break;
        case gfPadType::NUNCHUK: frame = 4.0f; break;
        default: frame = 5.0f; break;
        }
    }
    return frame;
}

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
