#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <it/it_manager.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_figure_get.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);
// gfSceneRoot::layerUpdateFrame(layer), gfSceneRoot::removeInfoCamResAnm() and gfSceneRoot::setInfoCamResAnm(animation)
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D6DC(gfSceneRoot* root, void* resAnmScn);
// gfArchive::readFileRequest(path, heap, offset, size, archive id)
extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);
// nw4r::g3d::ResFile::GetResTex(name), GetResAnmScn(index) and ResAnmScn::Bind(animation)
extern "C" void* GetResTex__Q34nw4r3g3d7ResFileCFPCc(const nw4r::g3d::ResFile* resFile, const char* name);
extern "C" void* GetResAnmScn__Q34nw4r3g3d7ResFileCFi(const nw4r::g3d::ResFile* resFile, int index);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void* self, void* anm);
// nw4r::g3d::ScnRoot::GetCamera(index), gfCameraManager::getManager(), gfCamera::setFromNwCameraAllParam(camera) and
// gfCamera::setNwCamera(camera)
extern "C" void* GetCamera__Q34nw4r3g3d7ScnRootFi(void* scnRoot, int index);
extern "C" u8* getManager__15gfCameraManagerFv();
extern "C" void fn_80019718(void* camera, void** nwCamera);
extern "C" void fn_80018EC4(void* camera, void** nwCamera);

// One model of the demo: the name, the slots of the task it fills (start, end), and the draw priority.
struct muFigureGetModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The init and main function of each step.
static const muFigureGetTask::StepFunc sStepFuncs[muFigureGetTask::Step_Count] = {
    {muFigureGetTask::mainStepMngDataLoadInit, muFigureGetTask::mainStepMngDataLoadMain},
    {muFigureGetTask::mainStepRequestInit, muFigureGetTask::mainStepRequestMain},
    {muFigureGetTask::mainStepGetResDataInit, muFigureGetTask::mainStepGetResDataMain},
    {muFigureGetTask::mainStepDataSetInit, muFigureGetTask::mainStepDataSetMain},
    {muFigureGetTask::mainStepInputWaitInit, muFigureGetTask::mainStepInputWaitMain},
};

// The models of the demo: the three pieces of the frame (slots 0, 1 and 2), the trophies (slots 103 ...) and their cards
// (slots 3 ...).
static const muFigureGetModel sModels[] = {
    {"FigureGet0000_TopN", 0, 0, 0x20},
    {"FigureGet0001_TopN", 1, 1, 0x22},
    {"FigureGet0002_TopN", 2, 2, 0x24},
    {"FigureGet0011_TopN", 103, 203, 0x21},
    {"FigureGet0012_TopN", 3, 103, 0x21},
};

muFigureGetTask* muFigureGetTask::create(const muFigureGetTaskParam* param) {
    muFigureGetTask* task = new (Heaps::MenuInstance) muFigureGetTask();
    task->m_unk44 = param->m_unk00;
    task->m_provider = param->m_provider;
    task->m_figId = param->m_figId;
    if (task->m_provider != NULL) {
        task->m_provider->rewind();
        task->m_figNum = task->m_provider->getCount();
        for (int i = 0; i < task->m_figNum; i++) {
            task->m_figIds[i] = task->m_provider->getFigId();
            task->m_provider->next();
        }
    }
    task->m_figMng = NULL;
    return task;
}

muFigureGetTask::muFigureGetTask()
    : gfTask("muAdvGameOver", Category_Menu, 0xF, 8, true), m_done(0), m_unk44(0), m_figId(0), m_figNum(0), m_unk1E4(0) {
    m_unk1360 = 0;
    m_unk1364 = 0;
    m_step = Step_MngDataLoad;
    m_stepPrev = Step_Count;
    m_isLeaving = false;
    m_scnMdls[0] = NULL;
    m_scnMdls[1] = NULL;
    m_scnMdls[2] = NULL;
    m_scnMdlStand = NULL;
    for (int i = 0; i < FigMax; i++) {
        m_scnMdlFigs[i] = NULL;
    }
    for (int i = 0; i < ObjNum; i++) {
        m_objects[i] = NULL;
    }
    m_standObjs[0] = NULL;
    m_standObjs[1] = NULL;
    m_figMng = NULL;
}

muFigureGetTask::~muFigureGetTask() {
    for (int i = 0; i < 3; i++) {
        if (m_scnMdls[i] != NULL) {
            m_scnMdls[i]->Destroy();
        }
    }
    for (int i = 0; i < m_figNum; i++) {
        if (m_scnMdlFigs[i] != NULL) {
            m_scnMdlFigs[i]->Destroy();
        }
    }
    if (m_scnMdlStand != NULL) {
        m_scnMdlStand->Destroy();
    }
    for (int i = 0; i < ObjNum; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
        }
    }
    if (m_standObjs[0] != NULL) {
        delete m_standObjs[0];
    }
    if (m_standObjs[1] != NULL) {
        delete m_standObjs[1];
    }
    m_handles[0].cancelRequest();
    gfHeapManager::free(m_handles[0].getBuffer());
    m_handles[0].release();
    m_handles[200].cancelRequest();
    gfHeapManager::free(m_handles[200].getBuffer());
    m_handles[200].release();
    m_handles[400].cancelRequest();
    gfHeapManager::free(m_handles[400].getBuffer());
    m_handles[400].release();
    m_handles[500].cancelRequest();
    gfHeapManager::free(m_handles[500].getBuffer());
    m_handles[500].release();
    delete m_archive;
    m_archive = NULL;
    fn_8000D7B8(g_gfSceneRoot);
}

void muFigureGetTask::processDefault() {
    if (m_stepPrev != m_step) {
        if (sStepFuncs[m_step].m_init != NULL) {
            sStepFuncs[m_step].m_init(this);
        }
        m_stepPrev = m_step;
    }
    if (sStepFuncs[m_step].m_main != NULL) {
        m_step = sStepFuncs[m_step].m_main(this);
    }
    fn_8000DB40(g_gfSceneRoot, 8);
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muFigureGetTask::createObjResFile(const muFigureGetModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++) {
        const muFigureGetModel* model = &models[i];
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

// Gets the manager of the trophy list from the item manager and starts the menu sound.
void muFigureGetTask::mainStepMngDataLoadInit(muFigureGetTask* task) {
    u8* itemManager = reinterpret_cast<u8*>(itManager::getInstance());
    // HYPOTHESIS: the item manager keeps a pointer to a record whose second word is the manager of the trophy list
    u8* record = *reinterpret_cast<u8**>(itemManager + 0x14B8);
    if (record != NULL) {
        task->m_figMng = *reinterpret_cast<tyFigListDataManager**>(record + 4);
    }
    muMenu::loadMenuSound();
}

// Waits for the menu sound and for the list of the trophies.
int muFigureGetTask::mainStepMngDataLoadMain(muFigureGetTask* task) {
    if (!muMenu::isLoadFinishMenuSound()) {
        return task->m_step;
    }
    if (task->m_figMng->isLoadFinish()) {
        task->m_step = Step_Request;
    }
    return task->m_step;
}

// Asks for the files: the frame, the trophy, the stand, the camera and the pack with the cards.
void muFigureGetTask::mainStepRequestInit(muFigureGetTask* task) {
    char path[0x20];
    task->m_handles[0].readRequest("/menu/figureget/fig_get.brres", Heaps::MenuResource, 0, 0);
    sprintf(path, "/toy/fig/%s.brres", task->m_figMng->getFigDataId(task->m_figId)->name);
    task->m_handles[400].readRequest(path, Heaps::MenuResource, 0, 0);
    task->m_handles[200].readRequest("/toy/fig/Stand.brres", Heaps::MenuResource, 0, 0);
    // HYPOTHESIS: the high bit of the byte at 0x28 of the menu data says that the screen is wide
    if ((reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordMenuDatap())[0x28] >> 7) != 0) {
        task->m_handles[500].readRequest("/menu/defaultcamera/FigureGetWide_0.brres", Heaps::MenuResource, 0, 0);
    } else {
        task->m_handles[500].readRequest("/menu/defaultcamera/FigureGet_0.brres", Heaps::MenuResource, 0, 0);
    }
    task->m_archive = new (Heaps::MenuInstance) gfArchive();
    fn_800151FC(task->m_archive, "toy/figdisp/figdisp.pac", Heaps::MenuResource, 0, 0, 0xFF);
    task->m_step = Step_GetResData;
}

int muFigureGetTask::mainStepRequestMain(muFigureGetTask* task) {
    return task->m_step;
}

void muFigureGetTask::mainStepGetResDataInit(muFigureGetTask* task) { }

// Waits for the files, then prepares the resources and the camera of the demo.
int muFigureGetTask::mainStepGetResDataMain(muFigureGetTask* task) {
    int next;
    if (!task->m_handles[0].isReady()) {
        next = task->m_step;
    } else if (task->m_handles[0].m_request == NULL) {
        next = task->m_step;
    } else if (!task->m_handles[200].isReady()) {
        next = task->m_step;
    } else if (task->m_handles[200].m_request == NULL) {
        next = task->m_step;
    } else if (!task->m_handles[400].isReady()) {
        next = task->m_step;
    } else if (task->m_handles[400].m_request == NULL) {
        next = task->m_step;
    } else if (!task->m_handles[500].isReady()) {
        next = task->m_step;
    } else if (task->m_handles[500].m_request == NULL) {
        next = task->m_step;
    } else if (task->m_archive->unk9) {
        task->m_resFileFigGet = nw4r::g3d::ResFile(task->m_handles[0].getBuffer());
        nw4r::g3d::ResFile::Init(&task->m_resFileFigGet);
        for (int i = 0; i < task->m_figNum; i++) {
            task->m_resFileFigs[i] = nw4r::g3d::ResFile(task->m_archive->getData(Data_Type_Tex, task->m_figIds[i] / 10, 0xFFFE));
            nw4r::g3d::ResFile::Init(&task->m_resFileFigs[i]);
        }
        task->m_resFileStand = nw4r::g3d::ResFile(task->m_handles[200].getBuffer());
        nw4r::g3d::ResFile::Init(&task->m_resFileStand);
        task->m_resFileFig = nw4r::g3d::ResFile(task->m_handles[400].getBuffer());
        nw4r::g3d::ResFile::Init(&task->m_resFileFig);
        void* anm2;
        void* anm = NULL;
        task->m_resFileCamera = nw4r::g3d::ResFile(task->m_handles[500].getBuffer());
        nw4r::g3d::ResFile::Init(&task->m_resFileCamera);
        anm = GetResAnmScn__Q34nw4r3g3d7ResFileCFi(&task->m_resFileCamera, 0);
        anm2 = anm;
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anm, &anm2);
        fn_8000D6DC(g_gfSceneRoot, &anm);
        // HYPOTHESIS: the game global keeps a record whose second and third word are scales that are reset to 1.0 here
        float* record = *reinterpret_cast<float**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x44);
        next = Step_DataSet;
        record[1] = 1.0f;
        record[2] = 1.0f;
    } else {
        next = task->m_step;
    }
    return next;
}

// Creates the models and puts the trophies on their places: six in a row, from left to right and from top to bottom.
void muFigureGetTask::mainStepDataSetInit(muFigureGetTask* task) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
    task->createObjResFile(sModels, 5, &task->m_resFileFigGet);
    MuObject* frame0 = task->m_objects[0];
    MuObject* frame1 = task->m_objects[1];
    MuObject* frame2 = task->m_objects[2];
    frame0->changeAnimN("FigureGet0000_TopN__0");
    frame0->setFrame(0.0f);
    frame0->m_modelAnim->setUpdateRate(1.0f);
    frame1->changeAnimN("FigureGet0001_TopN__0");
    frame1->setFrame(0.0f);
    frame1->m_modelAnim->setUpdateRate(1.0f);
    frame2->changeAnimN("FigureGet0002_TopN__1");
    frame2->setFrame(0.0f);
    frame2->m_modelAnim->setUpdateRate(0.0f);
    task->m_scnMdls[0] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(allocator, NULL, 1, frame0->m_sceneModel);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(task->m_scnMdls[0]));
    task->m_scnMdls[1] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(allocator, NULL, 1, frame1->m_sceneModel);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(task->m_scnMdls[1]));
    task->m_scnMdls[2] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(allocator, NULL, 1, frame2->m_sceneModel);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(task->m_scnMdls[2]));
    int column = 0;
    MuObject* cards[FigMax];
    MuObject* figures[FigMax];
    for (int i = 0; i < task->m_figNum; i++) {
        MuObject* figure = task->m_objects[103 + i];
        MuObject* card = task->m_objects[3 + i];
        figures[i] = figure;
        cards[i] = card;
        task->m_scnMdlFigs[i] = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(allocator, NULL, 1, figure->m_sceneModel);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(task->m_scnMdlFigs[i], cards[i]->m_sceneModel, "stand");
        char texName[0x40];
        task->m_figMng->getBilTexNameFigId(task->m_figIds[i], texName);
        u8* tex = reinterpret_cast<u8*>(GetResTex__Q34nw4r3g3d7ResFileCFPCc(&task->m_resFileFigs[i], texName));
        Vec3f cardScale;
        cardScale.m_x = 0.05f * static_cast<float>(*reinterpret_cast<u16*>(tex + 0x1C));
        cardScale.m_y = 0.05f * static_cast<float>(*reinterpret_cast<u16*>(tex + 0x1E));
        cardScale.m_z = 1.0f;
        cards[i]->setObjScale(&cardScale);
        Vec3f figureScale;
        figureScale.m_x = 3.0f;
        figureScale.m_y = 3.0f;
        figureScale.m_z = 3.0f;
        figures[i]->setObjScale(&figureScale);
        nw4r::g3d::ResMat mat = cards[i]->m_resMdl.GetResMat("Figure_Dammy_Mat");
        cards[i]->changeMaterialTex(mat.ptr()->m_id, texName, &task->m_resFileFigs[i]);
        int row = ((i / 6) * 6) / 6;
        if (row < 0) {
            row = 0;
        }
        Vec3f position;
        position.m_x = static_cast<float>(column * 0x12 - 0x2D);
        position.m_y = static_cast<float>(row * -0x19 - 0x1D);
        position.m_z = 0.5f;
        column++;
        if (column == 6) {
            column = 0;
        }
        figures[i]->setTrans(&position);
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(task->m_scnMdlFigs[i]));
    }
    task->m_standObjs[0] = MuObject::create(&task->m_resFileFig, 0x23, NULL, 0, Heaps::MenuInstance);
    task->m_standObjs[1] = MuObject::create(&task->m_resFileStand, 0x23, NULL, 0, Heaps::MenuInstance);
    task->m_scnMdlStand = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(allocator, NULL, 1,
                                                                                                      task->m_standObjs[1]->m_sceneModel);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(task->m_scnMdlStand, task->m_standObjs[0]->m_sceneModel, "StandN");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(task->m_scnMdls[0], task->m_scnMdlStand, "pos00");
    u8* cameraManager = getManager__15gfCameraManagerFv();
    void* scnRoot = *reinterpret_cast<void**>(reinterpret_cast<u8*>(g_gfSceneRoot) + 0x44);
    void* camera = GetCamera__Q34nw4r3g3d7ScnRootFi(scnRoot, 0);
    fn_80019718(cameraManager + 0x604, &camera);
    camera = GetCamera__Q34nw4r3g3d7ScnRootFi(scnRoot, 0);
    fn_80018EC4(cameraManager + 0x604, &camera);
}

// The trophy comes down on its stand; a sound plays on three frames of the animation.
int muFigureGetTask::mainStepDataSetMain(muFigureGetTask* task) {
    MuObject* object = task->m_objects[0];
    if (object->isAnimFinished()) {
        object->changeAnimN("FigureGet0000_TopN__1");
        task->m_step = Step_InputWait;
    } else {
        int frame = static_cast<int>(object->m_modelAnim->m_anmObjChrRes->GetFrame());
        if (frame == 0xAA) {
            g_sndSystem->playSE(static_cast<SndID>(0x1EE6), -1, 0, 0, -1);
        }
        if (frame == 0xBE) {
            g_sndSystem->playSE(static_cast<SndID>(0x1EE6), -1, 0, 0, -1);
        }
        if (frame == 0xD1) {
            g_sndSystem->playSE(static_cast<SndID>(0x1EE6), -1, 0, 0, -1);
        }
    }
    return task->m_step;
}

void muFigureGetTask::mainStepInputWaitInit(muFigureGetTask* task) { }

// A, B or Start (or the corresponding button) starts the animation that takes the stand away; when it is over the demo
// is done.
int muFigureGetTask::mainStepInputWaitMain(muFigureGetTask* task) {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    MuObject* object = task->m_objects[2];
    u32 pressed = pad.m_buttonsPressedThisFrame.bits;
    bool isPressed;
    if ((pressed & 0x100) != 0) {
        isPressed = true;
    } else if ((pressed & 0x200) != 0) {
        isPressed = true;
    } else if ((pressed & 0x1000) != 0) {
        isPressed = true;
    } else {
        isPressed = (pressed >> 20) & 1;
    }
    if (isPressed == true && !task->m_isLeaving) {
        task->m_isLeaving = true;
        object->changeAnimN("FigureGet0002_TopN__1");
        object->setFrame(0.0f);
        object->m_modelAnim->setUpdateRate(1.0f);
    }
    if (object->isAnimFinished()) {
        task->m_done = 1;
    }
    return task->m_step;
}

// Two objects of the module that are built when the module is loaded and that nothing uses. HYPOTHESIS: the type and the
// meaning of the values are unknown; they are two pairs of (0xFF, 0) and (0xFF, 1).
struct muFigureGetUnusedPair {
    int m_a;
    int m_b;
    muFigureGetUnusedPair(int a, int b) : m_a(a), m_b(b) { }
};
static muFigureGetUnusedPair sUnused0(0xFF, 0);
static muFigureGetUnusedPair sUnused1(0xFF, 1);
