#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_adv_name.h>
#include <sc/sc_adv_saveload.h>

// main.dol functions with names that the shared headers do not declare.
// muMenuController::getPadNo()
extern "C" int fn_800BA3D8(muMenuController* controller);
// GameGlobal::getNameCount() and GameGlobal::getName(number, buffer): the names that were saved
extern "C" int fn_8004D148(GameGlobal* global);
extern "C" void fn_8004D6C4(GameGlobal* global, int number, char* buffer);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// nw4r::g3d::ScnMdl::Construct(allocator, size, capacity, base model) and ScnMdl::Pushback(model, object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d6ScnMdlFPviiPv(void* allocator, int size, int capacity, void* scnMdl);
extern "C" void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void* scnMdl, void* object, const char* name);
// gfSceneRoot::layerUpdateFrame(layer) and gfSceneRoot::removeInfoCamResAnm()
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
// nw4r::g3d::ScnRoot::UpdateFrame()
extern "C" void UpdateFrame__Q34nw4r3g3d7ScnRootFv(void* scnRoot);

// The flag of the menu data (of the record of the game) that says that the screen is wide.
// HYPOTHESIS: the name; the flag is the high bit of the byte at 0x28.
struct muAdvNameMenuBits {
    u8 m_unk00[0x28];
    bool m_isWide : 1;
};

// The init and main function of each step.
static const muAdvNameTask::StepFunc sStepFuncs[muAdvNameTask::Step_Count] = {
    {muAdvNameTask::mainStepLoadInit, muAdvNameTask::mainStepLoadMain},
    {muAdvNameTask::mainStepStContInit, muAdvNameTask::mainStepStContMain},
    {muAdvNameTask::mainStepAppearInit, muAdvNameTask::mainStepAppearMain},
    {muAdvNameTask::mainStepSelectInit, muAdvNameTask::mainStepSelectMain},
    {muAdvNameTask::mainStepYesNoInit, muAdvNameTask::mainStepYesNoMain},
    {muAdvNameTask::mainStepBackInit, muAdvNameTask::mainStepBackMain},
    {muAdvNameTask::mainStep1POnlyInit, muAdvNameTask::mainStep1POnlyMain},
    {muAdvNameTask::mainStep2POnlyInit, muAdvNameTask::mainStep2POnlyMain},
};

muAdvNameTask* muAdvNameTask::create(const muAdvNameTaskParam* param) {
    muAdvNameTask* task = new (Heaps::MenuInstance) muAdvNameTask();
    task->initProc(param);
    return task;
}

muAdvNameTask::muAdvNameTask()
    : gfTask("muAdvName", Category_Menu, 0xF, 8, true), m_choice(0), m_wait(0), m_pad0(0), m_is2P(false), m_resFile(static_cast<void*>(NULL)),
      m_unk164(0), m_step(Step_Load), m_stepPrev(Step_Count) {
    m_started = false;
    m_unk351 = false;
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_unk164 = 0;
    m_msg = NULL;
    m_objects[0] = NULL;
    m_objects[1] = NULL;
    m_objects[2] = NULL;
    m_objects[3] = NULL;
    m_objects[4] = NULL;
    m_objects[5] = NULL;
    m_objects[6] = NULL;
    m_objects[7] = NULL;
    m_objects[8] = NULL;
    m_scnMdls[0] = NULL;
    m_scnMdls[1] = NULL;
    m_scnMdls[2] = NULL;
    m_result[0] = 0;
    m_players[0].m_unkA8 = 0;
    m_players[0].m_listState = 0;
    m_players[0].m_yesNoState = 0;
    m_players[0].m_stContState = 0;
    m_cursor[0] = 0;
    m_scroll[0] = 0;
    m_sel[0] = 0;
    m_sel2[0] = 0;
    m_result[1] = 0;
    m_players[1].m_unkA8 = 0;
    m_players[1].m_listState = 0;
    m_players[1].m_yesNoState = 0;
    m_players[1].m_stContState = 0;
    m_cursor[1] = 0;
    m_scroll[1] = 0;
    m_sel[1] = 0;
    m_sel2[1] = 0;
    m_name2[0] = 0x7F;
    m_name1[0] = 0x7F;
    m_name2[1] = 0x7E;
    m_name1[1] = 0x7E;
    m_name2[2] = 0x7D;
    m_name1[2] = 0x7D;
    m_name2[3] = 0x7C;
    m_name1[3] = 0x7C;
    m_name2[4] = 0x7B;
    m_name1[4] = 0x7B;
    m_name2[5] = 0;
    m_name1[5] = 0;
    m_loaded[0] = false;
    m_loaded[1] = false;
    m_flag352 = false;
    m_flag353 = false;
    m_handles[0].readRequest("/menu/adventure/name_sel.brres", Heaps::MenuResource, 0, 0);
    m_handles[1].readRequest("/menu/msg/adv_selname.msbin", Heaps::MenuResource, 0, 0);
    muMenu::loadMenuSound();
}

muAdvNameTask::~muAdvNameTask() {
    m_msg->detachMuObject(m_objects[2]);
    m_msg->detachMuObject(m_objects[1]);
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
    for (int i = 0; i < 3; i++) {
        if (m_scnMdls[i] != NULL) {
            m_scnMdls[i]->Destroy();
        }
    }
    for (int i = 0; i < 9; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
        }
    }
    fn_8000D7B8(g_gfSceneRoot);
    for (int i = 0; i < 2; i++) {
        m_handles[i].cancelRequest();
        gfHeapManager::free(m_handles[i].getBuffer());
        m_handles[i].release();
    }
}

    // The slots: the title (5), the first question (6), the question (7), the lists (8), the frame (0), the cursors of
// the first player (1, 2), the cursors of the second player (3, 4).
static const muAdvNameModel sModels[] = {
    {"MenAdvMenu0000_TopN", 5, 5, 1},
    {"MenAdvMenu0001_TopN", 6, 6, 7},
    {"MenAdvMenu0010_TopN", 8, 8, 9},
    {"MenAdvMenu0020_TopN", 0, 0, 2},
    {"MenAdvMenu0021_TopN", 1, 2, 3},
    {"MenAdvMenu0022_TopN", 3, 4, 5},
    {"MenAdvMenu0100_TopN", 7, 7, 8},
};
// The models that the other models are attached to.
static const muAdvNameScnMdl sScnMdls[] = {
    {0, 0, 0, 2},
    {1, 1, 1, 1},
    {2, 2, 2, 1},
};
void muAdvNameTask::initProc(const muAdvNameTaskParam* param) {
    m_unk34C = param->m_unkC;
    unk2C_b1 = false;
    m_pad0 = param->m_pad0;
    fn_800B96E8(&m_controller, param->m_pad0 & 0xFF);
    if (param->m_pad1 >= 0) {
        fn_800B96E8(&m_players[0].m_controller, param->m_pad1 & 0xFF);
    }
    if (param->m_pad2 >= 0) {
        fn_800B96E8(&m_players[1].m_controller, param->m_pad2 & 0xFF);
    }
    m_players[0].m_unkA8 = 0;
    m_players[1].m_unkA8 = 0;
    m_nameCount = fn_8004D148(g_GameGlobal);
    OSReport("MaxNameCount[%d]\n", m_nameCount);
    m_nameSel1 = 0;
    m_unk2E0 = 2;
    m_nameSel2 = 0;
}

void muAdvNameTask::initMsg() {
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x100, 0xF);
    m_msg->setMsgData(m_handles[1].getBuffer());
    m_msg->attachScnMdlSimple(0xC, m_objects[6]->m_sceneModel, 0, 0.0625f);
    m_msg->setAlignMode(0xC, MuMsg::Align_Center);
    m_msg->setFontColor(0xC, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(0xD, m_objects[6]->m_sceneModel, 1, 0.05f);
    m_msg->setFontColor(0xD, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(0, m_objects[4]->m_sceneModel, 0, 0.0625f);
    m_msg->setFontColor(0, 0xFF, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(1, m_objects[4]->m_sceneModel, 1, 0.0625f);
    m_msg->setFontColor(1, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(2, m_objects[4]->m_sceneModel, 2, 0.0625f);
    m_msg->setFontColor(2, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(3, m_objects[4]->m_sceneModel, 3, 0.0625f);
    m_msg->setFontColor(3, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(4, m_objects[4]->m_sceneModel, 4, 0.0625f);
    m_msg->setFontColor(4, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(5, m_objects[3]->m_sceneModel, 0, 0.0625f);
    m_msg->setFontColor(5, 0, 0, 0xFF, 0xFF);
    m_msg->attachScnMdlSimple(6, m_objects[3]->m_sceneModel, 1, 0.0625f);
    m_msg->setFontColor(6, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(7, m_objects[3]->m_sceneModel, 2, 0.0625f);
    m_msg->setFontColor(7, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(8, m_objects[3]->m_sceneModel, 3, 0.0625f);
    m_msg->setFontColor(8, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(9, m_objects[3]->m_sceneModel, 4, 0.0625f);
    m_msg->setFontColor(9, 0, 0, 0, 0xFF);
    m_msg->attachScnMdlSimple(0xA, m_objects[2]->m_sceneModel, 0, 0.0625f);
    m_msg->attachScnMdlSimple(0xB, m_objects[1]->m_sceneModel, 0, 0.0625f);
}

// Writes the names of the list of a player: the first line is the line for a new name when the list is at its top.
void muAdvNameTask::dispStr(int player) {
    int count = m_nameCount;
    int base = (player == 0) ? 0 : 5;
    int rows = 4;
    if (count < 4) {
        rows = count;
    }
    char buffer[0x15];
    if (m_scroll[player] == 0) {
        for (int i = -1; i < rows; i++) {
            if (i == -1) {
                if (player == 0) {
                    m_msg->setFontType(0, 0);
                    m_msg->setFontColor(0, 0xFF, 0, 0, 0xFF);
                    if (m_unk34C == -1) {
                        m_msg->printIndex(0, 3, NULL);
                    } else {
                        m_msg->printIndex(0, 8, NULL);
                    }
                } else {
                    m_msg->setFontType(5, 0);
                    m_msg->setFontColor(5, 0, 0, 0xFF, 0xFF);
                    m_msg->printIndex(5, 3, NULL);
                }
            } else {
                fn_8004D6C4(g_GameGlobal, i + m_scroll[player], buffer);
                int line = i + base + 1;
                m_msg->setFontType(line, 1);
                m_msg->setFontColor(line, 0, 0, 0, 0xFF);
                m_msg->printf(line, buffer);
                if (i + 1 == m_cursor[player]) {
                    if (player == 0) {
                        strcpy(m_name1, buffer);
                        m_nameSel1 = i + m_scroll[player];
                    } else {
                        strcpy(m_name2, buffer);
                        m_nameSel2 = i + m_scroll[player];
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < 5; i++) {
            fn_8004D6C4(g_GameGlobal, i + m_scroll[player] - 1, buffer);
            int line = i + base;
            m_msg->setFontType(line, 1);
            m_msg->setFontColor(line, 0, 0, 0, 0xFF);
            m_msg->printf(line, buffer);
            if (i == m_cursor[player]) {
                if (player == 0) {
                    strcpy(m_name1, buffer);
                    m_nameSel1 = i + m_scroll[player] - 1;
                } else {
                    strcpy(m_name2, buffer);
                    m_nameSel2 = i + m_scroll[player] - 1;
                }
            }
        }
    }
}

void muAdvNameTask::processDefault() {
    if (m_stepPrev != m_step) {
        if (sStepFuncs[m_step].m_init != NULL) {
            sStepFuncs[m_step].m_init(this);
        }
        m_stepPrev = m_step;
    }
    if (sStepFuncs[m_step].m_main != NULL) {
        m_step = sStepFuncs[m_step].m_main(this);
    }
    for (int i = 0; i < 2; i++) {
        fn_800B9984(&m_players[i].m_controller);
    }
    controllProc();
    gfSceneRoot* root = g_gfSceneRoot;
    UpdateFrame__Q34nw4r3g3d7ScnRootFv(*reinterpret_cast<void**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x44));
    fn_8000DB40(root, 8);
}

// Reads the pads: the lists are moved with up and down, A decides and B goes back; a pad that presses start joins as the
// second player.
void muAdvNameTask::controllProc() {
    u32 buttons[2] = {0, 0};
    gfPadStatus pad[2] = {};
    int pos[2] = {0, 0};
    if (m_wait > 0) {
        m_wait--;
        if (m_wait > 0) {
            return;
        }
    }
    for (int i = 0; i < 2; i++) {
        if (i != 1 || m_is2P) {
            buttons[i] = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&m_players[i].m_controller) + 0x10);
            g_gfPadSystem->getSysPadStatus(fn_800BA3D8(&m_players[i].m_controller), &pad[i]);
        } else {
            buttons[i] = 0;
            pad[i].init();
        }
        if (m_step == Step_StCont) {
            if ((buttons[i] & 8) != 0 || (buttons[i] & 1) != 0) {
                if (m_stCont != 0) {
                    m_stCont = 0;
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            }
            if ((buttons[i] & 4) != 0 || (buttons[i] & 2) != 0) {
                if (m_stCont != 1) {
                    m_stCont = 1;
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            }
            if (m_objects[8]->isNodeAnimFinished()) {
                if ((pad[i].m_buttonsPressedThisFrame.bits & 0x100) != 0) {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    m_players[i].m_stContState = 1;
                    m_sel[i] = 0;
                }
                if ((pad[i].m_buttonsPressedThisFrame.bits & 0x200) != 0) {
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    m_players[i].m_stContState = 1;
                    m_sel[i] = 2;
                }
            }
        }
        if (m_step == Step_Select) {
            pos[i] = m_cursor[i];
            if (m_players[i].m_listState == 0) {
                int starter = -1;
                for (int k = 0; k < 8; k++) {
                    if (k != fn_800BA3D8(&m_players[0].m_controller)) {
                        gfPadStatus other;
                        g_gfPadSystem->getSysPadStatus(k, &other);
                        if ((other.m_buttonsPressedThisFrame.bits & 0x1000) != 0) {
                            starter = k;
                        }
                    }
                }
                if (starter >= 0 && !m_is2P) {
                    fn_800B96E8(&m_players[1].m_controller, starter & 0xFF);
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    m_is2P = true;
                    m_step = Step_Appear;
                }
            }
            if (!m_is2P && i == 1) {
                return;
            }
            if ((buttons[i] & 8) != 0) {
                if (pos[i] > 0) {
                    pos[i]--;
                    if (i == 0) {
                        m_unk2E0 = 1;
                    } else if (i == 1) {
                        m_unk2E0 = 0;
                    }
                }
                if (m_cursor[i] == 0) {
                    if (m_scroll[i] > 0) {
                        m_scroll[i]--;
                        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                    }
                }
            }
            if ((buttons[i] & 4) != 0) {
                if (!m_is2P && i == 1) {
                    return;
                }
                int rows = 4;
                if (m_nameCount < 4) {
                    rows = m_nameCount;
                }
                if (pos[i] < rows) {
                    pos[i]++;
                    if (i == 0) {
                        m_unk2E0 = 1;
                    } else if (i == 1) {
                        m_unk2E0 = 0;
                    }
                }
                if (rows == m_cursor[i]) {
                    if (m_scroll[i] < m_nameCount - 4) {
                        m_scroll[i]++;
                        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                    }
                }
            }
            if (m_loaded[1]) {
                dispStr(i);
            }
            if (pos[i] != m_cursor[i]) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_cursor[i] = pos[i];
            }
            if ((pad[i].m_buttonsPressedThisFrame.bits & 0x100) != 0) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                m_sel[i] = 0;
                m_sel2[i] = 0;
                m_players[i].m_listState = 1;
            }
            if ((pad[i].m_buttonsPressedThisFrame.bits & 0x200) != 0) {
                if (!m_is2P) {
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    m_sel[0] = 2;
                    m_players[0].m_listState = 1;
                } else if (i == 1) {
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    m_sel[1] = 2;
                    m_players[1].m_listState = 1;
                }
            }
            if (m_players[i].m_listState == 3) {
                if ((pad[i].m_buttonsPressedThisFrame.bits & 0x200) != 0) {
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    m_players[i].m_listState = 1;
                    m_sel[i] = 1;
                    m_sel2[i] = 2;
                }
            }
        }
        if (m_step == Step_YesNo) {
            if (i == 1) {
                return;
            }
            if ((buttons[i] & 1) != 0) {
                if (m_choice != 0) {
                    m_choice = 0;
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            }
            if ((buttons[i] & 2) != 0) {
                if (m_choice != 1) {
                    m_choice = 1;
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            }
            if (m_objects[7]->isNodeAnimFinished()) {
                if ((pad[i].m_buttonsPressedThisFrame.bits & 0x100) != 0) {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    m_players[i].m_yesNoState = 1;
                    if (m_choice == 0) {
                        m_sel[i] = 0;
                    } else {
                        m_sel[i] = 2;
                    }
                }
                if ((pad[i].m_buttonsPressedThisFrame.bits & 0x200) != 0) {
                    g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    m_players[i].m_yesNoState = 1;
                    m_sel[i] = 2;
                }
            }
        }
        if (m_step == Step_1POnly) {
            if (i == 1) {
                return;
            }
            panelOnlyCtrl(0, buttons[0], pad[0]);
        }
        if (m_step == Step_2POnly) {
            panelOnlyCtrl(1, buttons[1], pad[1]);
        }
    }
}

// The list of a player in the menus where only one player is: the cursor, the scrolling and the decision.
void muAdvNameTask::panelOnlyCtrl(int player, u32 buttons, gfPadStatus pad) {
    int cursor = m_cursor[player];
    if (m_players[player].m_listState == 0) {
        if ((buttons & 8) != 0) {
            if (cursor > 0) {
                cursor = cursor - 1;
            }
            if (m_cursor[player] == 0) {
                if (m_scroll[player] > 0) {
                    m_scroll[player]--;
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            }
        }
        if ((buttons & 4) != 0) {
            int rows = 4;
            if (m_nameCount < 4) {
                rows = m_nameCount;
            }
            if (cursor < rows) {
                cursor = cursor + 1;
            }
            if (rows == m_cursor[player]) {
                if (m_scroll[player] < m_nameCount - 4) {
                    m_scroll[player]++;
                    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                }
            }
        }
        if (m_loaded[1]) {
            dispStr(player);
        }
        if (cursor != m_cursor[player]) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor[player] = cursor;
        }
        if ((pad.m_buttonsPressedThisFrame.bits & 0x100) != 0) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            m_sel[player] = 2;
            m_sel2[player] = 0;
            m_players[player].m_listState = 1;
        }
        if ((pad.m_buttonsPressedThisFrame.bits & 0x200) != 0) {
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            m_sel[player] = 2;
            m_sel2[player] = 2;
            m_players[player].m_listState = 1;
        }
    }
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muAdvNameTask::createObjResFile(const muAdvNameModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start + 1;
        }
        for (int j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, models->m_name, models->m_priority, NULL, Heaps::MenuInstance);
            m_objects[models->m_start + j] = object;
            char animName[0x40];
            strcpy(animName, models->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

// Makes the models that the other models are attached to.
// MATCH-ONLY: the game does not inline this function into the step that loads the menu.
#pragma dont_inline on
void muAdvNameTask::createExpMdl(const muAdvNameScnMdl* models, int count) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start + 1;
        }
        for (int j = 0; j < num; j++) {
            m_scnMdls[models->m_start + j] = Construct__Q34nw4r3g3d6ScnMdlFPviiPv(allocator, 0, models->m_capacity,
                                                                                  m_objects[models->m_objectNo + j]->m_sceneModel);
        }
    }
}

#pragma dont_inline reset

// Puts the cursor model on the line of the list that the cursor is on: the frames say which arrows are shown.
void muAdvNameTask::drawCursor(int player, MuObject* cursor) {
    switch (m_cursor[player]) {
    case 0:
        cursor->setFrameNode(0.0f);
        if (m_scroll[player] > 0) {
            cursor->setFrameVisible(3.0f);
        } else if (m_nameCount == 0) {
            cursor->setFrameVisible(0.0f);
        } else {
            cursor->setFrameVisible(1.0f);
        }
        break;
    case 1:
        cursor->setFrameNode(1.0f);
        if (m_nameCount == 1) {
            cursor->setFrameVisible(2.0f);
        } else {
            cursor->setFrameVisible(3.0f);
        }
        break;
    case 2:
        cursor->setFrameNode(2.0f);
        if (m_nameCount == 2) {
            cursor->setFrameVisible(2.0f);
        } else {
            cursor->setFrameVisible(3.0f);
        }
        break;
    case 3:
        cursor->setFrameNode(3.0f);
        if (m_nameCount == 3) {
            cursor->setFrameVisible(2.0f);
        } else {
            cursor->setFrameVisible(3.0f);
        }
        break;
    case 4:
        cursor->setFrameNode(4.0f);
        if (m_scroll[player] < m_nameCount - 4) {
            cursor->setFrameVisible(3.0f);
        } else {
            cursor->setFrameVisible(2.0f);
        }
        break;
    }
}

// Whether a file is still being loaded.
static bool isLoading(muAdvNameTask* task) {
    for (int i = 0; i < 2; i++) {
        if (!task->m_loaded[i] && task->m_handles[i].m_request != NULL) {
            if (!task->m_handles[i].isReady()) {
                return true;
            }
            task->m_loaded[i] = true;
        }
    }
    return false;
}

void muAdvNameTask::mainStepLoadInit(muAdvNameTask* task) { }

// Waits for the files and the menu sound, then creates the models and the texts.
int muAdvNameTask::mainStepLoadMain(muAdvNameTask* task) {
    if (isLoading(task) == 1) {
        return task->m_step;
    }
    task->m_resFile = nw4r::g3d::ResFile(task->m_handles[0].getBuffer());
    nw4r::g3d::ResFile::Init(&task->m_resFile);
    task->createObjResFile(sModels, 7, &task->m_resFile);
    task->createExpMdl(sScnMdls, 3);
    task->initMsg();
    int next;
    if (task->m_unk34C == 0) {
        next = Step_1POnly;
    } else if (task->m_unk34C == 1) {
        next = Step_2POnly;
    } else {
        next = Step_StCont;
        gfScene* scene = gfSceneManager::getInstance()->m_prevScene;
        if (scene != NULL) {
            dynamic_cast<scAdvLoad*>(scene);
        }
    }
    return next;
}

// Attaches the models to the scene (and the models that are attached to the models) and ends the screen that was kept
// while the menu was loaded.
static void addModelsToScene(muAdvNameTask* task) {
    if (task->m_step == muAdvNameTask::Step_StCont) {
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[5]->m_sceneModel);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[6]->m_sceneModel);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[7]->m_sceneModel);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[8]->m_sceneModel);
    }
    nw4r::g3d::G3dObj* scnMdl = task->m_scnMdls[0];
    nw4r::g3d::G3dObj* attached = task->m_scnMdls[2];
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(scnMdl));
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(scnMdl, attached, "pos00");
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[0], task->m_scnMdls[1], "pos01");
    task->m_started = true;
    g_gfApplication->unkD0.endKeepScreen();
}

// Shows the first question and attaches the models to the scene.
void muAdvNameTask::mainStepStContInit(muAdvNameTask* task) {
    task->m_objects[8]->changeNodeAnimNIf("MenAdvMenu0010_TopN__0");
    task->m_stCont = 1;
    if (!task->m_started) {
        addModelsToScene(task);
    }
}

int muAdvNameTask::mainStepStContMain(muAdvNameTask* task) {
    MuObject* lists = task->m_objects[8];
    lists->changeClrAnimNIf("MenAdvMenu0010_TopN__0");
    switch (task->m_stCont) {
    case 0:
        lists->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        break;
    case 1:
        lists->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        break;
    }
    for (int i = 0; i < 2; i++) {
        if (task->m_players[i].m_stContState == 1) {
            task->m_players[i].m_stContState = 2;
            lists->changeNodeAnimNIf("MenAdvMenu0010_TopN__1");
            lists->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            lists->setFrameNode(0.0f);
        }
        if (task->m_players[i].m_stContState == 2) {
            if (lists->isNodeAnimFinished()) {
                task->m_players[i].m_stContState = 3;
            }
        }
        if (task->m_players[i].m_stContState == 3) {
            task->m_players[i].m_stContState = 0;
            if (task->m_sel[i] == 0) {
                if (task->m_stCont == 0) {
                    task->m_step = Step_Appear;
                } else if (task->m_stCont == 1) {
                    task->m_result[i] = 2;
                }
            } else if (task->m_sel[i] == 2) {
                task->m_result[i] = 3;
            }
        }
    }
    return task->m_step;
}

// The cursors and the lists appear.
void muAdvNameTask::mainStepAppearInit(muAdvNameTask* task) {
    MuObject* question = task->m_objects[6];
    MuObject* frame = task->m_objects[0];
    MuObject* cursor2 = task->m_objects[2];
    MuObject* cursor1 = task->m_objects[1];
    MuObject* list1 = task->m_objects[4];
    MuObject* list2 = task->m_objects[3];
    if (!task->m_is2P) {
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[2], list1->m_sceneModel, "pos00");
        question->m_modelAnim->setUpdateRate(1.0f);
        frame->setFrameNode(10.0f);
        frame->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
        cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
        cursor2->m_modelAnim->setLoop(false);
        cursor2->setFrameNode(0.0f);
        cursor2->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        cursor2->setFrameVisible(0.0f);
        cursor2->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
        cursor2->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        list1->changeClrAnimNIf("MenAdvMenu0022_TopN__0");
    }
    if (task->m_is2P) {
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[1], task->m_objects[3]->m_sceneModel, "pos00");
        cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
        cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
        cursor1->m_modelAnim->setUpdateRate(1.0f);
        list2->changeClrAnimNIf("MenAdvMenu0022_TopN__1");
    }
}

int muAdvNameTask::mainStepAppearMain(muAdvNameTask* task) {
    MuObject* cursor2 = task->m_objects[2];
    cursor2->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    cursor2->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
    return Step_Select;
}

// The list of the names is shown.
void muAdvNameTask::mainStepSelectInit(muAdvNameTask* task) {
    task->m_objects[6]->m_modelAnim->setUpdateRate(1.0f);
    task->m_msg->printIndex(0xC, 0, NULL);
    task->m_msg->printIndex(0xD, 1, NULL);
    task->m_players[0].m_listState = 0;
    task->m_players[1].m_listState = 0;
}

// Moves the cursors and puts the names that the players picked on the screen.
int muAdvNameTask::mainStepSelectMain(muAdvNameTask* task) {
    MuObject* cursor2 = task->m_objects[2];
    MuObject* cursor1 = task->m_objects[1];
    MuObject* list2 = task->m_objects[3];
    task->drawCursor(0, task->m_objects[4]);
    if (task->m_players[0].m_listState == 1) {
        task->m_players[0].m_listState = 2;
        reinterpret_cast<nw4r::g3d::ScnGroup*>(task->m_scnMdls[2])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(task->m_objects[4]->m_sceneModel));
        if (task->m_sel[0] == 0) {
            cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__1");
            cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__1");
            cursor2->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            cursor2->setFrameNode(0.0f);
            cursor2->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
            cursor2->setFrameVisible(0.0f);
        } else if (task->m_sel[0] == 2) {
            cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__2");
            cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__2");
        }
    }
    if (task->m_is2P) {
        task->drawCursor(1, list2);
        if (task->m_players[1].m_listState == 1) {
            task->m_players[1].m_listState = 2;
            reinterpret_cast<nw4r::g3d::ScnGroup*>(task->m_scnMdls[1])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(task->m_objects[3]->m_sceneModel));
            if (task->m_sel[1] == 0) {
                cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__1");
                cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__1");
                cursor1->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
                cursor1->setFrameNode(0.0f);
                cursor1->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                cursor1->setFrameVisible(0.0f);
                task->m_objects[0]->setFrameVisible(1.0f);
            } else if (task->m_sel[1] == 2) {
                cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__2");
                cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__2");
                if (task->m_unk34C == -1) {
                    task->m_objects[0]->setFrameVisible(0.0f);
                }
            }
        }
    }
    if (task->m_players[0].m_listState == 2) {
        if (cursor2->isNodeAnimFinished()) {
            task->m_players[0].m_listState = 3;
            if (task->m_sel[0] == 0) {
                task->m_msg->setFontType(0xA, 1);
                task->m_msg->setAlignMode(0xA, MuMsg::Align_Center);
                task->m_msg->setFontColor(0xA, 0, 0, 0, 0xFF);
                task->m_msg->printf(0xA, task->m_name1);
                if (task->m_scroll[0] == 0 && task->m_cursor[0] == 0) {
                    task->m_msg->setFontType(0xA, 0);
                    task->m_msg->printIndex(0xA, 3, NULL);
                }
            } else if (task->m_sel[0] == 2) {
                MuObject* question = task->m_objects[6];
                question->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
                question->setFrameNode(0.0f);
                task->m_objects[0]->setFrameNode(0.0f);
            }
        }
    }
    if (task->m_players[1].m_listState == 2) {
        if (cursor1->isNodeAnimFinished()) {
            task->m_players[1].m_listState = 3;
            if (task->m_sel[1] == 0) {
                task->m_msg->setFontType(0xB, 1);
                task->m_msg->setAlignMode(0xB, MuMsg::Align_Center);
                task->m_msg->setFontColor(0xB, 0, 0, 0, 0xFF);
                task->m_msg->printf(0xB, task->m_name2);
                if (task->m_scroll[1] == 0 && task->m_cursor[1] == 0) {
                    task->m_msg->setFontType(0xB, 0);
                    task->m_msg->printIndex(0xB, 3, NULL);
                }
            } else if (task->m_sel[1] == 2) {
                task->m_is2P = false;
                task->m_sel[1] = 0;
                task->m_players[1].m_listState = 0;
            }
        }
    }
    if (!task->m_is2P) {
        if (task->m_players[0].m_listState == 3) {
            if (task->m_sel[0] == 0) {
                task->m_step = Step_YesNo;
            } else if (task->m_sel[0] == 2) {
                task->m_step = Step_StCont;
            }
        }
    } else {
        if (task->m_players[0].m_listState == 3 && task->m_players[1].m_listState == 3) {
            if (task->m_sel[0] == 0 && task->m_sel[1] == 0) {
                task->m_step = Step_YesNo;
            } else {
                task->m_step = Step_StCont;
            }
        }
        if (task->m_players[0].m_listState == 3 && task->m_players[1].m_listState != 3) {
            if (task->m_sel2[0] == 2) {
                task->m_sel2[0] = 0;
                task->m_players[0].m_listState = 0;
                task->m_msg->printf(0xA, "");
                MuObject* object = task->m_objects[2];
                object->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
                object->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
                object->setFrameNode(0.0f);
                object->setFrameVisible(0.0f);
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[2], task->m_objects[4]->m_sceneModel, "pos00");
            }
            if (task->m_sel2[1] == 2) {
                task->m_sel2[1] = 0;
                task->m_players[1].m_listState = 0;
                task->m_msg->printf(0xB, "");
                MuObject* object = task->m_objects[1];
                object->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
                object->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
                object->setFrameNode(0.0f);
                object->setFrameVisible(0.0f);
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[1], task->m_objects[3]->m_sceneModel, "pos00");
            }
        }
        if (task->m_players[0].m_listState != 3 && task->m_players[1].m_listState == 3) {
            if (task->m_sel2[1] == 2) {
                task->m_sel2[1] = 0;
                task->m_players[1].m_listState = 0;
                task->m_msg->printf(0xB, "");
                MuObject* object = task->m_objects[1];
                object->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
                object->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
                object->setFrameNode(0.0f);
                object->setFrameVisible(0.0f);
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[1], task->m_objects[3]->m_sceneModel, "pos00");
            }
        }
    }
    return task->m_step;
}

// The question that asks whether the names are the right ones.
void muAdvNameTask::mainStepYesNoInit(muAdvNameTask* task) {
    task->m_objects[6]->m_modelAnim->setUpdateRate(1.0f);
    task->m_msg->printIndex(0xC, 2, NULL);
    task->m_msg->printf(0xD, "");
    task->m_choice = 0;
    task->m_players[0].m_yesNoState = 0;
    task->m_players[1].m_yesNoState = 0;
    task->m_objects[7]->changeNodeAnimNIf("MenAdvMenu0100_TopN__0");
}

int muAdvNameTask::mainStepYesNoMain(muAdvNameTask* task) {
    MuObject* frame = task->m_objects[0];
    MuObject* question = task->m_objects[7];
    frame->setFrameVisible(1.0f);
    question->changeClrAnimNIf("MenAdvMenu0100_TopN__0");
    question->m_modelAnim->setUpdateRate(1.0f);
    switch (task->m_choice) {
    case 0:
        question->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        break;
    case 1:
        question->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        break;
    }
    for (int i = 0; i < 2; i++) {
        if (task->m_players[i].m_yesNoState == 1) {
            task->m_players[i].m_yesNoState = 2;
            question->changeNodeAnimNIf("MenAdvMenu0100_TopN__1");
            question->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            question->setFrameNode(0.0f);
        }
        if (task->m_players[i].m_yesNoState == 2) {
            if (question->isNodeAnimFinished()) {
                task->m_players[i].m_yesNoState = 3;
            }
        }
        if (task->m_players[i].m_yesNoState == 3) {
            if (task->m_sel[i] == 0) {
                task->m_result[0] = 1;
                task->m_step = Step_Back;
                task->m_msg->printf(0xC, "");
            } else if (task->m_sel[i] == 2) {
                task->m_step = Step_Select;
                task->m_msg->printf(0xA, "");
                task->m_msg->printf(0xB, "");
                MuObject* cursor2 = task->m_objects[2];
                cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
                cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[2], task->m_objects[4]->m_sceneModel, "pos00");
                frame->setFrameVisible(0.0f);
                if (task->m_is2P) {
                    MuObject* cursor1 = task->m_objects[1];
                    cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
                    cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
                    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[1], task->m_objects[3]->m_sceneModel, "pos00");
                }
            }
        }
    }
    return task->m_step;
}

void muAdvNameTask::mainStepBackInit(muAdvNameTask* task) { }

// Goes back to the lists when a step asks for it.
int muAdvNameTask::mainStepBackMain(muAdvNameTask* task) {
    if (task->m_flag352) {
        if (task->m_flag353) {
            task->m_result[0] = 2;
        } else {
            MuObject* frame = task->m_objects[0];
            task->m_result[0] = 0;
            task->m_step = Step_Select;
            task->m_msg->printf(0xA, "");
            task->m_msg->printf(0xB, "");
            MuObject* cursor2 = task->m_objects[2];
            cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
            cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
            Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[2], task->m_objects[4]->m_sceneModel, "pos00");
            frame->setFrameVisible(0.0f);
            if (task->m_is2P) {
                MuObject* cursor1 = task->m_objects[1];
                cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
                cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
                Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[1], task->m_objects[3]->m_sceneModel, "pos00");
            }
        }
        task->m_flag352 = false;
    }
    return task->m_step;
}

// Only the first player: the list of the names starts at once.
void muAdvNameTask::mainStep1POnlyInit(muAdvNameTask* task) {
    task->m_players[0].m_listState = 0;
    MuObject* frame = task->m_objects[0];
    MuObject* cursor2 = task->m_objects[2];
    MuObject* list1 = task->m_objects[4];
    frame->setFrameNode(10.0f);
    frame->setFrameVisible(1.0f);
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[2], task->m_objects[4]->m_sceneModel, "pos00");
    cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
    cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
    cursor2->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    cursor2->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
    list1->changeClrAnimNIf("MenAdvMenu0022_TopN__0");
    addModelsToScene(task);
    frame->m_scnMdl->SetPriorityDrawOpa(0x96);
    frame->m_scnMdl->SetPriorityDrawXlu(0x96);
    cursor2->m_scnMdl->SetPriorityDrawOpa(0x97);
    cursor2->m_scnMdl->SetPriorityDrawXlu(0x97);
    list1->m_scnMdl->SetPriorityDrawOpa(0x98);
    list1->m_scnMdl->SetPriorityDrawXlu(0x98);
}

int muAdvNameTask::mainStep1POnlyMain(muAdvNameTask* task) {
    MuObject* frame = task->m_objects[0];
    MuObject* cursor2 = task->m_objects[2];
    task->drawCursor(0, task->m_objects[4]);
    if (task->m_players[0].m_listState == 1) {
        task->m_players[0].m_listState = 2;
        if (task->m_sel[0] == 2) {
            cursor2->changeNodeAnimNIf("MenAdvMenu0021_TopN__2");
            cursor2->changeVisAnimNIf("MenAdvMenu0021_TopN__2");
        }
    }
    if (task->m_players[0].m_listState == 2) {
        if (cursor2->isNodeAnimFinished()) {
            task->m_players[0].m_listState = 3;
            if (task->m_sel[0] == 2) {
                if (task->m_sel2[0] == 0) {
                    task->m_result[0] = 2;
                } else if (task->m_sel2[0] == 2) {
                    task->m_result[0] = 3;
                }
            }
            frame->setFrameNode(0.0f);
            frame->setFrameVisible(1.0f);
        }
    }
    return task->m_step;
}

// Only the second player: the list of the names starts at once.
void muAdvNameTask::mainStep2POnlyInit(muAdvNameTask* task) {
    task->m_is2P = true;
    task->m_players[1].m_listState = 0;
    MuObject* frame = task->m_objects[0];
    MuObject* cursor1 = task->m_objects[1];
    MuObject* list2 = task->m_objects[3];
    frame->setFrameNode(10.0f);
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(task->m_scnMdls[1], task->m_objects[3]->m_sceneModel, "pos00");
    cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__0");
    cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__0");
    cursor1->m_modelAnim->setUpdateRate(1.0f);
    list2->changeClrAnimNIf("MenAdvMenu0022_TopN__1");
    addModelsToScene(task);
    frame->m_scnMdl->SetPriorityDrawOpa(0x96);
    frame->m_scnMdl->SetPriorityDrawXlu(0x96);
    cursor1->m_scnMdl->SetPriorityDrawOpa(0x97);
    cursor1->m_scnMdl->SetPriorityDrawXlu(0x97);
    list2->m_scnMdl->SetPriorityDrawOpa(0x98);
    list2->m_scnMdl->SetPriorityDrawXlu(0x98);
}

int muAdvNameTask::mainStep2POnlyMain(muAdvNameTask* task) {
    MuObject* frame = task->m_objects[0];
    MuObject* cursor1 = task->m_objects[1];
    task->drawCursor(1, task->m_objects[3]);
    if (task->m_players[1].m_listState == 1) {
        task->m_players[1].m_listState = 2;
        if (task->m_sel[1] == 2) {
            cursor1->changeNodeAnimNIf("MenAdvMenu0021_TopN__2");
            cursor1->changeVisAnimNIf("MenAdvMenu0021_TopN__2");
            frame->setFrameVisible(1.0f);
        }
    }
    if (task->m_players[1].m_listState == 2) {
        if (cursor1->isNodeAnimFinished()) {
            task->m_players[1].m_listState = 3;
            if (task->m_sel[1] == 2) {
                if (task->m_sel2[1] == 0) {
                    task->m_result[1] = 2;
                } else if (task->m_sel2[1] == 2) {
                    task->m_result[1] = 3;
                }
            }
            frame->setFrameNode(0.0f);
        }
    }
    return task->m_step;
}
