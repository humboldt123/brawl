#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>
#include <gf/gf_camera_controller.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <sr/sr_common.h>
#include <types.h>

class cmMenuPadController : public gfCameraController {
public:
    virtual void update(float);
};

class cmMenuFixedController : public gfCameraController {
public:
    void storeDefault();
    virtual void update(float);
};

// HYPOTHESIS: cmMenuPadController::storeDefault() (map name; its unit is not recovered).
extern "C" void fn_800A65E8(cmMenuPadController* controller);
// HYPOTHESIS: MenuRoot::overwriteMatAlpha() (map name; the shared nw4r header lacks the material access it needs).
extern "C" void fn_800B05A0(MenuRoot* root);
void G3DState_Invalidate(u32 flags);

// HYPOTHESIS: MenuRoot::MenuRoot(name, scnSize, cameraPath, heap) (map name; register allocation still differs).
extern "C" MenuRoot* fn_800AFC74(MenuRoot* obj, const char* name, int scnSize, const char* cameraPath, Heaps::HeapType heap);

MenuRoot* MenuRoot::create(const char* name, int scnSize, const char* cameraPath) {
    MenuRoot* obj = (MenuRoot*)operator new(sizeof(MenuRoot), Heaps::MenuInstance);
    if (obj != NULL) {
        obj = fn_800AFC74(obj, name, scnSize, cameraPath, Heaps::MenuResource);
    }
    return obj;
}

// HYPOTHESIS: MenuRoot::create(name, scnSize, archive, index); it keeps its fn_ name while the boot menus call it by that name.
extern "C" MenuRoot* fn_800AFBFC(const char* name, int scnSize, gfArchive* archive, int index) {
    return new (Heaps::MenuInstance) MenuRoot(name, scnSize, archive, index, Heaps::MenuResource);
}

MenuRoot::MenuRoot(const char* name, int scnSize, gfArchive* archive, int index, Heaps::HeapType heap)
    : gfTask(name, Category_Menu, 0xF, 8, true), unk40(1), m_resFile(), scene(NULL), m_anmScn(NULL), m_objects(NULL),
      m_objectCount(0), m_isLoaded(false), m_handle() {
    u32 size;
    scene = nw4r::g3d::ScnRoot::Construct(gfHeapManager::getMEMAllocator(Heaps::MenuInstance), &size, scnSize, scnSize,
                                          0x80, 0x80);
    m_taskName = name;
    unk2C_b1 = false;
    if (archive != NULL && index >= 0) {
        if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            index++;
        }
        attachScnAnm(archive->getData(0, Data_Type_Misc, index, 0xFFFE));
    }
    initOverwriteBuf();
}

void MenuRoot::initOverwriteBuf() {
    for (int i = 0; i < 16; i++) {
        m_overwriteBuf[i].m_object = NULL;
        m_overwriteBuf[i].m_index = -1;
        m_overwriteBuf[i].m_value = 0;
        if (i < 15) {
            m_overwriteBuf[i].m_next = &m_overwriteBuf[i + 1];
        } else {
            m_overwriteBuf[i].m_next = NULL;
        }
        if (i > 0) {
            m_overwriteBuf[i].m_prev = &m_overwriteBuf[i - 1];
        } else {
            m_overwriteBuf[i].m_prev = NULL;
        }
    }
    m_freeNodes = &m_overwriteBuf[0];
    m_matAlphaList = NULL;
    m_visibilityList = NULL;
}

MenuRoot::~MenuRoot() {
    if (unk40 == 0 && m_resFile.ptr() != NULL) {
        gfHeapManager::free(m_resFile.ptr());
    }
    if (scene != NULL) {
        scene->Destroy();
        scene = NULL;
    }
    if (m_anmScn != NULL) {
        m_anmScn->Destroy();
        m_anmScn = NULL;
    }
    m_handle.cancelRequest();
}

void MenuRoot::attachScnAnm(gfFileIOHandle* handle) {
    m_resFile = nw4r::g3d::ResFile(handle->getBuffer());
    if (m_resFile.ptr() == NULL) {
        gfHeapManager::dumpAll();
    }
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_resFile.Bind();
    m_anmScn = NULL;
    if (m_resFile.HasResAnmScn()) {
        nw4r::math::VEC3 pos;
        u32 size;
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
        nw4r::g3d::ResAnmScn anmScn = m_resFile.GetResAnmScn(0);
        anmScn.Bind();
        m_anmScn = nw4r::g3d::AnmScnRes::Construct(allocator, &size, anmScn, false);
        scene->SetAnmScn(m_anmScn);
        scene->CalcWorld();
        gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
        camera->setFromNwCameraAllParam(&scene->GetCamera(0));
        camera->setNwCamera(&scene->GetCamera(0));
        scene->GetCamera(0).GetPosition(&pos);
        camera->unkCC = pos.z;
        camera->unkFA.m_mask |= 0x80;
        camera->m_transformFlag.m_mask = 0xE1;
        camera->calc();
        fn_800A65E8(dynamic_cast<cmMenuPadController*>(
            CameraController::getInstance()->getCameraController(CameraController::MenuPad)));
        dynamic_cast<cmMenuFixedController*>(
            CameraController::getInstance()->getCameraController(CameraController::MenuFixed))->storeDefault();
    }
}

void MenuRoot::attachScnAnm(void* data) {
    m_resFile = nw4r::g3d::ResFile(data);
    if (data == NULL) {
        gfHeapManager::dumpAll();
    }
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_resFile.Bind();
    m_anmScn = NULL;
    if (m_resFile.HasResAnmScn()) {
        nw4r::math::VEC3 pos;
        u32 size;
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::MenuInstance);
        nw4r::g3d::ResAnmScn anmScn = m_resFile.GetResAnmScn(0);
        anmScn.Bind();
        m_anmScn = nw4r::g3d::AnmScnRes::Construct(allocator, &size, anmScn, false);
        scene->SetAnmScn(m_anmScn);
        scene->CalcWorld();
        gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
        camera->setFromNwCameraAllParam(&scene->GetCamera(0));
        camera->setNwCamera(&scene->GetCamera(0));
        scene->GetCamera(0).GetPosition(&pos);
        camera->unkCC = pos.z;
        camera->unkFA.m_mask |= 0x80;
        camera->m_transformFlag.m_mask = 0xE1;
        camera->calc();
        fn_800A65E8(dynamic_cast<cmMenuPadController*>(
            CameraController::getInstance()->getCameraController(CameraController::MenuPad)));
        dynamic_cast<cmMenuFixedController*>(
            CameraController::getInstance()->getCameraController(CameraController::MenuFixed))->storeDefault();
    }
}

// HYPOTHESIS: MenuRoot::getResFile(archive, index); it keeps its fn_ name while three menus call it by that name.
extern "C" void* fn_800B03F0(gfArchive* archive, int index) {
    nw4r::g3d::ResFile resFile;
    resFile = nw4r::g3d::ResFile(archive->getData(0, Data_Type_Misc, index, 0xFFFE));
    nw4r::g3d::ResFile::Init(&resFile);
    return resFile.ptr();
}

void MenuRoot::processDefault() {
    if (!m_isLoaded && m_handle.m_request != NULL && m_handle.isReady()) {
        attachScnAnm(&m_handle);
        m_isLoaded = true;
    }
}

void MenuRoot::processUpdate() {
    scene->UpdateFrame();
}

// HYPOTHESIS: MenuRoot::overwriteVisibility() (map name; kept fn_ while its register allocation differs).
extern "C" void fn_800B04AC(MenuRoot* root) {
    u32 j;
    if (root->m_objects != NULL) {
        for (u32 i = 0; i < root->m_objectCount; i++) {
            MuObject* object = root->m_objects[i];
            if (object != NULL && !object->m_flag) {
                u32 count = object->m_resMdl.GetResNodeNumEntries();
                for (j = 0; j < count; j++) {
                    nw4r::g3d::ScnMdl::CopiedVisAccess access(object->m_scnMdl, j);
                    access.SetVisibility(false);
                }
            }
        }
    }
    for (MenuRoot::OverwriteNode* node = root->m_visibilityList; node != NULL; node = node->m_next) {
        if (node->m_value == 0) {
            nw4r::g3d::ScnMdl::CopiedVisAccess access(node->m_object->m_scnMdl, node->m_index);
            access.SetVisibility(false);
        }
    }
}

void MenuRoot::processAnim() { }

void MenuRoot::renderPre() {
    if (scene != NULL) {
        scene->SetCurrentCamera(0);
        scene->CalcWorld();
        fn_800B04AC(this);
        scene->CalcMaterial();
        fn_800B05A0(this);
        if (scene != NULL) {
            scene->SetCurrentCamera(0);
            gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
            nw4r::g3d::Camera nwCamera = scene->GetCamera(0);
            camera->setNwCamera(&nwCamera);
            camera->calc();
        }
        scene->CalcView();
        scene->GatherDrawScnObj();
        scene->ZSort();
    }
}

void MenuRoot::renderOpa() { }

void MenuRoot::renderXlu() {
    G3DState_Invalidate(0x7FF);
    if (scene != NULL) {
        scene->DrawOpa();
        scene->DrawXlu();
    }
}

void MenuRoot::renderDebug() { }

void MenuRoot::init() { }
