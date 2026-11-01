#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_anmvis.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_intro.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                         int capacity, void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);


// One model of the intro: the name, the slots of the task it fills (start, end), and the draw priority.
struct muIntroModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The sound of each kind of entry and how long it takes (-1: no sound).
static const muIntroTask::Sound sEntrySounds[] = {
    {-1, 0},
    {-1, 0},
    {0x203F, 0x27},
    {0x2040, 0x31},
};

// The scene of the intro (the running scene).
static inline scIntro* getScene() {
    return dynamic_cast<scIntro*>(gfSceneManager::getInstance()->m_currentScene);
}

// Puts a model into the scene group of the camera.
static inline void insertToScene(void* object) {
    scIntro* scene = getScene();
    nw4r::g3d::ScnGroup* root = scene->m_camera->m_task->m_scnRoot;
    root->Insert(root->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(object));
}

// Takes the file that a handle has read and prepares it for use (null when the read failed).
static inline void loadResFile(muFileIOHandle* handle, nw4r::g3d::ResFile* out) {
    nw4r::g3d::ResFile resFile(static_cast<void*>(NULL));
    void* buffer;
    if (handle->getReturnStatus() == 0x15) {
        buffer = NULL;
    } else {
        buffer = handle->getBuffer();
        handle->release();
    }
    if (buffer != NULL) {
        resFile = nw4r::g3d::ResFile(buffer);
        nw4r::g3d::ResFile::Init(&resFile);
    }
    *out = resFile;
}

// Appends an entry to the sound script.
inline void muIntroTask::addSound(u32 id, int frames) {
    Sound* sound = &m_sounds[m_soundNum];
    sound->m_id = id;
    sound->m_frames = frames;
    m_soundNum++;
}

// The number of the mini intro (-1 when the intro is a fight).
inline int muIntroTask::getMiniNo() const {
    int miniNo = -1;
    if (m_mode >= 2) {
        miniNo = m_mode - 2;
    }
    return miniNo;
}

inline int muIntroTask::getPlayerNum() const {
    if (m_mode >= 2) {
        return -1;
    }
    return m_playerNum;
}

// How the player enters (4: it does not).
inline int muIntroTask::getPlayerType(int index) const {
    if (index < 0 && !(index < getPlayerNum())) {
        return 4;
    }
    int type;
    switch (m_mode) {
    case 0:
        type = m_players[index].m_type;
        break;
    case 1:
        type = 0;
        break;
    default:
        type = 4;
        break;
    }
    return type;
}

inline int muIntroTask::getEnemyNum() const {
    if (m_mode >= 2) {
        return -1;
    }
    return m_enemyNum;
}

inline int muIntroTask::getEnemyType(int index) const {
    if (index < 0 && !(index < getEnemyNum())) {
        return 4;
    }
    int type;
    switch (m_mode) {
    case 0:
    case 1:
        type = m_enemies[index].m_type;
        break;
    default:
        type = 4;
        break;
    }
    return type;
}

// The frame of the visibility animation that puts a player at his place, which depends on how many players there are.
inline float muIntroTask::getVisibleFrame(int index) const {
    float frame;
    if (m_mode == 1) {
        frame = 3.0f;
    } else {
        switch (getPlayerNum()) {
        case 1:
            frame = 0.0f;
            break;
        case 2:
            if (index == 0) {
                frame = 1.0f;
            } else {
                frame = 2.0f;
            }
            break;
        case 3:
            frame = 4.0f;
            break;
        default:
            frame = 3.0f;
            break;
        }
    }
    return frame;
}

muIntroTask* muIntroTask::create() {
    muIntroTask* task = new (Heaps::MenuInstance) muIntroTask();
    task->getStageSetting();
    task->m_handles[0].readRequest("/menu/intro/enter/cmn.brres", Heaps::MenuResource, 0, 0);
    int miniNo = task->getMiniNo();
    if (miniNo < 0) {
        task->loadCharModel();
    } else {
        char path[0x34];
        sprintf(path, "/menu/intro/enter/mini%d.brres", miniNo + 1);
        task->m_handles[5].readRequest(path, Heaps::MenuInstance, 0, 0);
    }
    muMenu::loadMenuSound();
    task->m_started = false;
    task->m_unkE4 = 0;
    task->makeSoundScript();
    return task;
}

muIntroTask::muIntroTask()
    : gfTask("Intro", Category_Menu, 0xF, 8, true), m_unkE4(0), m_started(false), m_soundNum(0), m_soundIdx(0), m_soundFrame(0) {
    unk2C_b1 = false;
    m_resFileCmn = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_resFileChrCmn = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    for (int i = 0; i < 3; i++) {
        m_resFileChar[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    for (int i = 0; i < 2; i++) {
        m_resFileEnemy[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    m_resFileMini = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    for (int i = 0; i < ObjNum; i++) {
        m_objects[i] = NULL;
    }
    m_scnMdl = NULL;
    memset(&m_introNo, 0, 0x38);
    memset(m_sounds, 0, sizeof(m_sounds));
}

muIntroTask::~muIntroTask() {
    if (m_scnMdl != NULL) {
        m_scnMdl->Destroy();
    }
    for (int i = 0; i < ObjNum; i++) {
        delete m_objects[i];
    }
    if (m_resFileCmn.ptr() != NULL) {
        gfHeapManager::free(m_resFileCmn.ptr());
    }
    if (m_resFileChrCmn.ptr() != NULL) {
        gfHeapManager::free(m_resFileChrCmn.ptr());
    }
    for (int i = 0; i < 3; i++) {
        if (m_resFileChar[i].ptr() != NULL) {
            gfHeapManager::free(m_resFileChar[i].ptr());
        }
    }
    for (int i = 0; i < 2; i++) {
        if (m_resFileEnemy[i].ptr() != NULL) {
            gfHeapManager::free(m_resFileEnemy[i].ptr());
        }
    }
    if (m_resFileMini.ptr() != NULL) {
        gfHeapManager::free(m_resFileMini.ptr());
    }
    for (int i = 0; i < HandleNum; i++) {
        m_handles[i].cancelRequest();
    }
}

// The script of the sounds: the voice of the fighters and the jingles in between, with how long each takes.
void muIntroTask::makeSoundScript() {
    // MATCH-ONLY: the loop counter is declared up front to fix the register allocation
    int i;
    if (getMiniNo() < 0) {
        addSound(0x203E, 0x3C);
        int num;
        if (m_mode == 1) {
            addSound(0x203C, 0x1E);
            num = 1;
        // MATCH-ONLY: getPlayerNum() written out so the mode load is shared with the test above
        } else if (m_mode >= 2) {
            num = -1;
        } else {
            num = m_playerNum;
        }
        for (i = 0; i < num; i++) {
            if (i > 0) {
                if (i < num - 1) {
                    addSound(-1, 10);
                } else {
                    addSound(0x203D, 0x2B);
                }
            }
            int type = getPlayerType(i);
            int entryFrames = sEntrySounds[type].m_frames;
            if (sEntrySounds[type].m_id != -1u) {
                addSound(sEntrySounds[type].m_id, entryFrames);
            }
            u32 voice = muMenu::exchangeSelchkind2SelCharVoice(m_players[i].m_charKind);
            if (voice != -1u) {
                int frames;
                if (g_GameGlobal->getLanguage() == 0) {
                    frames = muMenu::exchangeSelCharVoice2SelCharVoiceLengthE(m_players[i].m_charKind);
                } else {
                    frames = muMenu::exchangeSelCharVoice2SelCharVoiceLengthJ(m_players[i].m_charKind);
                }
                addSound(voice, frames);
            }
        }
    } else {
        addSound(0x2035, 0x5A);
    }
}

// Copies what the scene says about the fight.
void muIntroTask::getStageSetting() {
    scIntro* scene = getScene();
    m_introNo = scene->m_introNo;
    m_mode = scene->m_mode;
    m_playerNum = scene->m_playerNum;
    m_enemyNum = scene->m_enemyNum;
    m_players[0].m_charKind = scene->m_players[0].m_charKind;
    m_players[0].m_type = scene->m_players[0].m_type;
    m_players[1].m_charKind = scene->m_players[1].m_charKind;
    m_players[1].m_type = scene->m_players[1].m_type;
    m_players[2].m_charKind = scene->m_players[2].m_charKind;
    m_players[2].m_type = scene->m_players[2].m_type;
    m_enemies[0].m_charKind = scene->m_enemies[0].m_charKind;
    m_enemies[0].m_type = scene->m_enemies[0].m_type;
    m_enemies[1].m_charKind = scene->m_enemies[1].m_charKind;
    m_enemies[1].m_type = scene->m_enemies[1].m_type;
}

// Asks for the files of the panels and of the fighters and enemies.
void muIntroTask::loadCharModel() {
    char path[0x48];
    char anim[0x20];
    char vis[0x20];
    m_handles[1].readRequest("/menu/intro/enter/chrcmn.brres", Heaps::MenuResource, 0, 0);
    // MATCH-ONLY: reading the mode through a const view lets the inlined getters reuse the load.
    if (static_cast<const muIntroTask*>(this)->m_mode == 1) {
        getEnemyResFileName(path, anim, vis, m_players[0].m_charKind, getPlayerType(0));
        m_handles[2].readRequest(path, Heaps::MenuInstance, 0, 0);
    } else {
        int num = getPlayerNum();
        for (int i = 0; i < num; i++) {
            int type = getPlayerType(i);
            getEnemyResFileName(path, anim, vis, m_players[i].m_charKind, type);
            m_handles[2 + i].readRequest(path, Heaps::MenuInstance, 0, 0);
        }
    }
    int enemyNum = getEnemyNum();
    for (int i = 0; i < enemyNum; i++) {
        int type = getEnemyType(i);
        getEnemyResFileName(path, anim, vis, m_enemies[i].m_charKind, type);
        m_handles[6 + i].readRequest(path, Heaps::MenuInstance, 0, 0);
    }
}

bool muIntroTask::isLoadFinished() {
    for (int i = 0; i < HandleNum; i++) {
        if (!m_handles[i].isReady()) {
            return false;
        }
    }
    return muMenu::isLoadFinishMenuSound();
}

// The map (slot 0).
static const muIntroModel sMapModel[] = {
    {"ItrSimpleMap0000_TopN", 0, 0, 0x21},
};

// The panels of the intro (slots 1, 2, 3 and 15).
static const muIntroModel sPanelModels[] = {
    {"ItrSimplePanel0000_TopN", 1, 1, 0x10},
    {"ItrSimplePanel0001_TopN", 2, 2, 0x11},
    {"ItrSimplePanel0002_TopN", 3, 3, 0x1F},
    {"ItrSimplePanel0010_TopN", 15, 15, 0x20},
};

// Creates the models of the panels, the fighters and the enemies and attaches them to their places.
void muIntroTask::createCharModel() {
    char path[0x40];
    char anim[0x20];
    char vis[0x20];
    char name[8];
    loadResFile(&m_handles[1], &m_resFileChrCmn);
    createObjResFile(sPanelModels, 4, &m_resFileChrCmn);
    m_scnMdl = Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
        gfHeapManager::getMEMAllocator(Heaps::MenuInstance), NULL, 0xD, m_objects[2]->m_sceneModel);
    int num;
    if (m_mode == 1) {
        num = 11;
        getEnemyResFileName(path, anim, vis, m_players[0].m_charKind, getPlayerType(0));
        loadResFile(&m_handles[2], &m_resFileChar[0]);
        for (int i = 0; i < num; i++) {
            MuObject* object = MuObject::create(&m_resFileChar[0], 0x1C - i, NULL, 0, Heaps::MenuInstance);
            object->changeNodeAnimN(anim);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            object->changeClrAnimN(anim);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(i == 0 ? 1.0f : 0.0f);
            object->changeVisAnimN(vis);
            object->setFrameVisible(getVisibleFrame(i));
            object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            m_objects[4 + i] = object;
        }
    } else {
        num = getPlayerNum();
        for (int i = 0; i < num; i++) {
            loadResFile(&m_handles[2 + i], &m_resFileChar[i]);
            int type = getPlayerType(i);
            getEnemyResFileName(path, anim, vis, m_players[i].m_charKind, type);
            MuObject* object = MuObject::create(&m_resFileChar[i], 0x1C - i, NULL, 0, Heaps::MenuInstance);
            object->changeNodeAnimN(anim);
            object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            object->changeClrAnimN(anim);
            object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            object->changeVisAnimN(vis);
            object->setFrameVisible(getVisibleFrame(i));
            object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            m_objects[4 + i] = object;
        }
    }
    int enemyNum = getEnemyNum();
    for (int i = 0; i < enemyNum; i++) {
        loadResFile(&m_handles[6 + i], &m_resFileEnemy[i]);
        int type = getEnemyType(i);
        getEnemyResFileName(path, anim, vis, m_enemies[i].m_charKind, type);
        MuObject* object = MuObject::create(&m_resFileEnemy[i], 0x1E - i, NULL, 0, Heaps::MenuInstance);
        object->changeNodeAnimN(anim);
        object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        object->changeClrAnimN(anim);
        object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
        object->changeVisAnimN(vis);
        object->setFrameVisible(5.0f);
        object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        m_objects[16 + i] = object;
    }
    int base;
    switch (num) {
    case 1:
        base = 0;
        break;
    case 2:
        base = 1;
        break;
    case 3:
        base = 3;
        break;
    default:
        base = 10;
        break;
    }
    for (int i = 0; i < num; i++) {
        sprintf(name, "pos%02d", i + base);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_scnMdl, m_objects[4 + i]->m_sceneModel, name);
    }
    for (int i = 0; i < enemyNum; i++) {
        sprintf(name, "pos%02d", i + 0x1E);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_scnMdl, m_objects[16 + i]->m_sceneModel, name);
    }
}

// The names of the file and the animations of a character: type 0 and 1 are the same entry, 2 and 3 have their own.
void muIntroTask::getEnemyResFileName(char* path, char* anim, char* vis, int charKind, int type) {
    const char* fileKind = "chr";
    const char* animKind = "Chr";
    int variant;
    switch (type) {
    case 0:
        variant = 0;
        break;
    case 1:
        variant = 0;
        break;
    case 2:
        variant = 1;
        break;
    case 3:
        variant = 2;
        break;
    }
    int no = muMenu::exchangeGmCharacterKind2Something(charKind) + 1;
    sprintf(path, "/menu/intro/enter/%s%04d.brres", fileKind, no);
    sprintf(anim, "ItrSimple%s%04d_TopN__%d", animKind, no, variant);
    sprintf(vis, "ItrSimple%s%04d_TopN__0", animKind, no);
}

// Creates the models of the table; they are stored in the slots of the task and start with the animation "__0".
void muIntroTask::createObjResFile(const muIntroModel* models, int count, nw4r::g3d::ResFile* resFile) {
    for (int i = 0; i < count; i++) {
        const muIntroModel* model = &models[i];
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

// Puts the panels, the models and the enemy into the scene.
void muIntroTask::dispEnemy(int introNo) {
    MuObject* object = m_objects[1];
    object->setFrameMatCol(static_cast<float>(introNo));
    insertToScene(object->m_sceneModel);
    insertToScene(m_scnMdl);
    object = m_objects[2];
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    if (getEnemyNum() > 0) {
        object = m_objects[15];
        insertToScene(object->m_sceneModel);
        object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    }
    object = m_objects[3];
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    insertToScene(object->m_sceneModel);
}

// First waits until everything is loaded and puts the models into the scene; then plays the sound script and ends the
// scene when it is done.
void muIntroTask::processDefault() {
    if (!m_started) {
        if (isLoadFinished()) {
            loadResFile(&m_handles[0], &m_resFileCmn);
            createObjResFile(sMapModel, 1, &m_resFileCmn);
            if (getMiniNo() < 0) {
                createCharModel();
            } else {
                loadResFile(&m_handles[5], &m_resFileMini);
                m_objects[4] = MuObject::create(&m_resFileMini, 0x12, NULL, 0, Heaps::MenuInstance);
            }
            g_gfApplication->unkD0.endKeepScreen();
            MuObject* object = m_objects[0];
            char animName[0x1C];
            sprintf(animName, "ItrSimpleMap0000_TopN__%d", m_introNo + 1);
            object->changeNodeAnimNIf(animName);
            object->changeVisAnimNIf(animName);
            object->changeTexPatAnimNIf(animName);
            object->changeClrAnimNIf(animName);
            object->m_modelAnim->setUpdateRate(1.0f);
            insertToScene(object->m_sceneModel);
            if (getMiniNo() < 0) {
                dispEnemy(m_introNo);
            } else {
                object = m_objects[4];
                object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
                object->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
                insertToScene(object->m_sceneModel);
            }
            u32 setting = getScene()->m_rumbleSetting;
            if (setting == 0x78) {
                setting = -1;
            }
            muMenu::startRumbleController(getScene()->m_rumbleController, 0x14, setting);
            g_sndSystem->playSE(static_cast<SndID>(0x2B), -1, 0, 0, -1);
            m_unkE4 = 0;
            m_started = true;
        }
    } else {
        Sound* sound = &m_sounds[m_soundIdx];
        if (m_soundFrame <= 0) {
            g_sndSystem->playSE(static_cast<SndID>(sound->m_id), -1, 0, 0, -1);
        }
        m_soundFrame++;
        if (sound->m_frames <= m_soundFrame) {
            m_soundIdx++;
            if (m_soundIdx < m_soundNum) {
                m_soundFrame = 0;
            }
        }
        if (m_soundNum <= m_soundIdx) {
            getScene();
            gfSceneManager* manager = gfSceneManager::getInstance();
            manager->unk1 = 0;
            manager->processStep = 2;
        }
    }
}
