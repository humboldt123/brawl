#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <mu/mu_clear_get.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_restex.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

extern MuAnimNameData lbl_804569E0[];

// HYPOTHESIS: the result of AnmObjTexPatRes::GetResult (nw4r TexPatAnmResult): which textures the animation shows now.
struct ClearGetTexPatResult {
    u8 hasTex;
    u8 hasPltt;
    nw4r::g3d::ResTex tex[8];
    nw4r::g3d::ResTex pltt[8];
};

muClearGetTask::~muClearGetTask() {
    g_GameGlobal->m_record->unk1528[0x80] = m_hammerNum;
    for (u32 i = 0; i < GroupNum; i++) {
        if (m_groups[i]) {
            m_groups[i]->Destroy();
            m_groups[i] = NULL;
        }
    }
    for (u32 i = 0; i < ObjNum; i++) {
        if (m_objs[i]) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    if (m_fileImage) {
        gfHeapManager::free(m_fileImage);
        m_fileImage = NULL;
    }
    if (m_pacArchive) {
        delete m_pacArchive;
        m_pacArchive = NULL;
    }
    if (m_dataArchive) {
        delete m_dataArchive;
        m_dataArchive = NULL;
    }
    delete m_msg;
    g_sndSystem->freeGroup(m_soundGroup, 1);
    m_objTask->exit();
    m_objTask = NULL;
    m_menuRoot->exit();
    m_menuRoot = NULL;
}

bool muClearGetTask::isEnd() {
    return m_state >= 10;
}

void muClearGetTask::setCursor() {
    char name[8];
    if (m_objs[4]->m_modelAnim->m_anmObjVisRes->GetFrame() != 2.0f) {
        if (m_cursorOnGrid) {
            u32 position = m_column + m_row * 4;
            m_objs[4]->setFrameVisible(0.0f);
            sprintf(name, "pos%02d", position);
            m_groups[1]->Remove(m_objs[4]->m_scnMdl);
            m_groups[2]->Remove(m_objs[4]->m_scnMdl);
            m_groups[1]->PushBack(m_objs[4]->m_scnMdl, name);
        } else {
            u32 position = m_hammerSel + 95;
            m_objs[4]->setFrameVisible(1.0f);
            sprintf(name, "pos%02d", position);
            m_groups[1]->Remove(m_objs[4]->m_scnMdl);
            m_groups[2]->Remove(m_objs[4]->m_scnMdl);
            m_groups[2]->PushBack(m_objs[4]->m_scnMdl, name);
        }
        if (m_hammerHeld) {
            m_groups[1]->Remove(m_objs[7 + m_hammerSel]->m_scnMdl);
            m_groups[1]->PushBack(m_objs[7 + m_hammerSel]->m_scnMdl, name);
        }
    }
}

void muClearGetTask::printPrizeData(int state) {
    u32 box = m_column + m_row * 4;
    muClearGetData* data = &m_dataMgr.m_list[box];
    if (state == 3) {
        state = m_dataMgr.m_openState[box];
    }
    m_groups[0]->Remove(m_groups[3]);
    for (u32 i = 20; i <= 24; i++) {
        m_groups[0]->Remove(m_objs[i]->m_scnMdl);
    }
    if (state == 2) {
        switch (data->m_kind) {
        case 0:
            m_groups[0]->PushBack(m_objs[20]->m_scnMdl, "pos01");
            m_objs[20]->setFrameFlg(static_cast<float>(data->m_reward), 4);
            {
                nw4r::g3d::AnmObjTexPatRes* texPat = m_objs[20]->m_modelAnim->m_anmObjTexPatRes;
                ClearGetTexPatResult result;
                ClearGetTexPatResult* shown = reinterpret_cast<ClearGetTexPatResult*>(texPat->GetResult(reinterpret_cast<int*>(&result), 0));
                u8* tex = reinterpret_cast<u8*>(shown->tex[0].ptr());
                Vec3f scale;
                // BYTES: the texture header has no named width and height fields yet (0x1C, 0x1E)
                scale.m_x = 0.1f * *reinterpret_cast<u16*>(tex + 0x1C);
                scale.m_y = 0.1f * *reinterpret_cast<u16*>(tex + 0x1E);
                scale.m_z = 1.0f;
                m_objs[20]->setObjScale(&scale);
            }
            m_objs[1]->setFrameFlg(0.0f, 2);
            break;
        case 1: {
            m_groups[0]->PushBack(m_groups[3], "pos01");
            u16 figId = data->m_reward;
            s32 texFile = m_figMng->getBilTexFileIdxFigId(figId);
            char texName[32];
            m_figMng->getBilTexNameFigId(figId, texName);
            nw4r::g3d::ResFile* figRes = &m_figTexRes[texFile];
            nw4r::g3d::ResTex figTex = figRes->GetResTex(texName);
            if (figTex.IsValid()) {
                MuObject* card = m_objs[19];
                nw4r::g3d::ResMat mat = card->m_resMdl.GetResMat("yesno_lambert16");
                card->changeMaterialTex(mat.ptr()->m_id, texName, figRes);
                u8* tex = reinterpret_cast<u8*>(figTex.ptr());
                Vec3f scale;
                // BYTES: the texture header has no named width and height fields yet (0x1C, 0x1E)
                scale.m_x = 0.1f * *reinterpret_cast<u16*>(tex + 0x1C);
                scale.m_y = 0.1f * *reinterpret_cast<u16*>(tex + 0x1E);
                scale.m_z = 1.0f;
                tyFigListData* fig = m_figMng->getFigDataId(figId);
                // HYPOTHESIS: the scale of the trophy card is at 0x58 and 0x5C of the trophy record
                scale.m_x *= *reinterpret_cast<float*>(&fig->unk50[8]);
                scale.m_y *= *reinterpret_cast<float*>(&fig->unk50[12]);
                m_objs[19]->setObjScale(&scale);
            }
            m_objs[1]->setFrameFlg(0.0f, 2);
            break;
        }
        case 2: {
            m_groups[0]->PushBack(m_objs[21]->m_scnMdl, "pos01");
            gfPadStatus pad;
            g_gfPadSystem->getSysPadStatus(m_activePad, &pad);
            // HYPOTHESIS: the frame of the panel shows the button that plays the CD on the type of controller
            MuObject* panel = m_objs[1];
            float frame;
            if (pad.m_error != 0) {
                frame = 0.0f;
            } else {
                switch (pad.m_controllerType) {
                case gfPadType::GCC:
                    frame = 1.0f;
                    break;
                case gfPadType::WII_CLASSIC:
                    frame = 4.0f;
                    break;
                case gfPadType::WIIMOTE:
                    frame = 3.0f;
                    break;
                case gfPadType::NUNCHUK:
                    frame = 2.0f;
                    break;
                default:
                    frame = 1.0f;
                    break;
                }
            }
            if (frame >= 0.0f) {
                panel->setFrameFlg(frame, 2);
            }
            // the CD that is playing is marked
            muClearGetData* cd = &m_dataMgr.m_list[m_column + m_row * 4];
            if (g_sndSystem->getBGMId() == cd->m_reward) {
                m_objs[1]->setFrameTex(1.0f);
            } else {
                m_objs[1]->setFrameTex(0.0f);
            }
            break;
        }
        case 3:
            m_groups[0]->PushBack(m_objs[22]->m_scnMdl, "pos01");
            m_objs[1]->setFrameFlg(0.0f, 2);
            break;
        case 4:
            m_groups[0]->PushBack(m_objs[23]->m_scnMdl, "pos01");
            m_objs[1]->setFrameFlg(0.0f, 2);
            break;
        case 5:
            m_groups[0]->PushBack(m_objs[24]->m_scnMdl, "pos01");
            m_objs[1]->setFrameFlg(0.0f, 2);
            break;
        }
    } else {
        m_objs[1]->setFrameFlg(0.0f, 2);
    }
    if (state == 2) {
        m_objs[5]->setFrameTex(1.0f + static_cast<float>(data->m_kind));
    } else {
        m_objs[5]->setFrameTex(0.0f);
    }
    // the text of the prize, or the hint while its condition is not met
    u32 infoMsg = data->m_infoMsg;
    switch (data->m_hintKind) {
    case 1:
        if (data->m_hintArg == 0) {
            if (!g_GameGlobal->checkHideMenuBossBattle()) {
                infoMsg = data->m_hintMsg;
            }
        } else if (data->m_hintArg == 1) {
            if (!g_GameGlobal->checkHideMenuAllstar()) {
                infoMsg = data->m_hintMsg;
            }
        }
        break;
    case 2: {
        u32* flags = g_GameGlobal->getCharaAppFlag();
        int bit = data->m_hintArg * 3 + 1;
        if (!(flags[bit / 32] & (1 << (bit % 32)))) {
            infoMsg = data->m_hintMsg;
        }
        break;
    }
    case 3: {
        u32* flags = g_GameGlobal->getStageAppFlag();
        int bit = data->m_hintArg;
        if (!(flags[bit / 32] & (1 << (bit % 32)))) {
            infoMsg = data->m_hintMsg;
        }
        break;
    }
    case 4:
        if (!g_GameGlobal->isGlobalRecordEventOpened(0, data->m_hintArg - 1)) {
            infoMsg = data->m_hintMsg;
        }
        break;
    }
    switch (state) {
    case 0:
        m_msg->setFontColor(0, 0x8E, 0x8E, 0x8D, 0xFF);
        m_msg->printf(0, "?");
        m_msg->setFontColor(1, 0x8A, 0x98, 0x8E, 0xFF);
        m_msg->printf(1, "?");
        break;
    case 1:
        m_msg->setFontColor(0, 0x8E, 0x8E, 0x8D, 0xFF);
        m_msg->printf(0, "?");
        m_msg->setFontColor(1, 0x8A, 0x98, 0x8E, 0xFF);
        m_msg->printIndex(1, infoMsg, NULL);
        break;
    case 2:
        m_msg->setFontColor(0, 0, 0, 0, 0xFF);
        m_msg->printIndex(0, data->m_nameMsg, NULL);
        m_msg->setFontColor(1, 0xD9, 0xD9, 0xD9, 0xFF);
        m_msg->printIndex(1, infoMsg, NULL);
        break;
    }
}

void muClearGetTask::releaseHammer(int mode) {
    u32 index;
    char name[8];
    m_releaseMode = mode;
    m_hammerCancelling = true;
    m_stateFrame = 0;
    m_releaseBox = m_column + m_row * 4;
    switch (mode) {
    case 0:
        m_cursorOnGrid = false;
        index = m_hammerSel;
        break;
    case 2:
        index = m_hammerSel;
        m_objs[7 + m_hammerSel]->setAnimName(lbl_804569E0, true);
        m_objs[7 + m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
        m_objs[7 + m_hammerSel]->setFrameFlg(0.0f, 0x11);
        break;
    case 1:
        index = m_hammerNum - 1;
        m_objs[6 + m_hammerNum]->setAnimName(lbl_804569E0 + 2, true);
        break;
    }
    sprintf(name, "pos%02d", m_releaseBox);
    m_groups[1]->PushBack(m_objs[7 + index]->m_scnMdl, name);
    m_objs[7 + index]->setTrans(&m_hammerPos);
}

u32 muClearGetTask::setBreakBoxId(u32 id) {
    if (m_dataMgr.m_openState[id] != 2) {
        m_breakQueue[m_breakCount] = id;
        if (m_breakCount == 0 && m_state >= 2) {
            m_state = 6;
            setPosRot(id);
            m_column = id % 4;
            m_row = id / 4;
        }
        ++m_breakCount;
    }
    return m_breakCount;
}

void muClearGetTask::setPosRot(u32 id) {
    u32 row = id / 4;
    u32 count = 0;
    u32 position = m_scrollRow;
    if (row < m_row) {
        for (u32 current = m_row - 1; current >= row && current != 0; --current) {
            if (position > 3 && position > current + 1) {
                --position;
                ++count;
            }
        }
        m_scrollAngle -= 9.1f * count;
        m_scrollRow -= count;
    } else {
        for (u32 current = m_row + 1; current <= row; ++current) {
            if (position < 28 && current > position + 1) {
                ++position;
                ++count;
            }
        }
        m_scrollAngle += 9.1f * count;
        m_scrollRow += count;
    }
    if (count != 0) {
        m_scrolling = true;
    }
}

void muClearGetTask::setPrizeModel(u32 id, int state) {
    char name[8];
    sprintf(name, "pos%02d", id);
    if (state != 0) {
        switch (m_dataMgr.m_list[id].m_kind) {
        case 0:
            m_groups[1]->PushBack(m_groups[4 + id], name);
            break;
        case 1:
            m_groups[1]->PushBack(m_groups[4 + id], name);
            if (state == 1) {
                m_objs[0x219 + id]->setFrameFlg(1.0f, 0x10);
            } else {
                m_objs[0x219 + id]->setFrameFlg(0.0f, 0x10);
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            m_groups[1]->PushBack(m_objs[0x219 + id]->m_scnMdl, name);
            break;
        }
        for (u32 index = 0; index < 5; ++index) {
            if (id == m_hammerBox[index]) {
                if (state == 1) {
                    m_groups[1]->PushBack(m_objs[12 + index]->m_scnMdl, name);
                } else {
                    m_groups[1]->Remove(m_objs[12 + index]->m_scnMdl);
                }
                break;
            }
        }
    }
}

bool muClearGetTask::calcHammerClose() {
    char name[8];
    u32 index = m_hammerSel + 1;
    sprintf(name, "pos%02d", index + 94);
    Vec3f current;
    Vec3f target;
    target = m_objs[3]->getGlobalPosition(name);
    current = m_objs[7 + index]->getGlobalPosition();
    float distance = (target.m_x - current.m_x) / 6.0f;
    if (static_cast<float>(__fabs(target.m_x - (current.m_x + distance))) < 0.1f) {
        for (; index < m_hammerNum; ++index) {
            m_groups[2]->Remove(m_objs[7 + index]->m_scnMdl);
        }
        m_hammerSel = 0;
        --m_hammerNum;
        for (index = 0; index < m_hammerNum; ++index) {
            m_objs[7 + index]->setTrans(&m_hammerPos);
            sprintf(name, "pos%02d", index + 95);
            m_groups[2]->PushBack(m_objs[7 + index]->m_scnMdl, name);
        }
        return false;
    }
    for (; index < m_hammerNum; ++index) {
        Vec3f translation;
        translation = m_objs[7 + index]->m_trans;
        translation.m_x += distance;
        m_objs[7 + index]->setTrans(&translation);
    }
    return true;
}
