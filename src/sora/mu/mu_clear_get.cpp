#define MT_VEC3F_SCALE_FRESH_REGS
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
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_restex.h>
#include <ty/ty_fig_listmng.h>

// HYPOTHESIS: the scene maker of the menus; its body is `new MenuRoot(name, scnSize, archive, index, Heaps::MenuResource)`.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int scnSize, gfArchive* archive, int index);

// HYPOTHESIS: the result of AnmObjTexPatRes::GetResult (nw4r TexPatAnmResult): which textures the animation shows now.
struct ClearGetTexPatResult {
    u8 hasTex;
    u8 hasPltt;
    nw4r::g3d::ResTex tex[8];
    nw4r::g3d::ResTex pltt[8];
};

// The animations of the models (muClearGetTask::initMdlData and processAnim pick them by address). The strings of a table
// come right before it. 
// panel (MenClearPanel0000_TopN): 0 -> 9 plays once when the screen comes in
static MuAnimNameData lbl_804566BC[1] = {
    {0.0f, 9.0f, 0, "MenClearPanel0000_TopN__0", 3},
};
// boxes: [0] the closed box lights up (frame 16), [1] a box breaks (kind of box 0000), [2] a box breaks (box 0001)
static MuAnimNameData lbl_80456718[3] = {
    {0.0f, 16.0f, 0, "MenClearBox0000_TopN__0", 0x13},
    {0.0f, 30.0f, 0, "MenClearBox0000_TopN__1", 0x13},
    {0.0f, 30.0f, 0, "MenClearBox0001_TopN__0", 0x13},
};
// cursor: loops
static MuAnimNameData lbl_8045676C[1] = {
    {0.0f, 30.0f, 1, "MenClearCursor_TopN__0", 0x12},
};
// previews of the prizes: [0] and [1] the trophy, [2] and [3] the sticker, [4] CD, [5] stage, [6] and [7] hidden items
static MuAnimNameData lbl_80456860[8] = {
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
static MuAnimNameData lbl_80456918[1] = {
    {0.0f, 0.0f, 0, "MenClearKind_TopN__0", 4},
};
// yes / no window: [0] start, [1] opens, [2] closes
static MuAnimNameData lbl_80456944[3] = {
    {0.0f, 0.0f, 0, "MenClearNotice_TopN__0", 0x10},
    {0.0f, 4.0f, 0, "MenClearNotice_TopN__0", 1},
    {5.0f, 9.0f, 0, "MenClearNotice_TopN__0", 1},
};
// hammers: [0] rests, [1] hits, [2] and [3] fly
static MuAnimNameData lbl_804569E0[4] = {
    {0.0f, 10.0f, 0, "MenClearHammer_TopN__0", 0x11},
    {0.0f, 30.0f, 0, "MenClearHammer_TopN__1", 0x11},
    {0.0f, 19.0f, 0, "MenClearHammer_TopN__2", 0x11},
    {0.0f, 9.0f, 0, "MenClearHammer_TopN__3", 0x11},
};
// the text of the prize: [0] and [1] come in
static MuAnimNameData lbl_80456A58[2] = {
    {0.0f, 9.0f, 0, "MenClearGet_TopN__0", 1},
    {0.0f, 9.0f, 0, "MenClearGet_TopN__1", 1},
};

// Starts loading sc_clear_getter.pac and the sound group; the screen is built by processAnim once both are there.
// mode 1: the boxes of the queue (m_breakQueue) are broken at once, mode 0: the panel comes in first.
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

// The state machine of the screen (see muClearGetTask::State). Cases are in the order of the original code, not of the numbers.
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
    case State_Grab: {
        Vec3f pos;
        if (static_cast<float>(static_cast<u32>(m_stateFrame)) >= 4.0f) {
            m_state = State_Select;
            m_hammerHeld = true;
            m_menuRoot->scene->Remove(m_hammers[m_hammerSel]->m_scnMdl);
            sprintf(name, "pos%02d", m_column + m_row * 4);
            m_gridGroup->PushBack(m_hammers[m_hammerSel]->m_scnMdl, name);
            m_hammers[m_hammerSel]->setTrans(&m_hammerPos);
            pos.m_z = 0.0f;
            pos.m_y = 0.0f;
            pos.m_x = 0.0f;
            m_hammers[m_hammerSel]->setObjRotate(&pos);
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
            pos = boxPos - rowPos;
            Vec3fScale(&pos, &pos, static_cast<float>(static_cast<u32>(m_stateFrame)) * 0.25f);
            Vec3fAdd(&pos, &pos, &rowPos);
            Vec3fAdd(&pos, &pos, &m_hammerPos);
            m_hammers[m_hammerSel]->setTrans(&pos);
            Vec3fScale(&rotation, &rotation, static_cast<float>(static_cast<u32>(m_stateFrame)) * 0.25f);
            m_hammers[m_hammerSel]->setObjRotate(&rotation);
        }
        break;
    }
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

// ---- 0x800BC748-0x800BDA38 ----


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

// ---- 0x800BDA38-0x800BEEB8 ----


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
