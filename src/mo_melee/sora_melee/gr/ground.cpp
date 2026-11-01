#include <gr/ground.h>
#include <st/stage.h>
#include <types.h>

// HYPOTHESIS: grCollision::setJointStatus (main.dol, unnamed); the ground passes itself as the owner.
extern "C" void fn_80110B98(grCollision* collision, bool enable, Ground* ground);
// HYPOTHESIS: stShadow::removeGroundInfo (unnamed in st_shadow.cpp).
extern "C" void fn_27_228FD8(stShadow* shadow, Ground* ground);

Ground::~Ground() {
    unloadData();
}

void Ground::setupMelee() {
    m_isUseMdlNameForStartAnim = false;
    m_isSetup = true;
}

void Ground::setMotionFrame(float frame, u32 animIndex) {
    gfModelAnimation* anim = m_modelAnims[animIndex];
    if (anim) {
        anim->setFrame(frame);
    }
}

float Ground::getMotionFrame(u32 animIndex) {
    gfModelAnimation* anim = m_modelAnims[animIndex];
    if (anim == NULL) {
        return 0.0f;
    } else {
        return anim->getFrame();
    }
}

void Ground::setMotionLoop(bool shouldLoop, u32 animIndex) {
    gfModelAnimation* anim = m_modelAnims[animIndex];
    if (anim) {
        anim->setLoop(shouldLoop);
    }
}

void Ground::invalidatedByCameraClipping() { }

void Ground::update(float deltaFrame) { }

void Ground::processGameProc() { }

void Ground::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) { }

void Ground::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) { }

void Ground::receiveCollMsg_Wall(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) { }

void Ground::receiveCollMsg_Attack(grCollStatus* collStatus, grCollisionJoint* collisionJoint) { }

void Ground::renderDebug() { }

bool Ground::setNode() {
    return false;
}

void Ground::setEnableCollisionStatus(bool enableColl) {
    m_isEnableCollisionStatus = enableColl;
    grCollision* collision = m_collision;
    if (collision) {
        if (m_isEnableCollisionStatus && m_isEnableCollisionStatusByClipping) {
            fn_80110B98(collision, true, this);
        } else {
            fn_80110B98(collision, false, this);
        }
    }
}

void Ground::setEnableCollisionStatusByClipping(bool enableColl) {
    m_isEnableCollisionStatusByClipping = enableColl;
    grCollision* collision = m_collision;
    if (collision) {
        if (m_isEnableCollisionStatusByClipping && m_isEnableCollisionStatus) {
            fn_80110B98(collision, true, this);
        } else {
            fn_80110B98(collision, false, this);
        }
    }
}

void Ground::preExit() {
    fn_27_228FD8(g_Stage->m_shadow, this);
}

void Ground::setDontMoveGround() {
    if (m_resFile.HasResAnmChr() != true) {
        m_isUpdateG3dCalcWorld = false;
    }
}

void Ground::setTransparencyFlag(char flag) { }

void Ground::setMotionRatio(float ratio) {
    m_motionRatio = ratio;
}

// The remaining functions of the unit, written against the shadow header's layout.

#include <ec/ec_mgr.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <gm/gm_stage_data.h>
#include <gr/collision/gr_collision_joint.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <revolution/OS.h>
#include <string.h>
#include <ut/ut_nw.h>

extern "C" int strncmp(const char*, const char*, size_t);
// HYPOTHESIS: grCollision::getJoint(ground, mdlIndex, nodeIndex) (main.dol, unnamed); setNodeCollision flips its bits.
extern "C" grCollisionJoint* fn_80110C78(grCollision* collision, Ground* ground, u32 mdlIndex, u32 nodeIndex);
// HYPOTHESIS: stShadow::addGroundInfo (unnamed in st_shadow.cpp), the counterpart of fn_27_228FD8.
extern "C" void fn_27_228C74(stShadow* shadow, Ground* ground);
nw4r::g3d::ScnMdl* ScnMdl_Construct(MEMAllocator* allocator, u32* size, nw4r::g3d::ResMdl mdl, u32 bufferOption, int nView,
                                    void (*callback)());
extern "C" void fn_801B0A28();

Ground::Ground(const char* taskName) : gfTask(taskName, Category_Ground, 3, 2, true),
    m_resFile(),
    m_sceneModels(NULL),
    m_modelAnims(NULL),
    m_collision(NULL),
    m_nextGround(NULL),
    m_modelAnimNum(0),
    m_mdlIndex(-1),
    m_stageData(NULL),
    m_motionRatio(1.0f),
    unk68(0.0f),
    m_isVisible(true),
    m_isVisibleByClipping(true),
    m_isEnableCollisionStatus(true),
    m_isEnableCollisionStatusByClipping(true),
    m_isEnableCalcCollision(true),
    m_hasBindedData(false),
    m_hasUpdatedG3dCalcWorld(false),
    m_isUseMdlNameForStartAnim(false),
    m_isCopyVisibility(false),
    m_isValid(true),
    m_noUpdateAnim(false),
    m_isPauseAnim(false),
    m_isUpdateG3dCalcWorld(true),
    m_isGr2(false),
    m_isEnableBlendColor(false),
    m_isSetup(false),
    m_heapType(Heaps::StageInstance) {
    for (int i = 0; i < 8; i++) {
        m_scnObjOptions[i] = 1;
    }
    m_scnObjOptions[8] = 0;
}

void Ground::bindData(gfArchive* data) {
    if (m_mdlIndex == -1) {
        return;
    }
    nw4r::g3d::ResFile mdlFile(data->getData(Data_Type_Model, m_mdlIndex, 0xFFFE));
    nw4r::g3d::ResFile texFile(data->getData(Data_Type_Tex, 0, 0xFFFE));
    if (mdlFile.ptr() == NULL) {
        OSReport("TaskName = %s; mdlindex = %d\n", m_taskName, m_mdlIndex);
        return;
    }
    nw4r::g3d::ResFile::Init(&mdlFile);
    bool bound = mdlFile.Bind(mdlFile);
    if (!bound && texFile.ptr() != NULL) {
        if (!m_hasBindedData) {
            nw4r::g3d::ResFile::Init(&texFile);
            m_hasBindedData = true;
        }
        bound = mdlFile.Bind(texFile);
    }
    if (!bound) {
        OSReport("TaskName = %s\n", m_taskName);
    }
}

void Ground::loadGroundData(nw4r::g3d::ResFile resFile, u32 flags, gfSceneRoot::LayerType lt) {
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(m_heapType);
    m_resFile = resFile;
    u32 anmFlags = 0;
    if (resFile.HasResAnmChr()) {
        anmFlags |= 0x40;
    }
    if (resFile.HasResAnmTexPat()) {
        anmFlags |= 0x3;
    }
    if (resFile.HasResAnmTexSrt()) {
        anmFlags |= 0x204;
    }
    if (resFile.HasResAnmShp()) {
        anmFlags |= 0x7000;
    }
    flags = (flags | anmFlags) | 0x108;
    if (m_isSetup == true) {
        flags |= 0x30;
    }
    m_modelAnimNum = m_resFile.GetResMdlNumEntries();
    m_sceneModels = new (m_heapType) nw4r::g3d::ScnMdl*[m_modelAnimNum];
    m_isCopyVisibility = true;
    m_modelAnims = new (m_heapType) gfModelAnimation*[m_modelAnimNum];
    u32 numModels = m_modelAnimNum;
    for (u32 i = 0; i < numModels; i++) {
        nw4r::g3d::ResMdl resMdl = m_resFile.GetResMdl(i);
        u32 size;
        nw4r::g3d::ScnMdl* scnMdl = ScnMdl_Construct(allocator, &size, resMdl, flags, 1, fn_801B0A28);
        gfModelAnimation* modelAnim;
        if (m_isUseMdlNameForStartAnim == true) {
            nw4r::g3d::ResMdl namedMdl = m_resFile.GetResMdl(i);
            const char* mdlName = namedMdl.ptr()->m_toName != 0 ? reinterpret_cast<const char*>(namedMdl.ptr()) + namedMdl.ptr()->m_toName : NULL;
            nw4r::g3d::ResMdl animMdl = m_resFile.GetResMdl(i);
            modelAnim = new (m_heapType) gfModelAnimation(&m_resFile, &animMdl, true, mdlName, m_heapType);
        } else {
            nw4r::g3d::ResMdl animMdl = m_resFile.GetResMdl(i);
            modelAnim = new (m_heapType) gfModelAnimation(&m_resFile, &animMdl, true, static_cast<u32>(0), m_heapType);
        }
        modelAnim->_spacer[0] = 0; // the byte after the resource: the animation does not own the file (as in st_village/gr_village.h)
        gfModelAnimation::bind(scnMdl, modelAnim);
        m_sceneModels[i] = scnMdl;
        m_modelAnims[i] = modelAnim;
        modelAnim->setFrame(0.0f);
        addSceneRoot(scnMdl, lt);
    }
}

// Adds a scene model to the layer, or when none is given, to the layer its model name prefix selects.
void Ground::addSceneRoot(nw4r::g3d::ScnMdl* scnMdl, int lt) {
    nw4r::g3d::ResMdlData* mdl = scnMdl->m_resMdl.ptr();
    if (mdl == NULL) {
        return;
    }
    if (lt == 0) {
        const char* name = mdl->m_toName != 0 ? reinterpret_cast<const char*>(mdl) + mdl->m_toName : NULL;
        if (strncmp(name, "gr2_", 4) == 0) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Ground2, scnMdl);
            m_isGr2 = true;
        } else if (strncmp(name, "grf_", 4) == 0) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Special_Stage_Object, scnMdl);
            scnMdl->SetPriorityDrawOpa(0xFF);
            scnMdl->SetPriorityDrawXlu(0xFF);
        } else if (strncmp(name, "grpf_", 5) == 0) {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Effect, scnMdl);
            scnMdl->SetPriorityDrawOpa(0xFF);
            scnMdl->SetPriorityDrawXlu(0xFF);
        } else {
            g_gfSceneRoot->add(gfSceneRoot::Layer_Ground, scnMdl);
        }
    } else {
        g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(lt), scnMdl);
    }
}

void Ground::unloadData() {
    if (m_sceneModels != NULL) {
        u32 numModels = m_modelAnimNum;
        for (u32 i = 0; i != numModels; i++) {
            m_sceneModels[i]->m_calcWorldCallBack = NULL;
            g_gfSceneRoot->remove(m_sceneModels[i]);
            m_sceneModels[i]->Destroy();
            m_sceneModels[i] = NULL;
        }
        delete[] m_sceneModels;
        m_sceneModels = NULL;
    }
    if (m_modelAnims != NULL) {
        u32 numAnims = m_modelAnimNum;
        gfModelAnimation* modelAnim;
        for (u32 i = 0; i != numAnims; i++) {
            modelAnim = m_modelAnims[i];
            if (modelAnim->m_anmObjChrRes != NULL) {
                modelAnim->m_anmObjChrRes->Destroy();
            }
            if (modelAnim->m_anmObjVisRes != NULL) {
                modelAnim->m_anmObjVisRes->Destroy();
            }
            if (modelAnim->m_anmObjTexPatRes != NULL) {
                modelAnim->m_anmObjTexPatRes->Destroy();
            }
            if (modelAnim->m_anmObjTexSrtRes != NULL) {
                modelAnim->m_anmObjTexSrtRes->Destroy();
            }
            if (modelAnim->m_anmObjMatClrRes != NULL) {
                modelAnim->m_anmObjMatClrRes->Destroy();
            }
            if (modelAnim->m_anmObjShpRes != NULL) {
                modelAnim->m_anmObjShpRes->Destroy();
            }
            if (modelAnim->m_resFile.ptr() != NULL && modelAnim->_spacer[0]) {
                gfHeapManager::free(modelAnim->m_resFile.ptr());
                modelAnim->m_resFile = nw4r::g3d::ResFile();
            }
            modelAnim->m_anmObjChrRes = NULL;
            modelAnim->m_anmObjVisRes = NULL;
            modelAnim->m_anmObjTexPatRes = NULL;
            modelAnim->m_anmObjTexSrtRes = NULL;
            modelAnim->m_anmObjMatClrRes = NULL;
            modelAnim->m_anmObjShpRes = NULL;
            gfHeapManager::free(m_modelAnims[i]);
            m_modelAnims[i] = NULL;
        }
        delete[] m_modelAnims;
        m_modelAnims = NULL;
    }
}

void Ground::processAnim() {
    if (m_resFile.ptr() == NULL) {
        return;
    }
    gmStageData* stageData = g_GameGlobal->m_stageData;
    float rate = g_stGameFrame.m_frameDelta * m_motionRatio * stageData->m_motionRatio * stageData->m_motionSubRatio;
    u32 numModels = m_modelAnimNum;
    if (m_isPauseAnim == true) {
        rate = 0.0f;
    }
    for (u32 i = 0; i != numModels; i++) {
        m_modelAnims[i]->setUpdateRate(rate);
        if (m_noUpdateAnim == true) {
            m_sceneModels[i]->G3dProc(8, 0, 0);
        } else {
            m_sceneModels[i]->UpdateFrame();
        }
    }
    updateG3dProcCalcWorld();
    m_hasUpdatedG3dCalcWorld = false;
}

void Ground::updateG3dProcCalcWorld() {
    if (m_isUpdateG3dCalcWorld == true && !m_hasUpdatedG3dCalcWorld) {
        u32 i = 0;
        u32 numModels = m_modelAnimNum;
        for (; i != numModels; i++) {
            nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[i];
            scnMdl->SetScnObjOption(2, 0);
            scnMdl->G3dProc(1, 0, 0);
            scnMdl->SetScnObjOption(2, 1);
        }
    }
    m_hasUpdatedG3dCalcWorld = true;
}

void Ground::updateG3dProcCalcWorldForce() {
    u32 numModels = m_modelAnimNum;
    for (u32 i = 0; i < numModels; i++) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[i];
        scnMdl->SetScnObjOption(2, 0);
        scnMdl->G3dProc(1, 0, 0);
        scnMdl->SetScnObjOption(2, 1);
    }
    m_hasUpdatedG3dCalcWorld = true;
}

void Ground::processUpdate() {
    g_ecMgr->pushsetCurrentGroup(0x2000000);
    if (!m_isPauseAnim) {
        update(g_stGameFrame.m_frameDelta * g_GameGlobal->m_stageData->m_motionRatio *
               g_GameGlobal->m_stageData->m_motionSubRatio);
    }
    g_ecMgr->popCurrentGroup();
    if (g_Stage->unkAC) {
        invalidatedByCameraClipping();
    }
}

void Ground::setVisibility(u32 visibility) {
    if (m_resFile.ptr() == NULL) {
        return;
    }
    m_isVisible = visibility;
    u32 numModels = m_modelAnimNum;
    if (m_isVisible) {
        for (u32 i = 0; i < numModels; i++) {
            m_sceneModels[i]->SetScnObjOption(0x10001, 0);
        }
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            scnMdl->SetScnObjOption(1, m_scnObjOptions[0]);
            scnMdl->SetScnObjOption(2, m_scnObjOptions[1]);
            scnMdl->SetScnObjOption(3, m_scnObjOptions[2]);
            scnMdl->SetScnObjOption(4, m_scnObjOptions[3]);
            scnMdl->SetScnObjOption(5, m_scnObjOptions[4]);
            scnMdl->SetScnObjOption(6, m_scnObjOptions[5]);
            scnMdl->SetScnObjOption(7, m_scnObjOptions[6]);
            scnMdl->SetScnObjOption(8, m_scnObjOptions[7]);
            scnMdl->SetScnObjOption(9, m_scnObjOptions[8]);
            nwSMSetVisibility(scnMdl, true);
        }
    } else {
        for (u32 i = 0; i < numModels; i++) {
            m_sceneModels[i]->SetScnObjOption(0x10001, 1);
        }
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            nwSMSetVisibility(scnMdl, false);
            scnMdl->SetScnObjOption(1, 1);
            scnMdl->SetScnObjOption(2, 1);
            scnMdl->SetScnObjOption(3, 1);
            scnMdl->SetScnObjOption(4, 1);
            scnMdl->SetScnObjOption(5, 1);
            scnMdl->SetScnObjOption(6, 1);
            scnMdl->SetScnObjOption(7, 1);
            scnMdl->SetScnObjOption(8, 1);
            scnMdl->SetScnObjOption(9, 0);
        }
    }
}

void Ground::setVisibilityByClipping(u32 visibility) {
    u32 numModels = m_modelAnimNum;
    for (u32 i = 0; i < numModels; i++) {
        if (visibility && !m_isVisibleByClipping) {
            addSceneRoot(m_sceneModels[i], 0);
            setVisibility(m_isVisible);
        } else if (!visibility && m_isVisibleByClipping) {
            g_gfSceneRoot->remove(m_sceneModels[i]);
        }
    }
    m_isVisibleByClipping = visibility;
}

void Ground::setVisibilityByClipping(int visibility, u32 sceneModelIndex) {
    if (visibility && !m_isVisibleByClipping) {
        addSceneRoot(m_sceneModels[sceneModelIndex], 0);
        setVisibility(m_isVisible);
    } else if (!visibility && m_isVisibleByClipping) {
        g_gfSceneRoot->remove(m_sceneModels[sceneModelIndex]);
    }
}

void Ground::setVisibilityAttachedEffect(u32 visibility) {
    for (int i = 0; i < getModelCount(); i++) {
        if (visibility == 1) {
            addSceneRoot(m_sceneModels[i], 0);
        } else {
            g_gfSceneRoot->remove(m_sceneModels[i]);
        }
    }
    setVisibility(visibility);
}

bool Ground::searchNode(const char* mdlName, const char* nodeName, int* mdlIndex, u16* nodeIndex) {
    u32 numModels = getModelCount();
    bool found = false;
    for (u32 i = 0; i < numModels; i++) {
        nw4r::g3d::ResMdl resMdl = m_sceneModels[i]->m_resMdl;
        const char* name = resMdl.ptr()->m_toName != 0 ? reinterpret_cast<const char*>(resMdl.ptr()) + resMdl.ptr()->m_toName : NULL;
        if (strcmp(name, mdlName) == 0) {
            nw4r::g3d::ResNode node = resMdl.GetResNode(nodeName);
            if (node.ptr() != NULL) {
                *mdlIndex = i;
                *nodeIndex = node.IsValid() ? node->m_nodeIndex : 0;
                found = true;
                break;
            }
        }
    }
    return found;
}

void Ground::receiveCollMsg(int direction, grCollStatus* collStatus, grCollisionJoint* collisionJoint) {
    switch (direction) {
        case 0:
            receiveCollMsg_Landing(collStatus, collisionJoint, false);
            break;
        case 1:
            receiveCollMsg_Landing(collStatus, collisionJoint, true);
            break;
        case 2:
            // The original passes no third argument here.
            receiveCollMsg_Heading(collStatus, collisionJoint, false);
            break;
        case 3:
            receiveCollMsg_Wall(collStatus, collisionJoint, true);
            break;
        case 4:
            receiveCollMsg_Wall(collStatus, collisionJoint, false);
            break;
        case 5:
            receiveCollMsg_Attack(collStatus, collisionJoint);
            break;
    }
}

void Ground::startup(gfArchive* data, u32 flags, gfSceneRoot::LayerType lt) {
    if (m_mdlIndex >= 0) {
        bindData(data);
        loadGroundData(nw4r::g3d::ResFile(data->getData(Data_Type_Model, m_mdlIndex, 0xFFFE)), flags, lt);
        fn_27_228C74(g_Stage->m_shadow, this);
    }
    setNode();
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        scnMdl->GetScnObjOption(1, &m_scnObjOptions[0]);
        scnMdl->GetScnObjOption(2, &m_scnObjOptions[1]);
        scnMdl->GetScnObjOption(3, &m_scnObjOptions[2]);
        scnMdl->GetScnObjOption(4, &m_scnObjOptions[3]);
        scnMdl->GetScnObjOption(5, &m_scnObjOptions[4]);
        scnMdl->GetScnObjOption(6, &m_scnObjOptions[5]);
        scnMdl->GetScnObjOption(7, &m_scnObjOptions[6]);
        scnMdl->GetScnObjOption(8, &m_scnObjOptions[7]);
        scnMdl->GetScnObjOption(9, &m_scnObjOptions[8]);
    }
    m_hasUpdatedG3dCalcWorld = false;
    updateG3dProcCalcWorld();
    m_hasUpdatedG3dCalcWorld = false;
    if (m_isSetup == true) {
        setBlendColorDisable();
    }
}

u32 Ground::getNodeIndex(u32 sceneModelIndex, const char* nodeName) {
    if (nodeName == NULL) {
        return 0;
    }
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return 0;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return 0;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeName);
    if (node.ptr() == NULL) {
        return 0;
    }
    return node.IsValid() ? node->m_nodeIndex : 0;
}

bool Ground::getNodeIndex(u32* nodeIndex, u32 sceneModelIndex, const char* nodeName) {
    if (nodeName == NULL) {
        return false;
    }
    u32 index = 0;
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeName);
    if (node.ptr() == NULL) {
        return false;
    }
    if (node.IsValid()) {
        index = node->m_nodeIndex;
    }
    *nodeIndex = index;
    return true;
}

bool Ground::getNodePosition(Vec3f* nodePos, u32 sceneModelIndex, u32 nodeIndex) {
    if (nodePos == NULL) {
        return false;
    }
    nodePos->m_x = 0.0f;
    nodePos->m_y = 0.0f;
    nodePos->m_z = 0.0f;
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    if (resMdl.GetResNodeNumEntries() <= nodeIndex) {
        return false;
    }
    nw4r::math::MTX34 mtx;
    if (!scnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeIndex)) {
        return false;
    }
    *nodePos = Vec3f(mtx._03, mtx._13, mtx._23);
    return true;
}

bool Ground::getNodePosition(Vec3f* nodePos, u32 sceneModelIndex, const char* nodeName) {
    u32 nodeIndex;
    if (!getNodeIndex(&nodeIndex, sceneModelIndex, nodeName)) {
        return false;
    }
    nw4r::math::MTX34 mtx;
    if (!m_sceneModels[sceneModelIndex]->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeIndex)) {
        return false;
    }
    *nodePos = Vec3f(mtx._03, mtx._13, mtx._23);
    return true;
}

bool Ground::getNodeMatrix(Matrix* nodeMtx, u32 sceneModelIndex, u32 nodeIndex) {
    if (nodeMtx == NULL) {
        return false;
    }
    nodeMtx->setIdentity();
    bool result = false;
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    if (resMdl.GetResNodeNumEntries() > nodeIndex) {
        result = scnMdl->GetScnMtxPos(nodeMtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeIndex);
    }
    return result;
}

bool Ground::getNodeMatrix(Matrix* nodeMtx, u32 sceneModelIndex, const char* nodeName) {
    u32 nodeIndex;
    if (!getNodeIndex(&nodeIndex, sceneModelIndex, nodeName)) {
        return false;
    }
    return m_sceneModels[sceneModelIndex]->GetScnMtxPos(nodeMtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeIndex);
}

bool Ground::setNodeVisibility(bool setVisibility, u32 sceneModelIndex, u32 nodeIndex, bool recursive, bool recursive_) {
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeIndex);
    if (node.ptr() == NULL) {
        return false;
    }
    if (recursive == true) {
        nw4r::g3d::ResNode child(node->m_toChild != 0 ? reinterpret_cast<u8*>(node.ptr()) + node->m_toChild : NULL);
        if (child.ptr() != NULL) {
            this->setNodeVisibility(setVisibility, sceneModelIndex, child.IsValid() ? child->m_nodeIndex : 0, recursive, true);
        }
    }
    if (recursive_ == true) {
        nw4r::g3d::ResNode next(node->m_toNext != 0 ? reinterpret_cast<u8*>(node.ptr()) + node->m_toNext : NULL);
        if (next.ptr() != NULL) {
            this->setNodeVisibility(setVisibility, sceneModelIndex, next.IsValid() ? next->m_nodeIndex : 0, recursive, recursive_);
        }
    }
    if (m_isCopyVisibility) {
        nw4r::g3d::ScnMdl::CopiedVisAccess visAccess(scnMdl, nodeIndex);
        visAccess.SetVisibility(setVisibility);
    }
    if (node.ptr() != NULL) {
        node->m_isVisible = setVisibility;
    }
    return true;
}

bool Ground::setNodeVisibility(bool setVisibility, u32 sceneModelIndex, const char* nodeName, bool recursive, bool recursive_) {
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeName);
    if (node.ptr() == NULL) {
        return false;
    }
    return this->setNodeVisibility(setVisibility, sceneModelIndex, node.IsValid() ? node->m_nodeIndex : 0, recursive, recursive_);
}

bool Ground::setNodeVisibilityAll(bool setVisibility, u32 sceneModelIndex) {
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    u32 numNodes = resMdl.GetResNodeNumEntries();
    for (u32 i = 0; i != numNodes; i++) {
        nw4r::g3d::ResNode node = resMdl.GetResNode(i);
        if (node.ptr() == NULL) {
            break;
        }
        if (m_isCopyVisibility) {
            nw4r::g3d::ScnMdl::CopiedVisAccess visAccess(scnMdl, i);
            visAccess.SetVisibility(setVisibility);
        }
        if (node.ptr() != NULL) {
            node->m_isVisible = setVisibility;
        }
    }
    return true;
}

bool Ground::isNodeVisible(u32 sceneModelIndex, u32 nodeIndex) {
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeIndex);
    if (node.ptr() == NULL) {
        return false;
    }
    return node.IsValid() ? node->m_isVisible : false;
}

bool Ground::setNodeCollision(bool setCollision, u32 sceneModelIndex, u32 nodeIndex, bool recursive) {
    if (recursive) {
        nw4r::g3d::ResMdl resMdl(NULL);
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[static_cast<u16>(sceneModelIndex)];
        if (scnMdl == NULL) {
            return false;
        }
        resMdl = scnMdl->m_resMdl;
        if (resMdl.ptr() == NULL) {
            return false;
        }
        nw4r::g3d::ResNode node = resMdl.GetResNode(static_cast<int>(nodeIndex));
        if (node.ptr() == NULL) {
            return false;
        }
        nw4r::g3d::ResNode child(node->m_toChild != 0 ? reinterpret_cast<u8*>(node.ptr()) + node->m_toChild : NULL);
        if (child.ptr() != NULL) {
            this->setNodeCollision(setCollision, sceneModelIndex, static_cast<u16>(child.IsValid() ? child->m_nodeIndex : 0), recursive);
        }
        nw4r::g3d::ResNode next(node->m_toNext != 0 ? reinterpret_cast<u8*>(node.ptr()) + node->m_toNext : NULL);
        if (next.ptr() != NULL) {
            this->setNodeCollision(setCollision, sceneModelIndex, static_cast<u16>(next.IsValid() ? next->m_nodeIndex : 0), recursive);
        }
    }
    grCollision* collision = m_collision;
    if (collision != NULL) {
        grCollisionJoint* joint = fn_80110C78(collision, this, sceneModelIndex, nodeIndex);
        if (joint != NULL) {
            joint->m_0x56_7 = true;
            if (setCollision) {
                joint->m_0x54_6 = joint->m_0x54_4 = true;
            } else {
                joint->m_0x54_6 = false;
                joint->m_0x54_6 = joint->m_0x54_4 = joint->m_0x54_1;
            }
            return true;
        }
    }
    return false;
}

bool Ground::setNodeCollision(bool setCollision, u32 sceneModelIndex, const char* nodeName, bool recursive) {
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[static_cast<u16>(sceneModelIndex)];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeName);
    if (node.ptr() == NULL) {
        return false;
    }
    if (recursive) {
        nw4r::g3d::ResNode child(node->m_toChild != 0 ? reinterpret_cast<u8*>(node.ptr()) + node->m_toChild : NULL);
        if (child.ptr() != NULL) {
            this->setNodeCollision(setCollision, sceneModelIndex, static_cast<u16>(child.IsValid() ? child->m_nodeIndex : 0), recursive);
        }
    }
    return this->setNodeCollision(setCollision, sceneModelIndex, static_cast<u16>(node.IsValid() ? node->m_nodeIndex : 0), false);
}

bool Ground::getNodeScale(Vec3f* nodeScale, u32 sceneModelIndex, u32 nodeIndex) {
    if (nodeScale == NULL) {
        return false;
    }
    nodeScale->m_x = 1.0f;
    nodeScale->m_y = 1.0f;
    nodeScale->m_z = 1.0f;
    nw4r::g3d::ResMdl resMdl(NULL);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[sceneModelIndex];
    if (scnMdl == NULL) {
        return false;
    }
    resMdl = scnMdl->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(nodeIndex);
    if (node.ptr() == NULL) {
        return false;
    }
    nodeScale->m_x = node->m_scale.m_x;
    nodeScale->m_y = node->m_scale.m_y;
    nodeScale->m_z = node->m_scale.m_z;
    return true;
}

bool Ground::getNodeScale(Vec3f* nodeScale, u32 sceneModelIndex, const char* nodeName) {
    u32 nodeIndex;
    if (!getNodeIndex(&nodeIndex, sceneModelIndex, nodeName)) {
        return false;
    }
    return getNodeScale(nodeScale, sceneModelIndex, nodeIndex);
}

// Both setValid variants repeat setEnableCollisionStatus's body with the u32 argument (no bool conversion).
void Ground::setValid(u32 valid) {
    if (m_isValid != valid) {
        m_isValid = valid;
        setVisibility(valid);
        grCollision* collision = m_collision;
        m_isEnableCollisionStatus = valid;
        if (collision) {
            if (m_isEnableCollisionStatus && m_isEnableCollisionStatusByClipping) {
                fn_80110B98(collision, true, this);
            } else {
                fn_80110B98(collision, false, this);
            }
        }
    }
}

void Ground::setValidAttachedEffect(u32 valid) {
    if (m_isValid != valid) {
        m_isValid = valid;
        setVisibilityAttachedEffect(valid);
        grCollision* collision = m_collision;
        m_isEnableCollisionStatus = valid;
        if (collision) {
            if (m_isEnableCollisionStatus && m_isEnableCollisionStatusByClipping) {
                fn_80110B98(collision, true, this);
            } else {
                fn_80110B98(collision, false, this);
            }
        }
    }
}

bool Ground::isCollisionStatusOwnerTask(grCollStatus* collStatus, int* categoryMask) {
    CollCategoryFlag* categories = reinterpret_cast<CollCategoryFlag*>(categoryMask);
    gfTask* task = gfTask::getTask(collStatus->m_taskId);
    if (task == NULL) {
        return false;
    }
    if (categories->m_fighter && task->m_taskCategory == 0xA) {
        return true;
    }
    if (categories->m_enemy && task->m_taskCategory == 0x2) {
        return true;
    }
    if (categories->m_item && task->m_taskCategory == 0xB) {
        return true;
    }
    return false;
}

