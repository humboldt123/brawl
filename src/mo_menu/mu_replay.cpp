#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <ip/ip_switch.h>
#include <memory.h>
#include <ms/ms_message.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_replay.h>

// main.dol functions with names that the shared headers do not declare.
// Message::getPrintIndexData(data, index, text, size)
extern "C" void getPrintIndexData__7MessageFPvUlPPcPUl(void* data, u32 index, char** text, u32* size);
// HYPOTHESIS: the loader of a file of the menu: reset(loader) and request(loader, path, heap, 0)
extern "C" void fn_800FABEC(void* loader);
extern "C" void fn_800FAC58(void* loader, const char* path, Heaps::HeapType heap, int unk);
// gfFileIOHandle::~gfFileIOHandle(handle, flags)
extern "C" void __dt__14gfFileIOHandleFv(gfFileIOHandle* handle, int flags);
// the collection: the name of the file, and the check that the collection is ready
extern "C" muCollectionFileName fn_800C3538(const char* names, int index);
extern "C" int fn_800C39D8(void* collection);
// sndSystem::stopBGM(system, fade) and sndSystem::resumeClear(system)
extern "C" void fn_80073F84(sndSystem* system, int fade);
extern "C" void fn_80076C78(sndSystem* system);
// the section of the save data that holds a replay: constructor, setSection(section), extractData(heap), destructor
struct nteSection {
    u8 m_unk[0x10];
};
extern "C" void fn_80153488(nteSection* section);
extern "C" void fn_80153504(nteSection* section, int index);
extern "C" muReplayData* fn_80153818(nteSection* section, Heaps::HeapType heap);
extern "C" void fn_80153498(nteSection* section, int flags);
// muMenu::exchange... functions (the arguments after the first one are not set by the caller)
extern "C" int exchangeSrStageKindToMuStageKind__6muMenuFiii(int stageKind);
extern "C" float getStageFrameNo__6muMenuFiii(int stageKind);
extern "C" int exchangeMuStageKindToMsgID__6muMenuFiii(int stageKind);
extern "C" int exchangeGmCharacterKind2MuStockchkind__6muMenuFi(int kind);
extern "C" int exchangeGmCharacterKind2Something__6muMenuFi(int kind);
extern "C" const char* exchangeMuStockchkind2MuCharName__6muMenuFi(int kind);
// MuMsg::printData(msg, index, data, size)
extern "C" void printData__5MuMsgFUlPCvi(MuMsg* msg, u32 msgIndex, const void* data, int size);
// nw4r::g3d::ScnMdlExpand::PushBack(object, node name)
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);
// gfSceneManager of the match: the character that is preloaded for a player, and if it is done
extern "C" void fn_1_379C(int player, int characterKind, int costume);
extern "C" int fn_1_385C(int player);
// scReplay::setMotionObj(scene, objects, count) of the module sora_scene
extern "C" void fn_1_8E0C(scReplay* scene, MuObject** objects, int count);

// Functions of the other modules, which the module calls by their offset (module 6 is sora_menu_collect_viewer).
// muCollectionViewer::loadResource(), createObjResFile(name), setPanelMotion(table, count) and changeMedia(cursor)
extern "C" void fn_6_1134(muCollectionViewer* viewer);
extern "C" void fn_6_1260(muCollectionViewer* viewer, const char* name);
extern "C" void fn_6_1ECC(muCollectionViewer* viewer);
extern "C" void fn_6_1CC0(muCollectionViewer* viewer, const u8* table, int count);
extern "C" int fn_6_308C(muCollectionViewer* viewer, int* cursor);
// muCollectionViewer::createNoticeWindow(text), controlLoadStart() and controlLoading()
extern "C" void fn_6_535C(muCollectionViewer* viewer, void* text);
extern "C" void fn_6_5F24(muCollectionViewer* viewer);
extern "C" void fn_6_5F9C(muCollectionViewer* viewer);
// the summary of a file of the collection
extern "C" muReplaySummary* fn_6_8180(void* summaries, const muCollectionFileName& name);

// the data of the global record of the match (the players) and the data of the stage
extern u8* lbl_805A01D8;
extern utList lbl_8049E550;

// The text of a notice: the data and how much of it is used.
struct muReplayNotice {
    char m_data[0x200];
    int m_size;

    void append(const char* text, u32 size) {
        int used = m_size;
        memcpy(m_data + used, text, size);
        m_size = used + size;
    }
};

// A text that is put together from several lines of the message file.
struct muReplayBuf {
    char m_data[0x100];
    int m_size;

    void append(const char* text, u32 size) {
        int used = m_size;
        memcpy(m_data + used, text, size);
        m_size = used + size;
    }
};

// A text of the message file: where it is and how long it is.
// MATCH-ONLY: the two words are one structure so that the stack layout of the function that uses it is the same.
struct muReplayLabel {
    char* m_text;
    u32 m_size;
};


// The record of the game that the replay starts: it is copied from the replay into the global data.
// MATCH-ONLY: the split into a header, a body and a tail follows from how the compiler copies the structure.
struct muReplayRecordBlock {
    u64 m_data[0x12];
};
struct muReplayRecordBody {
    u32 m_data[0xA1];
};
struct muReplayRecord {
    u8 m_a;
    u8 m_b;
    u8 m_c;
    muReplayRecordBlock m_block;
    muReplayRecordBody m_body;
};

// Resets one of the loaders of the menu, clears the data around it and asks it to read the file.
static inline void requestResource(void* loader, int* unk, u8* flag, const char* path) {
    fn_800FABEC(loader);
    *unk = 0;
    *flag = 0;
    fn_800FAC58(loader, path, Heaps::MenuResource, 0);
}

static inline void callVirtualFloat(void* object, int offset, float value) {
    (*reinterpret_cast<void (**)(void*, float)>(*reinterpret_cast<u8**>(object) + offset))(object, value);
}

static inline void callVirtual(void* object, int offset, void* arg) {
    (*reinterpret_cast<void (**)(void*, void*)>(*reinterpret_cast<u8**>(object) + offset))(object, arg);
}

static inline const char* getResMdlName(MuObject* object) {
    const u8* resData = *reinterpret_cast<u8* const*>(&object->m_resMdl);
    int nameOffset = *reinterpret_cast<const int*>(resData + 0x3C);
    return nameOffset != 0 ? reinterpret_cast<const char*>(resData + nameOffset) : NULL;
}

inline utility::ListedNode::ListedNode(utList* list) : m_list(NULL) {
    m_next = NULL;
    m_prev = NULL;
    list->addTail(this);
    m_list = list;
}

inline utility::ListedNode::~ListedNode() {
    if (this != NULL) {
        if (this != NULL) {
            lbl_8049E550.removeExist(this);
            utList* list = NULL;
            m_list = list;
            if (this != NULL && list != NULL) {
                list->removeExist(this);
                m_list = list;
            }
        }
    }
}

inline utility::AutoFile::AutoFile() : ListedNode(&lbl_8049E550) {
    m_handle.m_request = NULL;
    m_flag = false;
}

utility::AutoFile::~AutoFile() {
    fn_800FABEC(this);
    m_handle.release();
    __dt__14gfFileIOHandleFv(&m_handle, -1);
}

inline muReplayTask::Work::Work() : muCollectionViewer::Work(0x1E, 5, "snap_PhotoMat"), m_scene(NULL), m_exitMode(-1), m_exitWait(-1) {
    m_unk1CF8 = 0x32;
}

muReplayTask* muReplayTask::create(scReplay* scene) {
    muReplayTask* task = new (Heaps::MenuInstance) muReplayTask("Replay");
    Work* work = new (Heaps::MenuInstance) Work();
    task->m_work = work;
    work->m_scene = scene;
    return task;
}

muReplayTask::~muReplayTask() { }

void muReplayTask::loadResource() {
    fn_6_1134(this);
    Work* work = getWork();
    requestResource(&work->m_brres, &work->m_unk000, &work->m_brresFlag, "/menu/collection/Replay.brres");
    work = getWork();
    requestResource(&work->m_wait, &work->m_unk09C, &work->m_waitFlag, "/menu/common/Wait.brres");
    fn_800FAC58(&getWork()->m_msgFile[0], "/menu/msg/replay.msbin", Heaps::MenuResource, 0);
    fn_800FAC58(&getWork()->m_msgFile[1], "/menu/msg/save_load.msbin", Heaps::MenuResource, 0);
    fn_800FAC58(&getWork()->m_stageNames, "/menu/msg/stage_name.msbin", Heaps::MenuResource, 0);
}

void muReplayTask::createModel() {
    fn_6_1260(this, "Replay");
    static char sNamesInit;
    static char sNames[3][0x44];
    if (!sNamesInit) {
        sNames[0][0] = 0;
        strcat(sNames[0], "MenReplayLoading_TopN");
        sNames[1][0] = 0;
        strcat(sNames[1], "MenReplayPreview1_TopN");
        sNames[2][0] = 0;
        strcat(sNames[2], "MenReplayPreview2_TopN");
        sNamesInit = 1;
    }
    changePicAnim(sNames[0], 3, m_work);
    static const u8 sPanelMotion[4] = {4, 0x19, 4, 0};
    fn_6_1CC0(this, sPanelMotion, 1);
    fn_1_8E0C(getWork()->m_scene, &m_work->m_objs[0], 0x1E);
    expandPic(2);
}

void muReplayTask::onPreCheckFinished() {
    int count = fn_6_308C(this, &getWork()->m_scene->m_cursor);
    if (count >= 0) {
        m_work->m_curMedia = getWork()->m_scene->m_cursor;
        m_work->m_numFiles = m_work->m_extraPanel + count;
    }
    g_sndSystem->playBGM(0x270C, 0, true);
    *reinterpret_cast<int*>(lbl_805A01D8 + 0x174) = 0x2D;
}

void muReplayTask::initMsg() {
    fn_6_1ECC(this);
    m_work->m_msg->attachScnMdlSimple(0x11, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_work->m_objs[25]->m_sceneModel), 0, 0.0625f);
    m_work->m_msg->setFontColor(0x11, 0, 0, 0, 0xFF);
    int i = 0;
    int offset = 0;
    do {
        u32 msgIndex = i + 0x12;
        m_work->m_msg->attachScnMdlSimple(msgIndex, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_work->m_objs[26 + offset / 4]->m_sceneModel), 0, 0.0625f);
        m_work->m_msg->setFontColor(msgIndex, 0, 0, 0, 0xFF);
        m_work->m_msg->setAlignMode(msgIndex, MuMsg::Align_Center);
        i++;
        offset += 4;
    } while (i < 4);
}

void muReplayTask::getMineLabel(muCollectionMsgBuf* buf) {
    void* data = m_work->m_msgFile[0].m_handle.getBuffer();
    u32 size;
    char* text;
    getPrintIndexData__7MessageFPvUlPPcPUl(data, 0xB, &text, &size);
    buf->append(text, size);
}

void muReplayTask::addObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Insert(*reinterpret_cast<int*>(reinterpret_cast<u8*>(root) + 0xE4), static_cast<nw4r::g3d::ScnObj*>(obj));
}

void muReplayTask::removeObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Remove(static_cast<nw4r::g3d::ScnObj*>(obj));
}

void* muReplayTask::getScnRoot() {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    if (holder->m_hasScnRoot) {
        return holder->m_scnRoot;
    }
    return NULL;
}

void muReplayTask::setPanelStateCore(int state, int unk) {
    // HYPOTHESIS: the table takes the names of the two panels, 11 animations each, and tells for each state which one to play.
    static const char* sPanelName = "MenReplayPanel_TopN__0";
    static const char* sCursorName = "MenReplayCursor_TopN__0";
    static muCollectionViewer::PanelMotionTable sTable(sPanelName, sCursorName);
    m_work->m_objs[1]->setAnimName(&sTable.m_anim[sTable.m_index[state]], false);
    if (unk != 0) {
        m_work->m_objs[23]->setAnimName(&sTable.m_anim[sTable.m_index[state]] + 11, false);
    }
    m_work->m_panelState = state;
}

void muReplayTask::onDecided() {
    const muCollectionFileName& fileName = getFileHandle(getCurFileID());
    muReplayNotice notice;
    muReplayLabel label;
    muReplaySummary* summary = fn_6_8180(&m_work->m_unk1C8, fileName);
    if (!summary->m_isValid) {
        g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
    } else {
        if (summary->m_info == NULL) {
            notice.m_size = 0;
            void* data = m_work->m_msgFile[2].m_handle.getBuffer();
            getPrintIndexData__7MessageFPvUlPPcPUl(data, 0x3A, &label.m_text, &label.m_size);
            notice.append(label.m_text, label.m_size);
            fn_6_535C(this, &notice);
        } else {
            m_work->m_state = 4;
            MuObject* object = m_work->m_objs[24];
            addObjCore(object->m_sceneModel);
            muCollectionName nameA(getResMdlName(object));
            muCollectionName nameB = nameA;
            nameB.addSeparator();
            muCollectionName nameC = nameB;
            nameC.addNumber(0);
            muCollectionName nameD = nameC;
            object->changeNodeAnimNIf(nameD.m_str);
        }
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
    }
}

u32 muReplayTask::getCurFileID() {
    Work* work = getWork();
    u32 count = work->m_extraPanel != 0 ? work->m_numFiles - 1 : work->m_numFiles;
    return count < getNumPic(work->m_curMedia) ? count : 0x80000000;
}

int muReplayTask::getNumPic(int media) {
    return m_work->m_media[media].m_numPic;
}

muCollectionFileName muReplayTask::getFileHandle(int index) {
    return fn_800C3538(m_work->m_media[m_work->m_curMedia].m_names, index);
}

void muReplayTask::controlLoadStart() {
    fn_6_5F24(this);
}

void muReplayTask::controlLoading() {
    fn_6_5F9C(this);
}

void muReplayTask::controlLoadFinish() {
    m_work->m_exitRequest = 1;
    getWork()->m_exitWait = 0x20;
    fn_80073F84(g_sndSystem, 0x1E);
    fn_80076C78(g_sndSystem);
    g_sndSystem->playSE(static_cast<SndID>(0x16), -1, 0, 0, -1);
    getWork()->m_preloaded = false;
    setMelee();
}

void muReplayTask::onLoadFailed() {
    removeObjCore(m_work->m_objs[24]->m_sceneModel);
}

void muReplayTask::setMelee() {
    nteSection section;
    int index = m_work->m_unk1CF0;
    fn_80153488(&section);
    fn_80153504(&section, index);
    muReplayData* data = fn_80153818(&section, Heaps::MenuResource);
    if (data != NULL) {
        void* buffer = static_cast<u8*>(data->m_data) + 0x34;
        int size = data->m_size;
        ipSwitch* ipsw = ipSwitch::getInstance();
        ipKeyRecorder* recorder = ipsw->m_keyRecorder;
        recorder->setBufferData(buffer, size);
        muReplayRecord* dest = *reinterpret_cast<muReplayRecord**>(reinterpret_cast<u8*>(g_GameGlobal) + 8);
        muReplayRecord* src = reinterpret_cast<muReplayRecord*>(recorder->getBaseInfo());
        dest->m_a = src->m_a;
        dest->m_b = src->m_b;
        dest->m_c = src->m_c;
        dest->m_block = src->m_block;
        dest->m_body = src->m_body;
        if (recorder->getStageData() != NULL) {
            void* stageDest = *reinterpret_cast<void**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x40);
            memcpy(stageDest, recorder->getStageData(), 0x470);
        }
        *(reinterpret_cast<u8*>(ipsw) + 0x24) |= 0x80;
        getWork()->m_exitMode = 0;
    } else {
        getWork()->m_exitMode = 1;
    }
    delete data;
    fn_80153498(&section, -1);
}

void muReplayTask::controlExit() {
    Work* work = getWork();
    if (work->m_exitMode == 0 && !work->m_preloaded) {
        work->m_preloaded = true;
        u8* player = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 8);
        for (int i = 0; i < 4; i++) {
            int slot = player[0x99];
            if (!slot || slot == 1) {
                int kind = player[0x98];
                if (kind >= 0 || kind < 0x3E) {
                    fn_1_379C(i, kind, player[0x9D]);
                }
            }
            player += 0x5C;
        }
    }
    if (getWork()->m_exitWait > 0) {
        getWork()->m_exitWait--;
    } else if (fn_1_385C(0) && fn_800C39D8(m_work->m_collection)) {
        int mode = getWork()->m_exitMode;
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = mode;
        manager->processStep = 2;
    }
}

void muReplayTask::cancelAndExit() {
    getWork()->m_exitMode = 1;
    m_work->m_exitRequest = 1;
}

int muReplayTask::dispThumbnail(MuObject* object, void* summary) {
    const muReplayInfo* info = static_cast<muReplaySummary*>(summary)->m_info;
    float frame = 0.0f;
    int mode = getGameMode(info);
    switch (mode) {
    case 0: {
        int stage = exchangeSrStageKindToMuStageKind__6muMenuFiii(info->m_stageKind);
        if (stage < 0x29) {
            frame = getStageFrameNo__6muMenuFiii(stage);
        }
        break;
    }
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        frame = mode + 0x3B;
        break;
    case 1:
        frame = 60.0f;
        break;
    case 7:
        frame = 67.0f;
        break;
    case 8:
        frame = 66.0f;
        break;
    }
    object->setFrameVisible(1.0f);
    muCollectionName name0(getResMdlName(object));
    muCollectionName name1 = name0;
    name1.addSeparator();
    muCollectionName name2 = name1;
    name2.addNumber(0);
    muCollectionName name3 = name2;
    object->changeTexPatAnimNIf(name3.m_str);
    callVirtualFloat(*reinterpret_cast<void**>(reinterpret_cast<u8*>(object->m_modelAnim) + 0x10), 0x28, 0.0f);
    object->setFrameTex(frame);
    return 1;
}

void muReplayTask::dispPreviewCore(void* infoPtr) {
    const muReplayInfo* info = static_cast<muReplayInfo*>(infoPtr);
    dispPreviewStage(static_cast<muReplayInfo*>(infoPtr));
    int count = 0;
    int offset = 0;
    for (u32 i = 0; i < 4; i++) {
        int slot = info->m_slotKind[i];
        if (!slot || slot == 1) {
            int kind = info->m_charKind[i];
            if (kind >= 0 || kind < 0x3E) {
                void* expand = m_work->m_objs[36];
                MuObject* object = m_work->m_objs[26 + offset / 4];
                int index = count;
                count++;
                offset += 4;
                callVirtual(expand, 0x3C, object->m_sceneModel);
                char name[8];
                sprintf(name, "pos%02d", index);
                PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_work->m_objs[36], object->m_sceneModel, name);
                if (kind >= 0 && kind < 0x3E) {
                    int stock = exchangeGmCharacterKind2MuStockchkind__6muMenuFi(kind);
                    int something = exchangeGmCharacterKind2Something__6muMenuFi(stock);
                    object->setFrameTex(something * 10 + 1);
                    m_work->m_msg->printf(index + 0x12, exchangeMuStockchkind2MuCharName__6muMenuFi(stock));
                }
            }
        }
    }
}

void muReplayTask::erasePreviewCore() {
    removeObjCore(m_work->m_objs[36]);
    for (int i = 0; i < 4; i++) {
        reinterpret_cast<nw4r::g3d::ScnGroup*>(m_work->m_objs[36])->Remove(static_cast<nw4r::g3d::ScnObj*>(m_work->m_objs[26 + i]->m_sceneModel));
    }
}

void muReplayTask::dispPreviewStage(muReplayInfo* info) {
    removeObjCore(m_work->m_objs[36]);
    addObjCore(m_work->m_objs[36]);
    int mode = getGameMode(info);
    switch (mode) {
    case 0: {
        int stage = exchangeSrStageKindToMuStageKind__6muMenuFiii(info->m_stageKind);
        if (stage < 0x29) {
            int msgId = exchangeMuStageKindToMsgID__6muMenuFiii(stage);
            m_work->m_msg->printIndex(0x11, msgId, getWork()->m_stageNames.m_handle.getBuffer());
        }
        break;
    }
    case 1: {
        char text[0x44];
        char name8[0x44];
        u16 name16[17];
        memcpy(name16, info->m_name, 0x20);
        name16[16] = 0;
        text[0] = 0;
        Message::utf16to8(name8, reinterpret_cast<const wchar_t*>(name16));
        strcat(text, name8);
        m_work->m_msg->printf(0x11, text);
        break;
    }
    case 2:
    case 3:
    case 4:
    case 5:
    case 6: {
        muReplayBuf buf;
        muReplayLabel label2;
        muReplayLabel label1;
        void* data = m_work->m_msgFile[0].m_handle.getBuffer();
        buf.m_size = 0;
        getPrintIndexData__7MessageFPvUlPPcPUl(data, mode + 4, &label1.m_text, &label1.m_size);
        getPrintIndexData__7MessageFPvUlPPcPUl(data, 5, &label2.m_text, &label2.m_size);
        buf.append(label2.m_text, label2.m_size);
        buf.append(label1.m_text, label1.m_size);
        printData__5MuMsgFUlPCvi(m_work->m_msg, 0x11, buf.m_data, buf.m_size);
        break;
    }
    case 7:
        m_work->m_msg->printIndex(0x11, 3, m_work->m_msgFile[0].m_handle.getBuffer());
        break;
    case 8:
        m_work->m_msg->printIndex(0x11, 4, m_work->m_msgFile[0].m_handle.getBuffer());
        break;
    }
}

int muReplayTask::getGameMode(const muReplayInfo* info) {
    int mode = 0;
    switch (info->m_gameMode) {
    case 8:
        mode = info->m_ruleKind + 2;
        break;
    case 9:
    case 0x12:
        mode = 7;
        break;
    case 10:
    case 0x10:
        mode = 8;
        break;
    default:
        if (info->m_stageKind == 0x35) {
            mode = 1;
        }
    }
    return mode;
}

bool muReplayTask::preLoad() {
    return true;
}

void muReplayTask::controlNewDataExit() { }
void muReplayTask::controlNewData() { }
void muReplayTask::controlNewDataChecking() { }
void muReplayTask::controlNewDataStart() { }
void muReplayTask::controlPopUpChecking() { }
void muReplayTask::controlPopUpCheckStart() { }
void muReplayTask::controlPopUp() { }
void muReplayTask::controlPopUpStart() { }
void muReplayTask::controlBrowsingLoading() { }
void muReplayTask::controlBrowsing() { }

int muReplayTask::getNumPanel(int media) {
    Work* work = getWork();
    u8 extra = work->m_extraPanel;
    return extra + getNumPic(media);
}

void muReplayTask::getNewLabel() { }

muReplayTask::Work::~Work() { }
