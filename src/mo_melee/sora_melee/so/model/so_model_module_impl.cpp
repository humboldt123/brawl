#define MT_VEC3F_SCALE_FRESH_REGS
#include <gf/gf_3d_scene.h>
#include <gf/gf_copyefb.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_loupe.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_calcworld.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/ut/ut_list.h>
#include <revolution/GX.h>
#include <revolution/MTX.h>
#include <revolution/OS.h>
#include <so/color/so_color_blend_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/shadow/so_shadow_module_impl.h>
#include <so/shake/so_shake_module_simple.h>
#include <so/so_dispose_instance_manager.h>
#include <so/so_heap_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_world.h>
#include <so/stageobject.h>
#include <so/templates/so_array_value_soDisposeInstanceTerm.h>
#include <string.h>

extern "C" char* strrchr(const char* str, int ch); // MSL, not in the project headers
extern "C" void* GXGetTexObjData(const GXTexObj* texture); // as in mu_stage_edit_ui.cpp
#include <types.h>
#include <ut/ut_nw.h>

// Node matrix helpers of the main binary that only this module uses (names unknown). HYPOTHESIS: the parameter
// meanings, read from the calls below: `flags` is the per-node mark array, `scales` the per-node scale array.
extern "C" void fn_800471A4(nw4r::g3d::ScnMdl* mdl, int, int, s8* flags, Vec3f* scales, int, int, int);
extern "C" void fn_800479A8(nw4r::g3d::ScnMdl* mdl, s8* flags);
extern "C" void fn_80047A50(nw4r::g3d::ScnMdl* mdl, s8* flags, u32 nodeId);
extern "C" void fn_80047AE0(nw4r::g3d::ScnMdl* mdl, s8* flags, u32* nodeIds, int numNodeIds);
extern "C" void fn_80047BAC(nw4r::g3d::ScnMdl* mdl, s8* flags);
extern "C" bool fn_80047C0C(s8* flags, u32 nodeId);
extern "C" void fn_8004765C(nw4r::g3d::ScnMdl* shadowMdl, nw4r::g3d::ScnMdl* mdl, void* shadowNode, u32 numNodes);
extern "C" void fn_80047FCC();
extern "C" void fn_801B0A28();
extern "C" void fn_8003E828(const Matrix* mtx, const Vec3f* scale, Matrix* out); // HYPOTHESIS: scales the matrix
extern "C" void fn_8001A194(int, const Vec3f* axes, const Vec3f* pos); // HYPOTHESIS: debug axis render
extern "C" void fn_8000D06C(gfSceneRoot* root, gfSceneRootEvent* evt); // HYPOTHESIS: gfSceneRoot::addEvent
extern "C" void fn_8000D074(gfSceneRoot* root, gfSceneRootEvent* evt); // HYPOTHESIS: gfSceneRoot::removeEvent
extern "C" void* fn_8018D310(const nw4r::g3d::ResFile* resFile, const char* name); // ResPltt lookup (st_croll.cpp)
extern "C" void* fn_801AFF94(nw4r::g3d::ScnMdl::CopiedMatAccess* acc, bool);
extern "C" void* fn_80190DC8(void* resPltt, int);
extern "C" bool fn_80192A44(const nw4r::g3d::ResTex* tex, void** data, u16* width, u16* height, int* format, float* minLod,
                            float* maxLod, u8* mipmap);
extern "C" bool fn_80192B00(const nw4r::g3d::ResTex* tex, void** data, u16* width, u16* height, int* format, float* minLod,
                            float* maxLod, u8* mipmap);
extern "C" void fn_27_AF08C(soDisposeInstanceManager* mgr, const soDisposeInstanceTerm* term); // HYPOTHESIS: addTerm
nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView,
                                    void (*callback)());
nw4r::g3d::ResTexObj CopiedMatAccess_GetResTexObj(nw4r::g3d::ScnMdl::CopiedMatAccess* acc, bool);
void getScalingFactor(Matrix* matrix, Vec3f* scale);

// HYPOTHESIS: the names of the three statics. The loupe index that means "no loupe", the model file index of the
// shadow model, and the two draw byte codes whose material ids switchEnvMap shifts.
// (They are writable data in the original.)
static u32 g_shadowMdlFileIndex = 10; // lbl_27_data_4A0
static u8 g_noLoupeNo = 0xFF; // lbl_27_data_4A4
static const char* g_drawByteCodeNames[2] = { "DrawOpa", "DrawXlu" }; // lbl_27_data_4B8

// The module whose constraint matrices are being set up (recursion guard of setUpConstraintNodeMtx).
static soModelModuleImpl* s_constraintSetUpModule; // lbl_27_bss_28

// One entry of a ResMdl draw byte code ("DrawOpa"/"DrawXlu"): op, material id (big-endian, unaligned), ...
// HYPOTHESIS: the layout; only the op and the material id are touched.
struct soModelDrawByteCode {
    u8 m_op; // 1 ends the list
    u8 m_matIdHi;
    u8 m_matIdLo;
    u8 m_rest[5];

    u16 getMatId() const { return (m_matIdHi << 8) | m_matIdLo; }
    void setMatId(u16 id) {
        m_matIdHi = id >> 8;
        m_matIdLo = id;
    }
};

soModelModuleImpl::soModelModuleImpl(soModuleAccesser* acc,
                                     soArray<soModelNodeSetUp>* nodeSetUps,
                                     float modelScale,
                                     void* extendNodeTbl,
                                     soArray<soModelVirtualNode>* virtualNodes,
                                     soEventObserverRegistrationDesc* regDesc) :
    soAnimCmdEventObserver(
        regDesc->checkAnimCmd() == true ? static_cast<soEventManager&>(acc->getEventManageModule()).getManageId() : (s16)-1,
        (s8)-1),
    soEventPresenter<soModelEventObserver>(
        regDesc->checkModel() == true ? static_cast<soEventManager&>(acc->getEventManageModule()).getManageId() : (s16)-1,
        9, soEventPresenterLocalStore),
    m_calcWorldCallback(this),
    m_scnObjCallback(this),
    m_nodeSetUpCursor(NULL),
    m_scale(modelScale),
    m_scaleZ(g_soWorld->m_8),
    m_transNNodeId(-1),
    m_extendNodeTbl(static_cast<u32*>(extendNodeTbl)),
    m_virtualNodes(virtualNodes),
    m_sceneRootEvent(this),
    m_nodeSetUpFlags(NULL),
    m_nodeScales(NULL),
    m_shadowNode(NULL),
    m_shadowScnMdl(NULL),
    m_isAddEnvMapMat(false),
    m_shadowNodeOverride(0),
    m_setUpState(SetUpState_None),
    m_instanceKind(InstanceKind_None),
    m_loupeNo(g_noLoupeNo),
    m_isEnvMap(false),
    m_isLockMtx(false),
    m_isRefFB(false),
    m_isDisposeInstanceImm(false),
    m_moduleAccesser(acc) {
    nw4r::ut::List_Init(&m_nodeSetUpList, offsetof(soModelNodeSetUp, m_link));
    for (int i = 0; i < nodeSetUps->size(); i++) {
        soModelNodeSetUp& nodeSetUp = nodeSetUps->at(i);
        nodeSetUp.m_flag = 0;
        nw4r::ut::List_Append(&m_nodeSetUpList, &nodeSetUp);
    }
    clearNodeSetUp();
}

soModelModuleImpl::~soModelModuleImpl() {
    destructInstance();
    while (m_nodeSetUpList.numObjects != 0) {
        nw4r::ut::List_Remove(&m_nodeSetUpList, nw4r::ut::List_GetNext(&m_nodeSetUpList, NULL));
    }
}

void soModelModuleImpl::constructInstance(gfSceneRoot::LayerType lt, u32 flags, bool p3, s8 kind, nw4r::g3d::ResFile resFile,
                                          const char* refFBTexName, const char* envMapMatName, s16 mdlResId,
                                          void* shadowNode, u8 loupeNo, int priorityDrawXlu) {
    if (m_scnMdl != NULL) {
        return;
    }
    m_instanceKind = kind;
    if (m_instanceKind == InstanceKind_Resource) {
        {
            if (!m_moduleAccesser->getColorBlendModule().isNull()) {
                flags |= 0x20;
            }
            constructScnMdl(&m_moduleAccesser->getResourceModule(), lt, flags, resFile, refFBTexName, envMapMatName,
                            mdlResId);
            if (refFBTexName != NULL) {
                m_isRefFB = true;
            }
            if (m_shadowNodeOverride != 0) {
                shadowNode = reinterpret_cast<void*>(m_shadowNodeOverride);
            }
            if (shadowNode != NULL) {
                nw4r::g3d::ResFile shadowFile =
                    m_moduleAccesser->getResourceModule().getMdlFile(g_shadowMdlFileIndex, mdlResId);
                nw4r::g3d::ResMdl shadowMdl = shadowFile.GetResMdl(0);
                nw4r::g3d::ResFile::Init(&shadowFile);
                MEMAllocator* allocator =
                    gfHeapManager::getMEMAllocator(m_moduleAccesser->getHeapModule().getNWModelInstanceHeap());
                u32 size = 0;
                nw4r::g3d::ScnMdl* shadowScnMdl = ScnMdl_Construct(allocator, &size, shadowMdl, 0x40, 1, fn_801B0A28);
                shadowScnMdl->m_callback = &m_scnObjCallback;
                soModelModuleScnObjCallback::SetCallbackCondition(shadowScnMdl);
                m_shadowScnMdl = shadowScnMdl;
                m_shadowNode = shadowNode;
                fn_8000D06C(g_gfSceneRoot, &m_sceneRootEvent);
            }
            if (loupeNo != g_noLoupeNo) {
                gfLoupe::addPlayerLoupeScnObj(loupeNo, lt, m_scnMdl);
                m_loupeNo = loupeNo;
            }
            m_scnMdl->SetPriorityDrawXlu(priorityDrawXlu);
        }
    } else if (m_instanceKind == InstanceKind_Default) {
        {
            // HYPOTHESIS: lbl_27_rodata_80 is the built-in default model resource.
            extern const u8 lbl_27_rodata_80[];
            nw4r::g3d::ResFile defaultFile(const_cast<u8*>(lbl_27_rodata_80));
            nw4r::g3d::ResMdl defaultMdl = defaultFile.GetResMdl(0);
            MEMAllocator* allocator =
                gfHeapManager::getMEMAllocator(m_moduleAccesser->getHeapModule().getNWModelInstanceHeap());
            u32 size = 0;
            nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, defaultMdl, 0, 1, fn_801B0A28);
            g_gfSceneRoot->add(lt, scnMdl);
            scnMdl->m_calcWorldCallBack = reinterpret_cast<nw4r::g3d::ICalcWorldCallBack*>(&m_calcWorldCallback); // the g3d header names the interface ICalcWorldCallBack, the so/model shadow ICalcWorldCallback
            soModelModuleCalcWorldCallback::SetCallbackCondition(scnMdl);
            scnMdl->m_callback = &m_scnObjCallback;
            soModelModuleScnObjCallback::SetCallbackCondition(scnMdl);
            m_scnMdl = scnMdl;
            m_layerType = lt;
        }
    } else {
        return;
    }

    Heaps::HeapType heap = m_moduleAccesser->getHeapModule().getNWModelInstanceHeap();
    m_nodeSetUpFlags = static_cast<s8*>(gfHeapManager::alloc(heap, soModelModuleSimple::getNodeNum()));
    m_nodeScales = static_cast<Vec3f*>(gfHeapManager::alloc(heap, soModelModuleSimple::getNodeNum() * sizeof(Vec3f)));
    m_setUpState = SetUpState_Begin;
    setLockMtx(false);
    clearSetUpMtx();

    soInstanceManagerFullProperty<soModelEventObserver*>* observers = getObserverList();
    for (int i = 0; i < observers->size(); i++) {
        observers->at(i)->notifyEventConstructInstance(p3, m_moduleAccesser);
    }
}

void soModelModuleImpl::setLockMtx(bool lock) {
    m_isLockMtx = lock;
}

void soModelModuleImpl::destructInstance() {
    if (m_scnMdl == NULL) {
        return;
    }

    soInstanceManagerFullProperty<soModelEventObserver*>* observers = getObserverList();
    for (int i = 0; i < observers->size(); i++) {
        observers->at(i)->notifyEventDestructInstance(m_moduleAccesser);
    }

    if (m_instanceKind != InstanceKind_Extern) {
        if (m_loupeNo != g_noLoupeNo) {
            gfLoupe::removePlayerLoupeScnObj(m_loupeNo, m_scnMdl);
            m_loupeNo = g_noLoupeNo;
        }
        if (!m_isDisposeInstanceImm) {
            soDisposeInstanceTerm term;
            term.unk8 = m_moduleAccesser->getStageObject().soGetKind();
            term.unk9 = m_moduleAccesser->getStageObject().soGetSubKind();
            term.unk0 = reinterpret_cast<u32>(m_scnMdl);
            term.unk4 = m_moduleAccesser->getStageObject().getTaskId();
            fn_27_AF08C(g_soDisposeInstanceManager, &term);
        } else {
            m_scnMdl->Destroy();
        }
        g_gfSceneRoot->remove(m_scnMdl);
        m_scnMdl->m_calcWorldCallBack = NULL;
        m_scnMdl->m_callback = NULL;
        gfHeapManager::free(m_nodeSetUpFlags);
        gfHeapManager::free(m_nodeScales);
        m_nodeSetUpFlags = NULL;
        m_nodeScales = NULL;
        if (isAddEnvMapMat() == true) {
            switchEnvMap(false);
            m_isAddEnvMapMat = false;
        }
    }

    m_scnMdl = NULL;
    m_instanceKind = InstanceKind_None;
    if (getScnMdl(true) != NULL) {
        if (!m_isDisposeInstanceImm) {
            soDisposeInstanceTerm term;
            term.unk8 = m_moduleAccesser->getStageObject().soGetKind();
            term.unk9 = m_moduleAccesser->getStageObject().soGetSubKind();
            term.unk0 = reinterpret_cast<u32>(getScnMdl(true));
            term.unk4 = m_moduleAccesser->getStageObject().getTaskId();
            fn_27_AF08C(g_soDisposeInstanceManager, &term);
        } else {
            // The original destroys m_scnMdl here, which was just cleared.
            m_scnMdl->Destroy();
        }
        getScnMdl(true)->m_callback = NULL;
        m_shadowScnMdl = NULL;
        fn_8000D074(g_gfSceneRoot, &m_sceneRootEvent);
    }
}

bool soModelModuleImpl::isAddEnvMapMat() {
    return m_isAddEnvMapMat;
}

void soModelModuleImpl::deactivate() {
    if (m_instanceKind == InstanceKind_Extern) {
        return;
    }
    if (m_scnMdl != NULL) {
        m_scnMdl->SetScnObjOption(1, 1);
        m_scnMdl->SetScnObjOption(2, 1);
        m_scnMdl->SetScnObjOption(3, 1);
        m_scnMdl->SetScnObjOption(4, 1);
        m_scnMdl->SetScnObjOption(5, 1);
        m_scnMdl->SetScnObjOption(6, 1);
        m_scnMdl->SetScnObjOption(7, 1);
        m_scnMdl->SetScnObjOption(8, 1);
    }
}

void soModelModuleImpl::activate() {
    if (m_instanceKind == InstanceKind_Extern) {
        return;
    }
    m_setUpState = SetUpState_None;
    m_isLockMtx = false;
    clearNodeSetUp();
    m_nodeSetUpCursor = NULL;
    if (m_scnMdl != NULL) {
        m_scnMdl->SetScnObjOption(1, 0);
        m_scnMdl->SetScnObjOption(2, 1);
        m_scnMdl->SetScnObjOption(3, 0);
        m_scnMdl->SetScnObjOption(4, 0);
        m_scnMdl->SetScnObjOption(5, 0);
        m_scnMdl->SetScnObjOption(6, 0);
        m_scnMdl->SetScnObjOption(7, 0);
        m_scnMdl->SetScnObjOption(8, 0);
    }
    m_isEnvMap = false;
}

void soModelModuleImpl::begin(bool begin) {
    if (begin == true) {
        m_setUpState = SetUpState_Begin;
        m_isLockMtx = false;
        clearSetUpMtx();
    } else {
        clearNodeSetUp();
    }
}

nw4r::g3d::ScnMdl* soModelModuleImpl::getScnMdl(bool shadow) const {
    if (shadow == false) {
        return m_scnMdl;
    }
    return m_shadowScnMdl;
}

void soModelModuleImpl::constructScnMdl(soResourceModule* resource, gfSceneRoot::LayerType lt, u32 flags,
                                        nw4r::g3d::ResFile resFile, const char* refFBTexName, const char* envMapMatName,
                                        s16 mdlResId) {
    nw4r::g3d::ResFile mdlFile = resource->getMdlFile(resource->getResourceIdAccesser()->getMdlResIndex(), mdlResId);
    mdlFile.Release();
    nw4r::g3d::ResMdl mdl = mdlFile.GetResMdl(0);
    nw4r::g3d::ResFile::Init(&mdlFile);
    if (resFile.ptr() != NULL) {
        nw4r::g3d::ResFile::Init(&resFile);
        mdlFile.Bind(resFile);
    }

    nw4r::g3d::ResTex refFBTex;
    nw4r::g3d::ResFile texFile = resource->getTexFile(resource->getResourceIdAccesser()->getTexResIndex(), mdlResId);
    if (texFile.ptr() != NULL) {
        nw4r::g3d::ResFile::Init(&texFile);
        mdlFile.Bind(texFile);
        if (refFBTexName != NULL) {
            refFBTex = texFile.GetResTex(refFBTexName);
        }
    }

    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(m_moduleAccesser->getHeapModule().getNWModelInstanceHeap());

    if (envMapMatName != NULL) {
        u32 numMats = mdl.GetResMatNumEntries();
        if (numMats != 0) {
            // The last material is the environment-map material when its name ends in "_<envMapMatName>"; the
            // environment-mapped copies of the materials are the first half of the material list.
            nw4r::g3d::ResMat lastMat = mdl.GetResMat(numMats - 1);
            const char* name = lastMat.ptr()->m_name != 0 ? reinterpret_cast<const char*>(lastMat.ptr()) + lastMat.ptr()->m_name : NULL;
            const char* suffix = strrchr(name, '_');
            if (suffix != NULL && strcmp(suffix + 1, envMapMatName) == 0) {
                mdl.ptr()->m_needTexMtxArray = true;
                m_isAddEnvMapMat = true;
                nw4r::g3d::ResMdl mdl2 = mdl;
                u32 half = mdl2.GetResMatNumEntries() >> 1;
                for (u32 i = 0; i < 2; i++) {
                    soModelDrawByteCode* code = reinterpret_cast<soModelDrawByteCode*>(mdl2.GetResByteCode(g_drawByteCodeNames[i]));
                    if (code != NULL && code->getMatId() >= half) {
                        while (code->m_op != 1) {
                            code->setMatId(code->getMatId() - half);
                            code++;
                        }
                    }
                }
            }
        }
    }

    u32 size = 0;
    nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, mdl, flags, 1, fn_80047FCC);
    unkC0 = 1;
    g_gfSceneRoot->add(lt, scnMdl);
    scnMdl->m_calcWorldCallBack = reinterpret_cast<nw4r::g3d::ICalcWorldCallBack*>(&m_calcWorldCallback); // the g3d header names the interface ICalcWorldCallBack, the so/model shadow ICalcWorldCallback
    soModelModuleCalcWorldCallback::SetCallbackCondition(scnMdl);
    scnMdl->m_callback = &m_scnObjCallback;
    soModelModuleScnObjCallback::SetCallbackCondition(scnMdl);

    if (refFBTex.ptr() != NULL) {
        // Point every texture object that samples the reference-framebuffer texture at the copy-EFB texture.
        u8* texData = NULL;
        if (refFBTex.ptr()->m_dataOffset != 0) {
            texData = reinterpret_cast<u8*>(refFBTex.ptr()) + refFBTex.ptr()->m_dataOffset;
        }
        u32 texDataPhys = reinterpret_cast<u32>(texData) & 0x1FFFFFFF;
        GXTexObj copyEFBTex;
        if (gfCopyEFBMgr::getInstance()->isValid(1) == true) {
            copyEFBTex = *gfCopyEFBMgr::getInstance()->getCopyEFBTex(1);
        }
        for (u32 i = 0; i < mdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess matAccess(scnMdl, i);
            nw4r::g3d::ResTexObj texObj = matAccess.GetResTexObjEx();
            for (int map = GX_TEXMAP0; map < GX_MAX_TEXMAP; map++) {
                if (texObj.IsValidTexObj(static_cast<GXTexMapID>(map)) == true &&
                    (reinterpret_cast<u32>(GXGetTexObjData(texObj.GetTexObj(static_cast<GXTexMapID>(map)))) & 0x1FFFFFFF) == texDataPhys) {
                    *texObj.GetTexObj(static_cast<GXTexMapID>(map)) = copyEFBTex;
                }
            }
        }
    }

    m_scnMdl = scnMdl;
    m_layerType = lt;
    scnMdl->SetScnObjOption(2, 1);
}

void soModelModuleImpl::setScnMdl(nw4r::g3d::ScnMdl* mdl, void* data, gfSceneRoot::LayerType lt) {
    soModelModuleSimple::setScnMdl(mdl, data, lt);
    InstanceData* instance = static_cast<InstanceData*>(data);
    m_nodeSetUpFlags = instance->m_nodeSetUpFlags;
    m_nodeScales = instance->m_nodeScales;
    m_shadowNode = instance->m_shadowNode;
    m_shadowScnMdl = instance->m_shadowScnMdl;
    m_isAddEnvMapMat = instance->m_isAddEnvMapMat;
    if (m_loupeNo != g_noLoupeNo) {
        gfLoupe::addPlayerLoupeScnObj(m_loupeNo, lt, mdl);
    }
    if (getScnMdl(true) != NULL) {
        fn_8000D06C(g_gfSceneRoot, &m_sceneRootEvent);
    }
}

void soModelModuleImpl::setScnMdlExtern(nw4r::g3d::ScnMdl* mdl) {
    m_scnMdl = mdl;
    m_instanceKind = InstanceKind_Extern;
}

nw4r::g3d::ScnMdl* soModelModuleImpl::detachScnMdl(void* data) {
    switchEnvMap(false);
    m_instanceKind = InstanceKind_None;

    soInstanceManagerFullProperty<soModelEventObserver*>* observers = getObserverList();
    for (int i = 0; i < observers->size(); i++) {
        observers->at(i)->notifyEventDestructInstance(m_moduleAccesser);
    }

    if (getScnMdl(true) != NULL) {
        fn_8000D074(g_gfSceneRoot, &m_sceneRootEvent);
    }

    InstanceData* instance = static_cast<InstanceData*>(data);
    instance->m_nodeSetUpFlags = m_nodeSetUpFlags;
    instance->m_nodeScales = m_nodeScales;
    instance->m_shadowNode = m_shadowNode;
    instance->m_shadowScnMdl = m_shadowScnMdl;
    instance->m_isAddEnvMapMat = m_isAddEnvMapMat;
    m_nodeSetUpFlags = NULL;
    m_nodeScales = NULL;
    m_shadowNode = NULL;
    m_shadowScnMdl = NULL;
    m_isAddEnvMapMat = false;
    if (m_loupeNo != g_noLoupeNo) {
        gfLoupe::removePlayerLoupeScnObj(m_loupeNo, m_scnMdl);
    }
    return soModelModuleSimple::detachScnMdl(data);
}

// Timing A of the calc-world callback: apply the node's SRT override to the animation result.
u32 soModelModuleImpl::calcWorldCallbackA(nw4r::g3d::ChrAnmResult* result, u16 nodeId) {
    soModelNodeSetUp* nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, m_nodeSetUpCursor));
    if (nodeSetUp != NULL && nodeSetUp->m_nodeId == nodeId && (nodeSetUp->m_flag & soModelNodeSetUp::Flag_All) != 0) {
        if (nodeId != static_cast<u32>(getRealNodeId(m_moduleAccesser->getLinkModule().getModelConstraintNode()))) {
            if (nodeSetUp->m_flag & soModelNodeSetUp::Flag_Scale) {
                result->SetScale(reinterpret_cast<const nw4r::math::VEC3*>(&nodeSetUp->m_scale));
            }
            u32 rotateFlag = nodeSetUp->m_flag & soModelNodeSetUp::Flag_Rotate;
            if (rotateFlag != 0) {
                if (rotateFlag == soModelNodeSetUp::Flag_Rotate) {
                    result->SetRotateDeg(reinterpret_cast<const nw4r::math::VEC3*>(&nodeSetUp->m_rotate));
                } else {
                    nw4r::math::VEC3 rotate;
                    result->GetRotateDeg(&rotate);
                    if (nodeSetUp->m_flag & soModelNodeSetUp::Flag_RotateX) {
                        rotate.x = nodeSetUp->m_rotate.m_x;
                    }
                    if (nodeSetUp->m_flag & soModelNodeSetUp::Flag_RotateY) {
                        rotate.y = nodeSetUp->m_rotate.m_y;
                    }
                    if (nodeSetUp->m_flag & soModelNodeSetUp::Flag_RotateZ) {
                        rotate.z = nodeSetUp->m_rotate.m_z;
                    }
                    result->SetRotateDeg(&rotate);
                }
            }
            if (nodeSetUp->m_flag & soModelNodeSetUp::Flag_Translate) {
                result->SetTranslate(reinterpret_cast<const nw4r::math::VEC3*>(&nodeSetUp->m_translate));
            }
        }
    }
    return -1;
}

// Timing B of the calc-world callback: the constraint node takes the constraint target's world matrix, then the
// cursor advances to the next node that has an SRT override. Returns the node to continue with (-1: none).
u32 soModelModuleImpl::calcWorldCallbackB(nw4r::g3d::WorldMtxManip* manip, u16 nodeId) {
    u32 constraintNodeId = getRealNodeId(m_moduleAccesser->getLinkModule().getModelConstraintNode());
    u32 nextNodeId = -1;
    if (nodeId == constraintNodeId) {
        if (fn_80047C0C(m_nodeSetUpFlags, nodeId) == true) {
            Matrix targetMtx(true);
            m_moduleAccesser->getLinkModule().calcModelConstraintNodeMtx(&targetMtx, true);
            if (&targetMtx != NULL) {
                PSMTXCopy(targetMtx.m, manip->m_mtx->m);
            } else {
                PSMTXIdentity(manip->m_mtx->m);
            }
            if (!(m_moduleAccesser->getLinkModule().getModelConstraintFlag() & 0x8)) {
                Vec3f scale = getNodeScale(0);
                float constraintScale = m_moduleAccesser->getLinkModule().getConstraintModelScale();
                scale.m_x *= constraintScale;
                scale.m_y *= constraintScale;
                scale.m_z *= constraintScale;
                manip->SetScale(scale.m_x, scale.m_y, scale.m_z);
            }
            m_nodeSetUpFlags[nodeId] = 1;
            m_moduleAccesser->getPostureModule().fixConstraintNodePosture(nodeId, &targetMtx);
        }
        soModelNodeSetUp* nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, m_nodeSetUpCursor));
        if (nodeSetUp != NULL && nodeSetUp->m_nodeId == nodeId) {
            m_nodeSetUpCursor = nodeSetUp;
            nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, nodeSetUp));
            if (nodeSetUp != NULL && nodeSetUp->m_flag != 0) {
                nextNodeId = nodeSetUp->m_nodeId;
            }
        } else {
            nextNodeId = m_nextSetUpNodeId;
        }
    } else {
        soModelNodeSetUp* nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, m_nodeSetUpCursor));
        if (nodeSetUp != NULL) {
            if (nodeSetUp->m_nodeId != nodeId) {
                nextNodeId = nodeSetUp->m_nodeId;
            } else if (nodeSetUp->m_flag != 0) {
                m_nodeSetUpCursor = nodeSetUp;
                nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, nodeSetUp));
                if (nodeSetUp != NULL && nodeSetUp->m_flag != 0) {
                    nextNodeId = nodeSetUp->m_nodeId;
                }
            }
        }
        if (constraintNodeId != -1 && nodeId < constraintNodeId && m_nextSetUpNodeId == -1 &&
            (nextNodeId == -1 || constraintNodeId < nextNodeId)) {
            m_nextSetUpNodeId = nextNodeId;
            return constraintNodeId;
        }
    }
    if (nextNodeId == -1) {
        m_nodeSetUpCursor = NULL;
    }
    return nextNodeId;
}

// Timing A of the scene object callback: remember the view matrix and apply the screen shake and the depth scale.
void soModelModuleImpl::calcViewCallbackPre(nw4r::math::MTX34* viewMtx) {
    Vec2f shakeOffset;
    m_viewMtx = *viewMtx;
    bool shake = static_cast<soShakeModule*>(m_moduleAccesser->m_enumerationStart->m_shakeModule)->getOffset(&shakeOffset);
    if (shake == true || m_scaleZ != 1.0f) {
        Mtx mtx;
        PSMTXIdentity(mtx);
        if (shake == true) {
            mtx[0][3] = shakeOffset.m_x;
            mtx[1][3] = shakeOffset.m_y;
            mtx[2][3] = 0.0f;
        }
        if (m_scaleZ != 1.0f && g_soWorld->m_13 != 0) {
            float z;
            if (m_moduleAccesser->getLinkModule().isModelConstraint() == true &&
                !(m_moduleAccesser->getLinkModule().getModelConstraintFlag() & 0x10)) {
                z = m_moduleAccesser->getLinkModule().getModelConstraintNodeGlobalPositionRecursive(0).m_z;
            } else {
                z = getNodeGlobalPosition(0, false).m_z;
            }
            mtx[0][2] *= m_scaleZ;
            mtx[1][2] *= m_scaleZ;
            mtx[2][2] *= m_scaleZ;
            mtx[2][3] = z;
        }
        PSMTXConcat(viewMtx->m, mtx, viewMtx->m);
    }
}

// Timing C of the scene object callback: restore the view matrix.
void soModelModuleImpl::calcViewCallbackPost(nw4r::math::MTX34* viewMtx) {
    *viewMtx = m_viewMtx;
}

// Builds the constraint node's matrix from the link target before the node matrices are set up.
void soModelModuleImpl::setUpConstraintNodeMtx() {
    if (s_constraintSetUpModule == this) {
        return;
    }
    if (m_moduleAccesser->getLinkModule().isModelConstraint() != true) {
        return;
    }
    if (m_moduleAccesser->getLinkModule().isModelConstraintMutual() == true) {
        s_constraintSetUpModule = this;
    }
    Matrix targetMtx(true);
    m_moduleAccesser->getLinkModule().calcModelConstraintNodeMtx(&targetMtx, false);
    s_constraintSetUpModule = NULL;

    u32 nodeId = getRealNodeId(m_moduleAccesser->getLinkModule().getModelConstraintNode());
    Matrix nodeMtx(true);
    soModelModuleSimple::getNodeGlobalMtx(nodeId, &nodeMtx, true);
    int i;
    for (i = 0; i < 12; i++) {
        if ((&targetMtx.m[0][0])[i] != (&nodeMtx.m[0][0])[i]) {
            break;
        }
    }
    if (i != 12) {
        if (m_moduleAccesser->getLinkModule().getModelConstraintFlag() & 0x10) {
            fn_80047A50(m_scnMdl, m_nodeSetUpFlags, nodeId);
        } else {
            clearSetUpMtx();
        }
    }
    m_nextSetUpNodeId = -1;
}

void soModelModuleImpl::setUpMtxFinal() {
    if (m_setUpState != SetUpState_Final) {
        setUpConstraintNodeMtx();
        nw4r::g3d::ScnMdl* scnMdl = m_scnMdl;
        m_nodeSetUpCursor = NULL;
        fn_80047BAC(scnMdl, m_nodeSetUpFlags);
        fn_800471A4(scnMdl, 0, 0, m_nodeSetUpFlags, m_nodeScales, 1, 0, 1);
        m_setUpState = SetUpState_Final;
    }
}

bool soModelModuleImpl::isVirtualNodeId(u32 nodeId) const {
    if (nodeId >= soModelModuleSimple::getNodeNum() && nodeId < ExtendNodeIdBase) {
        return true;
    }
    return false;
}

void soModelModuleImpl::setUpMtx(u32 nodeId) {
    if (isVirtualNodeId(nodeId) == true) {
        u32 parentId;
        int i;
        for (i = 0; i < m_virtualNodes->size(); i++) {
            if (m_virtualNodes->at(i).getNodeParent(nodeId, parentId) == true) {
                break;
            }
        }
        u32 parentNodeId = getRealNodeId(parentId);
        if (!m_isLockMtx && m_setUpState != SetUpState_Final) {
            setUpConstraintNodeMtx();
            if (m_nodeSetUpFlags[parentNodeId] != 1) {
                nw4r::g3d::ScnMdl* scnMdl = m_scnMdl;
                m_nodeSetUpCursor = NULL;
                fn_80047A50(scnMdl, m_nodeSetUpFlags, parentNodeId);
                fn_800471A4(scnMdl, 0, 0, m_nodeSetUpFlags, m_nodeScales, 0, 0, 0);
                m_setUpState = SetUpState_Begin;
            }
        }
        Vec3f scale = getNodeScale(nodeId);
        m_virtualNodes->at(i).updateMatrix(*this, nodeId, scale);
    } else if (!m_isLockMtx && m_setUpState != SetUpState_Final) {
        setUpConstraintNodeMtx();
        if (m_nodeSetUpFlags[nodeId] != 1) {
            nw4r::g3d::ScnMdl* scnMdl = m_scnMdl;
            m_nodeSetUpCursor = NULL;
            fn_80047A50(scnMdl, m_nodeSetUpFlags, nodeId);
            fn_800471A4(scnMdl, 0, 0, m_nodeSetUpFlags, m_nodeScales, 0, 0, 0);
            m_setUpState = SetUpState_Begin;
        }
    }
}

void soModelModuleImpl::setUpMtx(u32* nodeIds, s32 numNodeIds) {
    if (numNodeIds == 1) {
        setUpMtx(nodeIds[0]);
        return;
    }
    if (m_isLockMtx || m_setUpState == SetUpState_Final) {
        return;
    }
    setUpConstraintNodeMtx();
    nw4r::g3d::ScnMdl* scnMdl = m_scnMdl;
    struct VirtualNodeRef {
        int m_index;
        u32 m_parentNodeId;
    } virtualNodes[200];
    u32 setUpNodeIds[200];
    int numVirtualNodes = 0;
    int numSetUpNodeIds = 0;
    for (int i = 0; i < numNodeIds; i++) {
        u32 nodeId = nodeIds[i];
        if (isVirtualNodeId(nodeId) == true) {
            u32 parentId;
            int j;
            for (j = 0; j < m_virtualNodes->size(); j++) {
                if (m_virtualNodes->at(j).getNodeParent(nodeId, parentId) == true) {
                    break;
                }
            }
            nodeId = getRealNodeId(parentId);
            virtualNodes[numVirtualNodes].m_index = j;
            virtualNodes[numVirtualNodes].m_parentNodeId = parentId;
            numVirtualNodes++;
        }
        if (m_nodeSetUpFlags[nodeId] != 1) {
            setUpNodeIds[numSetUpNodeIds++] = nodeId;
        }
    }
    if (numSetUpNodeIds <= 0) {
        return;
    }
    fn_80047AE0(scnMdl, m_nodeSetUpFlags, setUpNodeIds, numSetUpNodeIds);
    m_nodeSetUpCursor = NULL;
    fn_800471A4(scnMdl, 0, 0, m_nodeSetUpFlags, m_nodeScales, 0, 0, 0);
    m_setUpState = SetUpState_Begin;
    for (int i = 0; i < numVirtualNodes; i++) {
        u32 nodeId = virtualNodes[i].m_parentNodeId;
        Vec3f scale = getNodeScale(nodeId);
        m_virtualNodes->at(virtualNodes[i].m_index).updateMatrix(*this, nodeId, scale);
    }
}

void soModelModuleImpl::setUpSkinningMtx() {
    // TODO: 0x14 bytes
}

void soModelModuleImpl::clearSetUpMtx() {
    if (m_setUpState != SetUpState_Begin) {
        return;
    }
    if (m_isLockMtx) {
        return;
    }
    nw4r::g3d::ScnMdl* scnMdl = m_scnMdl;
    m_setUpState = SetUpState_None;
    if (scnMdl != NULL) {
        fn_800479A8(scnMdl, m_nodeSetUpFlags);
    }
}

void soModelModuleImpl::clearNodeSetUp() {
    soModelNodeSetUp* nodeSetUp = NULL;
    while ((nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, nodeSetUp))) != NULL) {
        nodeSetUp->m_flag = 0;
    }
    clearSetUpMtx();
    if (m_scale != 1.0f) {
        Vec3f scale(1.0f, 1.0f, 1.0f);
        setNodeScale(0, &scale);
    }
}

void soModelModuleImpl::clearNodeSRT(u32 nodeId) {
    soModelNodeSetUp* nodeSetUp = NULL;
    while ((nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, nodeSetUp))) != NULL) {
        if (nodeSetUp->m_flag == 0) {
            break;
        }
        if (nodeId == nodeSetUp->m_nodeId) {
            nodeSetUp->m_flag = 0;
            nw4r::ut::List_Remove(&m_nodeSetUpList, nodeSetUp);
            nw4r::ut::List_Append(&m_nodeSetUpList, nodeSetUp);
            clearSetUpMtx();
            return;
        }
    }
}

// The record of a node, or the first free record when the node has none (the free records sit at the tail).
soModelNodeSetUp* soModelModuleImpl::findNodeSetUp(u32 nodeId) {
    soModelNodeSetUp* nodeSetUp = NULL;
    while ((nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, nodeSetUp))) != NULL) {
        if (nodeSetUp->m_flag != 0) {
            if (nodeId == nodeSetUp->m_nodeId) {
                return nodeSetUp;
            }
        } else {
            return nodeSetUp;
        }
    }
    return NULL;
}

// The record of a node, inserting a free record (taken from the tail) when the node has none, so the records stay
// sorted by node id.
soModelNodeSetUp* soModelModuleImpl::getNodeSetUp(u32 nodeId) {
    soModelNodeSetUp* nodeSetUp = NULL;
    while ((nodeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetNext(&m_nodeSetUpList, nodeSetUp))) != NULL) {
        if (nodeSetUp->m_flag != 0) {
            if (nodeId == nodeSetUp->m_nodeId) {
                return nodeSetUp;
            }
            if (nodeId < nodeSetUp->m_nodeId) {
                soModelNodeSetUp* freeSetUp = static_cast<soModelNodeSetUp*>(nw4r::ut::List_GetPrev(&m_nodeSetUpList, NULL));
                nw4r::ut::List_Remove(&m_nodeSetUpList, freeSetUp);
                nw4r::ut::List_Insert(&m_nodeSetUpList, nodeSetUp, freeSetUp);
                return freeSetUp;
            }
        } else {
            return nodeSetUp;
        }
    }
    return NULL;
}

void soModelModuleImpl::setNodeSRT(u32 nodeId, Vec3f* scale, Vec3f* rot, Vec3f* xlate) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    Vec3f* nodeScale;
    Vec3f scaledScale;
    if (nodeId == 0) {
        Vec3fScale(&scaledScale, scale, m_scale);
        nodeScale = &scaledScale;
    } else {
        nodeScale = scale;
    }
    Vec3f newScale = *nodeScale;
    if (m_setUpState >= SetUpState_Begin && nodeSetUp->m_flag == soModelNodeSetUp::Flag_All &&
        nodeSetUp->m_translate.m_x == xlate->m_x && nodeSetUp->m_translate.m_y == xlate->m_y &&
        nodeSetUp->m_translate.m_z == xlate->m_z && nodeSetUp->m_rotate.m_x == rot->m_x &&
        nodeSetUp->m_rotate.m_y == rot->m_y && nodeSetUp->m_rotate.m_z == rot->m_z &&
        nodeSetUp->m_scale.m_x == newScale.m_x && nodeSetUp->m_scale.m_y == newScale.m_y &&
        nodeSetUp->m_scale.m_z == newScale.m_z) {
        return;
    }
    nodeSetUp->m_scale = newScale;
    nodeSetUp->m_rotate = *rot;
    nodeSetUp->m_translate = *xlate;
    nodeSetUp->m_flag = soModelNodeSetUp::Flag_All;
    clearSetUpMtx();
}

Vec3f soModelModuleImpl::getNodeScale(u32 nodeId) {
    soModelNodeSetUp* nodeSetUp = findNodeSetUp(nodeId);
    if (nodeSetUp == NULL || !(nodeSetUp->m_flag & soModelNodeSetUp::Flag_Scale)) {
        return Vec3f(1.0f, 1.0f, 1.0f);
    }
    return *reinterpret_cast<Vec3f*>(&nodeSetUp->m_scale);
}

void soModelModuleImpl::setNodeScale(u32 nodeId, Vec3f* scale) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    if (nodeId == 0) {
        Vec3f scaledScale;
        Vec3fScale(&scaledScale, scale, m_scale);
        nodeSetUp->m_scale = scaledScale;
    } else {
        nodeSetUp->m_scale = *scale;
    }
    nodeSetUp->m_flag |= soModelNodeSetUp::Flag_Scale;
    clearSetUpMtx();
}

Vec3f soModelModuleImpl::getNodeRotate(u32 nodeId) {
    soModelNodeSetUp* nodeSetUp = findNodeSetUp(nodeId);
    if (nodeSetUp == NULL || !(nodeSetUp->m_flag & soModelNodeSetUp::Flag_Rotate)) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    return *reinterpret_cast<Vec3f*>(&nodeSetUp->m_rotate);
}

void soModelModuleImpl::setNodeRotateX(u32 nodeId, float x) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    nodeSetUp->m_rotate.m_x = x;
    nodeSetUp->m_flag |= soModelNodeSetUp::Flag_RotateX;
    clearSetUpMtx();
}

void soModelModuleImpl::setNodeRotateY(u32 nodeId, float y) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    nodeSetUp->m_rotate.m_y = y;
    nodeSetUp->m_flag |= soModelNodeSetUp::Flag_RotateY;
    clearSetUpMtx();
}

void soModelModuleImpl::setNodeRotateZ(u32 nodeId, float z) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    nodeSetUp->m_rotate.m_z = z;
    nodeSetUp->m_flag |= soModelNodeSetUp::Flag_RotateZ;
    clearSetUpMtx();
}

void soModelModuleImpl::setNodeRotate(u32 nodeId, Vec3f* rot) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    nodeSetUp->m_rotate = *rot;
    nodeSetUp->m_flag |= soModelNodeSetUp::Flag_Rotate;
    clearSetUpMtx();
}

Vec3f soModelModuleImpl::getNodeTranslate(u32 nodeId) {
    soModelNodeSetUp* nodeSetUp = findNodeSetUp(nodeId);
    if (nodeSetUp == NULL || !(nodeSetUp->m_flag & soModelNodeSetUp::Flag_Translate)) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    return *reinterpret_cast<Vec3f*>(&nodeSetUp->m_translate);
}

void soModelModuleImpl::setNodeTranslate(u32 nodeId, Vec3f* xlate) {
    soModelNodeSetUp* nodeSetUp = getNodeSetUp(nodeId);
    nodeSetUp->m_nodeId = nodeId;
    nodeSetUp->m_translate = *xlate;
    nodeSetUp->m_flag |= soModelNodeSetUp::Flag_Translate;
    clearSetUpMtx();
}

int soModelModuleImpl::getCorrectNodeId(u32 nodeId) {
    if (nodeId >= ExtendNodeIdBase) {
        u32 index = nodeId - ExtendNodeIdBase;
        u32* table = m_extendNodeTbl;
        return table[index];
    }
    return nodeId;
}

void soModelModuleImpl::getNodeGlobalMtx(u32 nodeId, Matrix* outMtx, bool isSetUp) const {
    if (!isSetUp) {
        const_cast<soModelModuleImpl*>(this)->setUpMtx(nodeId);
    }
    if (nodeId >= soModelModuleSimple::getNodeNum()) {
        int numVirtualNodes = m_virtualNodes->size();
        for (int i = 0; i < numVirtualNodes; i++) {
            if (m_virtualNodes->at(i).getMatrix(nodeId, *outMtx) == true) {
                return;
            }
        }
    }
    soModelModuleSimple::getNodeGlobalMtx(nodeId, outMtx, isSetUp);
    if (m_setUpState == SetUpState_Begin) {
        Vec3f scale = m_nodeScales[nodeId];
        if (scale.m_x != 1.0f || scale.m_y != 1.0f || scale.m_z != 1.0f) {
            Vec3f scaleCopy;
            scaleCopy = scale;
            fn_8003E828(outMtx, &scaleCopy, outMtx);
        }
    }
}

void soModelModuleImpl::setNodeGlobalMtx(u32 nodeId, Matrix* mtx, bool isSetUp) {
    if (!isSetUp) {
        clearSetUpMtx();
    }
    int numVirtualNodes = m_virtualNodes->size();
    for (int i = 0; i < numVirtualNodes; i++) {
        if (m_virtualNodes->at(i).setMatrix(nodeId, *mtx) == true) {
            break;
        }
    }
}

Vec3f soModelModuleImpl::getNodeGlobalPosition(u32 nodeId, bool isSetUp) {
    if (!isSetUp) {
        setUpMtx(nodeId);
    }
    if (isVirtualNodeId(nodeId) == true) {
        int numVirtualNodes = m_virtualNodes->size();
        for (int i = 0; i < numVirtualNodes; i++) {
            Matrix mtx(true);
            if (m_virtualNodes->at(i).getMatrix(nodeId, mtx) == true) {
                return Vec3f(mtx(0, 3), mtx(1, 3), mtx(2, 3));
            }
        }
    }
    return nwSMGetGlobalPosition(m_scnMdl, nodeId);
}

Vec3f soModelModuleImpl::getNodeGlobalPosition(u32 nodeId, const Vec3f* offset, bool isSetUp, bool noScale) {
    Matrix mtx(true);
    getNodeGlobalMtx(nodeId, &mtx, isSetUp);
    if (noScale == true) {
        Vec3f scale;
        getScalingFactor(&mtx, &scale);
        if (scale.m_x != 1.0f || scale.m_y != 1.0f || scale.m_z != 1.0f) {
            scale.m_x = 1.0f / scale.m_x;
            scale.m_y = 1.0f / scale.m_y;
            scale.m_z = 1.0f / scale.m_z;
            fn_8003E828(&mtx, &scale, &mtx);
        }
    }
    if (offset->m_x != 0.0f || offset->m_y != 0.0f || offset->m_z != 0.0f) {
        Vec3f pos;
        mtx.mulPos(offset, &pos);
        return pos;
    }
    return Vec3f(mtx(0, 3), mtx(1, 3), mtx(2, 3));
}

Vec3f soModelModuleImpl::getNodeLocalPosition(u32 nodeId, bool isSetUp) {
    if (!isSetUp) {
        setUpMtx(nodeId);
    }
    if (isVirtualNodeId(nodeId) == true) {
        int numVirtualNodes = m_virtualNodes->size();
        for (int i = 0; i < numVirtualNodes; i++) {
            Vec3f offset;
            if (m_virtualNodes->at(i).getOffset(nodeId, offset) == true) {
                return offset;
            }
        }
    }
    return nwSMGetLocalPosition(m_scnMdl, nodeId);
}

void soModelModuleImpl::getNodeLocalMtxFromNode(u32 nodeId1, u32 nodeId2, Matrix* outMtx, bool isSetUp) {
    if (!isSetUp) {
        setUpMtx(nodeId1);
        setUpMtx(nodeId2);
    }
    Matrix mtx1(true);
    Matrix mtx2(true);
    Matrix inv(true);
    getNodeGlobalMtx(nodeId2, &mtx2, false);
    getNodeGlobalMtx(nodeId1, &mtx1, false);
    mtx1.inverse(&inv);
    inv.mul(&mtx2, outMtx);
}

Vec3f soModelModuleImpl::getNodeGlobalRotation(u32 nodeId, bool isSetUp) {
    if (!isSetUp) {
        setUpMtx(nodeId);
    }
    if (isVirtualNodeId(nodeId) == true) {
        for (int i = 0; i < m_virtualNodes->size(); i++) {
            u32 parentId;
            if (m_virtualNodes->at(i).getNodeParent(nodeId, parentId) == true) {
                nodeId = parentId;
                break;
            }
        }
    }
    return nwSMGetGlobalRotation(m_scnMdl, nodeId);
}

bool soModelModuleImpl::isNode(u32 nodeId) {
    if (nodeId != -1) {
        int numVirtualNodes = m_virtualNodes->size();
        for (int i = 0; i < numVirtualNodes; i++) {
            if (m_virtualNodes->at(i).isNode(nodeId) == true) {
                return true;
            }
        }
        if (nodeId < soModelModuleSimple::getNodeNum()) {
            return true;
        }
    }
    return false;
}

u32 soModelModuleImpl::getNodeNum(bool shadow) const {
    nw4r::g3d::ResMdl resMdl = getScnMdl(shadow)->m_resMdl;
    return resMdl.GetResNodeNumEntries();
}

const char* soModelModuleImpl::getTransNNodeName() {
    return "TransN";
}

int soModelModuleImpl::getTransNNodeId() {
    u32 nodeId = m_transNNodeId;
    if (nodeId == -1) {
        return getNodeId(getTransNNodeName());
    }
    return nodeId;
}

Vec3f soModelModuleImpl::getResFileNodeTranslate(u32 nodeId) {
    nw4r::g3d::ResMdl resMdl = m_scnMdl->m_resMdl;
    nw4r::g3d::ResNode node = resMdl.GetResNode(static_cast<int>(nodeId));
    Vec3f translate;
    translate.m_x = node->m_translation.m_x;
    translate.m_y = node->m_translation.m_y;
    translate.m_z = node->m_translation.m_z;
    return translate;
}

Vec3f soModelModuleImpl::getResFileNodeRotation(u32 nodeId) {
    nw4r::g3d::ResMdl resMdl = m_scnMdl->m_resMdl;
    nw4r::g3d::ResNode node = resMdl.GetResNode(static_cast<int>(nodeId));
    Vec3f rotate;
    rotate.m_x = node->m_rotation.m_x;
    rotate.m_y = node->m_rotation.m_y;
    rotate.m_z = node->m_rotation.m_z;
    return rotate;
}

void soModelModuleImpl::setVisibility(bool visibility, bool shadow) {
    if (getScnMdl(shadow) != NULL) {
        nwSMSetVisibility(getScnMdl(shadow), visibility);
    }
}

void soModelModuleImpl::setNodeVisibility(u32 nodeId, bool visibility, bool shadow) {
    if (getScnMdl(shadow) != NULL) {
        nwSMSetNodeVisibility(getScnMdl(shadow), nodeId, visibility);
    }
}

void soModelModuleImpl::setNodeVisibility(s32 numIds, const u32* nodeIds, bool visibility, bool shadow) {
    if (getScnMdl(shadow) != NULL) {
        nwSMSetNodeVisibility(getScnMdl(shadow), numIds, nodeIds, visibility);
    }
}

void soModelModuleImpl::renderNodeAxis(u32 nodeId, float length) {
    // TODO: 0x2C4 bytes, debug drawing of a node's axes through the camera manager; the error text is
    // "soModelModuleImpl::renderNodeAxis" + a message (lbl_27_data_4C8).
}

bool soModelModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int p3) {
    // TODO: 0x23B0 bytes, the model anim-command interpreter (module 0x3: node SRT, visibility, scale, env map, ...).
    return false;
}

bool soModelModuleImpl::isObserv(char module) {
    return module == 3;
}

void soModelModuleImpl::setTransNNodeId(int nodeId) {
    m_transNNodeId = nodeId;
}

void soModelModuleImpl::changeMaterialTex(soResourceModule* resource, const char* resMat, const char* resTex,
                                          nw4r::g3d::ResFile resFile) {
    // TODO: 0x25C bytes, replaces the material's first texture object with resTex (CI textures load their palette).
}

void soModelModuleImpl::initVirtualNode(int index, u32 start, u32 end, void* info, Matrix* mtxs) {
    if (index >= 0 && index < m_virtualNodes->size()) {
        m_virtualNodes->at(index).init(start, end, static_cast<UnkVirtualNodeInfo*>(info), mtxs);
    }
}

void soModelModuleImpl::setVirtualNodeUpdate(u32 index, bool update) {
    m_virtualNodes->at(index).setUpdate(update);
}

void soModelModuleImpl::switchEnvMap(bool envMap) {
    if (isAddEnvMapMat() != true) {
        return;
    }
    if (m_isEnvMap == envMap) {
        return;
    }
    nw4r::g3d::ResMdl resMdl = m_scnMdl->m_resMdl;
    u32 half = resMdl.GetResMatNumEntries() >> 1;
    if (m_isEnvMap == true) {
        for (u32 i = 0; i < 2; i++) {
            soModelDrawByteCode* code = reinterpret_cast<soModelDrawByteCode*>(resMdl.GetResByteCode(g_drawByteCodeNames[i]));
            if (code != NULL) {
                while (code->m_op != 1) {
                    code->setMatId(code->getMatId() - half);
                    code++;
                }
            }
        }
        m_isEnvMap = false;
        m_moduleAccesser->getLinkModule().setModelConstraintAttribute(false, m_moduleAccesser->getStageObject().getTaskId());
    } else {
        for (u32 i = 0; i < 2; i++) {
            soModelDrawByteCode* code = reinterpret_cast<soModelDrawByteCode*>(resMdl.GetResByteCode(g_drawByteCodeNames[i]));
            if (code != NULL) {
                while (code->m_op != 1) {
                    code->setMatId(code->getMatId() + half);
                    code++;
                }
            }
        }
        m_isEnvMap = true;
        m_moduleAccesser->getLinkModule().setModelConstraintAttribute(true, m_moduleAccesser->getStageObject().getTaskId());
    }
}

void soModelModuleImpl::preShadowCalcView() {
    if (m_moduleAccesser->getShadowModule().isDispProjection() == true) {
        nw4r::g3d::ScnMdl* scnMdl;
        nw4r::g3d::ScnMdl* shadowScnMdl = getScnMdl(true);
        scnMdl = m_scnMdl;
        fn_8004765C(shadowScnMdl, scnMdl, m_shadowNode, getNodeNum(true));
    }
}

Matrix* soModelModuleImpl::getWorldMtxArray() {
    return reinterpret_cast<Matrix*>(m_scnMdl->m_worldMtxArray);
}

void soModelModuleImpl::setDisposeInstanceImm(bool imm) {
    m_isDisposeInstanceImm = imm;
}

void soModelModuleImpl::setShadowNode(int node) {
    m_shadowNodeOverride = node;
}

bool soModelModuleImpl::isRefFB() {
    return m_isRefFB;
}

bool soModelModuleImpl::isEnvMap() {
    return m_isEnvMap;
}

gfSceneRoot::LayerType soModelModuleImpl::getLayerType() {
    return m_layerType;
}

void soModelModuleImpl::setScaleZ(float scaleZ) {
    m_scaleZ = scaleZ;
}

void soModelModuleImpl::setScale(float scale) {
    m_scale = scale;
}

float soModelModuleImpl::getScale() {
    return m_scale;
}

void soModelModuleImpl::setLoupeNo(s8 loupeNo) {
    m_loupeNo = loupeNo;
}

s8 soModelModuleImpl::getLoupeNo() {
    return m_loupeNo;
}
