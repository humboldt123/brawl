#include <cm/cm_camera_controller.h>
#include <ec/ec_mgr.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_draw.h>
#include <gm/gm_global.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <revolution/GX.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_plankton/gr_plankton.h>
#include <st_plankton/st_plankton.h>

// HYPOTHESIS: makes a texture object of a texture resource, and sets the wrap mode of it (main, unnamed)
extern "C" void fn_8001AA88(GXTexObj* texObj, nw4r::g3d::ResTex* tex);
extern "C" void Destroy__Q34nw4r3g3d6G3dObjFv(void* obj);
extern "C" void fn_801F2C58(GXTexObj* texObj, int wrapS, int wrapT);

stClassInfoImpl<Stages::Plankton, stPlankton> stPlankton::bss_loc_14;

stPlankton* stPlankton::create() {
    return new (Heaps::StageInstance) stPlankton;
}

stPlankton::stPlankton() : stMelee("stPlankton", Stages::Plankton) {
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_intro = 0;
    m_waterState = 0;
    m_hanenbouState = 0;
    m_hanenbouTimer = 0.0f;
    m_hanenbouWait = 0.0f;
    for (u8 i = 0; i < 10; i++) {
        m_leaf[i].unk00 = 0.0f;
        m_leaf[i].m_angle = 0.0f;
        m_leaf[i].m_posA.m_x = 0.0f;
        m_leaf[i].m_posA.m_y = 0.0f;
        m_leaf[i].m_posA.m_z = 0.0f;
        m_leaf[i].m_posB.m_x = 0.0f;
        m_leaf[i].m_posB.m_y = 0.0f;
        m_leaf[i].m_posB.m_z = 0.0f;
        m_leaf[i].m_hitCount = 0;
        m_leaf[i].m_wait = 3;
        m_leaf[i].unk22 = 3;
        m_leaf[i].m_flip = 2;
        m_leaf[i].m_index = 10;
    }
    memset(&m_posAshibaLeft[0], 0, sizeof(Vec3f));
    memset(&m_posAshibaLeft[1], 0, sizeof(Vec3f));
    memset(m_posLimit, 0, sizeof(m_posLimit));
    memset(&m_posLimitNow, 0, sizeof(Vec3f));
    memset(m_posSuimen, 0, sizeof(m_posSuimen));
    for (u8 i = 0; i < 16; i++) {
        m_hamon[i].m_x = 0.0f;
        m_hamon[i].m_y = 0.0f;
        m_hamon[i].m_state = 3;
        m_hamon[i].m_kind = 0;
    }
    for (u8 i = 0; i < 8; i++) {
        m_hanenbou[i].m_x = 0.0f;
        m_hanenbou[i].m_y = 0.0f;
        m_hanenbou[i].m_prevX = 0.0f;
        m_hanenbou[i].m_prevY = 0.0f;
        m_hanenbou[i].m_speedX = 0.0f;
        m_hanenbou[i].m_speedY = 0.0f;
        m_hanenbou[i].m_angle = 0.0f;
        m_hanenbou[i].m_state = 3;
    }
    memset(m_wave, 0, sizeof(m_wave));
    m_stateWater = 3;
    m_scnProc = NULL;
    m_container = NULL;
    m_hanenbouKind = 0;
    m_hanenbouCount = 0;
    m_hanenbouStep = 0;
    m_event = 0;
}

stPlankton::~stPlankton() {
    if (m_scnProc != NULL) {
        Destroy__Q34nw4r3g3d6G3dObjFv(m_scnProc);
    }
    m_scnProc = NULL;
    if (m_container != NULL) {
        delete m_container;
    }
    m_container = NULL;
    releaseArchive();
}

// The water is drawn by the stage itself: a scene object whose draw function (drawProc) draws the water as strips of
// quads with the texture "suimen" and the height of the waves.
void stPlankton::drawProc(nw4r::g3d::ScnProc* proc, bool opa) {
    if (opa != true) {
        gfCameraManager::getManager()->m_cameras[0].setGX();
        stPlankton* stage = static_cast<stPlankton*>(proc->GetUserData());
        if (stage != NULL) {
            if (stage->isValidResTex() == 1) {
                gfDrawSetVtxPosColorTexPrimEnvironment();
                nw4r::g3d::ResTex tex = stage->getResTex("suimen");
                GXTexObj texObj;
                fn_8001AA88(&texObj, &tex);
                fn_801F2C58(&texObj, 0, 0);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
            } else {
                gfDrawSetVtxPosColorPrimEnvironment();
            }
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
            GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
            GXSetZMode(1, GX_LEQUAL, 0);
            GXSetZCompLoc(1);
            GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
            float width = stage->m_posLimit[1].m_x - stage->m_posLimit[0].m_x;
            float margin = width * 0.0078125f * 32.0f;
            float total = width + margin * 2.0f;
            float x;
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
            x = (stage->m_posLimit[0].m_x - margin) - 100.0f;
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(x, stage->m_posSuimen[0].m_y, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(0.0f, 0.0f);
            x = stage->m_posLimit[0].m_x - margin;
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(x, stage->m_posSuimen[0].m_y, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(0.0f, 0.0f);
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 0x180);
            stPlanktonWave* wave = stage->m_wave;
            for (int i = 0; i < 192; i++, wave++) {
                float* height = &wave->m_height;
                if (height == NULL) {
                    break;
                }
                float rate = (float)i / 191.0f;
                rate = nw4r::math::FSelect(rate - 0.0f, rate, 0.0f);
                rate = nw4r::math::FSelect(rate - 1.0f, 1.0f, rate);
                x = (stage->m_posLimit[0].m_x - margin) + total * rate;
                GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
                GXColor1u32(0xFFFFFFA5);
                GXTexCoord2f32(rate, 1.0f);
                GXPosition3f32(x, *height + stage->m_posSuimen[0].m_y, 1.0f);
                GXColor1u32(0xFFFFFFA5);
                GXTexCoord2f32(rate, 0.0f);
            }
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 8);
            x = (stage->m_posLimit[0].m_x - margin) - 100.0f;
            GXPosition3f32(x, stage->m_posLimitNow.m_y - 100.0f, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(0.0f, 1.0f);
            x = stage->m_posLimit[0].m_x - margin;
            GXPosition3f32(x, stage->m_posLimitNow.m_y - 100.0f, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(0.0f, 1.0f);
            x = stage->m_posLimit[1].m_x + margin;
            GXPosition3f32(x, stage->m_posLimitNow.m_y - 100.0f, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(0.0f, 1.0f);
            x = (stage->m_posLimit[1].m_x + margin) + 100.0f;
            GXPosition3f32(x, stage->m_posLimitNow.m_y - 100.0f, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(0.0f, 1.0f);
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
            x = stage->m_posLimit[1].m_x + margin;
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(x, stage->m_posSuimen[0].m_y, 1.0f);
            GXColor1u32(0xFFFFFFA5);
            GXTexCoord2f32(1.0f, 0.0f);
            x = (stage->m_posLimit[1].m_x + margin) + 100.0f;
            GXPosition3f32(x, stage->m_posLimitNow.m_y, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(x, stage->m_posSuimen[0].m_y, 1.0f);
            GXColor1u32(0xFFFFFF00);
            GXTexCoord2f32(1.0f, 0.0f);
        }
    }
}

bool stPlankton::isValidResTex() {
    if (m_resFile.ptr() != NULL && m_resFile.HasResTex()) {
        return true;
    }
    return false;
}

nw4r::g3d::ResTex stPlankton::getResTex(const char* name) {
    return m_resFile.GetResTex(name);
}

bool stPlankton::loading() {
    return true;
}

void stPlankton::createObj() {
    CameraController* camera = CameraController::getInstance();
    // HYPOTHESIS: flag and factor of the camera (their names are unknown)
    *(reinterpret_cast<u8*>(camera) + 0x44) |= 2;
    *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x190) = 0.87377f;
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0xF4);
    stDataContainerData* containerData = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, 0x15, 0xFFFE));
    if (containerData != NULL) {
        m_container = stDataMultiContainer::create(containerData, Heaps::StageInstance);
    }
    createObjBg(0);
    createObjFlower();
    createObjAshibaLeft();
    createObjAshiba(3);
    createObjAshiba(4);
    createObjAshiba(5);
    createObjAshiba(6);
    createObjAshiba(7);
    createObjAshiba(8);
    createObjAshiba(9);
    createObjAshiba(10);
    createObjAshiba(11);
    createObjAshiba(12);
    createCollision(m_fileData, 2, NULL);
    createObjHanenbou();
    createObjHamon();
    createObjWater();
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData != NULL) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x1E);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    m_resFile = nw4r::g3d::ResFile(m_fileData->getData(Data_Type_Tex, 0, 0xFFFE));
    u32 size;
    m_scnProc = nw4r::g3d::ScnProc::Construct(gfHeapManager::getMEMAllocator(Heaps::StageInstance), &size, drawProc, false, true);
    if (m_scnProc != NULL) {
        m_scnProc->SetUserData(this);
        g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(3), reinterpret_cast<nw4r::g3d::ScnMdl*>(m_scnProc));
        if (g_GameGlobal->m_modeMelee != NULL && planktonMeleeBytes()->m_mode == 7 && planktonMeleeBytes()->m_event == 0x24) {
            m_event = 1;
        }
    }
}

void stPlankton::createObjBg(int index) {
    grPlanktonBg* ground;
    if (index == 0) {
        ground = grPlanktonBg::create(0, "StgPlanktonMain", "grPlanktonMainBg");
    } else {
        ground = NULL;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(m_posLimit);
        ground->setPosSuimenWork(m_posSuimen);
        ground->setStateWater(&m_stateWater);
    }
}

void stPlankton::createObjFlower() {
    grPlanktonFlower* ground = grPlanktonFlower::create(10, "StgPlanktonFlower", "grPlanktonFlower");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setLeafData(m_leaf);
    }
}

// The ten leaves: every one is a ground of its own with a leaf of the stage's data (its start angle and which way it turns).
void stPlankton::createObjAshiba(int index) {
    stPlanktonData* data = static_cast<stPlanktonData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    grPlanktonAshiba* ground;
    stPlanktonLeaf* leaf = NULL;
    switch (index) {
    case 3:
        ground = grPlanktonAshiba::create(2, "StgPlanktonAshibaA", "grPlanktonAshibaA");
        leaf = &m_leaf[0];
        leaf->unk00 = -0.0f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 0;
        leaf->m_index = 0;
        break;
    case 4:
        ground = grPlanktonAshiba::create(3, "StgPlanktonAshibaB", "grPlanktonAshibaB");
        leaf = &m_leaf[1];
        leaf->unk00 = -0.2f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 0;
        leaf->m_index = 1;
        break;
    case 5:
        ground = grPlanktonAshiba::create(4, "StgPlanktonAshibaC", "grPlanktonAshibaC");
        leaf = &m_leaf[2];
        leaf->unk00 = -0.4f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 0;
        leaf->m_index = 2;
        break;
    case 6:
        ground = grPlanktonAshiba::create(5, "StgPlanktonAshibaD", "grPlanktonAshibaD");
        leaf = &m_leaf[3];
        leaf->unk00 = -0.5f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 1;
        leaf->m_index = 3;
        break;
    case 7:
        ground = grPlanktonAshiba::create(6, "StgPlanktonAshibaE", "grPlanktonAshibaE");
        leaf = &m_leaf[4];
        leaf->unk00 = -0.3f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 1;
        leaf->m_index = 4;
        break;
    case 8:
        ground = grPlanktonAshiba::create(7, "StgPlanktonAshibaF", "grPlanktonAshibaF");
        leaf = &m_leaf[5];
        leaf->unk00 = -0.1f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 1;
        leaf->m_index = 5;
        break;
    case 9:
        ground = grPlanktonAshiba::create(12, "StgPlanktonAshibaG", "grPlanktonAshibaG");
        leaf = &m_leaf[6];
        leaf->unk00 = -0.0f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 0;
        leaf->m_index = 6;
        break;
    case 10:
        ground = grPlanktonAshiba::create(13, "StgPlanktonAshibaH", "grPlanktonAshibaH");
        leaf = &m_leaf[7];
        leaf->unk00 = -0.1f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 0;
        leaf->m_index = 7;
        break;
    case 11:
        ground = grPlanktonAshiba::create(14, "StgPlanktonAshibaI", "grPlanktonAshibaI");
        leaf = &m_leaf[8];
        leaf->unk00 = -0.2f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 1;
        leaf->m_index = 8;
        break;
    case 12:
        ground = grPlanktonAshiba::create(15, "StgPlanktonAshibaJ", "grPlanktonAshibaJ");
        leaf = &m_leaf[9];
        leaf->unk00 = -0.3f;
        leaf->m_angle = data->unk08;
        leaf->m_flip = 1;
        leaf->m_index = 9;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->setLeafData(leaf);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

void stPlankton::createObjAshibaLeft() {
    grPlanktonAshibaLeft* ground = grPlanktonAshibaLeft::create(8, "StgPlanktonAshibaLeft", "grPlanktonAshibaLeft");
    if (ground != NULL) {
        char nodeName[140];
        strcpy(nodeName, "AshibaLeft_Loc");
        addGround(ground);
        ground->setTgtNodeTop(nodeName);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->initAshibaData();
    }
}

void stPlankton::createObjHanenbou() {
    for (u8 i = 0; i != 8; i++) {
        grPlanktonHanenbou* ground = grPlanktonHanenbou::create(1, "kao", "grPlanktonHanenbou");
        if (ground == NULL) {
            break;
        }
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(m_posLimit);
        ground->setLeafData(m_leaf);
        ground->setHanenbouData(&m_hanenbou[i]);
    }
}

void stPlankton::createObjHamon() {
    for (u8 i = 0; i != 16; i++) {
        grPlanktonHamon* ground = grPlanktonHamon::create(11, "StgPlanktonHamon", "grPlanktonHamon");
        if (ground == NULL) {
            break;
        }
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setHamonData(&m_hamon[i]);
    }
}

void stPlankton::createObjWater() {
}

void stPlankton::update(float deltaFrame) {
    switch (m_intro) {
    case 0: {
        g_ecMgr->setDrawPrio(1);
        Vec3f pos(0.0f, 0.0f, -15.0f);
        g_ecMgr->setEffect(static_cast<EfID>(0x560001), &pos);
        g_ecMgr->setDrawPrio(-1);
        g_sndSystem->playSE(snd_se_stage_plankton_start, 0, 0, 0, -1);
        m_intro = 1;
        break;
    }
    case 1:
        break;
    }
    updateLimit();
    updateAshibaLeft();
    updateHanenbou(deltaFrame);
    updateWater(deltaFrame);
}

// The camera's right and top limit.
void stPlankton::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    float top = camera->unk164;
    m_posLimitNow.m_x = camera->unk15C;
    m_posLimitNow.m_y = top;
    m_posLimitNow.m_z = 0.0f;
}

void stPlankton::updateAshibaLeft() {
    Ground* ground = getGround(2);
    if (ground != NULL) {
        ground->getNodePosition(&m_posAshibaLeft[0], 0, "AshibaLeft_Loc");
        ground->getNodePosition(&m_posAshibaLeft[1], 0, "StgPlanktonAshibaLeft");
    }
}

// A group of leaves falls from time to time; the stage data file holds the timing of every group.
void stPlankton::updateHanenbou(float deltaFrame) {
    stPlanktonData* data = static_cast<stPlanktonData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_hanenbouWait = m_hanenbouWait - deltaFrame;
    if (m_hanenbouWait < 0.0f) {
        m_hanenbouWait = 0.0f;
    }
    switch (m_hanenbouState) {
    case 2:
        break;
    case 0:
        m_hanenbouWait = data->unk58;
        m_hanenbouTimer = data->unk60;
        m_hanenbouState = 1;
        // fall through
    case 1:
        m_hanenbouTimer = m_hanenbouTimer - deltaFrame;
        if (m_hanenbouTimer < 0.0f) {
            m_hanenbouTimer = 0.0f;
        }
        if (m_hanenbouTimer == 0.0f && randf() < data->unk64) {
            u32 count = m_container->m_filedata->m_numFiles;
            u32 kind = (int)((float)count * randf());
            m_hanenbouKind = kind;
            stPlanktonHanenbouPattern* pattern = reinterpret_cast<stPlanktonHanenbouPattern*>(m_container->getData(kind & 0xFF));
            if (pattern == NULL) {
                m_hanenbouTimer = data->unk60;
            } else {
                u8 low = data->unk6C;
                float rate = randf();
                u8 high = data->unk6D;
                m_hanenbouState = 3;
                m_hanenbouStep = 0;
                m_hanenbouCount = (int)((float)low + (float)((int)high - (int)low) * rate);
            }
        } else {
            if (m_hanenbouWait == 0.0f) {
                makeHanenbou(data->unk4C);
                m_hanenbouWait = data->unk58;
            }
        }
        break;
    case 4:
        m_hanenbouTimer = m_hanenbouTimer - deltaFrame;
        if (m_hanenbouTimer < 0.0f) {
            m_hanenbouTimer = 0.0f;
        }
        if (m_hanenbouTimer == 0.0f) {
            if (m_hanenbouCount == 0) {
                m_hanenbouTimer = data->unk60;
                m_hanenbouState = 1;
            } else {
                m_hanenbouCount = m_hanenbouCount - 1;
                m_hanenbouStep = 0;
                m_hanenbouState = 3;
            }
        }
        break;
    case 3: {
        stPlanktonHanenbouPattern* pattern = reinterpret_cast<stPlanktonHanenbouPattern*>(m_container->getData(m_hanenbouKind));
        m_hanenbouTimer = m_hanenbouTimer + deltaFrame;
        if ((float)pattern->m_frame[m_hanenbouStep] <= m_hanenbouTimer) {
            makeHanenbou(data->unk4C);
            bool end = false;
            u8 step = m_hanenbouStep + 1;
            m_hanenbouStep = step;
            if (step != 0 && pattern->m_frame[step] == 0) {
                end = true;
            }
            if (step == 8) {
                end = true;
            }
            if (end) {
                m_hanenbouTimer = pattern->m_wait;
                m_hanenbouState = 4;
            }
        }
        break;
    }
    }
}

void stPlankton::updateWater(float deltaFrame) {
    if (m_stageData == NULL) {
        return;
    }
    switch (m_waterState) {
    case 0:
        m_waterState = 1;
        // fall through
    case 1:
        m_waterState = 2;
        // fall through
    case 2:
        updateWaterMakeWave(deltaFrame);
        updateWaterCtrlWave(deltaFrame);
        break;
    }
}

// Every ripple of a leaf that was made wakes up a wave and its sound.
void stPlankton::updateWaterMakeWave(float deltaFrame) {
    for (u32 i = 0; i < 8; i++) {
        stPlanktonHanenbou* leaf = &m_hanenbou[i & 0xFF];
        if (leaf->m_state == 1) {
            makeWave(leaf->m_x);
            playSEWater(leaf->m_x);
            leaf->m_state = 2;
        }
    }
}

// The height of every point of the water follows its neighbours (like a string); it settles down by itself.
void stPlankton::updateWaterCtrlWave(float deltaFrame) {
    stPlanktonWave* wave = m_wave;
    for (u32 i = 0; i < 192; i++) {
        float height;
        if (i == 0) {
            height = wave->m_height + (m_wave[i + 1].m_prev + (wave->m_prev + wave->m_prev)) / 3.6f;
        } else if (i == 191) {
            height = wave->m_height + (m_wave[i - 1].m_prev + (wave->m_prev + wave->m_prev)) / 3.6f;
        } else {
            height = wave->m_height + (m_wave[i - 1].m_prev + wave->m_prev + m_wave[i + 1].m_prev) / 3.6f;
        }
        height = height - height / 50.0f;
        wave->m_height = height - wave->m_prev;
        if (height <= 252.0f) {
            wave->m_next = height;
        } else {
            wave->m_next = 252.0f;
        }
        wave++;
    }
    for (int i = 0; i < 192; i++) {
        m_wave[i].m_prev = m_wave[i].m_next;
    }
}

void stPlankton::notifyEventInfoReady() {
}

void stPlankton::notifyEventInfoGo() {
    m_stateWater = 0;
}

// A ripple at the given place (if there is a free one).
void stPlankton::makeHamon(Vec3f* pos, u8 kind) {
    u8 index = getHamonBlankDataIndex();
    if (index != 16) {
        stPlanktonHamon* hamon = &m_hamon[index];
        hamon->m_x = pos->m_x;
        hamon->m_y = pos->m_y;
        hamon->m_state = 0;
        hamon->m_kind = kind;
    }
}

// A leaf of the hanenbou between the places of the left leaf.
void stPlankton::makeHanenbou(float speed) {
    u8 index = getHanenbouBlankDataIndex();
    if (index != 8) {
        Vec3f start;
        Vec3f dir;
        start = m_posAshibaLeft[1];
        dir = m_posAshibaLeft[0] - m_posAshibaLeft[1];
        dir.normalize();
        stPlanktonHanenbou* leaf = &m_hanenbou[index];
        leaf->m_x = start.m_x;
        leaf->m_y = start.m_y;
        leaf->m_prevX = 0.0f;
        leaf->m_prevY = 0.0f;
        leaf->m_speedX = dir.m_x * speed;
        leaf->m_speedY = dir.m_y * speed;
        float angle = nw4r::math::Atan2FIdx(dir.m_y, dir.m_x);
        m_hanenbou[index].m_angle = angle * 1.40625f;
        m_hanenbou[index].m_state = 0;
    }
}

// Pushes a wave into the water at x: five points around it (a tent shape).
void stPlankton::makeWave(float x) {
    int step = 0;
    float width = m_posLimit[1].m_x - m_posLimit[0].m_x;
    float margin = width * 0.0078125f * 32.0f;
    float rate = (margin + (x - m_posLimit[0].m_x)) / (width + margin * 2.0f);
    rate = nw4r::math::FSelect(rate - 0.0f, rate, 0.0f);
    rate = nw4r::math::FSelect(rate - 1.0f, 1.0f, rate);
    int index = (int)(rate * 192.0f) - 2;
    stPlanktonWave* wave = &m_wave[index];
    for (int n = 5; n != 0; n--) {
        if (index >= 0 && index < 192) {
            wave->m_prev = (3.0f - (float)fabs((float)step - 2.0f)) * 0.9f * 40.0f;
        }
        wave++;
        index++;
        step++;
    }
}

u8 stPlankton::getHamonBlankDataIndex() {
    u8 i;
    for (i = 0; i != 16; i++) {
        if (m_hamon[i].m_state == 3) {
            break;
        }
    }
    return i;
}

u8 stPlankton::getHanenbouBlankDataIndex() {
    u8 i;
    for (i = 0; i != 8; i++) {
        if (m_hanenbou[i].m_state == 3) {
            break;
        }
    }
    return i;
}

// The sound of the water depends on where along the water it is (eight sounds).
void stPlankton::playSEWater(float x) {
    float rate = (x - m_posLimit[0].m_x) / (m_posLimit[1].m_x - m_posLimit[0].m_x);
    if (rate < 0.0f) {
        rate = 0.0f;
    }
    if (rate > 1.0f) {
        rate = 1.0f;
    }
    SndID id;
    if (rate < 0.125f) {
        id = snd_se_stage_Plankton_01;
    } else if (rate < 0.25f) {
        id = snd_se_stage_Plankton_02;
    } else if (rate < 0.375f) {
        id = snd_se_stage_Plankton_03;
    } else if (rate < 0.5f) {
        id = snd_se_stage_Plankton_04;
    } else if (rate < 0.625f) {
        id = snd_se_stage_Plankton_05;
    } else if (rate < 0.75f) {
        id = snd_se_stage_Plankton_06;
    } else if (rate < 0.875f) {
        id = snd_se_stage_Plankton_07;
    } else {
        id = snd_se_stage_Plankton_08;
    }
    g_sndSystem->playSE(id, 0, 0, 0, -1);
}

// The event ends when every leaf has been hit often enough.
bool stPlankton::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_event == 0) {
        return false;
    }
    for (u8 i = 0; i < 6; i++) {
        if (m_leaf[i].m_hitCount != 14) {
            return false;
        }
    }
    *eventState = 6;
    *eventDecision = 4;
    return true;
}

GXColor stPlankton::getFinalTechniqColor() {
    u32 packed = 0x14000496;
    return *reinterpret_cast<GXColor*>(&packed);
}
