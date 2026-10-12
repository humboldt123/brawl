#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/menu.h>
#include <mu/wifi/mu_wifi_time_result.h>

// main.dol functions with names that the shared headers do not declare.
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// MuObject::changeMaterialTex(texture name, node, resource file) and (material, texture name, resource file)
extern "C" void changeMaterialTex__8MuObjectFPCciPQ34nw4r3g3d7ResFile(MuObject* object, const char* name, int node, nw4r::g3d::ResFile* resFile);
extern "C" void changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(MuObject* object, u32 material, const char* name,
                                                                         nw4r::g3d::ResFile* resFile);
extern "C" u32 GetResMat__Q34nw4r3g3d6ResMdlCFPCc(nw4r::g3d::ResMdl* mdl, const char* name);
// nw4r::g3d::ResFile::GetResAnmScn(index) and ResAnmScn::Bind(animation)
extern "C" u32 GetResAnmScn__Q34nw4r3g3d7ResFileCFi(nw4r::g3d::ResFile* resFile, int index);
extern "C" void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void* self, void* anm);
// gfSceneRoot::removeInfoCamResAnm(), setInfoCamResAnm(animation) and layerUpdateFrame(layer)
extern "C" void fn_8000D7B8(gfSceneRoot* root);
extern "C" void fn_8000D6DC(gfSceneRoot* root, void* resAnmScn);
extern "C" void fn_8000DB40(gfSceneRoot* root, int layer);
// gfFileIOHandle::gfFileIOHandle()
extern "C" void __ct__14gfFileIOHandleFv(gfFileIOHandle* handle);

// The flag of the menu data (of the record of the game) that says that the screen is wide.
// HYPOTHESIS: the name; the flag is the high bit of the byte at 0x28.
struct muWifiMenuBits {
    u8 m_unk00[0x28];
    bool m_isWide : 1;
};

// The models of the menu: the panel.
static const muWifiTimeResultModel sModels[1] = {{"MenWifiTime0000_TopN", 0, 0, 0}};

muWifiTimeResultTask* muWifiTimeResultTask::create(const muWifiTimeResultTaskParam* param) {
    muWifiTimeResultTask* task = new (Heaps::MenuInstance) muWifiTimeResultTask();
    task->m_pad = param->m_pad;
    fn_800B96E8(reinterpret_cast<muMenuController*>(&task->m_controller), param->m_pad & 0xFF);
    task->m_charKind = param->m_charKind;
    task->m_unk60 = param->m_time;
    task->m_unk64 = param->m_unkC;
    task->m_unk68 = param->m_unk10;
    task->m_unk6C = param->m_unk14;
    task->m_frame = param->m_unk18;
    char facePath[0x40];
    sprintf(facePath, "/menu/common/char_bust_tex/MenSelchrFaceB%03d.brres",
            static_cast<u32>(static_cast<float>(muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(task->m_charKind)) * 10 + 1)) - 1);
    char cameraPath[0x40];
    if (!reinterpret_cast<muWifiMenuBits*>(g_GameGlobal->getGlobalRecordMenuDatap())->m_isWide) {
        sprintf(cameraPath, "/menu/defaultcamera/CharacterSelect.brres");
    } else {
        sprintf(cameraPath, "/menu/defaultcamera/CharacterSelectWide.brres");
    }
    const char* paths[FileNum] = {facePath, "/menu/tournament/TourGame.brres", "/menu/wifi/wifi_time_result.brres", cameraPath,
                                  "/menu/msg/wifi_time_result.msbin"};
    for (int i = 0; i < FileNum; i++) {
        task->m_handles[i].readRequest(paths[i], Heaps::MenuResource, 0, 0);
    }
    sprintf(task->m_nameTex, "MenSelchrChrNm.%03d",
            static_cast<u32>(static_cast<float>(muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeGmCharacterKind2MuStockchkind(task->m_charKind)) * 10 + 1)));
    return task;
}

muWifiTimeResultTask::muWifiTimeResultTask()
    : gfTask("WifiTimeResult", Category_Menu, 0xF, 8, true), m_unk60(0), m_unk64(0), m_unk68(0), m_unk6C(0), m_frame(0), m_animChanged(false),
      m_started(false) {
    unk2C_b1 = false;
    m_object = NULL;
    m_loaded[0] = false;
    m_loaded[1] = false;
    m_loaded[2] = false;
    m_loaded[3] = false;
    m_loaded[4] = false;
}

muWifiTimeResultTask::~muWifiTimeResultTask() {
    delete m_object;
    fn_8000D7B8(g_gfSceneRoot);
    for (int i = 0; i < FileNum; i++) {
        gfHeapManager::free(m_handles[i].getBuffer());
        m_handles[i].release();
    }
}

void muWifiTimeResultTask::createObjResFile(const muWifiTimeResultModel* models, int count, nw4r::g3d::ResFile* resFile, int priority) {
    for (int i = 0; i < count; i++, models++) {
        int num = 1;
        if (models->m_start < models->m_end) {
            num = models->m_end - models->m_start;
        }
        for (int j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, models->m_name, models->m_priority + priority, NULL, Heaps::MenuResource);
            (&m_object)[models->m_start + j] = object;
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

void muWifiTimeResultTask::initMsg() {
    char buffer[10] = {0};
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, 7);
    m_msg->setMsgData(m_handles[4].getBuffer());
    MuObject* object = m_object;
        m_msg->attachScnMdlSimple(0, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 0, 0.0625f);
    m_msg->setHScale(0, 1.2f);
    m_msg->getHScale(0, 1.2f);
    m_msg->setAlignMode(0, MuMsg::Align_Center);
    m_msg->setVerticalMode(0, true);
    m_msg->setFontColor(0, 0xD0, 0xDE, 0xE7, 0xFF);
    m_msg->printIndex(0, 0);
    m_msg->attachScnMdlSimple(1, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 1, 0.0625f);
    m_msg->getHScale(1, 1.5f);
    m_msg->setEdgeEnable(1, true);
    m_msg->setEdge(1, 0, 0, 0, 3.0f);
    m_msg->setAlignMode(1, MuMsg::Align_Center);
    m_msg->setFontColor(1, 0xD2, 0xB2, 0xB2, 0xFF);
    m_msg->setFontType(1, 1);
    sprintf(buffer, "%d", m_unk60);
    m_msg->printf(1, buffer);
    m_msg->attachScnMdlSimple(2, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 2, 0.0625f);
    m_msg->setHScale(2, 1.2f);
    m_msg->getHScale(2, 1.2f);
    m_msg->setAlignMode(2, MuMsg::Align_Center);
    m_msg->setVerticalMode(2, true);
    m_msg->setFontColor(2, 0xD0, 0xDE, 0xE7, 0xFF);
    m_msg->printIndex(2, 1);
    m_msg->attachScnMdlSimple(3, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 3, 0.0625f);
    m_msg->getHScale(3, 1.5f);
    m_msg->setEdgeEnable(3, true);
    m_msg->setEdge(3, 0, 0, 0, 3.0f);
    m_msg->setAlignMode(3, MuMsg::Align_Center);
    m_msg->setFontColor(3, 0xD2, 0xB2, 0xB2, 0xFF);
    m_msg->setFontType(3, 1);
    sprintf(buffer, "%d", m_unk64);
    m_msg->printf(3, buffer);
    m_msg->attachScnMdlSimple(4, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 4, 0.0625f);
    m_msg->setHScale(4, 1.2f);
    m_msg->getHScale(4, 1.2f);
    m_msg->setAlignMode(4, MuMsg::Align_Center);
    m_msg->setVerticalMode(4, true);
    m_msg->setFontColor(4, 0xD0, 0xDE, 0xE7, 0xFF);
    m_msg->printIndex(4, 2);
    m_msg->attachScnMdlSimple(5, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 5, 0.0625f);
    m_msg->getHScale(5, 1.5f);
    m_msg->setEdgeEnable(5, true);
    m_msg->setEdge(5, 0, 0, 0, 3.0f);
    m_msg->setAlignMode(5, MuMsg::Align_Center);
    m_msg->setFontColor(5, 0xD2, 0xB2, 0xB2, 0xFF);
    m_msg->setFontType(5, 1);
    sprintf(buffer, "%d", m_unk68);
    m_msg->printf(5, buffer);
    m_msg->attachScnMdlSimple(6, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(object->m_sceneModel), 6, 0.0625f);
    m_msg->getHScale(6, 2.0f);
    m_msg->setAlignMode(6, MuMsg::Align_Right);
    m_msg->setFontColor(6, 0x6B, 0x58, 0, 0xFF);
    m_msg->setFontType(6, 1);
    sprintf(buffer, "%d", m_unk6C);
    m_msg->printf(6, buffer);
}

void muWifiTimeResultTask::processDefault() {
    for (int i = 0; i < FileNum; i++) {
        if (!m_loaded[i] && m_handles[i].m_request != NULL) {
            if (!m_handles[i].isReady()) {
                return;
            }
            m_loaded[i] = true;
        }
    }
    if (m_loaded[4] && !m_started) {
        m_started = true;
        u32 bind;
        u32 anim = 0;
        m_resFileCamera = nw4r::g3d::ResFile(m_handles[3].getBuffer());
        nw4r::g3d::ResFile::Init(&m_resFileCamera);
        anim = GetResAnmScn__Q34nw4r3g3d7ResFileCFi(&m_resFileCamera, 0);
        bind = anim;
        Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&anim, &bind);
        fn_8000D6DC(g_gfSceneRoot, &anim);
        m_resFileModel = nw4r::g3d::ResFile(m_handles[2].getBuffer());
        nw4r::g3d::ResFile::Init(&m_resFileModel);
        createObjResFile(sModels, 1, &m_resFileModel, 0);
        m_resFileFace = nw4r::g3d::ResFile(m_handles[0].getBuffer());
        nw4r::g3d::ResFile::Init(&m_resFileFace);
        m_resFileName = nw4r::g3d::ResFile(m_handles[1].getBuffer());
        nw4r::g3d::ResFile::Init(&m_resFileName);
        initMsg();
        g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(m_object->m_sceneModel));
        MuObject* object = m_object;
        object->setFrame(0.0f);
        object->m_modelAnim->setUpdateRate(1.0f);
    }
    MuObject* object = m_object;
    fn_8000DB40(g_gfSceneRoot, 8);
    if (!m_animChanged && object->m_modelAnim->getFrame() == 60.0f) {
        object->changeAnimN("MenWifiTime0000_TopN__1");
        m_animChanged = true;
    }
    m_object->setFrameTex(static_cast<float>(m_frame));
    changeMaterialTex__8MuObjectFPCciPQ34nw4r3g3d7ResFile(object, "Character_Bust_Mat", 0, &m_resFileFace);
    changeMaterialTex__8MuObjectFPCciPQ34nw4r3g3d7ResFile(object, "Character_Bust_Shadow_Mat", 0, &m_resFileFace);
    u32 mat = GetResMat__Q34nw4r3g3d6ResMdlCFPCc(&object->m_resMdl, "Character_Name_Mat");
    changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(object, *reinterpret_cast<u32*>(mat + 0xC), m_nameTex, &m_resFileName);
    mat = GetResMat__Q34nw4r3g3d6ResMdlCFPCc(&object->m_resMdl, "Character_Name_Shadow_Mat");
    changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(object, *reinterpret_cast<u32*>(mat + 0xC), m_nameTex, &m_resFileName);
}
