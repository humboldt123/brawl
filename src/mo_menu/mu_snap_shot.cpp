#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_snap_shot.h>

// main.dol functions with names that the shared headers do not declare.
// Message::getPrintIndexData(data, index, text, size)
extern "C" void getPrintIndexData__7MessageFPvUlPPcPUl(void* data, u32 index, char** text, u32* size);
// HYPOTHESIS: the loader of a file of the menu: reset(loader) and request(loader, path, heap, 0)
extern "C" void fn_800FABEC(muCollectionViewer::MsgFile* loader);
extern "C" void fn_800FAC58(muCollectionViewer::MsgFile* loader, const char* path, Heaps::HeapType heap, int unk);
// the pictures of the collection: the name of the file, the decoded picture and the check that the collection is ready
extern "C" muCollectionFileName fn_800C3538(const char* names, int index);
extern "C" void fn_800C7324(muSnapShotJpeg* result, void* summary, Heaps::HeapType heap, int unk);
extern "C" int fn_800C39D8(void* collection);
// MuObject::changeMaterialTex(texture name, picture, width, height)
extern "C" void changeMaterialTex__8MuObjectFPCcPvUsUs(MuObject* object, const char* name, void* pixels, u16 width, u16 height);

// Functions of the other modules, which the module calls by their offset (module 6 is sora_menu_collect_viewer).
// muCollectionViewer::loadResource() and muCollectionViewer::createObjResFile(name)
extern "C" void fn_6_1134(muCollectionViewer* viewer);
extern "C" void fn_6_1260(muCollectionViewer* viewer, const char* name);
// muCollectionViewer::checkBackgroundLoading(summary) and muCollectionViewer::createNoticeWindow(text)
extern "C" int fn_6_3934(muCollectionViewer* viewer, void** summary);
extern "C" void fn_6_535C(muCollectionViewer* viewer, void* text);
// scSnapShot::setMotionObj(scene, objects, count) of the module sora_scene
extern "C" void fn_1_7A94(scSnapShot* scene, MuObject** objects, int count);

// The text of a notice: the data and how much of it is used.
struct muSnapShotNotice {
    char m_data[0x200];
    int m_size;

    void append(const char* text, u32 size) {
        int used = m_size;
        memcpy(m_data + used, text, size);
        m_size = used + size;
    }
};

// A text of the message file: where it is and how long it is.
// MATCH-ONLY: the two words are one structure so that the stack layout of the function that uses it is the same.
struct muSnapShotLabel {
    char* m_text;
    u32 m_size;
};

// The decoded picture before it is handed to the menu (no constructor and no destructor: the menu resets it by hand).
// MATCH-ONLY: the destructor of muSnapShotJpeg would be called at the end of the scope instead of in the middle.
struct muSnapShotJpegTemp {
    void* m_pixels;
    u32 m_width;
    u32 m_height;
};

// What the checks of the background loading give: the picture of the snapshot and what is known of it.
// HYPOTHESIS: the names follow how the menu uses the fields.
struct muSnapShotSummary {
    u8 m_unk00[0xC];
    void* m_jpeg;                   // 0x0C
    u8* m_info;                     // 0x10 the first byte says which frame of the visibility animation is shown
};

// The byte of the menu data that holds the flags of the screen. HYPOTHESIS: the high bit says that the screen is wide.
struct muSnapShotMenuBits {
    u8 m_unk00[0x28];
    u8 m_flags;
};

// Resets one of the loaders of the menu, clears the data around it and asks it to read the file.
static inline void requestResource(muCollectionViewer::MsgFile* loader, int* unk, u8* flag, const char* path) {
    fn_800FABEC(loader);
    *unk = 0;
    *flag = 0;
    fn_800FAC58(loader, path, Heaps::MenuResource, 0);
}

static inline void callVirtualFloat(void* object, int offset, float value) {
    (*reinterpret_cast<void (**)(void*, float)>(*reinterpret_cast<u8**>(object) + offset))(object, value);
}

inline muSnapShotTask::Work::Work() : muCollectionViewer::Work(0x19, 4, "PhotoMat"), m_scene(NULL), m_slideResult(0) {
    m_unk1CF8 = 0xF;
}

muSnapShotTask* muSnapShotTask::create(scSnapShot* scene) {
    muSnapShotTask* task = new (Heaps::MenuInstance) muSnapShotTask("SnapShot");
    Work* work = new (Heaps::MenuInstance) Work();
    task->m_work = work;
    work->m_scene = scene;
    return task;
}

muSnapShotTask::~muSnapShotTask() { }

void muSnapShotTask::loadResource() {
    fn_6_1134(this);
    Work* work = getWork();
    requestResource(&work->m_brres, &work->m_unk000, &work->m_brresFlag, "/menu/collection/Snap.brres");
    work = getWork();
    requestResource(&work->m_wait, &work->m_unk09C, &work->m_waitFlag, "/menu/common/Wait.brres");
    fn_800FAC58(&getWork()->m_msgFile[0], "/menu/msg/snap_shot.msbin", Heaps::MenuResource, 0);
    fn_800FAC58(&getWork()->m_msgFile[1], "/menu/msg/save_load.msbin", Heaps::MenuResource, 0);
}

void muSnapShotTask::createModel() {
    fn_6_1260(this, "Snap");
    static muCollectionName sName("MenSnapPhoto2_TopN");
    changePicAnim(sName.m_str, 1, m_work);
    fn_1_7A94(getWork()->m_scene, &m_work->m_objs[0], 0x19);
    MuObject* object = m_work->m_objs[24];
    const u8* resData = *reinterpret_cast<u8* const*>(&object->m_resMdl);
    int nameOffset = *reinterpret_cast<const int*>(resData + 0x3C);
    const char* resName = nameOffset != 0 ? reinterpret_cast<const char*>(resData + nameOffset) : NULL;
    muCollectionName name0(resName);
    muCollectionName name1 = name0;
    name1.addSeparator();
    muCollectionName name2 = name1;
    name2.addNumber(0);
    muCollectionName name3 = name2;
    object->changeVisAnimNIf(name3.m_str);
    callVirtualFloat(*reinterpret_cast<void**>(reinterpret_cast<u8*>(object->m_modelAnim) + 8), 0x28, 0.0f);
    expandPic(1);
}

void muSnapShotTask::onPreCheckFinished() { }

void muSnapShotTask::getMineLabel(muCollectionMsgBuf* buf) {
    void* data = m_work->m_msgFile[0].m_handle.getBuffer();
    u32 size;
    char* text;
    getPrintIndexData__7MessageFPvUlPPcPUl(data, 3, &text, &size);
    buf->append(text, size);
}

void muSnapShotTask::addObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Insert(*reinterpret_cast<int*>(reinterpret_cast<u8*>(root) + 0xE4), static_cast<nw4r::g3d::ScnObj*>(obj));
}

void muSnapShotTask::removeObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Remove(static_cast<nw4r::g3d::ScnObj*>(obj));
}

void* muSnapShotTask::getScnRoot() {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    if (holder->m_hasScnRoot) {
        return holder->m_scnRoot;
    }
    return NULL;
}

void muSnapShotTask::setPanelStateCore(int state, int unk) {
    // HYPOTHESIS: the table takes the names of the two panels, 11 animations each, and tells for each state which one to play.
    static const char* sPanelNameA = "MenSnap_TopN__0";
    static const char* sPanelNameB = "MenSnap_TopN__0";
    static muCollectionViewer::PanelMotionTable sTable(sPanelNameA, sPanelNameB);
    m_work->m_objs[1]->setAnimName(&sTable.m_anim[sTable.m_index[state]], false);
    if (unk != 0) {
        m_work->m_objs[23]->setAnimName(&sTable.m_anim[sTable.m_index[state]] + 11, false);
    }
    m_work->m_panelState = state;
}

void muSnapShotTask::onDecided() {
    getWork()->m_slideResult = 0;
    m_work->m_state = 0x16;
    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
}

void muSnapShotTask::controlBrowsing() {
    if (m_work->m_media[m_work->m_curMedia].m_numPic == 0 || (m_input & 6) != 0) {
        removeObjCore(m_work->m_objs[24]->m_sceneModel);
        m_work->m_state = 0;
        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
    } else {
        int result = slidePic(1, 1);
        getWork()->m_slideResult = result;
        if (result != 0) {
            m_work->m_state = 0x16;
        }
    }
}

void muSnapShotTask::controlBrowsingLoading() {
    muSnapShotLabel label;
    muSnapShotSummary* summary[2];      // MATCH-ONLY: the second word is not used, it gives the frame the same size as the original
    if (!fn_6_3934(this, reinterpret_cast<void**>(summary))) {
        removeObjCore(m_work->m_objs[24]->m_sceneModel);
        m_work->m_state = 0;
    } else if (summary[0] != NULL) {
        if (summary[0]->m_jpeg != NULL && summary[0]->m_info != NULL) {
            float frames[4] = {0.0f, 1.0f, 0.0f, 2.0f};
            MuObject* object = m_work->m_objs[24];
            u8 bits = reinterpret_cast<muSnapShotMenuBits*>(g_GameGlobal->getGlobalRecordMenuDatap())->m_flags;
            object->setFrameVisible(reinterpret_cast<float*>(reinterpret_cast<u8*>(frames) + ((bits >> 4) & 8))[summary[0]->m_info[0]]);
            muSnapShotJpeg* jpeg = &getWork()->m_jpeg;
            jpeg->reset(NULL);
            muSnapShotJpegTemp decoded;
            fn_800C7324(reinterpret_cast<muSnapShotJpeg*>(&decoded), summary[0], Heaps::MenuResource, 1);
            void* pixels = decoded.m_pixels;
            decoded.m_pixels = NULL;
            jpeg->reset(pixels);
            jpeg->m_width = decoded.m_width;
            jpeg->m_height = decoded.m_height;
            fn_800C2BB4(reinterpret_cast<muSnapShotJpeg*>(&decoded), NULL);
            changeMaterialTex__8MuObjectFPCcPvUsUs(object, "photoMat", jpeg->m_pixels, jpeg->m_width, jpeg->m_height);
            removeObjCore(object->m_sceneModel);
            addObjCore(object->m_sceneModel);
            m_work->m_state = 0x15;
        } else if (getWork()->m_slideResult == 0) {
            removeObjCore(m_work->m_objs[24]->m_sceneModel);
            muSnapShotNotice notice;
            notice.m_size = 0;
            void* data = m_work->m_msgFile[2].m_handle.getBuffer();
            getPrintIndexData__7MessageFPvUlPPcPUl(data, 0x3A, &label.m_text, &label.m_size);
            notice.append(label.m_text, label.m_size);
            fn_6_535C(this, &notice);
        } else {
            slidePicCore(1);
        }
    }
}

void muSnapShotTask::cancelAndExit() {
    m_work->m_exitRequest = 1;
}

void muSnapShotTask::controlExit() {
    if (fn_800C39D8(m_work->m_collection)) {
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = 0;
        manager->processStep = 2;
    }
}

bool muSnapShotTask::preLoad() {
    return true;
}

void muSnapShotTask::controlNewDataExit() { }

void muSnapShotTask::controlNewData() { }

void muSnapShotTask::controlNewDataChecking() { }

void muSnapShotTask::controlNewDataStart() { }

void muSnapShotTask::controlPopUpChecking() { }

void muSnapShotTask::controlPopUpCheckStart() { }

void muSnapShotTask::controlPopUp() { }

void muSnapShotTask::controlPopUpStart() { }

void muSnapShotTask::onLoadFailed() { }

void muSnapShotTask::erasePreviewCore() { }

void muSnapShotTask::dispPreviewCore(void* info) { }

int muSnapShotTask::dispThumbnail(MuObject* object, void* summary) {
    return 0;
}

int muSnapShotTask::getNumPanel(int media) {
    Work* work = getWork();
    u8 extra = work->m_extraPanel;
    return extra + getNumPic(media);
}

int muSnapShotTask::getNumPic(int media) {
    return m_work->m_media[media].m_numPic;
}

muCollectionFileName muSnapShotTask::getFileHandle(int index) {
    return fn_800C3538(m_work->m_media[m_work->m_curMedia].m_names, index);
}

u32 muSnapShotTask::getCurFileID() {
    Work* work = getWork();
    u32 count = work->m_extraPanel != 0 ? work->m_numFiles - 1 : work->m_numFiles;
    return count < getNumPic(work->m_curMedia) ? count : 0x80000000;
}

void muSnapShotTask::getNewLabel() { }

void muSnapShotTask::controlLoadFinish() { }

muSnapShotTask::Work::~Work() { }

