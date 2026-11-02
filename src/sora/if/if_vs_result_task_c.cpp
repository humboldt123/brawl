#include <types.h>
#include <gf/gf_camera.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <gm/gm_result_info.h>
#include <if/if_vs_result_task.h>
#include <mu/menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <stdio.h>
#include <string.h>

// Unit c of the temporary split of if_vs_result_task: initMdlData (0x800E9878).

extern "C" void GXFlush(void);
extern "C" void GXDrawDone(void);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(nw4r::g3d::ResAnmScn* self, nw4r::g3d::ResAnmScn anmScn);

// HYPOTHESIS: map name muMenu::getStockFrameID-like helper; it returns the number of the stock mark of a stock character kind
// (the file InfResultMark%02d_TopN holds one model per mark).
extern "C" int fn_800AF6F0(int stock);
// HYPOTHESIS: map name gfSceneRoot::setClearDepthBufferState
extern "C" void fn_8000E590(gfSceneRoot* root, int layer, bool clear);

// The tables of the first unit (ifVsResultTask's data): the animation names of the objects of the screen, 9 rows per
// group: 0 the panel, 1 the hit numbers, 2..5 the four player rows. The flag bytes of some rows are patched here.
extern MuAnimNameData lbl_8045A530[54];
// The animations of the controller icon (row 1 is the one that is set) and of the text lines of the table.
extern MuAnimNameData lbl_8045A980[2];
extern MuAnimNameData lbl_8045A9C0[1];
// HYPOTHESIS: 4 players x 9 rows x 0x20 bytes: the names of the animations of the player rows ("<node>__<row>").
extern char lbl_8049E0D0[4][9][0x20];
// The strings of the full width digits and symbols (0..9, then the name and no-limit marks).
extern const char* lbl_8045B674[];

// The bytes of gmResultInfo that the header hides.
struct VsResultInfoView {
    u8 m_gameKind;          // 0x00 1 and 0x18 have their own title
    u8 m_gameRule;          // 0x01 0 time, 1 stock, 2 coin
    u8 m_isTeams;           // 0x02
    u8 _3;
    u32 m_timeOrCoins;      // 0x04 the limit of the time and coin rules
    u32 m_stockCount;       // 0x08 the limit of the stock rule
    s8 _C[2];
    u8 m_tournament : 1;    // 0x0E bit 7: a tournament-like result
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

// Whether the sequence that is running is the one with this name.
static inline bool isCurrentSequence(const char* name) {
    char sequence[0x44];
    sequence[0] = 0;
    strcat(sequence, gfSceneManager::getInstance()->m_currentSequence->m_sequenceName);
    return strcmp(sequence, name) == 0;
}

void ifVsResultTask::initMdlData() {
    gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
    VsResultInfoView* view = reinterpret_cast<VsResultInfoView*>(resultInfo);
    u8* resultBytes = reinterpret_cast<u8*>(resultInfo);

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
    for (u32 g = 0; g < 5; g++) {
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
    if (view->m_gameRule == 2) {
        hitFrame = 1.0f;
        rowFrame = hitFrame;
    }
    if (view->m_gameRule == 1) {
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
    for (int c = 0; c < 2; c++) {
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
                if (resultInfo->m_isTeams == 0) {
                    m_msgName->setFontColor(2 + i, fontColor[3 * i], fontColor[3 * i + 1], fontColor[3 * i + 2], 0xFF);
                } else {
                    const int teamColor[3] = { 0, 1, 3 };
                    int color = teamColor[reinterpret_cast<u8*>(g_GameGlobal->m_resultInfo)[0x2A + i * 0x2AC]] * 3;
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
    for (int c = 0; c < 3; c++) {
        m_msg->setAlignMode(c, MuMsg::Align_Center);
        m_msg->setFontColor(c, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msg->setFontWidthModeAuto(c);
    }
    m_msg->attachScnMdlSimple(0, m_objects[Obj_Panel]->m_sceneModel, 2, 0.0625f);
    switch (view->m_gameKind) {
    case 1:
        m_msg->printIndex(0, 0x30);
        break;
    case 0x18:
        m_msg->printIndex(0, 0x31);
        break;
    default: {
        if (isCurrentSequence("sqQuMelee")) {
            u32 index = 0x2E;
            if (reinterpret_cast<u8*>(g_GameGlobal->m_tournamentData)[2] == 0) {
                index = 0x2F;
            }
            m_msg->printIndex(0, index);
        } else if (view->m_isTeams == 1) {
            m_msg->printIndex(0, 1);
        } else if (view->m_tournament) {
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
        case 0:
            m_msg->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 3, 1.0f / 28.0f);
            m_msg->printIndex(1, 3);
            limit = view->m_timeOrCoins;
            break;
        case 1:
            m_msg->attachScnMdlSimple(1, m_objects[Obj_Panel]->m_sceneModel, 3, 1.0f / 32.0f);
            m_msg->printIndex(1, 5);
            limit = view->m_stockCount;
            isStock = true;
            break;
        case 2:
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
            char text[0x20] = { 0 };
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
            if (view->m_gameRule == 2 || m_unkD4) {
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
