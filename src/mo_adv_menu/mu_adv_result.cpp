#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <it/it_manager.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/mu_adv_result.h>
#include <mu/mu_menu_controller.h>

// main.dol functions with names that the shared headers do not declare.
// The manager of the sticker list: the constructor (memory, load) and the destructor (with a flag)
extern "C" void* fn_800AD808(void* memory, int load);
extern "C" void fn_800AD87C(void* manager, int flag);
// The list of names that the screen shows: create(description, heap, heap), add(...), show() and isLoadFinish()
struct muAdvResultListDesc;
extern "C" muAdvResultListDisp* fn_800F7CCC(muAdvResultListDesc* desc, int heap, int heap2);
extern "C" void fn_800FA7B8(muAdvResultListDisp* disp, u32 id, int unk, void* bufA, int sizeA, void* bufB, const char* label);
extern "C" void fn_800FA6CC(muAdvResultListDisp* disp);
extern "C" bool fn_800FA740(muAdvResultListDisp* disp);
// The scene module: the loader of the pack (load(heap, path, archive, size) and isLoaded()); deleting it frees the resource
extern "C" void fn_1_3C68(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);
extern "C" bool scResourceLoaderIsLoaded(scResourceLoader* loader);
// gfSceneRoot::layerUpdateFrame(layer), gfSceneRoot::removeInfoCamResAnm() and gfSceneRoot::setInfoCamResAnm(animation)
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D6DC(gfSceneRoot* root, void* resAnmScn);
extern "C" bool fn_8000D10C(gfSceneRoot* root, int layer, void* scnMdl);
// nw4r::g3d::ResFile::GetResAnmScn(index) and ResAnmScn::Bind(animation)
extern "C" void* GetResAnmScn__Q34nw4r3g3d7ResFileCFi(const nw4r::g3d::ResFile* resFile, int index);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void* self, void* anm);
// sndSystem::stopSE(handle, fade frames)
extern "C" void fn_80075F1C(sndSystem* system, s32 handle, int fade);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// The random numbers of main.dol: a float from 0 to 1 and an integer from 0 up to the argument
float randf();
long randi(long max);

// One model of the screen: the name, the slots of the task it fills (start, end), and the draw priority.
struct muAdvResultModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// What the list of names is made from.
struct muAdvResultListDesc {
    void* m_resFileMenu;                    // 0x00
    void* m_resFileCoin;                    // 0x04
    const char* m_name;                     // 0x08
    int m_unk0C;                            // 0x0C
    void* m_figMng;                         // 0x10
    void* m_sealMng;                        // 0x14
    int m_unk18;
    int m_unk1C;
    int m_unk20;                            // 0x20 (in the high byte)
    void* m_texts0;                         // 0x24
    void* m_texts1;                         // 0x28
    void* m_texts2;                         // 0x2C
};

// The init and main function of each step.
static const muAdvResultTask::StepFunc sStepFuncs[muAdvResultTask::Step_Count] = {
    {muAdvResultTask::mainStepLoadInit, muAdvResultTask::mainStepLoadMain},
    {muAdvResultTask::mainStepLoadInit2, muAdvResultTask::mainStepLoadMain2},
    {muAdvResultTask::stepFadeInInit, muAdvResultTask::stepFadeInMain},
    {muAdvResultTask::stepCoinInit, muAdvResultTask::stepCoinMain},
    {NULL, NULL},
    {NULL, NULL},
};

muAdvResultTask* muAdvResultTask::create(const muAdvResultTaskParam* param) {
    muAdvResultTask* task = new (Heaps::MenuInstance) muAdvResultTask();
    task->m_pad = param->m_pad;
    fn_800B96E8(reinterpret_cast<muMenuController*>(task->m_controller), param->m_pad & 0xFF);
    task->m_coins = param->m_coins;
    task->m_value0 = param->m_value0;
    task->m_value1 = param->m_value1;
    task->m_lists[0] = param->m_list0;
    task->m_lists[1] = param->m_list1;
    task->m_lists[2] = param->m_list2;
    u8* itemManager = reinterpret_cast<u8*>(itManager::getInstance());
    // HYPOTHESIS: the item manager keeps a pointer to a record whose second word is the manager of the trophy list
    u8* record = *reinterpret_cast<u8**>(itemManager + 0x14B8);
    if (record != NULL) {
        task->m_figMng = *reinterpret_cast<void**>(record + 4);
    }
    void* memory = operator new(0x3EB4, Heaps::MenuInstance);
    void* sealMng = NULL;
    if (memory != NULL) {
        sealMng = fn_800AD808(memory, 1);
    }
    task->m_sealMng = sealMng;
    task->m_inputWait = 0x3C;
    return task;
}

muAdvResultTask::muAdvResultTask() : gfTask("muAdvResult", Category_Menu, 0xF, 8, true), m_result(0), m_pad(0), m_inputWait(0x3C) {
    fn_800B96E8(reinterpret_cast<muMenuController*>(m_controller), 0xFF);
    m_coins = 0;
    m_value0 = 0;
    m_value1 = 0;
    m_lists[0] = NULL;
    m_lists[1] = NULL;
    m_lists[2] = NULL;
    m_figMng = NULL;
    m_unk118 = 0;
    m_sealMng = NULL;
    m_unk120 = 0;
    m_unk128 = 0;
    m_resFileMenu = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_msg = NULL;
    m_step = Step_Load;
    m_stepPrev = Step_Count;
    m_unk1C4 = 0;
    m_frame = 0;
    m_listDisp = NULL;
    m_archive = NULL;
    m_loader = NULL;
    m_isSecond = false;
    m_resFileCoin = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileCamera = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    for (int i = 0; i < ObjNum; i++) {
        m_objects[i] = NULL;
    }
    m_seHandle = static_cast<u32>(-1);
    m_archive = new (Heaps::MenuResource) gfArchive();
    m_loader = new (Heaps::MenuResource) scResourceLoader();
    fn_1_3C68(m_loader, 0x2B, "menu2/sc_adv_result.pac", m_archive, 0x552800);
    muMenu::loadMenuSound();
}

muAdvResultTask::~muAdvResultTask() {
    if (m_seHandle != static_cast<u32>(-1)) {
        fn_80075F1C(g_sndSystem, static_cast<s32>(m_seHandle), 0);
        m_seHandle = static_cast<u32>(-1);
    }
    fn_800AD87C(m_sealMng, 1);
    for (int i = 0; i < ObjNum; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
        }
    }
    fn_8000D7B8(g_gfSceneRoot);
    m_msg->detachMuObject(m_listDisp->m_object);
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
    m_lists[0] = NULL;
    m_lists[1] = NULL;
    m_lists[2] = NULL;
    m_listDisp->exit();
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    if (m_loader != NULL) {
        delete m_loader;
        m_loader = NULL;
    }
}

// Creates the texts: the title, the numbers (coins, stickers, trophies) and their labels.
void muAdvResultTask::initMsg() {
    char buffer[0x18];
    memset(buffer, 0, sizeof(buffer));
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x100, 8);
    m_unk118 = reinterpret_cast<int>(m_archive->getData(0, Data_Type_Misc, 0x28, 0xFFFE));
    m_unk120 = reinterpret_cast<int>(m_archive->getData(0, Data_Type_Misc, 0x32, 0xFFFE));
    m_msg->setMsgData(m_archive->getData(0, Data_Type_Misc, 0x1E, 0xFFFE));
    MuObject* object = m_listDisp->m_object;
    m_msg->attachScnMdlSimple(1, object->m_sceneModel, 0, 0.041666668f);
    m_msg->setAlignMode(1, MuMsg::Align_Left);
    m_msg->setFontColor(1, 0, 0, 0, 0xFF);
    m_msg->printIndex(1, 0, NULL);
    m_msg->attachScnMdlSimple(2, object->m_sceneModel, 1, 0.041666668f);
    m_msg->setAlignMode(2, MuMsg::Align_Left);
    m_msg->setFontColor(2, 0, 0, 0, 0xFF);
    sprintf(buffer, "%d", m_value0);
    m_msg->printf(2, buffer);
    m_msg->attachScnMdlSimple(3, object->m_sceneModel, 2, 0.041666668f);
    m_msg->setAlignMode(3, MuMsg::Align_Left);
    m_msg->setFontColor(3, 0, 0, 0, 0xFF);
    m_msg->printIndex(3, 1, NULL);
    m_msg->attachScnMdlSimple(4, object->m_sceneModel, 3, 0.041666668f);
    m_msg->setAlignMode(4, MuMsg::Align_Left);
    m_msg->setFontColor(4, 0, 0, 0, 0xFF);
    sprintf(buffer, "%d", m_value1);
    m_msg->printf(4, buffer);
    m_msg->attachScnMdlSimple(7, object->m_sceneModel, 6, 0.0625f);
    m_msg->setAlignMode(7, MuMsg::Align_Center);
    m_msg->setFontColor(7, 0x6B, 0x58, 0, 0xFF);
    m_msg->setFontType(7, 1);
    sprintf(buffer, "%d", m_coins);
    m_msg->printf(7, buffer);
}

// Puts the three lists (stickers, trophies and music) into the list of names.
void muAdvResultTask::listDisp() {
    u8* disp = reinterpret_cast<u8*>(m_listDisp);
    if (m_lists[0] != NULL) {
        m_lists[0]->rewind();
        for (u32 i = 0; i < static_cast<u32>(m_lists[0]->getCount()); i++) {
            u32 id = m_lists[0]->getId();
            fn_800FA7B8(m_listDisp, id, 1, disp + 0x3A4, 0x400, disp + 0x13A4, "\x83\x74\x83\x42");
            m_lists[0]->next();
        }
    }
    if (m_lists[1] != NULL) {
        m_lists[1]->rewind();
        for (u32 i = 0; i < static_cast<u32>(m_lists[1]->getCount()); i++) {
            u32 id = m_lists[1]->getId();
            fn_800FA7B8(m_listDisp, id, 1, disp + 0x13A6, 0x400, disp + 0x23A6, "\x83\x56\x81\x5B");
            m_lists[1]->next();
        }
    }
    if (m_lists[2] != NULL) {
        m_lists[2]->rewind();
        for (u32 i = 0; i < static_cast<u32>(m_lists[2]->getCount()); i++) {
            u32 id = m_lists[2]->getId();
            fn_800FA7B8(m_listDisp, id, 1, disp + 0x23A8, 0x100, disp + 0x27A8, "BGM");
            m_lists[2]->next();
        }
    }
    fn_800FA6CC(m_listDisp);
}

void muAdvResultTask::processDefault() {
    fn_800B9984(reinterpret_cast<muMenuController*>(m_controller));
    if (scResourceLoaderIsLoaded(m_loader) && muMenu::isLoadFinishMenuSound()) {
        controllProc();
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
    fn_8000DB40(g_gfSceneRoot, 8);
}

// A button skips the animation of the list to its end, and when it is over it leaves the screen.
void muAdvResultTask::controllProc() {
    if (m_listDisp != NULL) {
        MuObject* object = m_listDisp->m_object;
        if (m_inputWait > 0) {
            m_inputWait--;
            if (m_inputWait > 0) {
                return;
            }
        }
        gfPadStatus pad;
        g_gfPadSystem->getSysPadStatus(m_pad, &pad);
        u32 pressed = pad.m_buttonsPressedThisFrame.bits;
        if ((pressed & 0x100) == 0 && (pressed & 0x200) == 0 && (pressed & 0x1000) == 0) {
            return;
        }
        if (object != NULL) {
            g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
            if (120.0f <= object->m_modelAnim->getFrame()) {
                if (m_seHandle != static_cast<u32>(-1)) {
                    fn_80075F1C(g_sndSystem, static_cast<s32>(m_seHandle), 0);
                    m_seHandle = static_cast<u32>(-1);
                }
                m_result = 1;
            } else {
                object->setFrame(160.0f);
                m_frame = 0x78;
            }
        }
    }
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muAdvResultTask::createObjResFile(const muAdvResultModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++) {
        const muAdvResultModel* model = &models[i];
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

void muAdvResultTask::mainStepLoadInit(muAdvResultTask* task) { }

// Waits for the files and the menu sound, then binds the camera and creates the models.
int muAdvResultTask::mainStepLoadMain(muAdvResultTask* task) {
    // The models: the panel (slot 0) and the coins (slots 1 ... 31)
    static const muAdvResultModel sModels[] = {
        {"MenAdvResult0000_TopN", 0, 0, 0x10},
        {"MenAdvResult0010_TopN", 1, 0x1F, 0x11},
    };
    if (!scResourceLoaderIsLoaded(task->m_loader)) {
        return task->m_step;
    }
    if (!muMenu::isLoadFinishMenuSound()) {
        return task->m_step;
    }
    void* anm = NULL;
    void* anm2;
    // HYPOTHESIS: the high bit of the byte at 0x28 of the menu data says that the screen is wide
    int fileNo = 0x14 - (static_cast<s8>(reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordMenuDatap())[0x28]) >> 7);
    task->m_resFileCamera = nw4r::g3d::ResFile(task->m_archive->getData(0, Data_Type_Misc, fileNo, 0xFFFE));
    nw4r::g3d::ResFile::Init(&task->m_resFileCamera);
    anm2 = GetResAnmScn__Q34nw4r3g3d7ResFileCFi(&task->m_resFileCamera, 0);
    anm = anm2;
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anm, &anm2);
    fn_8000D6DC(g_gfSceneRoot, &anm);
    task->m_resFileMenu = nw4r::g3d::ResFile(task->m_archive->getData(0, Data_Type_Misc, 0, 0xFFFE));
    nw4r::g3d::ResFile::Init(&task->m_resFileMenu);
    task->m_resFileCoin = nw4r::g3d::ResFile(task->m_archive->getData(0, Data_Type_Misc, 10, 0xFFFE));
    nw4r::g3d::ResFile::Init(&task->m_resFileCoin);
    task->createObjResFile(sModels, 2, &task->m_resFileMenu);
    return Step_Load2;
}

// Creates the list of names with the resources and the texts.
void muAdvResultTask::mainStepLoadInit2(muAdvResultTask* task) {
    muAdvResultListDesc desc = {NULL, NULL, "MenAdvResult", 0x12, NULL, NULL, 0, 0, 0, NULL, NULL, NULL};
    desc.m_resFileMenu = task->m_resFileMenu.ptr();
    desc.m_resFileCoin = task->m_resFileCoin.ptr();
    desc.m_figMng = task->m_figMng;
    desc.m_sealMng = task->m_sealMng;
    int value;
    gmAdvData* advData = *reinterpret_cast<gmAdvData**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x30);
    if (advData == NULL) {
        value = -1;
    } else {
        value = *reinterpret_cast<int*>(reinterpret_cast<u8*>(advData) + 0x492C);
    }
    desc.m_unk20 = value << 24;
    desc.m_texts0 = task->m_archive->getData(0, Data_Type_Misc, 0x28, 0xFFFE);
    desc.m_texts1 = task->m_archive->getData(0, Data_Type_Misc, 0x32, 0xFFFE);
    desc.m_texts2 = task->m_archive->getData(0, Data_Type_Misc, 0x3C, 0xFFFE);
    task->m_listDisp = fn_800F7CCC(&desc, 0x2B, 0x2B);
}

// When the list of names is ready it is put into the scene with the panel, the lists and the texts.
int muAdvResultTask::mainStepLoadMain2(muAdvResultTask* task) {
    if (!fn_800FA740(task->m_listDisp)) {
        return task->m_step;
    }
    MuObject* object = task->m_objects[0];
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(object->m_sceneModel));
    object->setFrame(0.0f);
    object->m_modelAnim->setUpdateRate(1.0f);
    task->listDisp();
    task->initMsg();
    return Step_FadeIn;
}

// The music starts (one of two songs).
void muAdvResultTask::stepFadeInInit(muAdvResultTask* task) {
    if (task->m_isSecond == true) {
        g_sndSystem->playBGM(0x282F, 0, true);
    } else {
        g_sndSystem->playBGM(0x2824, 0, true);
    }
}

int muAdvResultTask::stepFadeInMain(muAdvResultTask* task) {
    if (task->m_frame == 0x78) {
        task->m_step = Step_Coin;
    }
    if (task->m_frame == 0x72) {
        g_sndSystem->playSE(static_cast<SndID>(0x2282), -1, 0, 0, -1);
    }
    task->m_frame++;
    return task->m_step;
}

void muAdvResultTask::stepCoinInit(muAdvResultTask* task) {
    task->initDropCoinList();
    task->m_coinsLeft = task->m_coins;
}

int muAdvResultTask::stepCoinMain(muAdvResultTask* task) {
    task->dropCoinMain();
    return task->m_step;
}

// Links the coins into the list of the coins that wait and starts the sound of the coins.
void muAdvResultTask::initDropCoinList() {
    muAdvResultCoin* prev = NULL;
    for (int i = 0; i < CoinNum; i++) {
        muAdvResultCoin* coin = &m_coinPool[i];
        coin->m_objectNo = i + 1;
        coin->m_prev = prev;
        if (i < CoinNum - 1) {
            coin->m_next = &m_coinPool[i + 1];
        } else {
            coin->m_next = NULL;
        }
        prev = coin;
    }
    m_coinFree = &m_coinPool[0];
    m_coinActive = NULL;
    m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x1EF8), -1, 0, 0, -1);
}

// Moves the coins that fall, puts the ones that left the screen back, and lets a new one fall at random.
void muAdvResultTask::dropCoinMain() {
    muAdvResultCoin* next = m_coinActive;
    muAdvResultCoin* coin;
    while ((coin = next) != NULL) {
        next = coin->m_next;
        if (!dropCoinMainOne(coin)) {
            muAdvResultCoin* prev = coin->m_prev;
            muAdvResultCoin* following = coin->m_next;
            if (prev == NULL) {
                m_coinActive = following;
            } else {
                prev->m_next = following;
            }
            if (following != NULL) {
                following->m_prev = prev;
            }
            if (m_coinFree != NULL) {
                m_coinFree->m_prev = coin;
            }
            coin->m_prev = NULL;
            coin->m_next = m_coinFree;
            m_coinFree = coin;
            g_gfSceneRoot->remove(reinterpret_cast<nw4r::g3d::ScnMdl*>(m_objects[coin->m_objectNo]->m_sceneModel));
        }
    }
    if (m_coinsLeft > 0 && randi(1) == 0) {
        coin = m_coinFree;
        if (coin != NULL) {
            muAdvResultCoin* prev = coin->m_prev;
            muAdvResultCoin* following = coin->m_next;
            if (prev == NULL) {
                m_coinFree = following;
            } else {
                prev->m_next = following;
            }
            if (following != NULL) {
                following->m_prev = prev;
            }
            if (m_coinActive != NULL) {
                m_coinActive->m_prev = coin;
            }
            coin->m_prev = NULL;
            coin->m_next = m_coinActive;
            m_coinActive = coin;
            dropCoinStartOne(coin);
            g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_objects[coin->m_objectNo]->m_sceneModel));
            m_coinsLeft--;
        }
    }
}

// Puts a coin at a random place above the screen with a random speed and spin.
void muAdvResultTask::dropCoinStartOne(muAdvResultCoin* coin) {
    MuObject* object = m_objects[coin->m_objectNo];
    Vec3f position;
    position.m_x = randf() * 24.0f;
    position.m_y = 33.0f;
    position.m_z = -8.0f;
    object->setTrans(&position);
    coin->m_speedY = 0.0f;
    coin->m_speedX = randf() - 0.5f;
    Vec3f rotation;
    rotation.m_x = randf() * 3.1415927f * 2.0f - 3.1415927f;
    rotation.m_y = randf() * 3.1415927f * 2.0f - 3.1415927f;
    rotation.m_z = randf() * 3.1415927f * 2.0f - 3.1415927f;
    object->setObjRotate(&rotation);
    coin->m_spinX = randf() * 0.6981317f - 0.34906584f;
    coin->m_spinY = randf() * 0.6981317f - 0.34906584f;
    coin->m_spinZ = randf() * 0.6981317f - 0.34906584f;
}

// Lets a coin fall one frame; it returns whether the coin is still on the screen. The sound of the coins stops when the
// last one is below the screen.
bool muAdvResultTask::dropCoinMainOne(muAdvResultCoin* coin) {
    MuObject* object = m_objects[coin->m_objectNo];
    coin->m_speedY -= 0.1f;
    // HYPOTHESIS: the position and the rotation of a model are at 0x3C and 0x48 of the object
    float* position = reinterpret_cast<float*>(reinterpret_cast<u8*>(object) + 0x3C);
    float* rotation = reinterpret_cast<float*>(reinterpret_cast<u8*>(object) + 0x48);
    Vec3f newPosition;
    newPosition.m_x = position[0] + coin->m_speedX;
    newPosition.m_y = position[1] + coin->m_speedY;
    newPosition.m_z = position[2];
    object->setTrans(&newPosition);
    Vec3f newRotation;
    newRotation.m_x = rotation[0] + coin->m_spinX;
    while (newRotation.m_x > 3.1415927f) {
        newRotation.m_x -= 6.2831855f;
    }
    while (newRotation.m_x < -3.1415927f) {
        newRotation.m_x += 6.2831855f;
    }
    newRotation.m_y = rotation[1] + coin->m_spinY;
    while (newRotation.m_y > 3.1415927f) {
        newRotation.m_y -= 6.2831855f;
    }
    while (newRotation.m_y < -3.1415927f) {
        newRotation.m_y += 6.2831855f;
    }
    newRotation.m_z = rotation[2] + coin->m_spinZ;
    while (newRotation.m_z > 3.1415927f) {
        newRotation.m_z -= 6.2831855f;
    }
    while (newRotation.m_z < -3.1415927f) {
        newRotation.m_z += 6.2831855f;
    }
    object->setObjRotate(&newRotation);
    if (m_coinsLeft < 1 && newPosition.m_y < -13.0f && m_seHandle != static_cast<u32>(-1)) {
        fn_80075F1C(g_sndSystem, static_cast<s32>(m_seHandle), 0);
        m_seHandle = static_cast<u32>(-1);
    }
    return !(newPosition.m_y < -33.0f);
}

// Two objects of the module that are built when the module is loaded and that nothing uses. HYPOTHESIS: the type and the
// meaning of the values are unknown; they are two pairs of (0xFF, 0) and (0xFF, 1).
struct muAdvResultUnusedPair {
    int m_a;
    int m_b;
    muAdvResultUnusedPair(int a, int b) : m_a(a), m_b(b) { }
};
static muAdvResultUnusedPair sUnused0(0xFF, 0);
static muAdvResultUnusedPair sUnused1(0xFF, 1);
