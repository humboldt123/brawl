#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_adv_difficulty.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);
// gfSceneRoot::layerUpdateFrame(layer) and gfSceneRoot::removeInfoCamResAnm()
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
// nw4r::g3d::ScnRoot::UpdateFrame()
extern "C" void UpdateFrame__Q34nw4r3g3d7ScnRootFv(void* scnRoot);

// One model of the menu: the name, the slots of the task it fills (start, end), and the draw priority.
struct muAdvDifficultyModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The flag of the menu data (of the record of the game) that says that the screen is wide.
// HYPOTHESIS: the name; the flag is the high bit of the byte at 0x28.
struct muAdvDifficultyMenuBits {
    u8 m_unk00[0x28];
    bool m_isWide : 1;
};

// A model with a place to attach other models to: the slots of the task it fills (start, end), the model it is made
// from, and how many models can be attached.
struct muAdvDifficultyScnMdl {
    u8 m_start;
    u8 m_end;
    u8 m_objectNo;
    u8 m_capacity;
};

// The init and main function of each step.
static const muAdvDifficultyTask::StepFunc sStepFuncs[muAdvDifficultyTask::Step_Count] = {
    {muAdvDifficultyTask::mainStepLoadInit, muAdvDifficultyTask::mainStepLoadMain},
    {muAdvDifficultyTask::mainStepAppearInit, muAdvDifficultyTask::mainStepAppearMain},
    {muAdvDifficultyTask::mainStepSelectInit, muAdvDifficultyTask::mainStepSelectMain},
    {muAdvDifficultyTask::mainStepSelectedInit, muAdvDifficultyTask::mainStepSelectedMain},
};

muAdvDifficultyTask* muAdvDifficultyTask::create(const muAdvDifficultyTaskParam* param) {
    muAdvDifficultyTask* task = new (Heaps::MenuInstance) muAdvDifficultyTask();
    task->m_pad0 = param->m_pad0;
    task->m_pad1 = param->m_pad1;
    task->m_difficulty = param->m_difficulty;
    fn_800B96E8(&task->m_controllers[0], param->m_pad0 & 0xFF);
    if (task->m_pad1 >= 0) {
        fn_800B96E8(&task->m_controllers[1], param->m_pad1 & 0xFF);
    }
    return task;
}

muAdvDifficultyTask::muAdvDifficultyTask()
    : gfTask("muAdvDifficulty", Category_Menu, 0xF, 8, true), m_result(0), m_pad0(0), m_pad1(0), m_inputWait(0), m_unk1C4(0),
      m_step(Step_Load), m_stepPrev(Step_Count), m_isSelected(true) {
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_unk1C4 = 0;
    m_objects[0] = NULL;
    m_objects[1] = NULL;
    m_scnMdls[0] = NULL;
    m_scnMdls[1] = NULL;
    m_loaded[0] = false;
    m_loaded[1] = false;
    m_loaded[2] = false;
    m_handles[0].readRequest("/menu/adventure/sellv.brres", Heaps::MenuResource, 0, 0);
    // HYPOTHESIS: the byte at 0x28 of the menu data says that the screen is wide
    if (!reinterpret_cast<muAdvDifficultyMenuBits*>(g_GameGlobal->getGlobalRecordMenuDatap())->m_isWide) {
        m_handles[1].readRequest("/menu/defaultcamera/CharacterSelect.brres", Heaps::MenuResource, 0, 0);
    } else {
        m_handles[1].readRequest("/menu/defaultcamera/CharacterSelectWide.brres", Heaps::MenuResource, 0, 0);
    }
    m_handles[2].readRequest("/menu/msg/adv_sellv.msbin", Heaps::MenuResource, 0, 0);
    muMenu::loadMenuSound();
}

muAdvDifficultyTask::~muAdvDifficultyTask() {
    m_msg->detachMuObject(m_objects[0]);
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
    for (int i = 0; i < 2; i++) {
        if (m_scnMdls[i] != NULL) {
            m_scnMdls[i]->Destroy();
        }
    }
    for (int i = 0; i < 2; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
        }
    }
    fn_8000D7B8(g_gfSceneRoot);
    for (int i = 0; i < 3; i++) {
        m_handles[i].cancelRequest();
        gfHeapManager::free(m_handles[i].getBuffer());
        m_handles[i].release();
    }
}

void muAdvDifficultyTask::processDefault() {
    if (m_stepPrev != m_step) {
        if (sStepFuncs[m_step].m_init != NULL) {
            sStepFuncs[m_step].m_init(this);
        }
        m_stepPrev = m_step;
    }
    if (sStepFuncs[m_step].m_main != NULL) {
        m_step = sStepFuncs[m_step].m_main(this);
    }
    fn_800B9984(&m_controllers[0]);
    fn_800B9984(&m_controllers[1]);
    controllProc();
    gfSceneRoot* root = g_gfSceneRoot;
    UpdateFrame__Q34nw4r3g3d7ScnRootFv(*reinterpret_cast<void**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x44));
    fn_8000DB40(root, 8);
}

// Moves the cursor with up and down and takes the level with A (B goes back); both players can do it.
void muAdvDifficultyTask::controllProc() {
    if (m_inputWait > 0) {
        m_inputWait--;
        if (m_inputWait > 0) {
            return;
        }
    }
    if (m_step == Step_Select) {
        u32 buttons[2];
        gfPadStatus pads[2];
        u8* controller = reinterpret_cast<u8*>(this);
        for (int i = 0; i < 2; i++) {
            buttons[i] = *reinterpret_cast<u32*>(controller + 0x64);
            g_gfPadSystem->getSysPadStatus(m_pad0, &pads[0]);
            if (m_pad1 >= 0) {
                g_gfPadSystem->getSysPadStatus(m_pad1, &pads[1]);
            } else {
                pads[1].init();
            }
            int old = m_difficulty;
            int selected = old;
            if ((buttons[i] & 8) != 0 && old > 0) {
                selected = old - 1;
            }
            if ((buttons[i] & 4) != 0 && selected < 4) {
                selected = selected + 1;
            }
            if (selected != old) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
                m_difficulty = selected;
            }
            if ((pads[i].m_buttonsPressedThisFrame.bits & 0x100) != 0) {
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                m_step = Step_Selected;
                m_isSelected = true;
            }
            if ((pads[i].m_buttonsPressedThisFrame.bits & 0x200) != 0) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                m_step = Step_Selected;
                m_isSelected = false;
            }
            controller += 0xA8;
        }
    }
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muAdvDifficultyTask::createObjResFile(const muAdvDifficultyModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++) {
        const muAdvDifficultyModel* model = &models[i];
        int num = 1;
        if (model->m_start < model->m_end) {
            num = model->m_end - model->m_start;
        }
        for (int j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, model->m_name, model->m_priority, NULL, Heaps::MenuInstance);
            m_objects[model->m_start + j] = object;
            char animName[0x40];
            strcpy(animName, model->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muAdvDifficultyTask::mainStepLoadInit(muAdvDifficultyTask* task) { }

// Waits for the files and the menu sound, then creates the models and the texts of the levels.
int muAdvDifficultyTask::mainStepLoadMain(muAdvDifficultyTask* task) {
    // The levels (slot 0) and the cursor (slot 1).
    static const muAdvDifficultyModel sModels[] = {
        {"MenAdvLvl0000_TopN", 0, 0, 0x84},
        {"MenAdvLvl0001_TopN", 1, 1, 0x85},
    };
    // The cursor is attached to the levels.
    static const muAdvDifficultyScnMdl sScnMdls[] = {
        {0, 0, 0, 1},
    };
    for (int i = 0; i < 3; i++) {
        if (!task->m_loaded[i] && task->m_handles[i].m_request != NULL) {
            if (!task->m_handles[i].isReady()) {
                return task->m_step;
            }
            task->m_loaded[i] = true;
        }
    }
    if (!muMenu::isLoadFinishMenuSound()) {
        return task->m_step;
    }
    task->m_resFile = nw4r::g3d::ResFile(task->m_handles[0].getBuffer());
    nw4r::g3d::ResFile::Init(&task->m_resFile);
    task->createObjResFile(sModels, 2, &task->m_resFile);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    for (int i = 0; i < 1; i++) {
        const muAdvDifficultyScnMdl* entry = &sScnMdls[i];
        int num = entry->m_end - entry->m_start;
        if (num <= 0) {
            num = 1;
        }
        for (int j = 0; j < num; j++) {
            task->m_scnMdls[entry->m_start + j] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
                allocator, NULL, entry->m_capacity, task->m_objects[entry->m_objectNo + j]->m_sceneModel);
        }
    }
    task->m_msg = MuMsg::create(2, Heaps::MenuInstance, Heaps::MenuResource);
    task->m_msg->allocMsgBuf(0x100, 5);
    task->m_msg->setMsgData(task->m_handles[2].getBuffer());
    for (int i = 0; i < 5; i++) {
        task->m_msg->attachScnMdlSimple(i, task->m_objects[0]->m_sceneModel, i, 0.071428575f);
        task->m_msg->setFontType(i, 1);
        task->m_msg->setAlignMode(i, MuMsg::Align_Left);
        task->m_msg->setFontColor(i, 0, 0, 0, 0xFF);
    }
    for (int i = 0; i < 5; i++) {
        task->m_msg->printIndex(i, i, NULL);
    }
    nw4r::g3d::ScnMdl* scnMdl = reinterpret_cast<nw4r::g3d::ScnMdl*>(task->m_scnMdls[0]);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, scnMdl);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(scnMdl, task->m_objects[1]->m_sceneModel, "pos00");
    return Step_Appear;
}

void muAdvDifficultyTask::mainStepAppearInit(muAdvDifficultyTask* task) {
    {
        MuObject* object = task->m_objects[0];
        object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    }
    MuObject* cursor = task->m_objects[1];
    cursor->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    cursor->setFrameNode(static_cast<float>(task->m_difficulty));
}

int muAdvDifficultyTask::mainStepAppearMain(muAdvDifficultyTask* task) {
    if (!task->m_objects[0]->isNodeAnimFinished()) {
        return task->m_step;
    }
    return Step_Select;
}

void muAdvDifficultyTask::mainStepSelectInit(muAdvDifficultyTask* task) { }

// Puts the cursor on the level that is selected.
int muAdvDifficultyTask::mainStepSelectMain(muAdvDifficultyTask* task) {
    int difficulty = task->m_difficulty;
    MuObject* object = task->m_objects[1];
    switch (difficulty) {
    case 0:
        object->setFrameNode(0.0f);
        break;
    case 1:
        object->setFrameNode(1.0f);
        break;
    case 2:
        object->setFrameNode(2.0f);
        break;
    case 3:
        object->setFrameNode(3.0f);
        break;
    case 4:
        object->setFrameNode(4.0f);
        break;
    }
    return task->m_step;
}

// The level that was taken flashes: the animation "__1" of the level.
void muAdvDifficultyTask::mainStepSelectedInit(muAdvDifficultyTask* task) {
    MuObject* object = task->m_objects[0];
    const char* animName = "MenAdvLvl0000_TopN__1";
    object->changeNodeAnimN(animName);
    object->changeClrAnimN(animName);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
}

int muAdvDifficultyTask::mainStepSelectedMain(muAdvDifficultyTask* task) {
    if (task->m_objects[0]->isNodeAnimFinished()) {
        if (task->m_isSelected) {
            task->m_result = 1;
        } else {
            task->m_result = 2;
        }
    }
    return task->m_step;
}
