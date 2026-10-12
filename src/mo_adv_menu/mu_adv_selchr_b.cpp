#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_adv_selchr_b.h>
#include <ut/ut_relocate.h>

// main.dol functions with names that the shared headers do not declare.
// muMenuController::getControllerID()
extern "C" int fn_800BA3D8(muMenuController* controller);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// muMenuController::exchangeDir8ToVec(direction): the vector of one of the eight directions of the stick
struct muAdvSelchrBDirVec {
    float m_x;
    float m_y;

};
extern "C" muAdvSelchrBDirVec fn_800BA0E0(int direction);
// gfSceneRoot::layerUpdateFrame(layer)
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
// nw4r::g3d::ScnRoot::UpdateFrame()
extern "C" void UpdateFrame__Q34nw4r3g3d7ScnRootFv(void* scnRoot);

// The flag of the menu data (of the record of the game) that says that the screen is wide.
// HYPOTHESIS: the name; the flag is the high bit of the byte at 0x28.
struct muAdvSelchrBMenuBits {
    u8 m_unk00[0x28];
    bool m_isWide : 1;
};

// The init and main function of each step.
static const muAdvSelchrBTask::StepFunc sStepFuncs[muAdvSelchrBTask::Step_Count] = {
    {muAdvSelchrBTask::stepAppearInit, muAdvSelchrBTask::stepAppearMain},
    {muAdvSelchrBTask::stepSelectInit, muAdvSelchrBTask::stepSelectMain},
    {muAdvSelchrBTask::stepResultInit, muAdvSelchrBTask::stepResultMain},
    {muAdvSelchrBTask::stepEndInit, muAdvSelchrBTask::stepEndMain},
};

// A list of the characters of the file of the lists: how many characters each player picks and the characters.
struct muAdvSelchrBList {
    u8 m_pickCount;
    muAdvSelchrBChar* m_chars;
};

muAdvSelchrBTask* muAdvSelchrBTask::create(const muAdvSelchrBTaskParam* param, Heaps::HeapType instanceHeap,
                                           Heaps::HeapType resourceHeap) {
    muAdvSelchrBTask* task = new (instanceHeap) muAdvSelchrBTask();
    task->m_instanceHeap = instanceHeap;
    task->m_resourceHeap = resourceHeap;
    task->m_mode = param->m_mode;
    if (param->m_pad1 >= 0) {
        fn_800B96E8(&task->m_players[0].m_controller, param->m_pad1 & 0xFF);
    }
    if (param->m_pad2 >= 0) {
        fn_800B96E8(&task->m_players[1].m_controller, param->m_pad2 & 0xFF);
    }
    task->m_result = static_cast<muAdvSelchrBResult*>(param->m_result);
    task->m_handles[0].readRequest("/menu/adventure/selchrb.brres", task->m_resourceHeap, 0, 0);
    task->m_handles[1].readRequest("/menu/adventure/selchr_common.brres", task->m_resourceHeap, 0, 0);
    task->m_handles[2].readRequest("/menu/adventure/mu_adv_selchrb_tbl.dat", task->m_resourceHeap, 0, 0);
    task->m_loaded = false;
    return task;
}

muAdvSelchrBTask::muAdvSelchrBTask()
    : gfTask("muAdvSelchrB", Category_Menu, 0xF, 8, true), m_resFileCommon(static_cast<void*>(NULL)),
      m_resFileMenu(static_cast<void*>(NULL)), m_resFileUnk(static_cast<void*>(NULL)), m_step(Step_Appear), m_stepPrev(Step_Count), m_charCount(0), m_state(0),
      m_result(NULL) {
    m_loaded = false;
    m_mode = 0;
    unk2C_b1 = false;
    m_resFileCommon = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileMenu = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileUnk = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    for (int i = 0; i < 26; i++) {
        m_objects[i] = NULL;
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 10; j++) {
            m_players[i].m_picked[j] = 0;
        }
        m_players[i].m_pickedCount = 0;
        m_players[i].m_cursor = 0;
    }
    m_maxPick[0] = 0;
    m_maxPick[1] = 0;
}

muAdvSelchrBTask::~muAdvSelchrBTask() {
    for (int i = 0; i < 26; i++) {
        delete m_objects[i];
    }
    if (m_resFileCommon.ptr() != NULL) {
        gfHeapManager::free(m_resFileCommon.ptr());
    }
    if (m_resFileMenu.ptr() != NULL) {
        gfHeapManager::free(m_resFileMenu.ptr());
    }
    if (m_resFileUnk.ptr() != NULL) {
        gfHeapManager::free(m_resFileUnk.ptr());
    }
    if (m_msg != NULL) {
        delete m_msg;
    }
    for (int i = 0; i < 3; i++) {
        m_handles[i].cancelRequest();
    }
}

// The position of the models of the cursors.
static inline Vec3f* getTrans(MuObject* object) {
    return reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(object) + 0x3C);
}

inline bool muAdvSelchrBTask::hasPad(int player) {
    return fn_800BA3D8(&m_players[player].m_controller) >= 0;
}

inline bool muAdvSelchrBTask::isNumbered(int pos) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < m_players[i].m_pickedCount; j++) {
            if (pos == m_players[i].m_picked[j]) {
                return true;
            }
        }
    }
    return false;
}

inline bool muAdvSelchrBTask::isFree(int pos, int player) {
    if (isNumbered(pos)) {
        return false;
    }
    for (int i = player - 1; i >= 0; i--) {
        if (pos == m_players[i].m_cursor) {
            return false;
        }
    }
    return true;
}

// Takes the buffer of a file that was read (it is NULL when the read failed).
static void* takeBuffer(gfFileIOHandle* handle) {
    void* buffer;
    if (handle->getReturnStatus() == 0x15) {
        buffer = NULL;
    } else {
        buffer = handle->getBuffer();
        handle->release();
    }
    return buffer;
}

// The status of the pad of the controller (nothing is pressed when the controller has no pad).
static inline gfPadStatus getPadStatus(muMenuController* controller) {
    gfPadStatus status;
    int padNo = fn_800BA3D8(controller);
    if (padNo >= 0) {
        g_gfPadSystem->getSysPadStatus(padNo, &status);
    } else {
        memset(&status, 0, sizeof(status));
    }
    return status;
}

// The frame of the picture of the type of the pad.
static inline float getPadFrame(muMenuController* controller) {
    gfPadStatus status = getPadStatus(controller);
    return static_cast<float>(static_cast<u32>(muAdvSelchrBTask::sPadTypeFrames[status.m_controllerType]));
}

// Whether the screen is wide.
static inline bool isWide() {
    return reinterpret_cast<muAdvSelchrBMenuBits*>(g_GameGlobal->getGlobalRecordMenuDatap())->m_isWide;
}

// Creates the models of the menu, the texts and the lists of the characters, and puts the first models in the scene.
void muAdvSelchrBTask::startSelChar() {
    nw4r::g3d::ResFile file(static_cast<void*>(NULL));
    void* buffer = takeBuffer(&m_handles[0]);
    if (buffer != NULL) {
        file = nw4r::g3d::ResFile(buffer);
        nw4r::g3d::ResFile::Init(&file);
    }
    m_resFileMenu = file;
    createObjResFile(sMenuModels, 2, &m_resFileMenu);

    nw4r::g3d::ResFile file2(static_cast<void*>(NULL));
    buffer = takeBuffer(&m_handles[1]);
    if (buffer != NULL) {
        file2 = nw4r::g3d::ResFile(buffer);
        nw4r::g3d::ResFile::Init(&file2);
    }
    m_resFileCommon = file2;
    createObjResFile(sCommonModels, 5, &m_resFileCommon);

    if (!isWide()) {
        mulScale(0.75f);
    }

    MuMsg* msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg = msg;
    msg->allocMsgBuf(0x100, 2);
    for (int i = 0; i < 2; i++) {
        msg->attachScnMdlSimple(i, m_objects[24 + i]->m_sceneModel, 0, 0.0625f);
        msg->setEdgeEnable(i, true);
    }

    int count;
    if (m_mode != 0xFFFF) {
        count = loadSettingTable(m_mode & 0xFF);
        if ((m_mode & 0x100) != 0) {
            if (fn_800BA3D8(&m_players[1].m_controller) >= 0) {
                count++;
            }
        }
    } else {
        count = loadSettingTable(0);
    }
    m_maxPick[0] = count;
    m_maxPick[1] = 0;
    if (fn_800BA3D8(&m_players[1].m_controller) >= 0 && m_maxPick[0] > 1) {
        m_maxPick[0]--;
        m_maxPick[1]++;
    }
    m_players[0].m_cursor = 0;
    if (m_maxPick[1] > 0) {
        m_players[1].m_cursor = 1;
    } else {
        m_players[1].m_cursor = 0;
    }

    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_objects[1]->m_sceneModel);
    for (int i = 0; i < 10; i++) {
        m_objects[14 + i]->setFrameMatCol(1.0f);
    }
    m_objects[1]->setFrameTex(getPadFrame(&m_players[0].m_controller));
}

// The models of the menu and the models that are shared with the other menus of the adventure mode: the name, the slots of
// the task it fills (start, end: the end is not a slot of the model) and the draw priority.
const muAdvSelchrBModel muAdvSelchrBTask::sMenuModels[] = {
    {"MenAdvChrB0000_TopN", 1, 1, 0x11},
    {"MenAdvChrB0001_TopN", 0x18, 0x1A, 0x11},
};
const muAdvSelchrBModel muAdvSelchrBTask::sCommonModels[] = {
    {"MenAdvChrCmn1000_TopN", 0, 0, 0x10},
    {"MenAdvChrCmn0000_TopN", 2, 2, 0x12},
    {"MenAdvChrCmn0001_TopN", 3, 3, 0x12},
    {"MenAdvChrCmn0002_TopN", 4, 0xE, 0x13},
    {"MenAdvChrCmn0002_TopN", 0xE, 0x18, 0x13},
};

// Scales the models of the cursors and of the picks (the screen is not wide).
void muAdvSelchrBTask::mulScale(float scale) {
    Vec3f scl(scale, scale, 1.0f);
    for (int i = 24; i < 26; i++) {
        m_objects[i]->setObjScale(&scl);
    }
    m_objects[2]->setObjScale(&scl);
    m_objects[3]->setObjScale(&scl);
    for (int i = 4; i < 14; i++) {
        m_objects[i]->setObjScale(&scl);
    }
    for (int i = 14; i < 24; i++) {
        m_objects[i]->setObjScale(&scl);
    }
}

// Reads one of the lists of the characters from the file of the lists and returns how many characters the players pick.
int muAdvSelchrBTask::loadSettingTable(int index) {
    int size = m_handles[2].getSize();
    void* buffer = takeBuffer(&m_handles[2]);
    utRelocate relocate(static_cast<u8*>(buffer), size);
    muAdvSelchrBList* lists = static_cast<muAdvSelchrBList*>(relocate.getPublicAddress("muAdvSelchrB_AllTbl"));
    muAdvSelchrBList* list = &lists[index];
    muAdvSelchrBChar* src = list->m_chars;
    float scale = 0.75f;
    int n = 0;
    for (; src->m_kind >= 0; n++, src++) {
        m_chars[n] = *src;
        if (!isWide()) {
            m_chars[n].m_pos[0] *= scale;
            m_chars[n].m_pos[1] *= scale;
            m_chars[n].m_namePos[0] *= scale;
            m_chars[n].m_namePos[1] *= scale;
        }
    }
    m_charCount = n;
    gfHeapManager::free(buffer);
    return list->m_pickCount;
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muAdvSelchrBTask::createObjResFile(const muAdvSelchrBModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start;
        }
        for (int j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, models->m_name, models->m_priority, NULL, m_resourceHeap);
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

// Frames of the picture of the type of the pad, in the order of the types of the pads.
const u8 muAdvSelchrBTask::sPadTypeFrames[4] = {0, 2, 3, 1};

// Waits for the files, then runs the step, moves the cursors and updates the models.
void muAdvSelchrBTask::processDefault() {
    if (!m_loaded) {
        bool ready = true;
        for (int i = 0; i < 3; i++) {
            if (!m_handles[i].isReady()) {
                ready = false;
                break;
            }
        }
        if (!ready) {
            return;
        }
        startSelChar();
        m_loaded = true;
        m_state = 1;
        if (m_mode == 0xFFFF) {
            m_step = Step_Result;
        }
    }
    for (int i = 0; i < 2; i++) {
        fn_800B9984(&m_players[i].m_controller);
    }
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
        if (hasPad(i)) {
            moveOneCursorMain(i);
        }
    }
    gfSceneRoot* root = g_gfSceneRoot;
    UpdateFrame__Q34nw4r3g3d7ScnRootFv(*reinterpret_cast<void**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x44));
    fn_8000DB40(root, 8);
}

#pragma inline_max_size(1000)
static inline void showName(muAdvSelchrBTask* task, int player, int pos) {
    int place = 3;
    if (player == 0) {
        place = 0;
        if (task->hasPad(1) && task->m_maxPick[1] <= 0) {
            place = 2;
        }
    } else if (task->m_maxPick[1] > 0) {
        place = 1;
    }
    if (place != 3) {
        float y = task->m_chars[pos].m_namePos[1];
        float x = task->m_chars[pos].m_namePos[0];
        char name[0x100];
        sprintf(name, "%s", muMenu::exchangeMuStockchkind2MuCharName(muMenu::exchangeMuSelchkind2MuStockchkind(task->m_chars[pos].m_kind)));
        task->m_msg->printf(place == 1, name);
        g_gfSceneRoot->remove(task->m_objects[place == 1 ? 25 : 24]->m_sceneModel);
        MuObject* nameModel = task->m_objects[place == 1 ? 25 : 24];
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, nameModel->m_sceneModel);
        Vec3f pos3(x, y, 0.0f);
        nameModel->setTrans(&pos3);
    }
}

// Shows the mark of a pick (its number) on the character.
static inline void putMark(muAdvSelchrBTask* task, int slot, int number, int pos) {
    MuObject* mark = task->m_objects[slot];
    mark->setFrameTex(static_cast<float>(number));
    Vec3f markPos = *getTrans(mark);
    Vec2f charPos = task->m_chars[pos].getPos();
    markPos.m_x = charPos.m_x;
    markPos.m_y = charPos.m_y;
    mark->setTrans(&markPos);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, mark->m_sceneModel);
}

// Moves the model of the cursor of the player toward the character that the cursor is on.
void muAdvSelchrBTask::moveOneCursorMain(int player) {
    MuObject* cursor = m_objects[2 + player];
    Vec3f* trans = getTrans(cursor);
    Vec3f pos;
    pos = *trans;
    Vec2f delta;
    Vec2f current(pos.m_x, pos.m_y);
    Vec2f targetPos = m_chars[m_players[player].m_cursor].getPos();
    delta = targetPos - current;
    if (delta.length() < 0.01f) {
        pos.m_x = targetPos.m_x;
        pos.m_y = targetPos.m_y;
        cursor->setTrans(&pos);
    } else {
        delta.m_x *= 0.3f;
        delta.m_y *= 0.3f;
        current.m_x += delta.m_x;
        current.m_y += delta.m_y;
        pos.m_x = current.m_x;
        pos.m_y = current.m_y;
        cursor->setTrans(&pos);
    }
}

// The first character that no player has picked (-1: there is none).
int muAdvSelchrBTask::getNotNumberingPos() {
    int used = 0;
    for (int i = 0; i < 2; i++) {
        if (hasPad(i)) {
            for (int j = 0; j < m_players[i].m_pickedCount; j++) {
                used |= 1 << m_players[i].m_picked[j];
            }
        }
    }
    int result = -1;
    if ((used & 0x001) == 0) {
        result = 0;
    } else if ((used & 0x002) == 0) {
        result = 1;
    } else if ((used & 0x004) == 0) {
        result = 2;
    } else if ((used & 0x008) == 0) {
        result = 3;
    } else if ((used & 0x010) == 0) {
        result = 4;
    } else if ((used & 0x020) == 0) {
        result = 5;
    } else if ((used & 0x040) == 0) {
        result = 6;
    } else if ((used & 0x080) == 0) {
        result = 7;
    } else if ((used & 0x100) == 0) {
        result = 8;
    } else if ((used & 0x200) == 0) {
        result = 9;
    }
    return result;
}

// Moves the cursor of the player, picks the character that the cursor is on and takes the last pick back.
void muAdvSelchrBTask::selectMain(int player) {
    PlayerWork* work = &m_players[player];
    int cursor = work->m_cursor;
    int pos = cursor;
    gfPadStatus pad;
    int padNo = fn_800BA3D8(&work->m_controller);
    if (padNo >= 0) {
        g_gfPadSystem->getSysPadStatus(padNo, &pad);
    } else {
        memset(&pad, 0, sizeof(pad));
    }
    gfPadStatus status = pad;
    if (work->m_pickedCount < m_maxPick[player]) {
        int next = getNextPos(player, pos);
        if (next != pos) {
            work->m_cursor = next;
            showName(this, player, next);
            pos = next;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
    }
    u32 pressed = status.m_buttonsPressedThisFrame2.bits;
    if ((pressed & 0x1100) != 0 && work->m_pickedCount < m_maxPick[player]) {
        if (!isNumbered(pos)) {
            int count = work->m_pickedCount;
            work->m_picked[count] = pos;
            work->m_pickedCount = count + 1;
            int base = 14;
            if (player == 0) {
                base = 4;
            }
            putMark(this, base + count, count + 1, pos);
            if (m_maxPick[player] - (count + 1) == 1 && m_charCount - (count + 1) == 1) {
                int last = getNotNumberingPos();
                putMark(this, base + count + 1, count + 2, last);
                work->m_picked[count + 1] = last;
                work->m_pickedCount = count + 2;
            } else if (work->m_pickedCount < m_maxPick[player]) {
                while (!isFree(pos, player)) {
                    pos++;
                    if (pos >= m_charCount) {
                        pos = 0;
                    }
                }
                work->m_cursor = pos;
                showName(this, player, pos);
            }
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        }
    }
    if (work->m_pickedCount < m_maxPick[player]) {
        int freeCount = 0;
        for (int i = 0; i < m_charCount; i++) {
            if (!isNumbered(i)) {
                freeCount++;
            }
        }
        if (freeCount <= 1) {
            int count = work->m_pickedCount;
            work->m_picked[count] = pos;
            work->m_pickedCount = count + 1;
            int base = 14;
            if (player == 0) {
                base = 4;
            }
            putMark(this, base + count, count + 1, pos);
            return;
        }
    }
    if ((pressed & 0x200) != 0 && work->m_pickedCount > 0) {
        int base = 14;
        if (player == 0) {
            base = 4;
        }
        g_gfSceneRoot->remove(m_objects[base + work->m_pickedCount - 1]->m_sceneModel);
        work->m_pickedCount--;
        int last = work->m_picked[work->m_pickedCount];
        work->m_cursor = last;
        showName(this, player, last);
        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
    }
    int next = player + 1;
    if (next < 2 && hasPad(next)) {
        int other = m_players[next].m_cursor;
        if (m_maxPick[next] > 0) {
            if (m_maxPick[next] > m_players[next].m_pickedCount) {
                if (!isFree(other, next)) {
                    if (isFree(cursor, next)) {
                        other = cursor;
                    } else {
                        do {
                            other++;
                            if (other >= m_charCount) {
                                other = 0;
                            }
                        } while (!isFree(other, next));
                    }
                }
            }
        } else {
            other = pos;
        }
        m_players[next].m_cursor = other;
        showName(this, next, other);
    }
}

// The character that the stick of the controller of the player points to: the one that is closest to the direction of
// the stick, among the characters that are free for the cursor (the cursor stays when there is none).
int muAdvSelchrBTask::getNextPos(int player, int pos) {
    float best = 0.0f;
    int next = pos;
    int direction = *reinterpret_cast<int*>(reinterpret_cast<u8*>(&m_players[player].m_controller) + 0x94);
    if (direction == 8) {
        return pos;
    }
    muAdvSelchrBDirVec dirVec = fn_800BA0E0(direction);
    Vec2f dir(dirVec.m_x, dirVec.m_y);
    Vec2f delta;
    Vec2f current = m_chars[pos].getPos();
    muAdvSelchrBChar* target = m_chars;
    for (int i = 0; i < m_charCount; i++, target++) {
        if (i == pos) {
            continue;
        }
        if (!isFree(i, player)) {
            continue;
        }
        Vec2f targetPos = target->getPos();
        delta = targetPos - current;
        float length = delta.length();
        if (length <= 0.0f) {
            continue;
        }
        delta.normalize();
        float dot = dir.m_x * delta.m_x + dir.m_y * delta.m_y;
        if (dot <= 0.0f) {
            continue;
        }
        float score = dot / length;
        if (score > best) {
            next = i;
            best = score;
        }
    }
    return next;
}

// Prints the result of the picks.
// HYPOTHESIS: the game prints this only in the builds for the debugging; nothing calls it and the linker leaves it out, but
// the strings stay in the data.
void muAdvSelchrBTask::dumpResult() {
    OSReport("--- Character select Type-B Result ---\n");
    OSReport("1P selected character\n");
    for (int i = 0; i < m_result->m_count; i++) {
        OSReport(" (%d) : %d\n", i, m_result->m_kind[i]);
    }
    OSReport("2P selected character\n");
    if (m_result->m_pad5C > 0) {
        OSReport(" (%d) : %d\n", 0, m_result->m_unk54);
    } else {
        OSReport(" (-) : %d\n", m_result->m_unk54);
    }
    OSReport("seal character\n");
    OSReport("       %d\n", m_result->m_unk54);
}

// Stores what the players picked in the result of the scene.
void muAdvSelchrBTask::storeResult() {
    if (m_result == NULL) {
        return;
    }
    memset(m_result, 0, sizeof(*m_result));
    for (int i = 0; i < m_players[0].m_pickedCount; i++) {
        m_result->m_kind[i] = m_chars[m_players[0].m_picked[i]].m_kind;
        m_result->m_unk28[i] = 0x3E;
    }
    m_result->m_count = m_players[0].m_pickedCount;
    m_result->m_pad5C = m_players[1].m_pickedCount;
    if (hasPad(1)) {
        int pick;
        if (m_players[1].m_pickedCount == 0) {
            pick = m_players[0].m_picked[0];
        } else {
            pick = m_players[1].m_picked[0];
        }
        m_result->m_unk54 = m_chars[pick].m_kind;
        m_result->m_unk58 = 0x3E;
    } else {
        m_result->m_unk54 = 0x28;
        m_result->m_unk58 = 0x3E;
    }
}

void muAdvSelchrBTask::stepAppearInit(muAdvSelchrBTask* task) {
    MuObject* object = task->m_objects[1];
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
}

// Waits for the animation of the frame.
int muAdvSelchrBTask::stepAppearMain(muAdvSelchrBTask* task) {
    MuObject* object = task->m_objects[1];
    if (object->isNodeAnimFinished() && object->isClrAnimFinished()) {
        return Step_Select;
    }
    return task->m_step;
}

// Puts the models of the cursors in the scene, where the cursors are, and shows the names.
void muAdvSelchrBTask::stepSelectInit(muAdvSelchrBTask* task) {
    for (int i = 0; i < 2; i++) {
        if (task->hasPad(i)) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[2 + i]->m_sceneModel);
            MuObject* cursor = task->m_objects[2 + i];
            Vec3f pos;
            pos = *getTrans(cursor);
            Vec2f charPos = task->m_chars[task->m_players[i].m_cursor].getPos();
            pos.m_x = charPos.m_x;
            pos.m_y = charPos.m_y;
            cursor->setTrans(&pos);
            showName(task, i, task->m_players[i].m_cursor);
        }
    }
    if (task->hasPad(1) && task->m_maxPick[1] <= 0) {
        MuObject* cursor = task->m_objects[3];
        cursor->changeAnimN(0, 0x11, false, false);
        cursor->setFrameNode(1.0f);
        cursor->setFrameMatCol(1.0f);
    }
}

// Runs the cursors of the players; the step ends when all the picks are made.
int muAdvSelchrBTask::stepSelectMain(muAdvSelchrBTask* task) {
    for (int i = 0; i < 10; i++) {
        task->m_objects[14 + i]->setFrameMatCol(1.0f);
    }
    task->m_objects[1]->setFrameTex(getPadFrame(&task->m_players[0].m_controller));
    for (int i = 0; i < 2; i++) {
        if (task->hasPad(i)) {
            task->selectMain(i);
            g_gfSceneRoot->remove(task->m_objects[2 + i]->m_sceneModel);
            if (i == 0) {
                if (task->m_players[i].m_pickedCount < task->m_maxPick[i]) {
                    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[2 + i]->m_sceneModel);
                }
            } else if (task->m_players[i].m_pickedCount < task->m_maxPick[i]
                       || (task->m_maxPick[1] <= 0 && task->m_players[0].m_pickedCount < task->m_maxPick[0])) {
                g_gfSceneRoot->add(gfSceneRoot::Layer_Info, task->m_objects[2 + i]->m_sceneModel);
            }
        }
    }
    if (task->m_players[0].m_pickedCount >= task->m_maxPick[0] && task->m_players[1].m_pickedCount >= task->m_maxPick[1]) {
        for (int i = 0; i < 2; i++) {
            if (task->hasPad(i)) {
                showName(task, i, task->m_players[i].m_picked[0]);
            }
        }
        return Step_Result;
    }
    return task->m_step;
}

// Stores the result and shows the frame of the result.
void muAdvSelchrBTask::stepResultInit(muAdvSelchrBTask* task) {
    task->storeResult();
    MuObject* object = task->m_objects[1];
    object->changeNodeAnimN("MenAdvChrB0000_TopN__1");
    object->changeClrAnimN("MenAdvChrB0000_TopN__1");
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    task->m_timer = 0;
}

// Waits 120 frames.
int muAdvSelchrBTask::stepResultMain(muAdvSelchrBTask* task) {
    task->m_timer++;
    if (task->m_timer >= 0x78) {
        task->m_state = 2;
        return Step_End;
    }
    return task->m_step;
}

void muAdvSelchrBTask::stepEndInit(muAdvSelchrBTask* task) { }

int muAdvSelchrBTask::stepEndMain(muAdvSelchrBTask* task) {
    return task->m_step;
}

bool muAdvSelchrBTask::isSelected() {
    if (m_step >= Step_Result && m_stepPrev >= Step_Result) {
        return true;
    }
    return false;
}
