#include <types.h>

#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <stdio.h>

#include <mu/mu_soft_list.h>

// main.dol function with no name yet: gfArchive::readFileRequest(path, heap, offset, size, archive id) (see mu_figure_get)
extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);

// The panel: 0 comes in (frames 0 ... 9), 1 stands, 2 the frame of its visibility animation.
static MuAnimNameData sPanelAnims[3] = {
    { 0.0f, 9.0f, 0, "MenSoftPanel_TopN__0", 0x1 },
    { 0.0f, 30.0f, 1, "MenSoftPanel_TopN__0", 0x10 },
    { 0.0f, 0.0f, 0, "MenSoftPanel_TopN__0", 0x2 },
};
// The name of the console: 0 its colour animation, 1 the frame of the console (CHR, VIS and PAT).
static MuAnimNameData sIconAnims[2] = {
    { 0.0f, 30.0f, 1, "MenSoftIconE_TopN__0", 0x10 },
    { 0.0f, 0.0f, 0, "MenSoftIconE_TopN__0", 0x7 },
};
// A line of the list: the colour of the even and the odd lines.
static MuAnimNameData sTextAnim = { 0.0f, 0.0f, 0, "MenSoftText_TopN__0", 0x10 };
// The new mark: 0 its colour animation, 1 shown or not.
static MuAnimNameData sNewAnims[2] = {
    { 0.0f, 30.0f, 1, "MenSoftNew_TopN__0", 0x10 },
    { 0.0f, 0.0f, 0, "MenSoftNew_TopN__0", 0x12 },
};

muSoftListTask::muSoftListTask() : gfTask("muSoftListTask", Category_Info, 0xE, 6, true), m_step(Step_Load), m_frame(0) {
    m_category = 0;
    m_line = 0;
    m_topIndex = 0;
    m_isSecondLine = false;
    m_scroll = 0;
    m_seenNum = 0;
    unk2C_b1 = false;
    m_menuRoot = MenuRoot::create("muSoftListTask", 0x1B, "/menu/defaultcamera/CharacterSelect.brres");
    m_language = g_GameGlobal->getGlobalRecordMenuDatap()->m_language;
    muObjProcessTask* objProcessTask = muObjProcessTask::create(Heaps::MenuInstance);
    m_objProcessTask = objProcessTask;
    objProcessTask->m_objs = m_objs;
    objProcessTask->m_count = Obj_Num;
    for (int i = 0; i < Obj_Num; i++) {
        m_objs[i] = NULL;
    }
    for (int i = 0; i < 1 + LineCount; i++) {
        m_expands[i] = NULL;
    }
    m_isArchiveLoaded = false;
    m_isMenuRootLoaded = false;
    m_resFile = nw4r::g3d::ResFile();
    m_archive = new (Heaps::MenuResource) gfArchive();
    fn_800151FC(m_archive, "/menu/soft_list/Soft.pac", Heaps::MenuResource, 0, 0, 0xFF);
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x400, 0x18);
    fn_800B96E8(&m_controller, 0xF0);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
}


// Waits for the files and shows the list; then L and R change the console, the stick and the + Control Pad scroll it,
// and B leaves.
void muSoftListTask::processAnim() {
    bool isPrev = false;
    bool isNext = false;
    bool isCancel = false;
    for (int i = 0; i < 8; i++) {
        gfPadStatus pad;
        g_gfPadSystem->getSysPadStatus(i, &pad);
        switch (pad.m_controllerType) {
        case gfPadType::WIIMOTE:
            isPrev = isPrev || (pad.m_buttonsPressedThisFrame.bits & 0x400000);
            isNext = isNext || (pad.m_buttonsPressedThisFrame.bits & 0x200000);
            isCancel = isCancel || (pad.m_buttonsPressedThisFrame.bits & 0x10000);
            break;
        case gfPadType::NUNCHUK:
            isPrev = isPrev || (pad.m_buttonsPressedThisFrame.bits & 0x10000);
            isNext = isNext || (pad.m_buttonsPressedThisFrame.bits & 0x20000);
            isCancel = isCancel || (pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B);
            break;
        default:
            isPrev = isPrev || (pad.m_buttonsPressedThisFrame.bits & (GF_PAD_BUTTON_BIT_Y | GF_PAD_BUTTON_BIT_L));
            isNext = isNext || (pad.m_buttonsPressedThisFrame.bits & (GF_PAD_BUTTON_BIT_X | GF_PAD_BUTTON_BIT_R));
            isCancel = isCancel || (pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B);
            break;
        }
    }
    fn_800B9984(&m_controller);
    u32 trigger = m_controller.m_trigger;
    bool prev = isPrev || (trigger & 1);
    bool next = isNext || (trigger & 2);
    switch (m_step) {
    case Step_Load: {
        if (!m_isMenuRootLoaded) {
            if (!m_menuRoot->m_isLoaded) {
                return;
            }
            m_isMenuRootLoaded = true;
        }
        if (!m_isArchiveLoaded) {
            if (!m_archive->unk9) {
                return;
            }
            m_isArchiveLoaded = true;
        }
        m_resFile = nw4r::g3d::ResFile(m_archive->getData(Data_Type_Model, 0, 0xFFFE));
        nw4r::g3d::ResFile::Init(&m_resFile);
        m_msg->setMsgData(m_archive->getData(Data_Type_Misc, 0, 0xFFFE));
        int size;
        void* list = m_archive->getData(Data_Type_Misc, 1, &size, 0xFFFE);
        m_dataManager.setData(static_cast<u8*>(list), size);
        m_step = Step_Appear;
        initMdlData();
        g_gfApplication->unkD0.endKeepScreen();
        g_sndSystem->playBGM(0x270C, 0, true);
        lbl_805A01D8->m_category = 0x2D;
        break;
    }
    case Step_Appear:
        if (m_objs[Obj_Panel]->m_modelAnim->getFrame() == 9.0f) {
            m_frame = 0;
            m_step = Step_Main;
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            m_objs[Obj_Panel]->setAnimName(&sPanelAnims[1], true);
        }
        break;
    case Step_Main: {
        if (isCancel) {
            m_step = Step_End;
            g_gfApplication->unkD0.startKeepScreen();
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        } else if (prev) {
            if (m_category > 0) {
                m_category--;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                if (m_category == 2) {
                    m_category = 1;
                }
                m_line = 0;
                m_topIndex = 0;
                m_isSecondLine = false;
                m_lineNum = m_dataManager.m_categoryLines[m_category];
                m_softNum = m_dataManager.m_categoryCount[m_category];
                m_scroll = 0;
                m_objs[Obj_Icon]->setFrameFlg(m_category + 1, 7);
                m_msg->printf(0, "%s", muSoftListDataManager::sCategoryNames[muSoftListDataManager::CategoryCount + m_category]);
                printSoftData();
            }
        } else if (next) {
            if (m_category < 10) {
                m_category++;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                if (m_category == 2) {
                    m_category = 3;
                }
                m_line = 0;
                m_topIndex = 0;
                m_isSecondLine = false;
                m_lineNum = m_dataManager.m_categoryLines[m_category];
                m_softNum = m_dataManager.m_categoryCount[m_category];
                m_scroll = 0;
                m_objs[Obj_Icon]->setFrameFlg(m_category + 1, 7);
                m_msg->printf(0, "%s", muSoftListDataManager::sCategoryNames[muSoftListDataManager::CategoryCount + m_category]);
                printSoftData();
            }
        } else {
            gfPadStatus pad;
            g_gfPadSystem->getSysPadStatus(0xF0, &pad);
            pad.clamp();
            scrollText(&pad);
        }
        break;
    }
    }
    m_frame++;
}

// The titles whose new mark was shown are no longer new.
muSoftListTask::~muSoftListTask() {
    u32 i;
    for (i = 0; i < m_seenNum; i++) {
        m_dataManager.resetNewFlag(m_seen[i].m_category, m_seen[i].m_index);
    }
    for (i = 0; i < 1 + LineCount; i++) {
        if (m_expands[i] != NULL) {
            m_expands[i]->Destroy();
            m_expands[i] = NULL;
        }
    }
    for (u32 i = 0; i < Obj_Num; i++) {
        if (m_objs[i] != NULL) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    delete m_archive;
    delete m_msg;
    m_objProcessTask->exit();
    m_objProcessTask = NULL;
    m_menuRoot->exit();
    m_menuRoot = NULL;
}

bool muSoftListTask::isEnd() {
    return m_step >= Step_End;
}

// Prints the titles from m_topIndex on the lines of the list; a title that is new gets its mark and is remembered.
void muSoftListTask::printSoftData() {
    u32 index = m_topIndex;
    int line = 0;
    if (m_isSecondLine) {
        m_objs[Obj_Text]->setFrameMatCol(index & 1);
        muSoftListData* data = m_dataManager.getSoftListData(m_category, index);
        m_msg->printf(2, "");
        m_msg->printIndex(13, data->m_subTitleMsg2, NULL);
        line = 1;
        index++;
    }
    for (; line < LineCount; line++, index++) {
        u32 color = index & 1;
        m_objs[Obj_Text + line]->setFrameMatCol(color);
        if (index < m_softNum) {
            muSoftListData* data = m_dataManager.getSoftListData(m_category, index);
            if (m_dataManager.isNew(m_category, index)) {
                m_objs[Obj_New + line]->setFrameVisible(1.0f);
                u32 i;
                for (i = 0; i < m_seenNum; i++) {
                    if (m_category == m_seen[i].m_category && index == m_seen[i].m_index) {
                        break;
                    }
                }
                if (i == m_seenNum) {
                    m_seen[m_seenNum].m_category = m_category;
                    m_seen[m_seenNum].m_index = index;
                    m_seenNum++;
                }
            } else {
                m_objs[Obj_New + line]->setFrameVisible(0.0f);
            }
            m_msg->printf(line + 2, "%s", data->m_title);
            switch (data->m_subTitleLines) {
            case 0:
                m_msg->printIndex(line + 13, data->m_subTitleMsg, NULL);
                break;
            case 1: {
                char text[0x200];
                for (int i = 0; i < 0x200; i++) {
                    text[i] = 0;
                }
                int length;
                const char* msg = m_msg->getMsgData(data->m_subTitleMsg, &length);
                memcpy(text, msg, length);
                int length1 = length;
                msg = m_msg->getMsgData(data->m_subTitleMsg2, &length);
                memcpy(text + length1, msg, length);
                m_msg->printData(line + 13, text, length1 + length);
                break;
            }
            case 2:
                m_msg->printIndex(line + 13, data->m_subTitleMsg, NULL);
                line++;
                if (line < LineCount) {
                    m_objs[Obj_New + line]->setFrameVisible(0.0f);
                    m_objs[Obj_Text + line]->setFrameMatCol(color);
                    m_msg->printf(line + 2, "");
                    m_msg->printIndex(line + 13, data->m_subTitleMsg2, NULL);
                }
                break;
            }
        } else {
            m_msg->printf(line + 2, "");
            m_msg->printf(line + 13, "");
            m_objs[Obj_New + line]->setFrameVisible(0.0f);
        }
    }
}

// Scrolls the list with the stick and the + Control Pad. Returns false when the list is at its end (or too short to
// scroll).
bool muSoftListTask::scrollText(gfPadStatus* pad) {
    int speed = 0;
    bool canScroll = true;
    Vec3f bar; // the scale of the scroll bar, then its position
    if (m_lineNum < LineCount) {
        m_objs[Obj_Panel]->setFrameVisible(0.0f);
        printSoftData();
        bar.m_x = 1.0f;
        bar.m_y = 0.0f;
        bar.m_z = 1.0f;
        m_objs[Obj_Bar]->setObjScale(&bar);
        for (u32 i = 0; i < LineCount; i++) {
            Vec3f pos;
            m_objs[Obj_Text + i]->getPos(&pos, "MenSoftText_TopN");
            pos.m_y = 2.9f * (m_scroll % LineHeight) / 290.0f - 2.9f * i;
            m_objs[Obj_Text + i]->setTrans(&pos);
        }
        return false;
    }
    int stick = pad->m_stickY > -80 ? pad->m_stickY : -80;
    stick = stick < 80 ? stick : 80;
    stick = stick * 90 / 80;
    if (stick < -15) {
        speed = -1 - (stick + 15);
    } else if (stick > 15) {
        speed = 1 - (stick - 15);
    }
    if (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_DPAD_UP) {
        speed -= 64;
    }
    if (pad->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_DPAD_DOWN) {
        speed += 64;
    }
    if (static_cast<int>(m_scroll) < -speed) {
        m_scroll = 0;
    } else {
        m_scroll += speed;
    }
    u32 max = (m_lineNum - 10) * LineHeight;
    if (m_scroll >= max) {
        m_scroll = max;
        canScroll = false;
    }
    u32 line = m_scroll / LineHeight;
    if (m_line != line) {
        if (m_line < line) {
            if (m_dataManager.isSubTitleTwoLine(m_category, m_topIndex) && !m_isSecondLine) {
                m_isSecondLine = true;
            } else {
                m_isSecondLine = false;
                m_topIndex++;
            }
        } else {
            if (m_dataManager.isSubTitleTwoLine(m_category, m_topIndex) && m_isSecondLine) {
                m_isSecondLine = false;
            } else {
                m_topIndex--;
                if (m_dataManager.isSubTitleTwoLine(m_category, m_topIndex)) {
                    m_isSecondLine = true;
                }
            }
        }
        g_sndSystem->playSE(static_cast<SndID>(7), -1, 0, 0, -1);
        m_line = line;
        printSoftData();
    }
    if (m_scroll == 0) {
        m_objs[Obj_Panel]->setFrameVisible(1.0f);
    } else if (!canScroll) {
        m_objs[Obj_Panel]->setFrameVisible(2.0f);
    } else {
        m_objs[Obj_Panel]->setFrameVisible(3.0f);
    }
    for (u32 i = 0; i < LineCount; i++) {
        Vec3f pos;
        m_objs[Obj_Text + i]->getPos(&pos, "MenSoftText_TopN");
        pos.m_y = 2.9f * (m_scroll % LineHeight) / 290.0f - 2.9f * i;
        m_objs[Obj_Text + i]->setTrans(&pos);
    }
    bar.set(1.0f, 11.0f / (1.0f + m_lineNum), 1.0f);
    m_objs[Obj_Bar]->setObjScale(&bar);
    bar.set(0.0f, -(25.0f * m_scroll / (290.0f * (1.0f + m_lineNum))), 0.0f);
    m_objs[Obj_Bar]->setTrans(&bar);
    return canScroll;
}

// Creates the models of the list and shows the first console.
void muSoftListTask::initMdlData() {
    // BYTES: the Japanese text of the date ("2007 nen 12 gatsu genzai", UTF-8) is in the .rodata of the US build
    // without a reader. HYPOTHESIS: a local constant of the Japanese build whose use the US build dropped.
    static const char sDateJapanese[] = "2007\xE5\xB9\xB4 12\xE6\x9C\x88 \xE7\x8F\xBE\xE5\x9C\xA8";
    // HYPOTHESIS: both counts are the titles of the console here (changeCategory takes the lines for m_lineNum); the
    // first console has no subtitle of two lines, so they are the same.
    m_lineNum = m_dataManager.m_categoryCount[m_category];
    m_softNum = m_dataManager.m_categoryCount[m_category];
    m_objs[Obj_Bg] = MuObject::create(&m_resFile, "MenSoftBg_TopN", 0x10, NULL, Heaps::MenuInstance);
    m_objs[Obj_Panel] = MuObject::create(&m_resFile, "MenSoftPanel_TopN", 0x1A, NULL, Heaps::MenuInstance);
    m_objs[Obj_Icon] = MuObject::create(&m_resFile, "MenSoftIconE_TopN", 0x11, NULL, Heaps::MenuInstance);
    // The names of the animations of the icon are written again with the same text.
    sprintf(const_cast<char*>(sIconAnims[0].m_name), "MenSoftIconE_TopN__0");
    sprintf(const_cast<char*>(sIconAnims[1].m_name), "MenSoftIconE_TopN__0");
    m_objs[Obj_Bar] = MuObject::create(&m_resFile, "MenSoftBar_TopN", 0x1B, NULL, Heaps::MenuInstance);
    m_objs[Obj_Mask] = MuObject::create(&m_resFile, "MenSoftMask_TopN", 0x12, NULL, Heaps::MenuInstance);
    for (u32 i = 0; i < LineCount; i++) {
        m_objs[Obj_Text + i] = MuObject::create(&m_resFile, "MenSoftText_TopN", (i & 1) * 5 + 0x13, NULL, Heaps::MenuInstance);
        m_objs[Obj_New + i] = MuObject::create(&m_resFile, "MenSoftNew_TopN", (i & 1) * 5 + 0x14, NULL, Heaps::MenuInstance);
    }
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    u32 size;
    m_expands[0] = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 13, m_objs[Obj_Panel]->m_scnMdl);
    for (u32 i = 0; i < LineCount; i++) {
        m_expands[1 + i] = nw4r::g3d::ScnMdlExpand::Construct(allocator, &size, 1, m_objs[Obj_Text + i]->m_scnMdl);
        m_expands[1 + i]->PushBack(m_objs[Obj_New + i]->m_scnMdl, "pos00");
    }
    m_expands[0]->PushBack(m_objs[Obj_Icon]->m_scnMdl, "pos00");
    m_expands[0]->PushBack(m_objs[Obj_Bar]->m_scnMdl, "pos02");
    for (u32 i = 0; i < LineCount; i++) {
        m_expands[0]->PushBack(m_expands[1 + i], "pos01");
    }
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_expands[0]);
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_objs[Obj_Bg]->m_scnMdl);
    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_objs[Obj_Mask]->m_scnMdl);
    m_objs[Obj_Panel]->setAnimName(&sPanelAnims[2], true);
    m_objs[Obj_Panel]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[Obj_Panel]->setFrameVisible(2.0f);
    m_objs[Obj_Panel]->setAnimName(&sPanelAnims[0], true);
    m_objs[Obj_Icon]->setAnimName(&sIconAnims[1], true);
    m_objs[Obj_Icon]->setFrameFlg(1.0f, 7);
    m_objs[Obj_Icon]->setUpdateRateFlg(0.0f, 7);
    m_objs[Obj_Icon]->setAnimName(&sIconAnims[0], true);
    for (int i = 0; i < LineCount; i++) {
        m_objs[Obj_Text + i]->setAnimName(&sTextAnim, true);
        m_objs[Obj_Text + i]->setUpdateRateFlg(0.0f, 0x10);
        m_objs[Obj_New + i]->setAnimName(&sNewAnims[1], true);
        m_objs[Obj_New + i]->setUpdateRateFlg(0.0f, 2);
        m_objs[Obj_New + i]->setFrameFlg(0.0f, 2);
        m_objs[Obj_New + i]->setAnimName(&sNewAnims[0], true);
    }
    m_msg->attachScnMdlSimple(0, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Icon]->m_sceneModel), 0, 0.0625f);
    m_msg->setAlignMode(0, MuMsg::Align_Center);
    m_msg->setFontColor(0, 0xF9, 0xF9, 0xF9, 0xFF);
    m_msg->setFontWidthModeAuto(0);
    m_objs[Obj_Icon]->setFrameFlg(m_category + 1, 7);
    m_msg->printf(0, "%s", muSoftListDataManager::sCategoryNames[muSoftListDataManager::CategoryCount + m_category]);
    printSoftData();
    m_msg->attachScnMdlSimple(1, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Panel]->m_sceneModel), 0, 0.0625f);
    m_msg->setAlignMode(1, MuMsg::Align_Center);
    m_msg->setFontColor(1, 0x2F, 0x6B, 0x2D, 0xFF);
    m_msg->setFontWidthModeAuto(1);
    m_language = g_GameGlobal->getGlobalRecordMenuDatap()->m_language;
    m_msg->printf(1, "As of December 2007");
    for (int i = 0; i < LineCount; i++) {
        m_msg->attachScnMdlSimple(i + 2, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Text + i]->m_sceneModel), 0,
                                  1.0f / 19.0f);
        m_msg->setAlignMode(i + 2, MuMsg::Align_Left);
        m_msg->setFontColor(i + 2, 0xF5, 0xF5, 0xF5, 0xFF);
        m_msg->setFontWidthModeAuto(i + 2);
        m_msg->attachScnMdlSimple(i + 13, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Text + i]->m_sceneModel), 1,
                                  1.0f / 18.0f);
        m_msg->setAlignMode(i + 13, MuMsg::Align_Left);
        m_msg->setFontColor(i + 13, 0xFF, 0xFF, 0xFF, 0xFF);
        m_msg->setFontWidthModeAuto(i + 13);
    }
    printSoftData();
    for (u32 i = 0; i < LineCount; i++) {
        Vec3f pos;
        m_objs[Obj_Text + i]->getPos(&pos, "MenSoftText_TopN");
        pos.m_y = -2.9f * i;
        m_objs[Obj_Text + i]->setTrans(&pos);
    }
}
