#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_common.h>
#include <gm/gm_hide_lib.h>
#include <gm/gm_syssave_task.h>
#include <mu/menu.h>
#include <mu/mu_clear_get.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <revolution/CX/CXCompression.h>
#include <revolution/OS/OSCache.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <types.h>
#include <ut/ut_relocate.h>

// HYPOTHESIS: the scene maker of the menus; its body is `new MenuRoot(name, scnSize, archive, index, Heaps::MenuResource)`.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int scnSize, gfArchive* archive, int index);

// The animations of the models (muClearGetTask::initMdlData and processAnim pick them by address). Each strings comes
// right before its table.
// panel (MenClearPanel0000_TopN): 0 -> 9 plays once when the screen comes in
MuAnimNameData lbl_804566BC[1] = {
    {0.0f, 9.0f, 0, "MenClearPanel0000_TopN__0", 3},
};
// boxes: [0] the closed box lights up (frame 16), [1] a box breaks (kind of box 0000), [2] a box breaks (box 0001)
MuAnimNameData lbl_80456718[3] = {
    {0.0f, 16.0f, 0, "MenClearBox0000_TopN__0", 0x13},
    {0.0f, 30.0f, 0, "MenClearBox0000_TopN__1", 0x13},
    {0.0f, 30.0f, 0, "MenClearBox0001_TopN__0", 0x13},
};
// cursor: loops
MuAnimNameData lbl_8045676C[1] = {
    {0.0f, 30.0f, 1, "MenClearCursor_TopN__0", 0x12},
};
// previews of the prizes: [0] and [1] the trophy, [2] and [3] the sticker, [4] CD, [5] stage, [6] and [7] hidden items
MuAnimNameData lbl_80456860[8] = {
    {0.0f, 180.0f, 1, "MenClearPrize0000_TopN__0", 1},
    {0.0f, 0.0f, 0, "MenClearPrize0001_TopN__0", 0x10},
    {0.0f, 180.0f, 1, "MenClearPrize0010_TopN__0", 1},
    {0.0f, 0.0f, 0, "MenClearPrize0011_TopN__0", 4},
    {0.0f, 180.0f, 1, "MenClearPrize0020_TopN__0", 1},
    {0.0f, 180.0f, 1, "MenClearPrize0030_TopN__0", 1},
    {0.0f, 180.0f, 1, "MenClearPrize0040_TopN__0", 1},
    {0.0f, 180.0f, 1, "MenClearPrize0050_TopN__0", 1},
};
// kind of the prize
MuAnimNameData lbl_80456918[1] = {
    {0.0f, 0.0f, 0, "MenClearKind_TopN__0", 4},
};
// yes / no window: [0] start, [1] opens, [2] closes
MuAnimNameData lbl_80456944[3] = {
    {0.0f, 0.0f, 0, "MenClearNotice_TopN__0", 0x10},
    {0.0f, 4.0f, 0, "MenClearNotice_TopN__0", 1},
    {5.0f, 9.0f, 0, "MenClearNotice_TopN__0", 1},
};
// hammers: [0] rests, [1] hits, [2] and [3] fly
MuAnimNameData lbl_804569E0[4] = {
    {0.0f, 10.0f, 0, "MenClearHammer_TopN__0", 0x11},
    {0.0f, 30.0f, 0, "MenClearHammer_TopN__1", 0x11},
    {0.0f, 19.0f, 0, "MenClearHammer_TopN__2", 0x11},
    {0.0f, 9.0f, 0, "MenClearHammer_TopN__3", 0x11},
};
// the text of the prize: [0] and [1] come in
MuAnimNameData lbl_80456A58[2] = {
    {0.0f, 9.0f, 0, "MenClearGet_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenClearGet_TopN__1", 1},
};

muClearGetTask::muClearGetTask(int mode, tyFigListDataManager* figMng, int decisionPad)
    : gfTask("muClearGetTask", Category_Info, 0xE, 6, true), m_state(0), m_stateFrame(0), m_activePad(g_GameGlobal->m_setRule->m_ctrlNamePlayerNo),
      m_decisionPad(decisionPad) {
    m_figMng = figMng;
    m_row = 0x10;
    m_column = 2;
    m_cursorOnGrid = true;
    m_yesNo = 1;
    m_scrollRow = 0x10;
    m_scrolling = false;
    m_breakCount = 0;
    m_scrollAngle = 0.0f;
    m_hammerSel = 0;
    m_hammerHeld = false;
    m_unk12D9 = false;
    m_hammerClosing = false;
    m_hammerCancelling = false;
    m_unk12EC = false;
    m_bgmId = 0;
    m_fileHandle.m_request = NULL;
    m_fileImage = NULL;
    m_mode = mode;
    if (m_decisionPad < 0 || m_decisionPad >= 8) {
        m_decisionPad = 0;
    }
    if (m_activePad < 0 || m_activePad >= 8) {
        m_activePad = 0;
    }
    fn_800B96E8(&m_controller, m_decisionPad & 0xFF);
    m_hammerNum = g_GameGlobal->m_record->unk1528[BoxNum];
    m_hammerPos.m_x = 0.0f;
    m_hammerPos.m_y = 0.0f;
    m_hammerPos.m_z = 8.0f;
    m_objTask = muObjProcessTask::create(Heaps::MenuInstance);
    m_objTask->m_objs = m_objs;
    m_objTask->m_count = ObjNum;
    for (int i = 0; i < ObjNum; i++) {
        m_objs[i] = NULL;
    }
    for (int i = 0; i < GroupNum; i++) {
        m_groups[i] = NULL;
    }
    m_resFile = nw4r::g3d::ResFile();
    for (int i = 0; i < FigTexNum; i++) {
        m_figTexRes[i] = nw4r::g3d::ResFile();
    }
    m_dataArchive = new (Heaps::MenuInstance) gfArchive();
    m_pacArchive = new (Heaps::MenuInstance) gfArchive();
    m_fileHandle.readRequest("/menu2/sc_clear_getter.pac", Heaps::MenuResource, 0, 0);
    m_soundGroup = g_sndSystem->loadSoundGroup(static_cast<SndGroupID>(0x72), 1, true);
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x400, 5);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
}


// HYPOTHESIS: MuObject +0x48 is the rotation that setObjRotate stores (mu_object.cpp calls it m_rot); the shared header only has padding there.
static inline Vec3f& getObjRotation(MuObject* object) {
    return *reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(object) + 0x48);
}

// The icon of the controller type that is shown on the panel (frame of its visibility animation).
static inline void showPadIcon(muClearGetTask* task) {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(task->m_activePad, &status);
    MuObject* panel = task->m_panelTop;
    float frame;
    if (status.m_error != 0) {
        frame = 0.0f;
    } else {
        switch (status.m_controllerType) {
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
}

// Puts the model of the box into the grid that its open state asks for (the three models of a box are boxA / boxB / boxC of
// the file; the state picks which one is on top: 0 closed, 1 hittable, 2 opened) and the preview of its prize.
// state 3 stands for the open state of the box that is in the data.
// MATCH-ONLY: the original processAnim was compiled with copy propagation off (see TIPS.md, "extsb ... neg ... or" entry);
// the pragma also keeps the literal argument 3 of showBox from being folded.
#pragma push
#pragma opt_propagation off
static inline void showBox(muClearGetTask* task, u32 box, int state) {
    MuObject* sets[3][3] = {
        {task->m_boxA[box], task->m_boxB[box], task->m_boxC[box]},
        {task->m_boxB[box], task->m_boxA[box], task->m_boxC[box]},
        {task->m_boxC[box], task->m_boxB[box], task->m_boxA[box]},
    };
    char name[8];
    sprintf(name, "pos%02d", box);
    if (state == 3) {
        state = task->m_dataMgr.m_openState[box];
    }
    task->m_gridGroup->Remove(sets[task->m_dataMgr.m_openState[box]][1]->m_scnMdl);
    task->m_gridGroup->Remove(sets[task->m_dataMgr.m_openState[box]][2]->m_scnMdl);
    task->m_gridGroup->PushBack(sets[task->m_dataMgr.m_openState[box]][0]->m_scnMdl, name);
    task->setPrizeModel(box, state);
}

// HYPOTHESIS: muClearGetTask::setBreakBoxId (0x800BCxxx, another unit of the split for now) is expanded inline here; call the
// member again when the units are merged.
static inline void appendBreakBox(muClearGetTask* task, u32 id) {
    if (task->m_dataMgr.m_openState[id] != 2) {
        task->m_breakQueue[task->m_breakCount] = id;
        if (task->m_breakCount == 0 && task->m_state >= 2) {
            task->m_state = 6;
            task->setPosRot(id);
            task->m_column = id % 4;
            task->m_row = id / 4;
        }
        ++task->m_breakCount;
    }
}

void muClearGetTask::processAnim() {
    gfPadStatus pad;
    char name[8];
    bool moved = false;
    if (m_state >= 2) {
        g_gfPadSystem->getSysPadStatus(m_decisionPad, &pad);
        pad.clamp();
        fn_800B9984(&m_controller);
        pad.m_buttonsPressedThisFrame2.bits |= m_controller.m_trigger;
        if (m_scrolling) {
            // The grid turns towards m_scrollAngle (degrees) in steps of a sixth of the rest.
            Vec3f& turn = getObjRotation(m_panelGrid);
            Vec3f rotation;
            rotation = turn;
            float current = rotation.m_y * 57.29578f;
            float step = (m_scrollAngle - current) / 6.0f;
            bool scrolling = true;
            if (static_cast<float>(__fabs(m_scrollAngle - (current + step))) < 0.1f) {
                scrolling = false;
                rotation.m_y = 0.017453292f * m_scrollAngle;
                rotation.m_x = 0.0f;
                rotation.m_z = 0.0f;
            } else {
                rotation.m_y = rotation.m_y + 0.017453292f * step;
            }
            m_panelGrid->setObjRotate(&rotation);
            m_scrolling = scrolling;
        }
        if (m_hammerClosing) {
            m_hammerClosing = calcHammerClose();
        }
        if (m_hammerCancelling) {
            m_hammerCancelling = calcHammerCancel();
        }
    }
    switch (m_state) {
    case State_Load: {
        if (!m_figMng->isLoadFinish()) {
            return;
        }
        if (!m_fileHandle.isReady()) {
            return;
        }
        void* compressed = m_fileHandle.getBuffer();
        int size = CXGetUncompressedSize(compressed);
        m_fileImage = gfHeapManager::alloc(Heaps::MenuResource, size);
        CXUncompressLZ(compressed, m_fileImage);
        DCFlushRange(m_fileImage, size);
        m_pacArchive->setFileImage(m_fileImage, size, Heaps::MenuResource);
        m_dataArchive->setFileImage(m_pacArchive->getData(Data_Type_Misc, 0, &size, 0xFFFE), size, Heaps::MenuResource);
        gfHeapManager::free(m_fileHandle.getBuffer());
        m_fileHandle.release();
        m_menuRoot = fn_800AFBFC("muClearGetTask", ObjNum, m_pacArchive, 2);
        CameraController::getInstance()->changeInput(CameraController::MenuFixed);
        m_resFile = nw4r::g3d::ResFile(m_dataArchive->getData(Data_Type_Model, 0, 0xFFFE));
        nw4r::g3d::ResFile::Init(&m_resFile);
        m_msg->setMsgData(m_dataArchive->getData(Data_Type_Misc, 0, 0xFFFE));
        int dataSize;
        m_dataMgr.setData(static_cast<u8*>(m_dataArchive->getData(Data_Type_Misc, 1, &dataSize, 0xFFFE)), dataSize);
        m_state = State_Init;
        break;
    }
    case State_Init: {
        for (u32 i = 0; i < BoxNum; i++) {
            if (m_dataMgr.m_list[i].m_kind == 1) {
                int index = m_figMng->getBilTexFileIdxFigId(m_dataMgr.m_list[i].m_reward & 0xFFFF);
                m_figTexRes[index] = nw4r::g3d::ResFile(m_pacArchive->getData(Data_Type_Misc, index * 10, 0xFFFE));
                nw4r::g3d::ResFile::Init(&m_figTexRes[index]);
            }
        }
        if (!g_sndSystem->isFinishLoad1(0x72)) {
            return;
        }
        initMdlData();
        g_gfApplication->unkD0.endKeepScreen();
        break;
    }
    case State_Intro:
        if (m_panelTop->m_modelAnim->getFrame() == 9.0f) {
            m_stateFrame = 0;
            m_state = State_Select;
        }
        break;
    case State_Select:
        if (m_panelTop->m_modelAnim->m_anmObjVisRes->GetFrame() > 0.0f) {
            showPadIcon(this);
        }
        if (!m_hammerCancelling) {
            if (pad.m_buttonsPressedThisFrame.bits & 0x200) {
                // B: put the hammer back, or leave the screen
                if (m_hammerHeld) {
                    releaseHammer(0);
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                } else {
                    g_gfApplication->unkD0.startKeepScreen();
                    m_state = State_End;
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    if (m_mode == 1) {
                        g_sndSystem->stopBGM(0);
                    }
                }
            } else if (pad.m_buttonsPressedThisFrame2.bits & 1) {
                // up
                if (m_cursorOnGrid) {
                    if (m_row != 0) {
                        setPosRot(m_column + (m_row - 1) * 4);
                        moved = true;
                        m_row = m_row - 1;
                    }
                } else if (m_hammerSel != 0) {
                    m_hammers[m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
                    m_hammers[m_hammerSel]->setFrameFlg(0.0f, 0x11);
                    m_hammerSel = m_hammerSel - 1;
                    m_hammers[m_hammerSel]->setAnimName(lbl_804569E0, true);
                    setCursor();
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            } else if (pad.m_buttonsPressedThisFrame2.bits & 2) {
                // down
                if (m_cursorOnGrid) {
                    if (m_row < 0x1F) {
                        setPosRot(m_column + (m_row + 1) * 4);
                        moved = true;
                        m_row = m_row + 1;
                    }
                } else if (m_hammerSel < m_hammerNum - 1) {
                    m_hammers[m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
                    m_hammers[m_hammerSel]->setFrameFlg(0.0f, 0x11);
                    m_hammerSel = m_hammerSel + 1;
                    m_hammers[m_hammerSel]->setAnimName(lbl_804569E0, true);
                    setCursor();
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            } else if (pad.m_buttonsPressedThisFrame2.bits & 8) {
                // left
                if (m_cursorOnGrid) {
                    if (m_column != 0) {
                        moved = true;
                        m_column = m_column - 1;
                    }
                } else {
                    m_hammers[m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
                    m_hammers[m_hammerSel]->setFrameFlg(0.0f, 0x11);
                    m_cursorOnGrid = true;
                    setCursor();
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            } else if (pad.m_buttonsPressedThisFrame2.bits & 4) {
                // right
                if (m_cursorOnGrid) {
                    if (m_column < 3) {
                        moved = true;
                        m_column = m_column + 1;
                    } else if (m_column == 3 && m_hammerNum != 0) {
                        if (m_hammerHeld) {
                            releaseHammer(0);
                            m_cursorOnGrid = false;
                        } else {
                            m_hammers[m_hammerSel]->setAnimName(lbl_804569E0, true);
                            m_cursorOnGrid = false;
                            setCursor();
                        }
                        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                    }
                }
            } else if (pad.m_buttonsPressedThisFrame.bits & 0x100) {
                // A
                u32 box = m_column + m_row * 4;
                int state = m_dataMgr.m_openState[box];
                if (!m_cursorOnGrid) {
                    // take the chosen hammer in hand
                    g_sndSystem->playSE(static_cast<SndID>(0x1ED3), -1, 0, 0, -1);
                    m_state = State_Grab;
                    m_stateFrame = 0;
                    m_cursorOnGrid = true;
                    setCursor();
                    sprintf(name, "pos%02d", m_hammerSel + 95);
                    Vec3f pos;
                    pos = m_panelHammer->getGlobalPosition(name);
                    Vec3fAdd(&pos, &pos, &m_hammerPos);
                    m_hammers[m_hammerSel]->setTrans(&pos);
                    m_hammerGroup->Remove(m_hammers[m_hammerSel]->m_scnMdl);
                    m_menuRoot->scene->Insert(m_menuRoot->scene->sceneItemsCount, m_hammers[m_hammerSel]->m_scnMdl);
                } else if (m_hammerHeld) {
                    if (state == 2) {
                        g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
                    } else if (m_dataMgr.m_list[box].m_openable == 0) {
                        g_sndSystem->playSE(static_cast<SndID>(0x1ED6), -1, 0, 0, -1);
                    } else {
                        g_sndSystem->playSE(static_cast<SndID>(0x1ED4), -1, 0, 0, -1);
                        m_state = State_Confirm;
                        m_notice->setAnimName(lbl_80456944 + 1, true);
                        m_yesNo = 1;
                        m_notice->setFrameMatCol(static_cast<float>(static_cast<u32>(m_yesNo)));
                        m_hammers[m_hammerSel]->setAnimName(lbl_804569E0 + 3, true);
                    }
                } else {
                    // no hammer in hand: a CD plays or stops
                    if (m_dataMgr.m_list[box].m_kind == 2 && state == 2) {
                        int bgm = m_dataMgr.m_list[box].m_reward;
                        if (bgm == g_sndSystem->getBGMId()) {
                            g_sndSystem->stopBGM(0);
                            m_bgmId = 0;
                            m_panelTop->setFrameTex(0.0f);
                        } else {
                            g_sndSystem->stopBGM(0);
                            m_bgmId = bgm;
                            g_sndSystem->playBGM(bgm, 0, true);
                            m_panelTop->setFrameTex(1.0f);
                        }
                    }
                }
            } else if (pad.m_buttonsPressedThisFrame.bits & 0x1000) {
                // START: stop the CD
                if (m_bgmId != 0) {
                    g_sndSystem->stopBGM(0x3C);
                    m_bgmId = 0;
                    m_panelTop->setFrameTex(0.0f);
                }
            }
            if (moved) {
                setCursor();
                printPrizeData(3);
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
        }
        break;
    case State_Grab:
        if (static_cast<float>(static_cast<u32>(m_stateFrame)) >= 4.0f) {
            m_state = State_Select;
            m_hammerHeld = true;
            m_menuRoot->scene->Remove(m_hammers[m_hammerSel]->m_scnMdl);
            sprintf(name, "pos%02d", m_column + m_row * 4);
            m_gridGroup->PushBack(m_hammers[m_hammerSel]->m_scnMdl, name);
            m_hammers[m_hammerSel]->setTrans(&m_hammerPos);
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            m_hammers[m_hammerSel]->setObjRotate(&rotation);
        } else {
            // The hammer flies from its place in the row to the box that was chosen.
            Vec3f axis(0.0f, 1.0f, 0.0f);
            sprintf(name, "pos%02d", m_column + m_row * 4);
            Vec3f boxPos;
            boxPos = m_panelGrid->getGlobalPosition(name);
            sprintf(name, "box%d", m_row);
            Vec3f rotation;
            m_panelGrid->getRotate(&rotation, name);
            rotation.m_z = 0.017453292f * rotation.m_z;
            rotation.m_y = 0.017453292f * (rotation.m_y + m_scrollAngle);
            rotation.m_x = 0.017453292f * rotation.m_x;
            Vec3f offset;
            m_hammerPos.rot(&axis, rotation.m_y, &offset);
            boxPos = offset + boxPos - m_hammerPos;
            sprintf(name, "pos%02d", m_hammerSel + 95);
            Vec3f rowPos;
            rowPos = m_panelHammer->getGlobalPosition(name);
            Vec3f pos;
            pos = boxPos - rowPos;
            Vec3fScale(&pos, &pos, static_cast<float>(static_cast<u32>(m_stateFrame)) * 0.25f);
            Vec3fAdd(&pos, &pos, &rowPos);
            Vec3fAdd(&pos, &pos, &m_hammerPos);
            m_hammers[m_hammerSel]->setTrans(&pos);
            Vec3fScale(&rotation, &rotation, static_cast<float>(static_cast<u32>(m_stateFrame)) * 0.25f);
            m_hammers[m_hammerSel]->setObjRotate(&rotation);
        }
        break;
    case State_Confirm: {
        bool no = false;
        bool yes = false;
        int move = 0;
        if (pad.m_buttonsPressedThisFrame.bits & 0x200) {
            no = true;
        } else if (pad.m_buttonsPressedThisFrame.bits & 0x100) {
            if (m_yesNo == 0) {
                yes = true;
            }
            if (static_cast<u32>(m_yesNo) == 1) {
                no = true;
            }
        } else if (pad.m_buttonsPressedThisFrame2.bits & 1) {
            move = -1;
        } else if (pad.m_buttonsPressedThisFrame2.bits & 2) {
            move = 1;
        }
        if (no) {
            m_state = State_Select;
            m_notice->setAnimName(lbl_80456944 + 2, true);
            m_hammers[m_hammerSel]->setAnimName(lbl_804569E0, true);
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        } else if (yes) {
            m_notice->setAnimName(lbl_80456944 + 2, true);
            m_activePad = muMenuController::getAButtonPushedControllerID();
            m_state = State_Hit;
            m_hammers[m_hammerSel]->setAnimName(lbl_804569E0 + 1, true);
            u32 last = m_hammerNum - 1;
            if (m_hammerSel != last) {
                m_hammerClosing = true;
            } else {
                m_hammerNum = last;
            }
        } else if (move != 0) {
            u32 before = m_yesNo;
            if (before == 0 && move == 1) {
                m_yesNo = 1;
            }
            if (static_cast<u32>(m_yesNo) == 1 && move == -1) {
                m_yesNo = 0;
            }
            if (before != m_yesNo) {
                m_notice->setFrameMatCol(static_cast<float>(static_cast<u32>(m_yesNo)));
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
        }
        break;
    }
    case State_Hit:
        if (m_hammers[m_hammerSel]->m_modelAnim->m_anmObjChrRes->GetFrame() == 17.0f) {
            u32 box = m_column + m_row * 4;
            appendBreakBox(this, box);
            box = m_breakQueue[0];
            int state = m_dataMgr.m_openState[box];
            m_state = State_Break;
            g_sndSystem->playSE(static_cast<SndID>(0x1ED5), -1, 0, 0, -1);
            muMenu::startRumbleController(m_decisionPad, 0x1E, -1);
            MuObject* boxObj = m_boxA[box];
            int priority = boxObj->m_scnMdl->m_priorityDrawOpa + 1;
            boxObj->m_scnMdl->SetPriorityDrawOpa(priority);
            boxObj->m_scnMdl->SetPriorityDrawXlu(priority);
            if (state == 0) {
                m_boxA[box]->setAnimName(lbl_80456718 + 1, true);
            } else {
                m_boxB[box]->setAnimName(lbl_80456718 + 2, true);
            }
            setPrizeModel(box, 2);
            if (box >= 4 && m_dataMgr.m_openState[box - 4] == 0) {
                m_boxA[box - 4]->setAnimName(lbl_80456718, true);
                setPrizeModel(box - 4, 1);
            }
            if (box + 4 < BoxNum && m_dataMgr.m_openState[box + 4] == 0) {
                m_boxA[box + 4]->setAnimName(lbl_80456718, true);
                setPrizeModel(box + 4, 1);
            }
        }
        break;
    case State_Break: {
        u32 box = m_breakQueue[0];
        if (m_hammerHeld && m_hammers[m_hammerSel]->isNodeAnimFinished()) {
            m_gridGroup->Remove(m_hammers[m_hammerSel]->m_scnMdl);
            m_hammers[m_hammerSel]->setAnimName(lbl_804569E0, true);
            m_hammers[m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
            m_hammers[m_hammerSel]->setFrameFlg(0.0f, 0x11);
            u32 last = m_hammerNum - 1;
            m_hammerHeld = false;
            if (m_hammerSel > last) {
                m_hammerSel = last;
            }
        }
        u32 boxIndex = 0x99 + box;
        if (m_dataMgr.m_openState[box] == 0) {
            boxIndex = 0x19 + box;
        }
        if (m_objs[boxIndex]->isNodeAnimFinished()) {
            gmActionOpenClearGet(box, m_dataMgr.m_list[box].m_kind, m_dataMgr.m_list[box].m_reward);
            m_dataMgr.setOpenState(box);
            MuObject* opened = m_boxA[m_breakQueue[0]];
            int priority = opened->m_scnMdl->m_priorityDrawOpa - 1;
            opened->m_scnMdl->SetPriorityDrawOpa(priority);
            opened->m_scnMdl->SetPriorityDrawXlu(priority);
            showBox(this, box, 3);
            if (box >= 4) {
                showBox(this, box - 4, 3);
            }
            if (box + 4 < BoxNum) {
                showBox(this, box + 4, 3);
            }
            printPrizeData(3);
            g_sndSystem->playSE(static_cast<SndID>(0x1ED7), -1, 0, 0, -1);
            m_getObj->setAnimName(lbl_80456A58, true);
            m_state = State_Get;
            if (m_dataMgr.m_list[box].m_hasHammer != 0) {
                m_hammerNum = m_hammerNum + 1;
                releaseHammer(1);
                g_sndSystem->playSE(static_cast<SndID>(0x1ED9), -1, 0, 0, -1);
            }
            g_GameGlobal->m_record->unk1528[BoxNum] = m_hammerNum;
            gmSysSaveTask::create(0x2E);
        }
        break;
    }
    case State_Move:
        if (!m_scrolling) {
            m_state = State_Break;
            int state = m_dataMgr.m_openState[m_breakQueue[0]];
            g_sndSystem->playSE(static_cast<SndID>(0x1ED5), -1, 0, 0, -1);
            muMenu::startRumbleController(m_decisionPad, 0x1E, -1);
            MuObject* boxObj = m_boxA[m_breakQueue[0]];
            int priority = boxObj->m_scnMdl->m_priorityDrawOpa + 1;
            boxObj->m_scnMdl->SetPriorityDrawOpa(priority);
            boxObj->m_scnMdl->SetPriorityDrawXlu(priority);
            if (state == 0) {
                m_boxA[m_breakQueue[0]]->setAnimName(lbl_80456718 + 1, true);
            } else {
                m_boxB[m_breakQueue[0]]->setAnimName(lbl_80456718 + 2, true);
            }
            setPrizeModel(m_breakQueue[0], 2);
            u32 box = m_breakQueue[0];
            if (box >= 4 && m_dataMgr.m_openState[box - 4] == 0) {
                m_boxA[box - 4]->setAnimName(lbl_80456718, true);
                setPrizeModel(m_breakQueue[0] - 4, 1);
            }
            u32 lower = m_breakQueue[0] + 4;
            if (lower < BoxNum && m_dataMgr.m_openState[lower] == 0) {
                m_boxA[m_breakQueue[0] + 4]->setAnimName(lbl_80456718, true);
                setPrizeModel(m_breakQueue[0] + 4, 1);
            }
        }
        break;
    case State_Get:
        if (m_getObj->isNodeAnimFinished()) {
            if (m_panelTop->m_modelAnim->m_anmObjVisRes->GetFrame() > 0.0f) {
                showPadIcon(this);
            }
            pad.m_buttonsPressedThisFrame.bits &= 0xFF7FFFFF;
            pad.m_buttonsPressedThisFrame2.bits &= 0xFF7FFFFF;
            if (pad.m_buttonsPressedThisFrame.bits != 0 || pad.m_buttonsPressedThisFrame2.bits != 0) {
                g_sndSystem->playSE(static_cast<SndID>(0x1ED8), -1, 0, 0, -1);
                m_getObj->setAnimName(lbl_80456A58 + 1, true);
                for (u32 i = 0; i < m_breakCount; i++) {
                    m_breakQueue[i] = m_breakQueue[i + 1];
                }
                m_breakCount = m_breakCount - 1;
                if (m_breakCount == 0) {
                    gmCheckHideCondition();
                    for (int i = 0; i < BoxNum; i++) {
                        if (!isClearGetterOpen(i)) {
                            u32* flags = reinterpret_cast<u32*>(g_GameGlobal->getClearGetAppFlag());
                            u32 isSet = (flags[i / 32] & (1 << (i % 32))) != 0;
                            if (isSet == 1) {
                                appendBreakBox(this, i);
                            }
                        }
                    }
                }
                if (m_breakCount == 0) {
                    m_state = State_Select;
                    m_cursor->setFrameVisible(0.0f);
                    setCursor();
                } else {
                    m_state = State_Move;
                    setPosRot(m_breakQueue[0]);
                    m_column = m_breakQueue[0] & 3;
                    m_row = m_breakQueue[0] >> 2;
                    printPrizeData(3);
                    m_stateFrame = 0;
                }
            }
        }
        break;
    }
    m_stateFrame = m_stateFrame + 1;
}
#pragma pop
