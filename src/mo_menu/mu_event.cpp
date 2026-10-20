#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <ut/ut_relocate.h>

#include <mu/menu.h>
#include <mu/mu_event.h>
#include <sc/sc_sel_event.h>

// main.dol functions with names that the shared headers do not declare.
// muMenuController::setPadMask(mask), setPad(pad) and resetRepeat(buttons); getPadNo()
extern "C" void fn_800B98DC(muMenuController* controller, int mask);
extern "C" void fn_800B9804(muMenuController* controller, int pad);
extern "C" void fn_800BA364(muMenuController* controller, int buttons);
extern "C" int fn_800BA3D8(muMenuController* controller);
// GameGlobal::getName(number, buffer) and GameGlobal::isEventOpen(list, event)
extern "C" void fn_8004D6C4(GameGlobal* global, int number, char* buffer);
extern "C" bool fn_8004E7A0(GameGlobal* global, u8 list, u8 event);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// MuMsg::printData(message, data, length) and MuMsg::setFontWidthModeFixed(message, width)
extern "C" void printData__5MuMsgFUlPCvi(MuMsg* msg, u32 msgIndex, const void* data, int length);
extern "C" void setFontWidthModeFixed__5MuMsgFUlf(MuMsg* msg, u32 msgIndex, float width);
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);
// The archive: the resource of a node of the pack and the data of a file (archive, id, type, index, size, file id)
extern "C" void* fn_800B03F0(gfArchive* archive, int index);
extern "C" void* fn_80015E34(gfArchive* archive, int id, int type, int index, int* size, u16 fileId);
// The sound system: stop the music (fade frames) and play a sound with a volume and a pitch
extern "C" void fn_80073F84(sndSystem* system, int fade);
extern "C" void fn_80074ECC(sndSystem* system, int id, float volume, float pitch);
// The system that stops the music with a fade and the instance it works on
extern "C" void fn_80078DAC(void* system, int a, int b, int c);
extern void* lbl_805A01D8;
// The window of the module of the name menus where a player picks a name: create(group, kind, resource, resource, pad, heap, model,
// node, ...)
extern "C" muEventNameWnd* fn_16_1CB0(void* group, int kind, nw4r::g3d::ResFile* resFileA, nw4r::g3d::ResFile* resFileB, int pad, int heap,
                                      void* scnMdl, const char* node, int a, int b, int c);
// muMenu::exchangeGmCharacterKind2MuSelchkind: the symbol takes three integers, the menu gives one
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int kind);
// The module of the scenes: the setting of the fighter of the event (sequence, setting) and two functions that the event uses
extern "C" void fn_1_33094(gfSequence* sequence, void* out, int a, int b, int c, int d, int e, int f, int g);
extern "C" void fn_1_379C(int slot, int kind, int color);
extern "C" bool fn_1_385C(int a);
// The random numbers of main.dol: an integer from 0 up to the argument
long randi(long max);
// Message::halfToFull(dst, src)
extern "C" int halfToFull__7MessageFPcPCc(char* dst, const char* src);

// The number of players that are in the menu.
static inline int getPlayerCount(const muEventTask* task) {
    return (task->m_mode == 1) + 1;
}

// The best score of an event is stored in the game with its level: the flag is in the high bits.
// The init and main function of each step.
static const muEventTask::StepFunc sStepFuncs[muEventTask::Step_Count] = {
    {muEventTask::mainStepLoadingInit, muEventTask::mainStepLoadingMain},
    {muEventTask::mainStepAppearInit, muEventTask::mainStepAppearMain},
    {muEventTask::mainStepSelectEventInit, muEventTask::mainStepSelectEventMain},
    {muEventTask::mainStepSelectLevelInit, muEventTask::mainStepSelectLevelMain},
    {muEventTask::mainStepDecidedInit, muEventTask::mainStepDecidedMain},
};

const muEventModel muEventTask::sModels[13] = {
    {"MenEventBg_TopN", 0, 0, 0x10},
    {"MenEvent0000_TopN", 1, 1, 0x12},
    {"MenEvent0001_TopN", 2, 2, 0x11},
    {"MenEvent0002_TopN", 3, 0xE, 0x11},
    {"MenEvent0003_TopN", 0xE, 0xE, 0x11},
    {"MenEvent0003_TopN", 0xF, 0xF, 0x11},
    {"MenEvent0004_TopN", 0x10, 0x10, 0x16},
    {"MenEvent0010_TopN", 0x11, 0x51, 0x14},
    {"MenEvent0011_TopN", 0x51, 0x51, 0x15},
    {"MenEvent0020_TopN", 0x52, 0x52, 0x13},
    {"MenEvent0021_TopN", 0x53, 0x53, 0x13},
    {"MenEvent0012_TopN", 0x54, 0x54, 0x13},
    {"MenEvent0040_TopN", 0x55, 0x55, 0x13},
};

muEventTask* muEventTask::create(nw4r::g3d::ScnGroup* group, const muEventSetting* setting, gfArchive* archive) {
    muEventTask* task = new (Heaps::MenuInstance) muEventTask(archive);
    task->initProc(group, setting);
    return task;
}

muEventTask::muEventTask(gfArchive* archive)
    : gfTask("Event", Category_Menu, 0xF, 8, true), m_step(Step_Loading), m_stepPrev(Step_Count), m_group(NULL) {
    m_cursor = 0;
    m_mode = 0;
    m_arrowDir = 0;
    m_unk17E8 = false;
    m_unk17E9 = false;
    m_wait = 0;
    m_archive = archive;
    unk2C_b1 = false;
    for (int i = 0; i < 0x56; i++) {
        m_objects[i] = NULL;
    }
    m_scnMdls[0] = NULL;
    m_scnMdls[1] = NULL;
    m_resFileMenu = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileWndA = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileWndB = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    memset(m_records, 0, sizeof(m_records));
    memset(m_lists, 0, sizeof(m_lists));
    m_wnds[0] = NULL;
    m_wnds[1] = NULL;
    m_pad[0] = 0;
    m_pad[1] = 0;
    m_name[0] = -1;
    m_name[1] = -1;
    m_wndShown[0] = false;
    m_wndShown[1] = false;
}

muEventTask::~muEventTask() {
    for (int i = 0; i < 2; i++) {
        if (m_wnds[i] != NULL) {
            m_wnds[i]->vf64();
        }
    }
    if (m_msg != NULL) {
        delete m_msg;
    }
    for (int i = 0; i < 2; i++) {
        if (m_scnMdls[i] != NULL) {
            m_scnMdls[i]->Destroy();
        }
    }
    for (int i = 0; i < 0x56; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
        }
    }
}

void muEventTask::initProc(nw4r::g3d::ScnGroup* group, const muEventSetting* setting) {
    m_group = group;
    m_mode = setting->m_playerCount != 1;
    m_name[0] = setting->m_name[0];
    m_pad[0] = setting->m_pad[0];
    m_name[1] = setting->m_name[1];
    m_pad[1] = setting->m_pad[1];
    u8 mask = 0;
    for (int i = 0; i < setting->m_playerCount; i++) {
        mask |= 1 << setting->m_pad[i];
    }
    fn_800B98DC(&m_controllers[0], mask);
    fn_800B9804(&m_controllers[1], setting->m_pad[0] & 0xFF);
    if (setting->m_playerCount > 1) {
        fn_800B9804(&m_controllers[2], setting->m_pad[1] & 0xFF);
    }
    m_level = setting->m_level;
    m_cursor = setting->m_cursor;
    m_unk17E8 = setting->m_unk20;
}

// Copies what the player did in each event from the data of the game: the high bits of the word say how far the player got, the
// rest is the score. The level is cleared when the player got further than the level.
void muEventTask::setEventRecord() {
    u32* data = reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0x32C);
    for (int set = 0; set < 2; set++) {
        for (int i = 0; i < m_lists[set].m_count; i++) {
            u32* levelData = data;
            for (u32 level = 0; level < 3; level++) {
                muEventLevel* record = &m_records[set][i].m_levels[level];
                u32 value = levelData[m_lists[set].m_entries[i].m_eventNo];
                if ((value >> 30) > level) {
                    record->m_cleared = 1;
                } else {
                    record->m_cleared = 0;
                }
                record->m_score = value & 0x3FFFFFFF;
                levelData += 0x33;
            }
        }
        data += 0x99;
    }
}

void muEventTask::createModel() {
    m_resFileMenu = nw4r::g3d::ResFile(fn_800B03F0(m_archive, 0x28));
    m_resFileWndA = nw4r::g3d::ResFile(fn_800B03F0(m_archive, 0x32));
    m_resFileWndB = nw4r::g3d::ResFile(fn_800B03F0(m_archive, 0x14));
    createObjResFile(sModels, 13, &m_resFileMenu);
    createPanelExpMdl();
    m_scnMdls[1] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
        gfHeapManager::getMEMAllocator(Heaps::MenuResource), NULL, 0xB, m_objects[2]->m_sceneModel);
    m_objects[0]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    m_objects[0]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_objects[1]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
    m_objects[1]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_objects[0x10]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_objects[0x53]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    m_objects[0x53]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_objects[0xE]->setFrameNode(1.0f);
    m_objects[0xF]->setFrameNode(0.0f);
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0". The models
// of the table fill the slots from the start up to (but not with) the end.
void muEventTask::createObjResFile(const muEventModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start;
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

// Makes the model that the icons of the events are attached to, and attaches the other panels.
// MATCH-ONLY: the game does not inline this function into the one that creates the models.
#pragma dont_inline on
void muEventTask::createPanelExpMdl() {
    nw4r::g3d::G3dObj* panel = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
        gfHeapManager::getMEMAllocator(Heaps::MenuResource), NULL, 0x51, m_objects[1]->m_sceneModel);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[0x52]->m_sceneModel, "pos98");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[0x53]->m_sceneModel, "pos99");
    if (m_mode == 1) {
        MuObject* wnd = m_objects[0x53];
        wnd->changeNodeAnimNIf("MenEvent0021_TopN__1");
        wnd->changeClrAnimNIf("MenEvent0021_TopN__1");
        wnd->setFrameNode(50.0f);
        MuObject* bg = m_objects[0];
        bg->changeNodeAnimNIf("MenEventBg_TopN__1");
        bg->changeClrAnimNIf("MenEventBg_TopN__1");
        m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objects[0x55]->m_sceneModel));
    }
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, m_objects[0x54]->m_sceneModel, "pos97");
    m_scnMdls[0] = panel;
}

#pragma dont_inline reset

// Makes the texts of the menu.
void muEventTask::initMsg() {
    MuMsg* msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg = msg;
    msg->allocMsgBuf(0x200, 0x12);
    msg->setMsgData(m_archive->getData(0, Data_Type_Misc, 0x1E, 0xFFFE));
    MuObject* panel = m_objects[1];
    msg->attachScnMdlSimple(0, panel->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(0, 0, 0, 0, 0xFF);
    msg->setVerticalMode(0, true);
    msg->attachScnMdlSimple(1, panel->m_sceneModel, 1, 0.0625f);
    msg->setFontColor(1, 0x1E, 0x1E, 0x1E, 0xFF);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 1, 32.0f);
    msg->setAlignMode(1, MuMsg::Align_Left);
    msg->attachScnMdlSimple(2, panel->m_sceneModel, 2, 0.0625f);
    msg->setFontColor(2, 0xBE, 0xB1, 0x89, 0xFF);
    msg->setAlignMode(2, MuMsg::Align_Left);
    msg->attachScnMdlSimple(3, panel->m_sceneModel, 3, 0.0625f);
    msg->setFontColor(3, 0xFA, 0xFA, 0xFA, 0xFF);
    msg->getHScale(3, 0.8f);
    msg->getWScale(3, 40.0f);
    for (int i = 0; i < 11; i++) {
        msg->attachScnMdlSimple(4 + i, m_objects[3 + i]->m_sceneModel, 0, 0.0625f);
        msg->setFontColor(4 + i, 0xFA, 0xFA, 0xFA, 0xFF);
        msg->setFontType(4 + i, 1);
    }
    msg->attachScnMdlSimple(0x10, m_objects[0x52]->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(0x10, 0, 0, 0, 0xFF);
    msg->attachScnMdlSimple(0x11, m_objects[0x53]->m_sceneModel, 0, 0.0625f);
    msg->setFontColor(0x11, 0, 0, 0, 0xFF);
}

// The frame of the model that shows the kind of the pad of a player.
static const u8 sPadTypeFrames[4] = {0, 2, 3, 1};

// The frames of the arrow animation when the cursor moves (to the right, none, to the left).
static const float sArrowFrames[3] = {19.0f, 0.0f, 10.0f};

// The buttons that open the window of the name, by kind of pad.
static const u32 sNameButtons[4] = {0x400, 0x400, 0x200000, 0x10000};

// What each player has on the screen: the model that shows the name, the text of the name.
struct muEventNameSlot {
    u8 m_object;
    int m_msg;
};
static const muEventNameSlot sNameSlots[2] = {{0x52, 0x10}, {0x53, 0x11}};

// The node of the model where the window of a player is attached, and the same slots.
struct muEventWndSlot {
    const char* m_node;
    u8 m_object;
    int m_msg;
};
static const muEventWndSlot sWndSlots[2] = {{"pos98", 0x52, 0x10}, {"pos99", 0x53, 0x11}};

// Shows the models of the menu in the scene, the names of the players that are already known, the kind of the pads and the list.
void muEventTask::initDisp() {
    int mode = m_mode;
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objects[0]->m_sceneModel));
    initPanel(mode);
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_scnMdls[0]));
    pasteThumb(m_cursor, mode);
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_scnMdls[1]));
    setCurEventListPos(m_cursor, mode);
    for (int i = 0; i < 2; i++) {
        int name = m_name[i];
        if (name >= 0) {
            char buffer[0x20];
            m_objects[sNameSlots[i].m_object]->setFrameVisible(1.0f);
            fn_8004D6C4(g_GameGlobal, name, buffer);
            m_msg->printf(sNameSlots[i].m_msg, buffer);
        }
    }
    dispPadType(0, true);
    if (m_mode == 1) {
        dispPadType(1, true);
    }
    MuObject* arrow = m_objects[2];
    arrow->setFrameNode(sArrowFrames[0]);
    arrow->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    m_arrowDir = 0;
    fn_80078DAC(lbl_805A01D8, 0x2A, 0, 0);
}

// The pad status of a controller; for the controller of all the players (no pad) it is the merge of the pads of the players.
static inline gfPadStatus getPadStatus(const muEventTask* task, int padNo) {
    gfPadStatus status;
    int count = getPlayerCount(task);
    if (padNo < 0) {
        gfPadStatus pads[2];
        for (int i = 0; i < count; i++) {
            g_gfPadSystem->getSysPadStatus(task->m_pad[i], &pads[i]);
        }
        gfPadSystem::merge(pads, count, &status);
    } else {
        g_gfPadSystem->getSysPadStatus(padNo, &status);
    }
    return status;
}

// Reads the status of the pad of a controller into the status; for the controller of all the players (no pad) it is the merge of
// the pads of the players.
static inline void fillPadStatus(const muEventTask* task, int padNo, gfPadStatus* pads, gfPadStatus* status) {
    int count = getPlayerCount(task);
    if (padNo < 0) {
        for (int i = 0; i < count; i++) {
            g_gfPadSystem->getSysPadStatus(task->m_pad[i], &pads[i]);
        }
        gfPadSystem::merge(pads, count, status);
    } else {
        g_gfPadSystem->getSysPadStatus(padNo, status);
    }
}

// The merge of the pads of the players.
static inline gfPadStatus getMergedPadStatus(const muEventTask* task) {
    gfPadStatus status;
    gfPadStatus pads[2];
    int count = getPlayerCount(task);
    for (int i = 0; i < count; i++) {
        g_gfPadSystem->getSysPadStatus(task->m_pad[i], &pads[i]);
    }
    gfPadSystem::merge(pads, count, &status);
    return status;
}

// The kind of a pad (the status is given by value).
static inline int getPadType(gfPadStatus pad) {
    return pad.m_controllerType;
}

// Shows the kind of the pad of a player. The status of the pad says whether it is connected; with none the kind is guessed
// from the number of the pad.
void muEventTask::dispPadType(int player, bool force) {
    int padNo = fn_800BA3D8(&m_controllers[player + 1]);
    gfPadStatus pad2;
    gfPadStatus pad = getPadStatus(this, padNo);
    bool connected = pad.m_error == gfPadError::NONE;
    pad2 = pad;
    if (force || connected) {
        int type;
        if (!connected) {
            if (padNo < 4) {
                type = 0;
            } else {
                type = 2;
            }
        } else {
            type = pad2.m_controllerType;
        }
        MuObject* object = (player == 0) ? m_objects[0x52] : m_objects[0x53];
        object->setFrameTex(static_cast<float>(sPadTypeFrames[type]));
    }
}

void muEventTask::processDefault() {
    for (int i = 0; i < 3; i++) {
        fn_800B9984(&m_controllers[i]);
    }
    bool run = true;
    if (static_cast<u32>(m_step - 2) <= 1) {
        bool picking = false;
        int count = 2 - (m_mode == 0);
        for (int i = 0; i < count; i++) {
            if (nameSelectMainPlayer(i)) {
                picking = true;
            }
        }
        if (picking) {
            run = false;
        }
    }
    if (run) {
        if (m_stepPrev != m_step) {
            if (sStepFuncs[m_step].m_init != NULL) {
                sStepFuncs[m_step].m_init(this);
            }
            m_stepPrev = m_step;
        }
        if (sStepFuncs[m_step].m_main != NULL) {
            m_step = sStepFuncs[m_step].m_main(this);
        }
    }
    if (m_step != Step_Loading && m_step != Step_Decided) {
        if (m_mode == 0 && m_step == Step_SelectEvent) {
            acceptIntruder();
        }
        if (static_cast<u32>(m_arrowDir - 1) <= 1) {
            float limit;
            if (m_arrowDir == 1) {
                limit = 9.0f;
            } else {
                limit = 19.0f;
            }
            if (m_objects[2]->m_modelAnim->m_anmObjChrRes->GetFrame() >= limit) {
                MuObject* arrow = m_objects[2];
                arrow->setFrameNode(sArrowFrames[0]);
                arrow->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
                m_arrowDir = 0;
            }
        }
    }
}

// Opens the window where the second player picks a name, and shows the name that was picked. It returns whether the window of
// the player is open.
bool muEventTask::nameSelectMainPlayer(int player) {
    dispPadType(player, false);
    if (m_wnds[player] == NULL) {
        int padNo = m_pad[player];
        gfPadStatus pads[2];
        gfPadStatus status;
        fillPadStatus(this, padNo, pads, &status);
        gfPadStatus copy = status;
        gfPadStatus pad;
        pad = copy;
        if ((pad.m_buttonsPressedThisFrame.bits & sNameButtons[pad.m_controllerType]) != 0) {
            g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
            m_wnds[player] = fn_16_1CB0(m_group, 2, &m_resFileWndA, &m_resFileWndB, padNo, 0x17, m_scnMdls[0], sWndSlots[player].m_node, 0, 0, 0);
            m_wndShown[player] = false;
        }
    }
    if (m_wnds[player] != NULL) {
        switch (m_wnds[player]->m_state) {
        case 1:
        case 2: {
            char buffer[0x20];
            m_name[player] = m_wnds[player]->m_value;
            m_objects[sNameSlots[player].m_object]->setFrameVisible(1.0f);
            fn_8004D6C4(g_GameGlobal, m_wnds[player]->m_value, buffer);
            m_msg->printf(sNameSlots[player].m_msg, buffer);
            if (m_wnds[player]->m_state == 1) {
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            }
            m_wnds[player]->m_unk9C = 4;
            m_wnds[player]->m_state = 0;
            m_wnds[player]->m_value = 0;
            break;
        }
        case 3:
            if (!m_wndShown[player]) {
                g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
                m_wndShown[player] = true;
            }
            break;
        case 4:
            m_wnds[player]->vf64();
            m_wnds[player] = NULL;
            break;
        }
    }
    return m_wnds[player] != NULL;
}

// Lets a pad that is not in the menu join as the second player when it presses start (in the menu of one player).
bool muEventTask::acceptIntruder() {
    if (m_mode == 1) {
        return false;
    }
    int pad;
    for (pad = 0; pad < 8; pad++) {
        if (pad == m_pad[0]) {
            continue;
        }
        gfPadStatus status = getPadStatus(this, pad);
        if ((status.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_START) != 0) {
            break;
        }
    }
    if (pad >= 8) {
        return false;
    }
    m_mode = 1;
    m_cursor = 0;
    m_pad[1] = pad;
    fn_800B98DC(&m_controllers[0], *reinterpret_cast<u8*>(reinterpret_cast<u8*>(&m_controllers[0]) + 4) | (1 << pad));
    fn_800B9804(&m_controllers[2], pad & 0xFF);
    initPanel(1);
    setCurEventListPos(m_cursor, 1);
    pasteThumb(m_cursor, 1);
    MuObject* wnd = m_objects[0x53];
    wnd->changeClrAnimNIf("MenEvent0021_TopN__1");
    wnd->changeNodeAnimNIf("MenEvent0021_TopN__1");
    MuObject* bg = m_objects[0];
    bg->changeNodeAnimNIf("MenEventBg_TopN__1");
    bg->changeClrAnimNIf("MenEventBg_TopN__1");
    m_msg->printf(2, "-");
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objects[0x54]->m_sceneModel));
    return true;
}

// The names of the lists of the events in the file of the data.
static const char* const sEventTblNames[2] = {"muEventTbl_1P", "muEventTbl_2P"};

// Reads the lists of the events (for one player and for two players) from the file, leaving out the events that are not open.
void muEventTask::makeEventList(muEventList* lists) {
    int size;
    void* data = fn_80015E34(m_archive, 0, 1, 0x3C, &size, 0xFFFE);
    utRelocate relocate(reinterpret_cast<u8*>(data), size);
    for (int set = 0; set < 2; set++) {
        const muEventEntry* src = reinterpret_cast<const muEventEntry*>(relocate.getPublicAddress(sEventTblNames[set]));
        muEventEntry* dst = lists[set].m_entries;
        int count = 0;
        for (int i = 0; src->m_msgNo != -1; i++, src++) {
            if (i > 0 && !fn_8004E7A0(g_GameGlobal, set, i)) {
                continue;
            }
            dst->m_msgNo = src->m_msgNo;
            dst->m_descNo = src->m_descNo;
            dst->m_kind1P = src->m_kind1P;
            dst->m_unk09 = src->m_unk09;
            dst->m_kind2P = src->m_kind2P;
            dst->m_flags = src->m_flags;
            dst->m_eventNo = src->m_eventNo;
            count++;
            dst++;
        }
        lists[set].m_count = count;
    }
}

// The best level that was cleared in an event (3: none).
static inline int getClearedLevel(const muEventRecord* record) {
    int level = 3;
    if (record->m_levels[0].m_cleared == 1) {
        level = 0;
    }
    if (record->m_levels[1].m_cleared == 1) {
        level = 1;
    }
    if (record->m_levels[2].m_cleared == 1) {
        level = 2;
    }
    return level;
}

// Puts the icons of the events of the list on the panel: the icon is lit when the event was cleared.
void muEventTask::initPanel(int mode) {
    nw4r::g3d::ScnGroup* panel = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_scnMdls[0]);
    for (int i = 0; i < 0x40; i++) {
        panel->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objects[0x11 + i]->m_sceneModel));
    }
    for (int i = 0; i < m_lists[mode].m_count; i++) {
        char name[8];
        sprintf(name, "pos%02d", i + 1);
        MuObject* icon = m_objects[0x11 + i];
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, icon->m_sceneModel, name);
        int level = getClearedLevel(&m_records[mode][i]);
        icon->setFrameTex(level != 3 ? 1.0f : 0.0f);
    }
}

// Puts the thumbnails of the events around the cursor on the list (the cursor is the fifth row), with their numbers and the
// mark of the level that was cleared.
void muEventTask::pasteThumb(int pos, int mode) {
    nw4r::g3d::ScnGroup* list = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_scnMdls[1]);
    char name[0x10];
    char full[0xC];
    char text[4];
    while (list->sceneItemsCount != 0) {
        list->Remove(static_cast<u32>(list->sceneItemsCount - 1));
    }
    int object = 3;
    int msg = 4;
    int row = 5;
    int index = pos;
    const muEventRecord* record = &m_records[mode][pos];
    while (row < 0xB && index < m_lists[mode].m_count) {
        MuObject* thumb = m_objects[object];
        sprintf(name, "pos%02d", row);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(list, thumb->m_sceneModel, name);
        float frame = static_cast<float>(index);
        if (mode == 1) {
            frame += 100.0f;
        }
        thumb->setFrameTex(frame);
        sprintf(text, "%d", index + 1);
        halfToFull__7MessageFPcPCc(full, text);
        m_msg->printf(msg, full);
        int level = getClearedLevel(record);
        thumb->setFrameVisible(level == 3 ? 0.0f : static_cast<float>(level + 1));
        object++;
        msg++;
        row++;
        record++;
        index++;
    }
    if (row < 0xB) {
        sprintf(name, "pos%02d", row);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(list, m_objects[0x1E]->m_sceneModel, name);
    }
    int back = pos - 1;
    record = &m_records[mode][back];
    row = 4;
    while (row >= 0 && back >= 0) {
        MuObject* thumb = m_objects[object];
        sprintf(name, "pos%02d", row);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(list, thumb->m_sceneModel, name);
        float frame = static_cast<float>(back);
        if (mode == 1) {
            frame += 100.0f;
        }
        thumb->setFrameTex(frame);
        sprintf(text, "%d", back + 1);
        halfToFull__7MessageFPcPCc(full, text);
        m_msg->printf(msg, full);
        int level = getClearedLevel(record);
        thumb->setFrameVisible(level == 3 ? 0.0f : static_cast<float>(level + 1));
        object++;
        msg++;
        row--;
        record--;
        back--;
    }
    if (row >= 0) {
        sprintf(name, "pos%02d", row);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(list, m_objects[0x1F]->m_sceneModel, name);
    }
}

// Writes the best score of a level: a time (minutes, seconds and hundredths) or a number of points, or dashes when the level was
// not cleared. It returns the length of the text.
int muEventTask::makeHighScoreMsgData(char* out, bool hasScore, u32 score, int mode, bool isTime) {
    if (isTime) {
        if (hasScore) {
            u32 minutes = score / 3600;
            u32 seconds;
            u32 hundredths;
            if (minutes >= 100) {
                minutes = 99;
                seconds = 59;
                hundredths = 99;
            } else {
                u32 rest = score - minutes * 3600;
                seconds = rest / 60;
                rest -= seconds * 60;
                hundredths = rest * 100 / 60;
            }
            return sprintf(out, "%2d:%02d %02d", minutes, seconds, hundredths);
        }
        return sprintf(out, "--:-- --");
    }
    if (hasScore) {
        u32 value = score;
        if (value >= 99999) {
            value = 99999;
        }
        return sprintf(out, "%d", value);
    }
    return sprintf(out, "-");
}

// Shows the best score of the level of the event under the cursor.
static inline void showHighScore(muEventTask* task, int level) {
    const muEventEntry* entry = &task->m_lists[task->m_mode].m_entries[task->m_cursor];
    char buffer[0x80];
    bool hasScore;
    u32 score;
    if (level != 3) {
        const muEventLevel* result = &task->m_records[task->m_mode][task->m_cursor].m_levels[level];
        hasScore = result->m_cleared;
        score = result->m_score;
    } else {
        hasScore = false;
        score = 0;
    }
    int length = task->makeHighScoreMsgData(buffer, hasScore, score, task->m_mode, entry->m_isTime);
    printData__5MuMsgFUlPCvi(task->m_msg, 1, buffer, length);
    float frame;
    if (level == 3) {
        frame = 0.0f;
    } else {
        frame = static_cast<float>(level + 1);
    }
    task->m_objects[0x54]->setFrameVisible(frame);
}

// Shows the event under the cursor: its number and name, the best score, the fighters, and the arrows that say where the list can
// be scrolled.
void muEventTask::setCurEventListPos(int pos, int mode) {
    nw4r::g3d::ScnGroup* panel = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_scnMdls[0]);
    MuObject* cursor = m_objects[0x51];
    char index[0x10];
    char name[0x80];
    panel->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(cursor->m_sceneModel));
    sprintf(index, "pos%02d", pos + 1);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(panel, cursor->m_sceneModel, index);
    muEventEntry* entry = &m_lists[mode].m_entries[pos];
    int length = sprintf(name, "#%d ", pos + 1);
    int msgLength;
    const char* title = m_msg->getMsgData(entry->m_msgNo, &msgLength);
    memcpy(name + length, title, msgLength);
    msgLength += length;
    printData__5MuMsgFUlPCvi(m_msg, 0, name, msgLength);
    m_msg->printIndex(3, entry->m_descNo, NULL);
    int level = getClearedLevel(&m_records[mode][pos]);
    if (level == 3) {
        level = 3;
    }
    showHighScore(this, level);
    int stock;
    float frame;
    if (entry->m_kind1P != 0x28) {
        stock = muMenu::exchangeMuSelchkind2MuStockchkind(entry->m_kind1P);
        frame = static_cast<float>(muMenu::exchangeGmCharacterKind2Something(stock) * 10 + 1);
    } else {
        stock = 0x3D;
        frame = 0.0f;
    }
    m_objects[1]->setFrameTex(frame);
    if (m_mode == 0) {
        if (stock != 0x3D) {
            m_msg->printf(2, muMenu::exchangeMuStockchkind2MuCharName(stock));
        } else {
            m_msg->printIndex(2, 2, NULL);
        }
    } else {
        float frame2;
        if (entry->m_kind2P != 0x28) {
            frame2 = static_cast<float>(muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeMuSelchkind2MuStockchkind(entry->m_kind2P)) * 10 + 1);
        } else {
            frame2 = 0.0f;
        }
        m_objects[0x55]->setFrameTex(frame2);
    }
    int arrows = 0;
    if (pos > 0) {
        arrows |= 1;
    }
    if (pos < m_lists[mode].m_count - 1) {
        arrows |= 2;
    }
    m_objects[1]->setFrameVisible(static_cast<float>(arrows));
}

// The setting of the choice that the scene keeps.
muEventSetting muEventTask::getScEventSetting() {
    muEventSetting setting;
    setting.m_cursor = m_cursor;
    setting.m_playerCount = 2 - (m_mode == 0);
    setting.m_level = m_level;
    setting.m_pad[0] = m_pad[0];
    setting.m_pad[1] = m_pad[1];
    setting.m_name[0] = m_name[0];
    setting.m_name[1] = m_name[1];
    setting.m_eventNo = m_lists[m_mode].m_entries[m_cursor].m_eventNo;
    return setting;
}

void muEventTask::mainStepLoadingInit(muEventTask* task) {
    muMenu::loadMenuSound();
}

int muEventTask::mainStepLoadingMain(muEventTask* task) {
    if (!muMenu::isLoadFinishMenuSound()) {
        return Step_Loading;
    }
    task->createModel();
    task->initMsg();
    task->makeEventList(task->m_lists);
    task->setEventRecord();
    task->initDisp();
    g_gfApplication->unkD0.endKeepScreen();
    return Step_Appear;
}

void muEventTask::mainStepAppearInit(muEventTask* task) {
    MuObject* panel = task->m_objects[1];
    panel->setFrameNode(0.0f);
    panel->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
}

int muEventTask::mainStepAppearMain(muEventTask* task) {
    MuObject* panel = task->m_objects[1];
    if (panel->m_modelAnim->m_anmObjChrRes->GetFrame() >= 9.0f) {
        panel->setFrameNode(9.0f);
        panel->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        if (!task->m_unk17E8) {
            fn_80074ECC(g_sndSystem, 0x1FF2, 1.0f, 1.0f);
        }
        return Step_SelectEvent;
    }
    return task->m_step;
}

void muEventTask::mainStepSelectEventInit(muEventTask* task) {
    task->pasteThumb(task->m_cursor, task->m_mode);
    task->setCurEventListPos(task->m_cursor, task->m_mode);
}

int muEventTask::mainStepSelectEventMain(muEventTask* task) {
    u32 buttons = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&task->m_controllers[0]) + 0x10);
    int count = task->m_lists[task->m_mode].m_count;
    int dir = 3;
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
        if (task->m_cursor < count - 1) {
            task->m_cursor++;
            dir = 1;
        } else {
            task->m_cursor = 0;
            dir = 0;
        }
        if (task->m_cursor == count - 1) {
            fn_800BA364(&task->m_controllers[0], 2);
        }
    }
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
        if (task->m_cursor > 0) {
            task->m_cursor--;
            dir = 2;
        } else {
            task->m_cursor = count - 1;
            dir = 0;
        }
        if (task->m_cursor == 0) {
            fn_800BA364(&task->m_controllers[0], 1);
        }
    }
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_DOWN) {
        if (task->m_cursor < count - 20) {
            task->m_cursor += 20;
            dir = 0;
        } else if (task->m_cursor >= 20) {
            while (task->m_cursor >= 20) {
                task->m_cursor -= 20;
            }
            dir = 0;
        }
        fn_800BA364(&task->m_controllers[0], 4);
    }
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_UP) {
        if (task->m_cursor >= 20) {
            task->m_cursor -= 20;
            dir = 0;
        } else if (task->m_cursor + 20 < count) {
            while (task->m_cursor + 20 < count) {
                task->m_cursor += 20;
            }
            dir = 0;
        }
        fn_800BA364(&task->m_controllers[0], 8);
    }
    if (dir != 3) {
        task->setCurEventListPos(task->m_cursor, task->m_mode);
        MuObject* arrow = task->m_objects[2];
        arrow->setFrameNode(sArrowFrames[dir]);
        arrow->m_modelAnim->m_anmObjChrRes->SetUpdateRate(dir == 0 ? 0.0f : 1.0f);
        task->m_arrowDir = dir;
        task->pasteThumb(task->m_cursor, task->m_mode);
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    gfPadStatus pad = getMergedPadStatus(task);
    if ((pad.m_buttonsPressedThisFrame.bits & (GF_PAD_BUTTON_BIT_A | GF_PAD_BUTTON_BIT_START)) != 0) {
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        return Step_SelectLevel;
    }
    gfPadStatus pad2 = getMergedPadStatus(task);
    if ((pad2.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B) != 0) {
        muEventSetting setting = task->getScEventSetting();
        scSelEvent* scene = dynamic_cast<scSelEvent*>(gfSceneManager::getInstance()->m_currentScene);
        scene->m_setting = setting;
        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        dynamic_cast<scSelEvent*>(gfSceneManager::getInstance()->m_currentScene);
        gfSceneManager::getInstance()->unk1 = 0;
        gfSceneManager::getInstance()->processStep = 2;
    }
    return task->m_step;
}

void muEventTask::mainStepSelectLevelInit(muEventTask* task) {
    task->m_group->Insert(task->m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(task->m_objects[0x10]->m_sceneModel));
    task->m_objects[0x10]->setFrameNode(static_cast<float>(task->m_level));
    task->m_objects[1]->setFrameVisible(0.0f);
    showHighScore(task, task->m_level);
}

int muEventTask::mainStepSelectLevelMain(muEventTask* task) {
    u32 buttons = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(&task->m_controllers[0]) + 0x10);
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
        if (task->m_level < 2) {
            task->m_level++;
        } else {
            task->m_level = 0;
        }
        if (task->m_level == 2) {
            fn_800BA364(&task->m_controllers[0], 2);
        }
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        task->m_objects[0x10]->setFrameNode(static_cast<float>(task->m_level));
        showHighScore(task, task->m_level);
    }
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
        if (task->m_level > 0) {
            task->m_level--;
        } else {
            task->m_level = 2;
        }
        if (task->m_level == 0) {
            fn_800BA364(&task->m_controllers[0], 1);
        }
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        task->m_objects[0x10]->setFrameNode(static_cast<float>(task->m_level));
        showHighScore(task, task->m_level);
    }
    gfPadStatus pad = getMergedPadStatus(task);
    if ((pad.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_B) != 0) {
        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        task->m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(task->m_objects[0x10]->m_sceneModel));
        return Step_SelectEvent;
    }
    gfPadStatus pad2 = getMergedPadStatus(task);
    if ((pad2.m_buttonsPressedThisFrame.bits & (GF_PAD_BUTTON_BIT_A | GF_PAD_BUTTON_BIT_START)) != 0) {
        g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
        return Step_Decided;
    }
    return task->m_step;
}

// The event is decided: the fighters of the event are set up in the scene (the ones that the event leaves open are picked at
// random) and the music is faded out.
void muEventTask::mainStepDecidedInit(muEventTask* task) {
    bool noFighter = false;
    gfSequence* sequence = gfSceneManager::getInstance()->searchSequence("sqEvent");
    scEventFighters fighters;
    if (sequence != NULL) {
        muEventSetting setting = task->getScEventSetting();
        fn_1_33094(sequence, &fighters, setting.m_eventNo, setting.m_level, setting.m_playerCount, setting.m_pad[0], setting.m_pad[1], setting.m_name[0], setting.m_name[1]);
        scEventFighter* fighter = fighters.m_fighters;
        for (int i = 0; i < fighters.m_count; i++, fighter++) {
            u8 kind = fighter->m_kind;
            if (kind == 0x3E) {
                continue;
            }
            switch (kind) {
            case 0x48:
                kind = "\x1D\x1F\x21"[randi(3)];
                break;
            case 0x4A:
                kind = "\x0E\x0F"[randi(2)];
                break;
            case 0x49:
                kind = "\x03\x04"[randi(2)];
                break;
            }
            fighter->m_kind = kind;
            u8 costume = fighter->m_costume;
            int selchk = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
            int color;
            if (selchk == 0x28) {
                color = 0;
            } else {
                color = muMenu::getFighterColorFileNo(selchk, costume);
            }
            fn_1_379C(i, kind, color);
        }
        if (fighters.m_fighters[0].m_kind == 0x3E) {
            noFighter = true;
        }
    }
    task->m_objects[0x10]->changeAnimN(1, 0x10, !noFighter, false);
    muEventSetting setting2 = task->getScEventSetting();
    scSelEvent* scene = dynamic_cast<scSelEvent*>(gfSceneManager::getInstance()->m_currentScene);
    scene->m_setting = setting2;
    if (&fighters != NULL) {
        scene->m_fighters = fighters;
    }
    task->m_unk17E9 = !noFighter;
    task->m_wait = 2;
    if (task->m_unk17E9) {
        fn_80073F84(g_sndSystem, 0);
        g_sndSystem->playSE(static_cast<SndID>(0x16), -1, 0, 0, -1);
    }
}

int muEventTask::mainStepDecidedMain(muEventTask* task) {
    bool done = false;
    if (--task->m_wait >= 0) {
        return task->m_step;
    }
    if (task->m_unk17E9) {
        if (fn_1_385C(0)) {
            done = true;
        }
    } else {
        done = true;
    }
    if (done) {
        dynamic_cast<scSelEvent*>(gfSceneManager::getInstance()->m_currentScene);
        gfSceneManager::getInstance()->unk1 = 1;
        gfSceneManager::getInstance()->processStep = 2;
    }
    return task->m_step;
}
