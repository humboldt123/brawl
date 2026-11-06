#define MT_VEC3F_SCALE_FRESH_REGS
#include <mu/mu_clear_get.h>
#include <gf/gf_heap_manager.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_restex.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

// Defined by the first part of the unit (mu_clear_get.cpp).
extern MuAnimNameData lbl_804566BC[];
extern MuAnimNameData lbl_8045676C[];
extern MuAnimNameData lbl_80456860[];
extern MuAnimNameData lbl_80456918[];
extern MuAnimNameData lbl_80456944[];
extern MuAnimNameData lbl_804569E0[];

// HYPOTHESIS: the result of AnmObjTexPatRes::GetResult (nw4r TexPatAnmResult): which textures the animation shows now.
struct ClearGetTexPatResult {
    u8 hasTex;
    u8 hasPltt;
    nw4r::g3d::ResTex tex[8];
    nw4r::g3d::ResTex pltt[8];
};

// HYPOTHESIS: the offset of the hammer turned around the axis, as a value (the original has a helper like this: its result
// is a temporary of the caller).
static inline Vec3f rotateOffset(const Vec3f& offset, const Vec3f& axis, float angle) {
    Vec3f out;
    offset.rot(&axis, angle, &out);
    return out;
}

// The hammer that was released flies back to its place in the row of hammers (releaseHammer sets the mode):
// mode 0 and 2 put a hammer back in 4 frames, mode 1 (the new hammer) waits 30 frames and then moves in 6.
// Returns false once the hammer is back in the row.
bool muClearGetTask::calcHammerCancel() {
    char name[8];
    Vec3f boxPos;
    Vec3f rowPos;
    Vec3f pos;
    Vec3f rotation;
    Vec3f axis(0.0f, 1.0f, 0.0f);
    float end;
    float start = 0.0f;
    float span;
    int hammer;
    switch (m_releaseMode) {
    case 0:
        end = 4.0f;
        hammer = m_hammerSel;
        span = end;
        break;
    case 1:
        end = 36.0f;
        start = 30.0f;
        hammer = m_hammerNum - 1;
        span = 6.0f;
        break;
    case 2:
        end = 4.0f;
        hammer = m_hammerSel;
        span = end;
        break;
    }
    if (static_cast<float>(static_cast<u32>(m_stateFrame)) >= end) {
        switch (m_releaseMode) {
        case 0:
            m_hammers[m_hammerSel]->setAnimName(lbl_804569E0, true);
            m_hammers[m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
            m_hammers[m_hammerSel]->setFrameFlg(10.0f, 0x11);
            break;
        case 2:
            break;
        }
        m_hammerHeld = false;
        m_menuRoot->scene->Remove(m_hammers[hammer]->m_scnMdl);
        sprintf(name, "pos%02d", hammer + 95);
        m_groups[2]->PushBack(m_hammers[hammer]->m_scnMdl, name);
        m_hammers[hammer]->setTrans(&m_hammerPos);
        pos.m_x = pos.m_y = pos.m_z = 0.0f;
        m_hammers[hammer]->setObjRotate(&pos);
        setCursor();
        return false;
    }
    if (static_cast<float>(static_cast<u32>(m_stateFrame)) >= start) {
        if (static_cast<float>(static_cast<u32>(m_stateFrame)) == start || start == 0.0f) {
            // The hammer is taken out of the row and put in the scene at the box it was released at.
            sprintf(name, "pos%02d", m_releaseBox);
            pos = m_objs[2]->getGlobalPosition(name);
            sprintf(name, "box%d", m_row);
            m_objs[2]->getRotate(&rotation, name);
            float yaw = rotation.m_y + m_scrollAngle;
            rotation.m_z = 0.017453292f * rotation.m_z;
            rotation.m_y = 0.017453292f * yaw;
            rotation.m_x = 0.017453292f * rotation.m_x;
            pos = rotateOffset(m_hammerPos, axis, rotation.m_y) + pos;
            m_hammers[hammer]->setTrans(&pos);
            m_hammers[hammer]->setObjRotate(&rotation);
            m_groups[1]->Remove(m_hammers[hammer]->m_scnMdl);
            m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_hammers[hammer]->m_scnMdl);
        }
        sprintf(name, "pos%02d", hammer + 95);
        rowPos = m_objs[3]->getGlobalPosition(name);
        sprintf(name, "pos%02d", m_releaseBox);
        boxPos = m_objs[2]->getGlobalPosition(name);
        sprintf(name, "box%d", m_row);
        m_objs[2]->getRotate(&rotation, name);
        float yaw = rotation.m_y + m_scrollAngle;
        rotation.m_z = 0.017453292f * rotation.m_z;
        rotation.m_y = 0.017453292f * yaw;
        rotation.m_x = 0.017453292f * rotation.m_x;
        boxPos = rotateOffset(m_hammerPos, axis, rotation.m_y) + boxPos - m_hammerPos;
        pos = rowPos - boxPos;
        Vec3fScale(&pos, &pos, (static_cast<float>(static_cast<u32>(m_stateFrame)) - start) / span);
        Vec3fAdd(&pos, &pos, &boxPos);
        Vec3fAdd(&pos, &pos, &m_hammerPos);
        m_hammers[hammer]->setTrans(&pos);
        Vec3fScale(&rotation, &rotation, (start + (span - static_cast<float>(static_cast<u32>(m_stateFrame)))) / span);
        m_hammers[hammer]->setObjRotate(&rotation);
    }
    return true;
}

// Makes the objects and the scene groups of the screen, puts the boxes with their prizes on the grid, sets the first
// animations and prints the fixed texts. Called by processAnim once the data and the file are there.
void muClearGetTask::initMdlData() {
    u32 size;
    switch (m_mode) {
    case 0:
        m_state = State_Intro;
        break;
    case 1:
        // the boxes of the queue are broken at the start: go to the first of them
        m_state = State_Move;
        setPosRot(m_breakQueue[0]);
        m_column = m_breakQueue[0] & 3;
        m_row = m_breakQueue[0] >> 2;
        m_stateFrame = 0;
        break;
    }
    m_bg = MuObject::create(&m_resFile, "MenClearBg_TopN", 0x10, NULL, Heaps::MenuInstance);
    m_panelTop = MuObject::create(&m_resFile, "MenClearPanel0000_TopN", 0x13, NULL, Heaps::MenuInstance);
    m_panelGrid = MuObject::create(&m_resFile, "MenClearPanel0001_TopN", 0x13, NULL, Heaps::MenuInstance);
    m_panelHammer = MuObject::create(&m_resFile, "MenClearPanel0002_TopN", 0x13, NULL, Heaps::MenuInstance);
    m_cursor = MuObject::create(&m_resFile, "MenClearCursor_TopN", 0x15, NULL, Heaps::MenuInstance);
    m_kindObj = MuObject::create(&m_resFile, "MenClearKind_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_getObj = MuObject::create(&m_resFile, "MenClearGet_TopN", 0x17, NULL, Heaps::MenuInstance);
    m_notice = MuObject::create(&m_resFile, "MenClearNotice_TopN", 0x17, NULL, Heaps::MenuInstance);
    for (u32 i = 0; i < HammerMax; i++) {
        m_hammers[i] = MuObject::create(&m_resFile, "MenClearHammer_TopN", 0x16, NULL, Heaps::MenuInstance);
        m_hammerMarks[i] = MuObject::create(&m_resFile, "MenClearHammer_TopN", 0x16, NULL, Heaps::MenuInstance);
    }
    m_previews[0] = MuObject::create(&m_resFile, "MenClearPrize0000_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_previews[1] = MuObject::create(&m_resFile, "MenClearPrize0001_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_previews[2] = MuObject::create(&m_resFile, "MenClearPrize0011_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_previews[3] = MuObject::create(&m_resFile, "MenClearPrize0020_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_previews[4] = MuObject::create(&m_resFile, "MenClearPrize0030_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_previews[5] = MuObject::create(&m_resFile, "MenClearPrize0040_TopN", 0x14, NULL, Heaps::MenuInstance);
    m_previews[6] = MuObject::create(&m_resFile, "MenClearPrize0050_TopN", 0x14, NULL, Heaps::MenuInstance);
    // the box of every prize, and the models that show what is behind it
    for (u32 i = 0; i < BoxNum; i++) {
        muClearGetData* data = &m_dataMgr.m_list[i];
        m_boxA[i] = MuObject::create(&m_resFile, "MenClearBox0000_TopN", 0x12, NULL, Heaps::MenuInstance);
        m_boxB[i] = MuObject::create(&m_resFile, "MenClearBox0001_TopN", 0x12, NULL, Heaps::MenuInstance);
        m_boxC[i] = MuObject::create(&m_resFile, "MenClearBox0002_TopN", 0x12, NULL, Heaps::MenuInstance);
        switch (data->m_kind) {
        case 0:
            m_prizeFrame[i] = MuObject::create(&m_resFile, "MenClearPrize0010_TopN", 0x11, NULL, Heaps::MenuInstance);
            m_prizeItem[i] = MuObject::create(&m_resFile, "MenClearPrize0011_TopN", 0x11, NULL, Heaps::MenuInstance);
            break;
        case 1:
            m_prizeFrame[i] = MuObject::create(&m_resFile, "MenClearPrize0000_TopN", 0x11, NULL, Heaps::MenuInstance);
            m_prizeItem[i] = MuObject::create(&m_resFile, "MenClearPrize0001_TopN", 0x11, NULL, Heaps::MenuInstance);
            break;
        case 2:
            m_prizeItem[i] = MuObject::create(&m_resFile, "MenClearPrize0020_TopN", 0x11, NULL, Heaps::MenuInstance);
            break;
        case 3:
            m_prizeItem[i] = MuObject::create(&m_resFile, "MenClearPrize0030_TopN", 0x11, NULL, Heaps::MenuInstance);
            break;
        case 4:
            m_prizeItem[i] = MuObject::create(&m_resFile, "MenClearPrize0040_TopN", 0x11, NULL, Heaps::MenuInstance);
            break;
        case 5:
            m_prizeItem[i] = MuObject::create(&m_resFile, "MenClearPrize0050_TopN", 0x11, NULL, Heaps::MenuInstance);
            break;
        }
    }
    // the scene groups: the preview and the kind on the text window, the grid, the hammers, the trophy
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    m_previewGroup = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 4, m_panelTop->m_scnMdl);
    m_previewGroup->PushBack(m_kindObj->m_scnMdl, "pos02");
    m_gridGroup = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 0x182, m_panelGrid->m_scnMdl);
    m_previewGroup->PushBack(m_gridGroup, "pos00");
    m_hammerGroup = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 6, m_panelHammer->m_scnMdl);
    m_previewGroup->PushBack(m_hammerGroup, "pos00");
    m_figureGroup = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 1, m_previews[0]->m_scnMdl);
    m_figureGroup->PushBack(m_previews[1]->m_scnMdl, "pos00");
    for (u32 i = 0; i < BoxNum; i++) {
        int kind = m_dataMgr.m_list[i].m_kind;
        if (kind < 2 && kind >= 0) {
            m_prizeGroups[i] = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 1, m_prizeFrame[i]->m_scnMdl);
            m_prizeGroups[i]->PushBack(m_prizeItem[i]->m_scnMdl, "pos00");
        }
    }
    // the hammers that are left sit in the row of hammers
    for (u32 i = 0; i < m_hammerNum; i++) {
        char name[8];
        sprintf(name, "pos%02d", i + 95);
        m_hammerGroup->PushBack(m_hammers[i]->m_scnMdl, name);
        m_hammers[i]->setTrans(&m_hammerPos);
    }
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_bg->m_scnMdl);
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_notice->m_scnMdl);
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_getObj->m_scnMdl);
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_previewGroup);
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_gridGroup);
    // the first animations
    m_panelTop->setAnimName(lbl_804566BC, true);
    m_panelTop->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_panelTop->changeAnimN(0, 4, false, false);
    if (m_state != State_Intro) {
        m_panelTop->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        m_panelTop->setFrameNode(9.0f);
    }
    Vec3f zero;
    zero.m_x = zero.m_y = zero.m_z = 0.0f;
    m_panelGrid->setObjRotate(&zero);
    m_cursor->setAnimName(lbl_8045676C, true);
    m_cursor->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    if (m_mode == 1) {
        m_cursor->setFrameVisible(2.0f);
    }
    m_kindObj->setAnimName(lbl_80456918, true);
    m_kindObj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    m_previews[2]->setAnimName(lbl_80456860 + 3, true);
    m_previews[2]->setUpdateRateFlg(0.0f, 4);
    m_notice->setAnimName(lbl_80456944, true);
    m_notice->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    m_notice->setFrameMatCol(static_cast<float>(static_cast<u32>(m_yesNo)));
    u32 hammers = 0;
    for (u32 i = 0; i < BoxNum; i++) {
        switch (m_dataMgr.m_list[i].m_kind) {
        case 0: {
            m_prizeFrame[i]->setAnimName(lbl_80456860 + 2, true);
            m_prizeItem[i]->setAnimName(lbl_80456860 + 3, true);
            m_prizeItem[i]->setUpdateRateFlg(0.0f, 4);
            m_prizeItem[i]->setFrameFlg(static_cast<float>(m_dataMgr.m_list[i].m_reward), 4);
            nw4r::g3d::AnmObjTexPatRes* texPat = m_prizeItem[i]->m_modelAnim->m_anmObjTexPatRes;
            ClearGetTexPatResult result;
            ClearGetTexPatResult* shown = reinterpret_cast<ClearGetTexPatResult*>(texPat->GetResult(reinterpret_cast<int*>(&result), 0));
            u8* tex = reinterpret_cast<u8*>(shown->tex[0].ptr());
            Vec3f scale;
            // BYTES: the texture header has no named width and height fields yet (0x1C, 0x1E)
            scale.m_x = 0.1f * *reinterpret_cast<u16*>(tex + 0x1C);
            scale.m_y = 0.1f * *reinterpret_cast<u16*>(tex + 0x1E);
            scale.m_z = 1.0f;
            m_prizeItem[i]->setObjScale(&scale);
            break;
        }
        case 1: {
            m_prizeFrame[i]->setAnimName(lbl_80456860, true);
            m_prizeItem[i]->setAnimName(lbl_80456860 + 1, true);
            m_prizeItem[i]->setUpdateRateFlg(0.0f, 0x10);
            u16 figId = m_dataMgr.m_list[i].m_reward;
            s32 texFile = m_figMng->getBilTexFileIdxFigId(figId);
            char texName[32];
            m_figMng->getBilTexNameFigId(figId, texName);
            nw4r::g3d::ResFile* figRes = &m_figTexRes[texFile];
            nw4r::g3d::ResTex figTex = figRes->GetResTex(texName);
            if (figTex.IsValid()) {
                MuObject* card = m_prizeItem[i];
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
                m_prizeItem[i]->setObjScale(&scale);
            }
            break;
        }
        case 2:
            m_prizeItem[i]->setAnimName(lbl_80456860 + 4, true);
            break;
        case 3:
            m_prizeItem[i]->setAnimName(lbl_80456860 + 5, true);
            break;
        case 4:
            m_prizeItem[i]->setAnimName(lbl_80456860 + 6, true);
            break;
        case 5:
            m_prizeItem[i]->setAnimName(lbl_80456860 + 7, true);
            break;
        }
        if (m_dataMgr.m_list[i].m_hasHammer) {
            m_hammerBox[hammers++] = i;
        }
        // the box that matches the state of the prize (closed, locked or opened) goes on the grid
        char name[8];
        MuObject* boxes[9] = { m_boxA[i], m_boxB[i], m_boxC[i], m_boxB[i], m_boxA[i], m_boxC[i], m_boxC[i], m_boxB[i], m_boxA[i] };
        sprintf(name, "pos%02d", i);
        int state = m_dataMgr.m_openState[i];
        m_gridGroup->PushBack(boxes[state * 3]->m_scnMdl, name);
        setPrizeModel(i, state);
    }
    setCursor();
    // the text window: line 0 the title, line 1 the text
    m_msg->attachScnMdlSimple(0, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_panelTop->m_sceneModel), 0, 0.0625f);
    m_msg->setAlignMode(0, MuMsg::Align_Center);
    m_msg->setFontWidthModeAuto(0);
    m_msg->attachScnMdlSimple(1, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_panelTop->m_sceneModel), 1, 0.0625f);
    m_msg->setAlignMode(1, MuMsg::Align_Center);
    m_msg->setFontWidthModeAuto(1);
    m_msg->getWScale(1, 48.0f);
    printPrizeData(3);
    // the yes / no window: three lines
    for (int i = 0; i < 3; i++) {
        m_msg->attachScnMdlSimple(i + 2, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_notice->m_sceneModel), i, 0.0625f);
        m_msg->setAlignMode(i + 2, MuMsg::Align_Center);
        m_msg->setFontColor(i + 2, 0, 0, 0, 0xFF);
        m_msg->setFontWidthModeAuto(i + 2);
    }
    m_msg->printIndex(2, 0x180, NULL);
    m_msg->printIndex(3, 0x181, NULL);
    m_msg->printIndex(4, 0x182, NULL);
}
