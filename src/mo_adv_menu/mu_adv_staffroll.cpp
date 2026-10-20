#include <cm/cm_camera_controller.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <revolution/MTX/mtx.h>
#include <string.h>
#include <types.h>
#include <ut/ut_relocate.h>

#include <mu/mu_adv_staffroll.h>

// main.dol functions with names that the shared headers do not declare.
// the root of a group of models that the menu draws: name, layer and the camera pack (or NULL)
extern "C" muAdvStaffRoot* fn_800AFB94(const char* name, int layer, const char* cameraPack);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
extern "C" void allocMsgBuf__5MuMsgFUlUl(MuMsg* msg, u32 size, u32 num);
extern "C" void setMsgData__5MuMsgFPv(MuMsg* msg, void* data);
extern "C" void* getMsgData__5MuMsgFUlPi(MuMsg* msg, u32 index, int* length);
extern "C" void setHScale__5MuMsgFUlf(MuMsg* msg, u32 index, float scale);
extern "C" void getHScale__5MuMsgFUlf(MuMsg* msg, u32 index, float scale);
extern "C" void printData__5MuMsgFUlPCvi(MuMsg* msg, u32 index, const void* data, int size);
// mvMoviePlayer: constructor, destructor(flags), open(path ...), setScnMdl(model, 0), play(), drawFrame(...),
// getVideoState() and getTotalFrame()
extern "C" void fn_8007E99C(void* player);
extern "C" void fn_8007EA78(void* player, int flags);
extern "C" void fn_8007EC08(void* player, const char* path, int unk1, int unk2, int unk3);
extern "C" void fn_8007F2B4(void* player, void* model, int unk);
extern "C" void fn_8007F148(void* player);
extern "C" int fn_8007EF70(void* player, int x, int y, int w, int h);
extern "C" int fn_8007EF64(void* player);
extern "C" int fn_8007EF68(void* player);
// nw4r::g3d::ScnMdl: set the matrix of a node, and the matrix of a node of a ScnObj
extern "C" void ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(void* scnMdl, u32 node, const Mtx mtx);
extern "C" void GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(void* scnObj, int type, Mtx mtx);
// the channel of a material of a model, and its color
extern "C" u32 GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(nw4r::g3d::ScnMdl::CopiedMatAccess* access);
struct muAdvStaffColor {
    u8 m_r;
    u8 m_g;
    u8 m_b;
    u8 m_a;
};
extern "C" nw4r::g3d::ResMat GetResMat__Q34nw4r3g3d6ResMdlCFi(const nw4r::g3d::ResMdl* mdl, int index);
extern "C" void GXGetChanMatColor__Q34nw4r3g3d10ResMatChanCF12_GXChannelIDP8_GXColor(u32* chan, int id, muAdvStaffColor* color);
extern "C" void GXSetChanMatColor__Q34nw4r3g3d10ResMatChanF12_GXChannelID8_GXColor(u32* chan, int id, muAdvStaffColor color);

extern "C" void Release__Q34nw4r3g3d7ResFileFv(nw4r::g3d::ResFile* resFile);
extern "C" void setRotate__8MuObjectFP5Vec3f(MuObject* object, Vec3f* rotation);

// Adds a model to the group of models a root draws, or takes it away again (virtual functions of the group).
static inline void addModel(muAdvStaffRoot* root, void* model) {
    u8* group = reinterpret_cast<u8*>(root->scene);
    u8* vtbl = *reinterpret_cast<u8**>(group);
    (*reinterpret_cast<void (**)(void*, void*, void*)>(vtbl + 0x34))(group, *reinterpret_cast<void**>(group + 0xE4), model);
}

static inline void removeModel(muAdvStaffRoot* root, void* model) {
    u8* group = reinterpret_cast<u8*>(root->scene);
    u8* vtbl = *reinterpret_cast<u8**>(group);
    (*reinterpret_cast<void (**)(void*, void*)>(vtbl + 0x3C))(group, model);
}

muAdvStaffrollTask* muAdvStaffrollTask::create() {
    muAdvStaffrollTask* task = new (Heaps::MenuInstance) muAdvStaffrollTask();
    return task;
}

muAdvStaffrollTask::muAdvStaffrollTask() : gfTask("AdvStaffroll", Category_Menu, 0xF, 8, true) {
    m_objectRoot = NULL;
    m_movieRoot = NULL;
    m_textRoot = NULL;
    m_movie = NULL;
    memset(m_logos, 0, sizeof(m_logos));
    m_allRights = NULL;
    m_theEnd = NULL;
    m_texts = NULL;
    m_msg = NULL;
    m_player = NULL;
    m_init = NULL;
    m_end = NULL;
    m_rootVisible = NULL;
    m_rootMove = NULL;
    m_modelParam = NULL;
    m_scaleParam = NULL;
    m_colorParam = NULL;
    m_textParam = NULL;
    m_textY = NULL;
    m_state = 0;
    m_stateFrame = 0;
}

muAdvStaffrollTask::~muAdvStaffrollTask() { }

// Sets the alpha of the color of every material of the model.
static void setModelAlpha(MuObject* object, float fade) {
    nw4r::g3d::ResMdl& mdl = object->m_resMdl;
    u32 count = mdl.GetResMatNumEntries();
    int alpha = static_cast<int>(fade * 255.0f);
    for (u32 i = 0; i < count; i++) {
        nw4r::g3d::ScnMdl* scnMdl = object->m_scnMdl;
        nw4r::g3d::ResMat mat = GetResMat__Q34nw4r3g3d6ResMdlCFi(&mdl, i);
        nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
        u32 chan = GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(&access);
        muAdvStaffColor color;
        GXGetChanMatColor__Q34nw4r3g3d10ResMatChanCF12_GXChannelIDP8_GXColor(&chan, 2, &color);
        color.m_a = static_cast<u8>(alpha);
        GXSetChanMatColor__Q34nw4r3g3d10ResMatChanF12_GXChannelID8_GXColor(&chan, 2, color);
    }
}

// Moves the camera one frame along its path.
static inline void updateCamera(muAdvStaffCamera* camera) {
    if (camera->m_active) {
        int total = camera->m_total;
        int count = camera->m_count + 1;
        float t = static_cast<float>(count) / static_cast<float>(total);
        camera->m_count = count;
        Vec3f delta = camera->m_to - camera->m_from;
        Vec3f step = delta * t;
        camera->m_cur = camera->m_from + step;
        if (total <= count) {
            camera->m_active = false;
        }
    }
}

void muAdvStaffrollTask::processDefault() {
    gfPadStatus pad[4];
    g_gfPadSystem->getSysPadStatus(0xF0, pad);
    switch (m_state) {
    case 1:
        m_movieDraw = true;
        m_state = 2;
        break;
    case 3: {
        CameraController::getInstance()->changeInput(CameraController::MenuPad);
        m_frame = 0;
        m_state = 4;
        break;
    }
    case 4: {
        muAdvStaffCamera* cameras[2] = {&m_cameras[0], &m_cameras[1]};
        if (m_init->m_length <= m_scrollY) {
            m_state = 5;
        }
        float y = m_scrollY + m_scrollSpeed;
        m_frame++;
        m_scrollY = y;
        if (1550.0f <= y) {
            m_scrollSpeed = m_init->m_length / static_cast<float>(m_init->m_frameNum) * 1.14f;
        }
        updateCamera(cameras[0]);
        updateCamera(cameras[1]);
        for (int i = 0; i < m_rootVisibleNum; i++) {
            muAdvStaffRootVisible* entry = &m_rootVisible[i];
            if (entry->m_frame <= static_cast<float>(m_frame) && entry->m_pending == 1) {
                entry->m_pending = 0;
                if (m_rootVisible[i].m_show == 1) {
                    addModel(m_movieRoot, m_movie->m_sceneModel);
                    m_movieShown = true;
                } else {
                    removeModel(m_movieRoot, m_movie->m_sceneModel);
                    m_movieShown = false;
                }
            }
        }
        for (int i = 0; i < m_rootMoveNum; i++) {
            muAdvStaffRootMove* entry = &m_rootMove[i];
            if (entry->m_frame <= static_cast<float>(m_frame) && entry->m_pending == 1) {
                entry->m_pending = 0;
                float toX = m_rootMove[i].m_toX;
                float toY = m_rootMove[i].m_toY;
                muAdvStaffCamera* camera = cameras[m_rootMove[i].m_target];
                int frames = m_rootMove[i].m_frameNum;
                camera->m_active = true;
                camera->m_from = camera->m_cur;
                camera->m_to = Vec3f(toX, toY, 0.0f);
                camera->m_total = frames;
                camera->m_count = 0;
            }
        }
        for (int i = 0; i < m_textNum; i++) {
            float y0 = m_textY[i];
            if (y0 - 30.0f <= m_scrollY && m_scrollY <= y0 + 30.0f) {
                addModel(m_textRoot, m_texts[i]->m_sceneModel);
            } else {
                removeModel(m_textRoot, m_texts[i]->m_sceneModel);
            }
        }
        for (int i = 0; i < 2; i++) {
            float y0 = m_modelParam[i];
            if (y0 - 30.0f <= m_scrollY && m_scrollY <= y0 + 30.0f) {
                addModel(m_textRoot, m_logos[i]->m_sceneModel);
            } else {
                removeModel(m_textRoot, m_logos[i]->m_sceneModel);
            }
        }
        Mtx mtx0;
        PSMTXIdentity(mtx0);
        Vec3f origin;
        origin = Vec3f(0.0f, 0.0f, 0.0f);
        PSMTXTransApply(mtx0, mtx0, origin.m_x, origin.m_y, origin.m_z);
        ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(m_textRoot->scene, 0, mtx0);
        Mtx mtx1;
        GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(m_textRoot->scene, 0, mtx1);
        Vec3f scroll;
        scroll = Vec3f(0.0f, m_scrollY, 0.0f);
        PSMTXTransApply(mtx1, mtx1, scroll.m_x, scroll.m_y, scroll.m_z);
        ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(m_textRoot->scene, 0, mtx1);
        Vec3f cam0 = m_cameras[0].m_cur;
        Mtx mtx2;
        GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(m_textRoot->scene, 0, mtx2);
        PSMTXTransApply(mtx2, mtx2, cam0.m_x, cam0.m_y, cam0.m_z);
        ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(m_textRoot->scene, 0, mtx2);
        Vec3f cam1 = m_cameras[1].m_cur;
        Mtx mtx3;
        PSMTXIdentity(mtx3);
        PSMTXTransApply(mtx3, mtx3, cam1.m_x, cam1.m_y, cam1.m_z);
        ScnMdl_SetNodeMtx__FPQ34nw4r3g3d6ScnMdlUlPC6Matrix(m_movieRoot->scene, 0, mtx3);
        break;
    }
    case 5:
        CameraController::getInstance()->changeInput(CameraController::MenuFixed);
        m_scrollSpeed = 0.0f;
        if (m_done) {
            m_state = 0xF;
        } else {
            m_state = 6;
        }
        break;
    case 6:
        addModel(m_objectRoot, m_allRights->m_sceneModel);
        m_state = 7;
        m_fade = 0.0f;
        m_stateFrame = 0;
        break;
    case 7:
        m_fade += 0.033333335f;
        if (m_stateFrame >= 0x1E) {
            m_state = 8;
        }
        break;
    case 8:
        m_state = 9;
        break;
    case 9:
        m_state = 10;
        m_fade = 1.0f;
        m_stateFrame = 0;
        break;
    case 10:
        if (m_stateFrame >= 300) {
            m_state = 0xB;
        }
        break;
    case 0xB:
        m_state = 0xC;
        break;
    case 0xC:
        m_state = 0xD;
        m_fade = 1.0f;
        m_stateFrame = 0;
        break;
    case 0xD:
        m_fade -= 0.033333335f;
        if (m_stateFrame >= 0x1E) {
            m_state = 0xE;
        }
        break;
    case 0xE:
        removeModel(m_objectRoot, m_allRights->m_sceneModel);
        m_state = 0xF;
        break;
    case 0xF:
        addModel(m_objectRoot, m_theEnd->m_sceneModel);
        m_state = 0x10;
        m_fade = 0.0f;
        m_stateFrame = 0;
        break;
    case 0x10:
        m_fade += 0.008333334f;
        if (m_stateFrame >= 0x78) {
            m_state = 0x11;
        }
        break;
    case 0x11:
        m_state = 0x12;
        break;
    case 0x12:
        m_state = 0x13;
        m_fade = 1.0f;
        break;
    case 0x13: {
        u32 pressed;
        if ((pad[0].m_buttonsPressedThisFrame.bits & 0x100) == 0) {
            pressed = (pad[0].m_buttonsPressedThisFrame.bits >> 12) & 1;
        } else {
            pressed = 1;
        }
        if (pressed == 1) {
            m_state = 0x14;
        }
        break;
    }
    case 0x14:
        m_state = 0x15;
        break;
    case 0x15:
        m_state = 0x16;
        m_fade = 1.0f;
        m_stateFrame = 0;
        break;
    case 0x16:
        m_fade -= 0.008333334f;
        if (m_stateFrame >= 0x78) {
            m_state = 0x17;
        }
        break;
    case 0x17:
        m_movieDraw = false;
        removeModel(m_objectRoot, m_theEnd->m_sceneModel);
        m_state = 0x18;
        break;
    case 0x18:
    case 0x19:
        break;
    }
    switch (m_state) {
    case 7:
    case 10:
    case 0xD:
        setModelAlpha(m_allRights, m_fade);
        break;
    }
    switch (m_state) {
    case 0x10:
    case 0x13:
    case 0x16:
        setModelAlpha(m_theEnd, m_fade);
        break;
    }
    m_stateFrame++;
}

void muAdvStaffrollTask::renderOpa() {
    if (m_movieDraw == true) {
        m_unk84 = fn_8007EF70(m_player, 0, 0, -1, -1);
    }
}

static inline void initCamera(muAdvStaffCamera* camera) {
    camera->m_cur = Vec3f(0.0f, 0.0f, 0.0f);
    camera->m_active = false;
    camera->m_from = Vec3f(0.0f, 0.0f, 0.0f);
    camera->m_to = Vec3f(0.0f, 0.0f, 0.0f);
    camera->m_total = 0;
    camera->m_count = 0;
}

void muAdvStaffrollTask::initialize() {
    unk2C_b1 = false;
    void* player = new (Heaps::MenuInstance) u8;
    if (player != NULL) {
        fn_8007E99C(player);
    }
    m_player = player;
    m_movieShown = false;
    m_movieDraw = false;
    m_movieFrames = 0;
    m_unk84 = 0;
    m_textNum = 0;
    m_rootVisibleNum = 0;
    m_rootMoveNum = 0;
    m_scrollY = 0.0f;
    m_scrollSpeed = 1.0f;
    m_frame = 0;
    m_fade = 0.0f;
    m_done = false;
    initCamera(&m_cameras[0]);
    initCamera(&m_cameras[1]);
}

void muAdvStaffrollTask::release() {
    m_objectRoot->exit();
    m_objectRoot = NULL;
    m_movieRoot->exit();
    m_movieRoot = NULL;
    m_textRoot->exit();
    m_textRoot = NULL;
    delete m_movie;
    m_movie = NULL;
    for (int i = 0; i < 2; i++) {
        delete m_logos[i];
        m_logos[i] = NULL;
    }
    delete m_allRights;
    m_allRights = NULL;
    delete m_theEnd;
    m_theEnd = NULL;
    for (int i = 0; i < m_textNum; i++) {
        m_msg->detachMuObject(m_texts[i]);
        delete m_texts[i];
        m_texts[i] = NULL;
    }
    delete[] m_texts;
    delete m_msg;
    m_msg = NULL;
    fn_8007EA78(m_player, 1);
    m_player = NULL;
    delete[] m_textY;
    m_textY = NULL;
    gfHeapManager::free(m_handleModels.getBuffer());
    m_handleModels.release();
    gfHeapManager::free(m_handleTexts.getBuffer());
    m_handleTexts.release();
    gfHeapManager::free(m_handleExcel.getBuffer());
    m_handleExcel.release();
    Release__Q34nw4r3g3d7ResFileFv(&m_resFile);
}

void muAdvStaffrollTask::requestFiles() {
    m_handleModels.readRequest("/menu/adventure/staffroll.brres", Heaps::MenuResource, 0, 0);
    m_handleTexts.readRequest("/menu/msg/adv_staffroll.msbin", Heaps::MenuResource, 0, 0);
    m_handleExcel.readRequest("/menu/adventure/mu_staffroll.dat", Heaps::MenuResource, 0, 0);
}

void muAdvStaffrollTask::openMovie() {
    fn_8007EC08(m_player, "/movie/ADV_ending.thp", 0, 0, 0);
}

int muAdvStaffrollTask::isLoadingFiles() {
    int loading;
    if (!m_handleModels.isReady()) {
        loading = 1;
    } else if (!m_handleTexts.isReady()) {
        loading = 1;
    } else {
        loading = m_handleExcel.isReady() == 0;
    }
    return loading;
}

bool muAdvStaffrollTask::isLoadingMovie() {
    if (fn_8007EF64(m_player) != 1) {
        return true;
    }
    m_movieFrames = fn_8007EF68(m_player);
    return false;
}

void muAdvStaffrollTask::createData() {
    createExcelData();
    m_texts = new (Heaps::MenuInstance) MuObject*[m_textNum];
    m_textY = new (Heaps::MenuInstance) float[m_textNum];
    m_resFile = nw4r::g3d::ResFile(m_handleModels.getBuffer());
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_objectRoot = fn_800AFB94("ObjectRoot", 5, NULL);
    m_movieRoot = fn_800AFB94("MovieRoot", 1, NULL);
    m_textRoot = fn_800AFB94("TextRoot", 0x100, "/menu/defaultcamera/CharacterSelect.brres");
    m_movie = MuObject::create(&m_resFile, "MovieTopN", 1, NULL, Heaps::MenuInstance);
    m_logos[0] = MuObject::create(&m_resFile, "Logo_sora", 1, NULL, Heaps::MenuInstance);
    Vec3f rot0(10.0f, 10.0f, 1.0f);
    setRotate__8MuObjectFP5Vec3f(m_logos[0], &rot0);
    m_logos[1] = MuObject::create(&m_resFile, "Logo_havok", 1, NULL, Heaps::MenuInstance);
    Vec3f rot1(10.0f, 10.0f, 1.0f);
    setRotate__8MuObjectFP5Vec3f(m_logos[1], &rot1);
    m_allRights = MuObject::create(&m_resFile, "All_rights", 1, NULL, Heaps::MenuInstance);
    m_theEnd = MuObject::create(&m_resFile, "the_end", 1, NULL, Heaps::MenuInstance);
    for (int i = 0; i < m_textNum; i++) {
        m_texts[i] = MuObject::create(&m_resFile, "TextTopN", 1, NULL, Heaps::MenuInstance);
    }
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, Heaps::MenuInstance, 0x2B);
    allocMsgBuf__5MuMsgFUlUl(m_msg, 0x400, m_textNum);
    setMsgData__5MuMsgFPv(m_msg, m_handleTexts.getBuffer());
    for (u32 i = 0; static_cast<int>(i) < m_textNum; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_texts[i]->m_sceneModel), 0, 0.0625f);
        m_msg->setFontWidthModeProportional(i);
    }
    fn_8007F2B4(m_player, m_movie->m_sceneModel, 0);
    fn_8007F148(m_player);
    fn_8007EF70(m_player, 0, 0, -1, -1);
    setupExcelParam();
}

bool muAdvStaffrollTask::isEnableSkip() {
    u32 flags = *reinterpret_cast<u32*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x24) + 0x804);
    if ((flags >> 31) == 0) {
        return false;
    }
    switch (m_state) {
    case 0:
    case 1:
    case 2:
    case 4:
        return true;
    }
    return false;
}

void muAdvStaffrollTask::skip() {
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
    m_scrollY = m_init->m_length;
    m_frame = m_init->m_frameNum;
    m_scrollSpeed = 0.0f;
    m_movieDraw = false;
    m_done = true;
}

void muAdvStaffrollTask::createExcelData() {
    u8* buffer = static_cast<u8*>(m_handleExcel.getBuffer());
    u32 size = m_handleExcel.getSize();
    utRelocate relocate(buffer, size);
    m_init = static_cast<muAdvStaffInit*>(relocate.getPublicAddress("muAdvStaffInit"));
    m_end = static_cast<muAdvStaffInit*>(relocate.getPublicAddress("muAdvStaffEnd"));
    m_rootVisible = static_cast<muAdvStaffRootVisible*>(relocate.getPublicAddress("muAdvStaffRootVisible"));
    m_rootMove = static_cast<muAdvStaffRootMove*>(relocate.getPublicAddress("muAdvStaffRootMove"));
    m_modelParam = static_cast<float*>(relocate.getPublicAddress("muAdvStaffModelParam"));
    m_scaleParam = static_cast<float*>(relocate.getPublicAddress("muAdvStaffScaleParam"));
    m_colorParam = static_cast<u8*>(relocate.getPublicAddress("muAdvStaffColorParam"));
    m_textParam = static_cast<muAdvStaffText*>(relocate.getPublicAddress("muAdvStaffTextParam"));
    m_textNum = m_init->m_textNum;
    m_rootVisibleNum = m_init->m_rootVisibleNum;
    m_rootMoveNum = m_init->m_rootMoveNum;
}

void muAdvStaffrollTask::setupExcelParam() {
    m_scrollSpeed = m_init->m_length / static_cast<float>(m_init->m_frameNum);
    int lineY = 0;
    int offset = 0;
    for (u32 i = 0; static_cast<int>(i) < m_textNum; i++) {
        muAdvStaffText* text = &m_textParam[offset / sizeof(muAdvStaffText)];
        lineY += text->m_lineAdd;
        Vec3f pos(static_cast<float>(text->m_x), static_cast<float>(text->m_y - lineY), 0.0f);
        m_texts[i]->setTrans(&pos);
        m_textY[i] = -1.0f * pos.m_y;
        int scaleOffset = m_textParam[offset / sizeof(muAdvStaffText)].m_scaleIndex * 8;
        float scaleY = *reinterpret_cast<float*>(reinterpret_cast<u8*>(m_scaleParam) + scaleOffset + 4);
        setHScale__5MuMsgFUlf(m_msg, i, *reinterpret_cast<float*>(reinterpret_cast<u8*>(m_scaleParam) + scaleOffset));
        getHScale__5MuMsgFUlf(m_msg, i, scaleY);
        u8* color = m_colorParam + m_textParam[offset / sizeof(muAdvStaffText)].m_colorIndex * 4;
        m_msg->setFontColor(i, color[0], color[1], color[2], color[3]);
        switch (m_textParam[offset / sizeof(muAdvStaffText)].m_align) {
        case 0:
            m_msg->setAlignMode(i, MuMsg::Align_Center);
            break;
        case 1:
            m_msg->setAlignMode(i, MuMsg::Align_Left);
            break;
        case 2:
            m_msg->setAlignMode(i, MuMsg::Align_Right);
            break;
        }
        char buffer[0x100];
        memset(buffer, 0, 0x100);
        int length;
        void* data = getMsgData__5MuMsgFUlPi(m_msg, m_textParam[offset / sizeof(muAdvStaffText)].m_msgIndex, &length);
        memcpy(buffer, data, length);
        printData__5MuMsgFUlPCvi(m_msg, i, buffer, strlen(buffer));
        offset += sizeof(muAdvStaffText);
    }
    for (int i = 0; i < 2; i++) {
        Vec3f pos(0.0f, -1.0f * m_modelParam[i], 0.0f);
        m_logos[i]->setTrans(&pos);
    }
}
