#include <cm/cm_camera_controller.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/gr_path.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <string.h>
#include <types.h>

#include <st_dolpic/gr_dolpic.h>
#include <st_dolpic/st_dolpic.h>

// grCollision::setJointStatus / setJointAILowPriority (unnamed functions of main)
extern "C" void fn_80110B98(grCollision* collision, int status, int unk);
extern "C" void fn_80110C40(grCollision* collision, int unk);
// sndSystem::setVol / setPitch / setPan / stopSE (unnamed functions of main)
extern "C" void fn_800777DC(sndSystem* system, float volume, int handle, int fadeFrames);
extern "C" void fn_800778CC(sndSystem* system, float pitch, int handle);
extern "C" void fn_8007798C(sndSystem* system, float pan, int handle);
extern "C" void fn_80075F1C(sndSystem* system, int handle, int fadeFrames);
// Matrix helpers of main: place a matrix with a translation / scale
extern "C" void fn_8003F074(float x, float y, float z, Matrix* matrix);
extern "C" void fn_8003E828(Matrix* in, Vec3f* scale, Matrix* out);

// MATCH-ONLY: plain copy of a vertex (the original copies it by words)
struct dolpicVertex {
    u32 m_x;
    u32 m_y;
};

// MATCH-ONLY: a view of the joint's flag word (HYPOTHESIS: byte 2 selects the joint's collision mode).
struct stDolpicJointBits {
    unsigned m_hi : 8;
    unsigned m_mode : 8;
    unsigned m_lo : 16;
};

stClassInfoImpl<Stages::Dolpic, stDolpic> stDolpic::bss_loc_14;

stDolpic* stDolpic::create() {
    return new (Heaps::StageInstance) stDolpic;
}

stDolpic::stDolpic() : stMelee("stDolpic", Stages::Dolpic) {
    m_state = 0;
    m_scene = 0x10;
    m_sceneTimer = 0.0f;
    memset(m_sceneVisit, 0, sizeof(m_sceneVisit));
    m_ashibaRate = 1.0f;
    m_cameraRate = 1.0f;
    m_cameraData = 100;
    unk1F0 = 0;
    m_ashibaLast = 5;
    m_ashibaState[0] = 3;
    m_ashibaState[1] = 3;
    m_ashibaState[2] = 3;
    m_ashibaState[3] = 3;
    m_ashibaState[4] = 3;
    m_seHandle[0] = -1;
    m_seHandle[1] = -1;
    m_seHandle[2] = -1;
    m_seHandle[3] = -1;
    m_seHandle[4] = -1;
    m_seHandle[5] = -1;
    m_seHandle[6] = -1;
    m_seTimer = 0.0f;
    m_seFade = 0.0f;
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    m_trainerIndex = 0x24;
    m_limit[0].m_x = 0.0f;
    m_limit[0].m_y = 0.0f;
    m_limit[0].m_z = 0.0f;
    m_limit[1].m_x = 0.0f;
    m_limit[1].m_y = 0.0f;
    m_limit[1].m_z = 0.0f;
    for (u8 i = 0; i < 15; i++) {
        mtxGimmick(i)->setIdentity();
    }
    m_collision[0] = NULL;
    m_collision[1] = NULL;
    m_collision[2] = NULL;
    m_collision[3] = NULL;
    m_collision[4] = NULL;
    m_collision[5] = NULL;
    m_collision[6] = NULL;
    m_collision[7] = NULL;
    m_collision[8] = NULL;
    m_collision[9] = NULL;
    collisionMtx()->setIdentity();
    m_collisionScene = 0;
    m_collisionScenePrev = 0;
    m_collisionTimer = 0.0f;
    m_scale.m_x = 1.0f;
    m_scale.m_y = 1.0f;
    m_scale.m_z = 1.0f;
    m_pathData = NULL;
    m_waterData = NULL;
    m_trigger = NULL;
}

stDolpic::~stDolpic() {
    if (m_waterData != NULL) {
        delete m_waterData;
    }
    releaseArchive();
}

bool stDolpic::loading() {
    return true;
}

void stDolpic::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x24);
    initPosPokeTrainer(4, 1);
    createObjPathData();
    createObjMainBg();
    createObjWater(1);
    createObjAshiba(2);
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
    createCollision(m_fileData, 3, NULL);
    createCollision(m_fileData, 4, NULL);
    createCollision(m_fileData, 5, NULL);
    createCollision(m_fileData, 6, NULL);
    createCollision(m_fileData, 7, NULL);
    createObjShine();
    createObjBell(0xE);
    createObjBell(0xF);
    createObjKamome(0x10);
    createObjKamome(0x11);
    createObjKamome(0x12);
    createObjKamome(0x13);
    createObjSwimArea();
    initCameraParam();

    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 30);
}

// The paths the plaza's boats follow.
void stDolpic::createObjPathData() {
    grFixedPathCollection* pathData = static_cast<grFixedPathCollection*>(m_fileData->getData(Data_Type_Misc, 0x28, 0xFFFE));
    if (pathData != NULL) {
        if (pathData != NULL) {
            pathData->relocation();
        }
        m_pathData = pathData;
    }
}

void stDolpic::createObjMainBg() {
    grDolpicMainBg* ground = grDolpicMainBg::create(0, "StgDolpic_Chikei", "grDolpicMainBg");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setMtxGimmickWork(mtxGimmick(0));
        ground->setScaleWork(&m_scale);
        nw4r::g3d::ScnMdl* model = *ground->m_sceneModels;
        if (model != NULL) {
            model->G3dProc(1, 0, 0);
        }
        m_collision[1] = createCollision(m_fileData, 0x32, ground);
        m_collision[2] = createCollision(m_fileData, 0x33, ground);
        m_collision[3] = createCollision(m_fileData, 0x34, ground);
        m_collision[4] = createCollision(m_fileData, 0x35, ground);
        m_collision[5] = createCollision(m_fileData, 0x36, ground);
        m_collision[6] = createCollision(m_fileData, 0x37, ground);
        m_collision[7] = createCollision(m_fileData, 0x38, ground);
        m_collision[8] = createCollision(m_fileData, 0x39, ground);
        m_collision[9] = createCollision(m_fileData, 0x3A, ground);
    }
}

void stDolpic::createObjWater(int index) {
    grDolpicWater* ground;
    switch (index) {
    case 1:
        ground = grDolpicWater::create(0x32, "StgDolpic_Suimen", "grDolpicWater");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtxGimmick(14));
    }
}

void stDolpic::createObjAshiba(int index) {
    grDolpicAshiba* ground;
    u8* state;
    u8 type;
    switch (index) {
    case 2:
        ground = grDolpicAshiba::create(1, "StgDolpicAshiba_Dodai1", "grDolpicAshiba");
        state = &m_ashibaState[0];
        type = 2;
        break;
    case 3:
        ground = grDolpicAshiba::create(0xC, "StgDolpicAshibaA_ashibaA2", "grDolpicAshibaA2");
        state = &m_ashibaState[1];
        type = 3;
        break;
    case 4:
        ground = grDolpicAshiba::create(0xD, "StgDolpicAshibaA_ashibaA3", "grDolpicAshibaA3");
        state = &m_ashibaState[1];
        type = 4;
        break;
    case 5:
        ground = grDolpicAshiba::create(0x16, "StgDolpicAshibaB_ashibaB2", "grDolpicAshibaB2");
        state = &m_ashibaState[2];
        type = 5;
        break;
    case 6:
        ground = grDolpicAshiba::create(0x17, "StgDolpicAshibaB_ashibaB3", "grDolpicAshibaB3");
        state = &m_ashibaState[2];
        type = 6;
        break;
    case 7:
        ground = grDolpicAshiba::create(0x20, "StgDolpicAshibaC_ashibaC2", "grDolpicAshibaC2");
        state = &m_ashibaState[3];
        type = 7;
        break;
    case 8:
        ground = grDolpicAshiba::create(0x21, "StgDolpicAshibaC_ashibaC3", "grDolpicAshibaC3");
        state = &m_ashibaState[3];
        type = 8;
        break;
    case 9:
        ground = grDolpicAshiba::create(0x22, "StgDolpicAshibaC_ashibaC4", "grDolpicAshibaC4");
        state = &m_ashibaState[3];
        type = 9;
        break;
    case 10:
        ground = grDolpicAshiba::create(0x2A, "StgDolpicAshibaD_ashibaD2", "grDolpicAshibaD2");
        state = &m_ashibaState[4];
        type = 10;
        break;
    case 11:
        ground = grDolpicAshiba::create(0x2B, "StgDolpicAshibaD_ashibaD3", "grDolpicAshibaD3");
        state = &m_ashibaState[4];
        type = 11;
        break;
    case 12:
        ground = grDolpicAshiba::create(0x2C, "StgDolpicAshibaD_ashibaD4", "grDolpicAshibaD4");
        state = &m_ashibaState[4];
        type = 12;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setRatePosWork(&m_ashibaRate);
        ground->setStateWork(state);
        ground->setType(type);
        m_ashibaState[0] = 1;
        m_ashibaState[1] = 1;
    }
}

void stDolpic::createObjShine() {
    grDolpicShine* ground = grDolpicShine::create(4, "StgDolpic_Shine", "grDolpicShine");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtxGimmick(13));
    }
}

void stDolpic::createObjBell(int index) {
    grDolpicBell* ground;
    Matrix* mtx;
    u8 type;
    switch (index) {
    case 0xE:
        ground = grDolpicBell::create(5, "StgDolpic_Bell1", "grDolpicBell1");
        mtx = mtxGimmick(11);
        type = 0;
        break;
    case 0xF:
        ground = grDolpicBell::create(6, "StgDolpic_Bell2", "grDolpicBell2");
        mtx = mtxGimmick(12);
        type = 1;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtx);
        ground->setType(type);
    }
}

void stDolpic::createObjKamome(int index) {
    grDolpicKamome* ground;
    switch (index) {
    case 0x10:
        ground = grDolpicKamome::create(7, "StgDolpicKamome_KamomeA", "grDolpicKamomeA");
        break;
    case 0x11:
        ground = grDolpicKamome::create(8, "StgDolpicKamome_KamomeB", "grDolpicKamomeB");
        break;
    case 0x12:
        ground = grDolpicKamome::create(9, "StgDolpicKamome_KamomeC", "grDolpicKamomeC");
        break;
    case 0x13:
        ground = grDolpicKamome::create(10, "StgDolpicKamome_KamomeD", "grDolpicKamomeD");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(mtxGimmick(10));
    }
}

// The area of the water: its depth and extent are set every frame (updateWater).
void stDolpic::createObjSwimArea() {
    m_waterData = new (Heaps::StageInstance) grGimmickWaterData;
    if (m_waterData != NULL) {
        memset(m_waterData, 0, sizeof(grGimmickWaterData));
        m_waterData->m_swimHeight = 0.0f;
        m_waterData->m_canDrown = true;
        m_waterData->m_speed = 1.0f;
        grGimmickWaterData* area = m_waterData;
        area->m_areaData.m_offsetPos.m_x = 0.0f;
        area->m_areaData.m_offsetPos.m_y = -25.0f;
        area->m_areaData.m_range.m_x = 500.0f;
        area->m_areaData.m_range.m_y = 25.0f;
        m_trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Water, -1);
        m_trigger->setWaterTrigger(m_waterData);
        m_trigger->setAreaSleep(true);
    }
}

void stDolpic::update(float deltaFrame) {
    updateLimit(deltaFrame);
    updateScene(deltaFrame);
    updateAshiba(deltaFrame);
    updateSE(deltaFrame);
    updateCollision(deltaFrame);
    updateWater(deltaFrame);
    updateCameraCenter(deltaFrame);
    updateTrainer(deltaFrame);
}

// Takes over the limits of the camera.
static inline void setLimitPoint(Vec3f* dst, float x, float y) {
    dst->m_x = x;
    dst->m_y = y;
    dst->m_z = 0.0f;
}

void stDolpic::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    setLimitPoint(&m_limit[0], camera->unk158, camera->unk160);
    setLimitPoint(&m_limit[1], camera->unk15C, camera->unk164);
}

// The scene script: the plaza waits (even states), then the stage picks the next scene, loads it and starts the plaza's
// animation for it (odd states).
void stDolpic::updateScene(float deltaFrame) {
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data == NULL) {
        return;
    }
    grDolpicMainBg* plaza = static_cast<grDolpicMainBg*>(getGround(0));
    if (plaza == NULL) {
        return;
    }
    m_sceneTimer -= deltaFrame;
    if (m_sceneTimer < 0.0f) {
        m_sceneTimer = 0.0f;
    }
    switch (m_state) {
    case 0:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(0, 0);
        }
        break;
    case 1:
        m_sceneVisit[0] = 2;
        setScene(0, 0);
        plaza->setMotion(0, false, false, NULL);
        m_state = 2;
        setStateGroundCollision(1, 1);
        break;
    case 2:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(1, 0);
        }
        break;
    case 3:
        m_sceneVisit[0] = 3;
        setScene(1, 0);
        plaza->setMotion(1, false, false, NULL);
        m_state = 4;
        setStateGroundCollision(2, 1);
        break;
    case 4:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(2, 0);
        }
        break;
    case 5:
        m_sceneVisit[1] = 4;
        setScene(2, 0);
        plaza->setMotion(2, false, false, NULL);
        m_state = 6;
        setStateGroundCollision(3, 1);
        break;
    case 6:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(3, 0);
        }
        break;
    case 7:
        m_sceneVisit[1] = 5;
        setScene(3, 0);
        plaza->setMotion(3, false, false, NULL);
        m_state = 8;
        setStateGroundCollision(4, 1);
        break;
    case 8:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(4, 0);
        }
        break;
    case 9:
        setScene(4, 0);
        plaza->setMotion(4, false, false, NULL);
        m_sceneVisit[1] = 5;
        m_state = 10;
        setStateGroundCollision(4, 1);
        break;
    case 10:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(4, 0);
        }
        break;
    case 11:
        setScene(5, 0);
        plaza->setMotion(5, false, false, NULL);
        m_sceneVisit[1] = 6;
        m_state = 12;
        setStateGroundCollision(5, 1);
        break;
    case 12:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(5, 0);
        }
        break;
    case 13:
        m_sceneVisit[2] = 7;
        setScene(6, 0);
        plaza->setMotion(6, false, false, NULL);
        m_state = 14;
        setStateGroundCollision(6, 1);
        break;
    case 14:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(6, 0);
        }
        break;
    case 15:
        m_sceneVisit[2] = 7;
        setScene(7, 0);
        plaza->setMotion(7, false, false, NULL);
        m_state = 16;
        setStateGroundCollision(6, 1);
        break;
    case 16:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(6, 0);
        }
        break;
    case 17:
        m_sceneVisit[2] = 8;
        setScene(8, 0);
        plaza->setMotion(8, false, false, NULL);
        m_state = 18;
        setStateGroundCollision(7, 1);
        break;
    case 18:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(7, 0);
        }
        break;
    case 19:
        m_sceneVisit[2] = 8;
        setScene(9, 0);
        plaza->setMotion(9, false, false, NULL);
        m_state = 20;
        setStateGroundCollision(7, 1);
        break;
    case 20:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(7, 0);
        }
        break;
    case 21:
        m_sceneVisit[3] = 9;
        setScene(10, 0);
        plaza->setMotion(10, false, false, NULL);
        m_state = 22;
        setStateGroundCollision(8, 1);
        break;
    case 22:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(8, 0);
        }
        break;
    case 23:
        m_sceneVisit[3] = 10;
        setScene(11, 0);
        plaza->setMotion(11, false, false, NULL);
        m_state = 24;
        setStateGroundCollision(9, 1);
        break;
    case 24:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(9, 0);
        }
        break;
    case 25:
        m_sceneVisit[3] = 9;
        setScene(12, 0);
        plaza->setMotion(12, false, false, NULL);
        m_state = 26;
        setStateGroundCollision(8, 1);
        break;
    case 26:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(8, 0);
        }
        break;
    case 27:
        m_sceneVisit[3] = 10;
        setScene(13, 0);
        plaza->setMotion(13, false, false, NULL);
        m_state = 28;
        setStateGroundCollision(9, 1);
        break;
    case 28:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(9, 0);
        }
        break;
    case 29:
        setScene(14, 0);
        plaza->setMotion(14, false, false, NULL);
        m_state = 30;
        break;
    case 30:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(0, 0);
        }
        break;
    case 31:
        setScene(15, 0);
        plaza->setMotion(15, false, false, NULL);
        m_state = 32;
        break;
    case 32:
        if (m_sceneTimer == 0.0f) {
            m_sceneTimer = data->m_waitTime + data->m_baseTime;
            updateSceneSelect(deltaFrame);
            setStateGroundCollision(0, 0);
        }
        break;
    }
}

// Picks the scene that follows the one that is waiting (a coin toss that favours the other variant than the last one).
void stDolpic::updateSceneSelect(float deltaFrame) {
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data != NULL && *(int*)((u8*)g_GameGlobal + 8) != 0) { // HYPOTHESIS: g_GameGlobal+8 is the match setup
        float rnd = randf();
        switch (m_state) {
        case 0:
            if (rnd < 0.5f) {
                m_state = 1;
            } else {
                m_state = 3;
            }
            break;
        case 2:
            switch (m_sceneVisit[1]) {
            case 4:
                if (rnd < data->m_chance) {
                    m_state = 5;
                } else {
                    m_state = 7;
                }
                break;
            case 5:
                if (rnd < data->m_chance) {
                    m_state = 7;
                } else {
                    m_state = 5;
                }
                break;
            default:
                if (rnd < 0.5f) {
                    m_state = 5;
                } else {
                    m_state = 7;
                }
                break;
            }
            break;
        case 4:
            switch (m_sceneVisit[1]) {
            case 5:
                if (rnd < data->m_chance) {
                    m_state = 9;
                } else {
                    m_state = 0xB;
                }
                break;
            case 6:
                if (rnd < data->m_chance) {
                    m_state = 0xB;
                } else {
                    m_state = 9;
                }
                break;
            default:
                if (rnd < 0.5f) {
                    m_state = 9;
                } else {
                    m_state = 0xB;
                }
                break;
            }
            break;
        case 6:
            m_state = 0xD;
            break;
        case 8:
        case 10:
            switch (m_sceneVisit[2]) {
            case 7:
                if (rnd < data->m_chance) {
                    m_state = 0xF;
                } else {
                    m_state = 0x11;
                }
                break;
            case 8:
                if (rnd < data->m_chance) {
                    m_state = 0x11;
                } else {
                    m_state = 0xF;
                }
                break;
            default:
                if (rnd < 0.5f) {
                    m_state = 0xF;
                } else {
                    m_state = 0x11;
                }
                break;
            }
            break;
        case 0xC:
            m_state = 0x13;
            break;
        case 0xE:
        case 0x10:
            switch (m_sceneVisit[3]) {
            case 9:
                if (rnd < data->m_chance) {
                    m_state = 0x15;
                } else {
                    m_state = 0x17;
                }
                break;
            case 10:
                if (rnd < data->m_chance) {
                    m_state = 0x17;
                } else {
                    m_state = 0x15;
                }
                break;
            default:
                if (rnd < 0.5f) {
                    m_state = 0x15;
                } else {
                    m_state = 0x17;
                }
                break;
            }
            break;
        case 0x12:
        case 0x14:
            switch (m_sceneVisit[3]) {
            case 9:
                if (rnd < data->m_chance) {
                    m_state = 0x19;
                } else {
                    m_state = 0x1B;
                }
                break;
            case 10:
                if (rnd < data->m_chance) {
                    m_state = 0x1B;
                } else {
                    m_state = 0x19;
                }
                break;
            default:
                if (rnd < 0.5f) {
                    m_state = 0x19;
                } else {
                    m_state = 0x1B;
                }
                break;
            }
            break;
        case 0x16:
        case 0x1A:
            m_state = 0x1D;
            break;
        case 0x18:
        case 0x1C:
            m_state = 0x1F;
            break;
        case 0x1E:
        case 0x20:
            switch (m_sceneVisit[0]) {
            case 2:
                if (rnd < data->m_chance) {
                    m_state = 1;
                } else {
                    m_state = 3;
                }
                break;
            case 3:
                if (rnd < data->m_chance) {
                    m_state = 3;
                } else {
                    m_state = 1;
                }
                break;
            default:
                if (rnd < 0.5f) {
                    m_state = 1;
                } else {
                    m_state = 3;
                }
                break;
            }
            break;
        }
    }
}

// While a scene waits, its platforms move: which group moves depends on the state.
void stDolpic::updateAshiba(float deltaFrame) {
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 2:
        updateAshiba1(data->m_ashiba[0], 0, 1);
        break;
    case 4:
        updateAshiba1(data->m_ashiba[0], 0, 1);
        break;
    case 6:
        updateAshiba1(data->m_ashiba[1], 1, 1);
        break;
    case 8:
        updateAshiba1(data->m_ashiba[1], 1, 1);
        break;
    case 10:
        updateAshiba1(data->m_ashiba[2], 1, 1);
        break;
    case 12:
        updateAshiba1(data->m_ashiba[2], 1, 1);
        break;
    case 14:
        updateAshiba1(data->m_ashiba[3], 1, 1);
        break;
    case 16:
        updateAshiba1(data->m_ashiba[4], 1, 1);
        break;
    case 18:
        updateAshiba1(data->m_ashiba[4], 1, 1);
        break;
    case 20:
        updateAshiba1(data->m_ashiba[5], 1, 1);
        break;
    case 22:
        updateAshiba1(data->m_ashiba[6], 1, 1);
        break;
    case 24:
        updateAshiba1(data->m_ashiba[6], 1, 1);
        break;
    case 26:
        updateAshiba1(data->m_ashiba[7], 1, 1);
        break;
    case 28:
        updateAshiba1(data->m_ashiba[7], 1, 1);
        break;
    case 30:
        updateAshiba1(data->m_ashiba[8], 1, 0);
        break;
    case 32:
        updateAshiba1(data->m_ashiba[9], 1, 0);
        break;
    }
}

void stDolpic::updateAshiba1(u8 group, u32 sinking, u32 unk) {
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data == NULL) {
        return;
    }
    if (m_ashibaLast != 5) {
        group = m_ashibaLast;
    }
    float base = data->m_baseTime;
    float fade = data->m_fadeTime;
    float timer = m_sceneTimer;
    float fadeStart = (base + data->m_waitTime) - fade;
    u32 phase;
    float rate;
    if (timer > fadeStart) {
        phase = 1;
        rate = 1.0f - (timer - fadeStart) / fade;
    } else if (timer > base + fade) {
        rate = 1.0f;
        phase = 2;
    } else if (timer > base) {
        phase = 3;
        rate = 1.0f - (timer - base) / fade;
    } else {
        phase = 4;
        rate = 0.0f;
    }
    if (rate < 0.0f) {
        rate = 0.0f;
    }
    if (rate > 1.0f) {
        rate = 1.0f;
    }
    float eased = nw4r::math::SinIdx((u16)(int)(rate * 16384.0f));
    switch (phase) {
    case 0:
        break;
    case 1:
        if (sinking == 1) {
            m_ashibaState[0] = 0;
            m_ashibaState[group] = 0;
            setSEVolAshibaSet(eased, group);
        } else {
            setSEVolAshibaSet(1.0f, group);
        }
        m_ashibaLast = group;
        break;
    case 2: {
        grDolpicMainBg* plaza = static_cast<grDolpicMainBg*>(getGround(0));
        if (plaza != NULL) {
            switch (m_state) {
            case 0xA:
            case 0xC:
                plaza->setNodeVisibility(true, 0, "StgDolpic_HideAREA03", false, false);
                break;
            case 0x1A:
            case 0x1C:
                plaza->setNodeVisibility(true, 0, "StgDolpic_HideAREA08", false, false);
                resetCameraLimitRange();
                break;
            case 0x20:
                plaza->setNodeVisibility(true, 0, "StgDolpic_HideAREA10", false, false);
                break;
            }
            m_ashibaLast = group;
            setSEVolAshibaSet(eased, group);
        }
        break;
    }
    case 3:
        if (unk == 1) {
            m_ashibaState[0] = 2;
            m_ashibaState[group] = 2;
            setSEVolAshibaSet(1.0f - eased, group);
        } else {
            setSEVolAshibaSet(1.0f, group);
        }
        {
            grDolpicMainBg* plaza = static_cast<grDolpicMainBg*>(getGround(0));
            if (plaza != NULL) {
                switch (m_state) {
                case 4:
                    plaza->setNodeVisibility(false, 0, "StgDolpic_HideAREA03", false, false);
                    break;
                case 0x12:
                case 0x14: {
                    plaza->setNodeVisibility(false, 0, "StgDolpic_HideAREA08", false, false);
                    float down;
                    CameraController* camera = CameraController::getInstance();
                    cmStageParam* param = &camera->m_stageCameraParam;
                    if (param != NULL) {
                        float cosine = nw4r::math::CosFIdx(10.666667f);
                        float sine = nw4r::math::SinFIdx(10.666667f);
                        down = -36.0f - param->m_verticalRotationFactor * (sine / cosine) * 0.25f;
                    }
                    setCameraLimitRange(-90.0f, 160.0f, 90.0f, down);
                    break;
                }
                case 0x18:
                case 0x1C:
                    plaza->setNodeVisibility(false, 0, "StgDolpic_HideAREA10", false, false);
                    break;
                }
            }
        }
        break;
    case 4:
        if (unk == 1) {
            setSEVolAshibaSet(0.0f, group);
            m_ashibaLast = 5;
        }
        break;
    }
}

static inline float dolpicClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// Vec3f::length() inlined (the original inlines it here)
static inline float dolpicLength(const Vec3f& v) {
    float lengthSq = v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

// The sound points of the plaza: every point plays its sound when the plaza's node is near and fades or stops it when it
// leaves. A few ambient sounds are played at random, and the sound of the ferry's wake is tied to the scene.
void stDolpic::updateSE(float deltaFrame) {
    Ground* plaza;
    Vec3f pos;
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data == NULL) {
        return;
    }
    plaza = getGround(0);
    if (plaza == NULL) {
        return;
    }

    plaza->getNodePosition(&pos, 0, "StgDolpic_SEPoint2");
    m_snd0.setPos(&pos);
    if (m_seHandle[0] < 0) {
        if (dolpicLength(pos) < 400.0f) {
            m_seHandle[0] = m_snd0.playSE(static_cast<SndID>(0x1B54), 0, 0x78, -1);
        }
    } else if (dolpicLength(pos) > 400.0f) {
        m_snd4.stopSE(m_seHandle[0], 0xB4);
        m_seHandle[0] = -1;
    } else {
        float volume = 0.5f + 0.5f * (dolpicLength(pos) / 200.0f);
        volume = dolpicClamp(volume, 0.0f, 1.0f);
        switch (m_state) {
        case 0x18:
        case 0x1C:
        case 0x20:
            if (m_ashibaState[0] == 0) {
                m_seFade += 0.02f;
            } else if (m_ashibaState[0] == 2) {
                m_seFade -= 0.02f;
            } else {
                m_seFade = 0.0f;
            }
            volume += m_seFade;
            volume = dolpicClamp(volume, 0.125f, 0.5f);
            break;
        }
        fn_800777DC(g_sndSystem, volume, m_seHandle[0], 0);
    }

    plaza->getNodePosition(&pos, 0, "StgDolpic_SEPoint3");
    m_snd1.setPos(&pos);
    if (m_seHandle[1] < 0) {
        if (dolpicLength(pos) < 400.0f) {
            m_seHandle[1] = m_snd1.playSE(static_cast<SndID>(0x1B55), 0, 0x78, -1);
        }
    } else if (dolpicLength(pos) > 400.0f) {
        m_snd4.stopSE(m_seHandle[1], 0xB4);
        m_seHandle[1] = -1;
    } else {
        float volume = 0.5f + 0.5f * (dolpicLength(pos) / 100.0f);
        volume = dolpicClamp(volume, 0.0f, 1.0f);
        switch (m_state) {
        case 0x18:
        case 0x1C:
        case 0x20:
            volume = 0.125f;
            break;
        }
        fn_800777DC(g_sndSystem, volume, m_seHandle[1], 0);
    }

    plaza->getNodePosition(&pos, 0, "StgDolpic_SEPoint1");
    m_snd2.setPos(&pos);
    if (m_seHandle[2] < 0) {
        if (dolpicLength(pos) < 400.0f) {
            m_seHandle[2] = m_snd2.playSE(static_cast<SndID>(0x1B56), 0, 0x78, -1);
        }
    } else if (dolpicLength(pos) > 400.0f) {
        m_snd4.stopSE(m_seHandle[2], 0xB4);
        m_seHandle[2] = -1;
    }

    plaza->getNodePosition(&pos, 0, "StgDolpic_SEPoint4");
    m_snd3.setPos(&pos);
    if (m_seHandle[3] < 0) {
        if (dolpicLength(pos) < 400.0f) {
            m_seHandle[3] = m_snd3.playSE(static_cast<SndID>(0x1B56), 0, 0x78, -1);
        }
    } else if (dolpicLength(pos) > 400.0f) {
        m_snd4.stopSE(m_seHandle[3], 0xB4);
        m_seHandle[3] = -1;
    }

    plaza->getNodePosition(&pos, 0, "StgDolpic_ShinePosition");
    m_snd4.setPos(&pos);
    if (m_seHandle[4] < 0) {
        if (dolpicLength(pos) < 400.0f) {
            m_seHandle[4] = m_snd4.playSE(static_cast<SndID>(6999), 0, 0x78, -1);
        }
    } else if (dolpicLength(pos) > 400.0f) {
        m_snd4.stopSE(m_seHandle[4], 0xB4);
        m_seHandle[4] = -1;
    }

    m_seTimer -= deltaFrame;
    if (m_seTimer < 0.0f) {
        m_seTimer = 0.0f;
    }
    if (m_seTimer == 0.0f) {
        float pan = -0.75f + 1.5f * randf();
        float volume = 0.05f + 0.15f * randf();
        bool west;
        switch (m_state) {
        case 2:
        case 0xC:
            if (m_sceneTimer > data->m_baseTime + data->m_fadeTime) {
                west = false;
            } else {
                west = true;
            }
            break;
        case 6:
        case 8:
        case 0x14:
            if (m_sceneTimer > data->m_baseTime + data->m_fadeTime) {
                west = true;
            } else {
                west = false;
            }
            break;
        default:
            west = false;
            break;
        }
        SndID id;
        float rnd = randf();
        // HYPOTHESIS: the thresholds of the variants are all the same (the compiler folds the comparisons)
        if (!west) {
            if (rnd < 0.25f) {
                id = static_cast<SndID>(0x1B58);
            } else if (rnd < 0.25f) {
                id = static_cast<SndID>(0x1B59);
            } else if (rnd < 0.25f) {
                id = static_cast<SndID>(0x1B5A);
            } else {
                id = static_cast<SndID>(0x1B5B);
            }
        } else if (rnd < 0.25f) {
            id = static_cast<SndID>(0x1B5C);
        } else if (rnd < 0.25f) {
            id = static_cast<SndID>(0x1B5D);
        } else if (rnd < 0.25f) {
            id = static_cast<SndID>(0x1B5E);
        } else {
            id = static_cast<SndID>(0x1B5F);
        }
        int handle = g_sndSystem->playSE(id, 0, 0, 0, -1);
        if (0 < handle) {
            fn_8007798C(g_sndSystem, pan, handle);
            fn_800777DC(g_sndSystem, volume, handle, 0x78);
        }
        float time = (5.0f + 5.0f * randf()) * 60.0f;
        m_seTimer = time;
        switch (m_state) {
        case 2:
        case 4:
        case 6:
        case 8:
        case 0xA:
        case 0xC:
        case 0x20:
            m_seTimer = time * 0.5f;
            break;
        }
    }

    float base = data->m_baseTime;
    float timer = m_sceneTimer;
    if (!(timer > (base + data->m_waitTime) - data->m_fadeTime)) {
        if (timer > base + data->m_fadeTime) {
            if (m_seHandle[5] < 0) {
                m_seHandle[5] = g_sndSystem->playSE(static_cast<SndID>(0x1B52), 0, 0x78, 0, -1);
            }
        } else if (timer > base) {
            if (m_seHandle[5] >= 0) {
                fn_80075F1C(g_sndSystem, m_seHandle[5], 0xB4);
                m_seHandle[5] = -1;
            }
        }
    }
}

// Switches the collision of the scenes on and off: the current scene's collision stays on, the previous one is turned
// off after a short time.
void stDolpic::updateCollision(float deltaFrame) {
    m_collisionTimer -= deltaFrame;
    if (m_collisionTimer < 0.0f) {
        m_collisionTimer = 0.0f;
    }
    if (m_collisionScene < 10) {
        bool flag = false;
        if (m_collisionScenePrev != 0) {
            if (m_collisionTimer == 0.0f) {
                if (m_collision[m_collisionScenePrev] != NULL) {
                    m_collision[m_collisionScenePrev]->setDisable();
                    fn_80110C40(m_collision[m_collisionScenePrev], 0);
                }
                if (m_collision[m_collisionScene] != NULL) {
                    m_collision[m_collisionScene]->setEnable();
                }
                m_collisionScenePrev = 0;
            } else {
                flag = true;
            }
        }
        updateCollision1(deltaFrame, m_collisionScene, 0);
        updateCollision1(deltaFrame, m_collisionScenePrev, flag);
    }
}

// Places the collision of a scene with the matrix of the water line.
void stDolpic::updateCollision1(float deltaFrame, u32 scene, bool enable) {
    if (scene != 0 && scene < 10 && m_collision[(u8)scene] != NULL) {
        grCollision* collision = m_collision[(u8)scene];
        Vec3f translation;
        mtxGimmick(scene)->getPosition(&translation);
        float length = dolpicLength(translation);
        u16 count = collision->m_jointLen;
        u16 i = 0;
        while (i != count) {
            grCollisionJoint* joint = collision->getJoint(i);
            if (joint == NULL) {
                break;
            }
            dolpicVertex vertex;
            float lowered;
            u16 vertexCount = joint->m_vtxLen;
            for (u16 j = 0; j < vertexCount; j++) {
                vertex = *reinterpret_cast<dolpicVertex*>(&joint->m_vtxDatas[j]);
                lowered = *reinterpret_cast<float*>(&vertex.m_y) - length;
            }
            joint->m_0x55_3 = enable;
            collisionMtx()->setIdentity();
            fn_8003F074(0.0f, -length, 0.0f, collisionMtx());
            fn_8003E828(collisionMtx(), &m_scale, collisionMtx());
            joint->m_matrix = collisionMtx();
            i++;
            joint->m_0x54_3 = 1;
            reinterpret_cast<stDolpicJointBits*>(reinterpret_cast<u8*>(joint) + 0x48)->m_mode = 2;
        }
    }
}

// stTrigger::setWaterParam
extern "C" void fn_27_233C70(stTrigger* trigger, grGimmickWaterData* data);

// The water follows the scene's collision: its surface is at the water line of the scene (the distance the plaza places
// the scene's matrix at, plus a fixed offset per scene).
void stDolpic::updateWater(float deltaFrame) {
    if (m_trigger == NULL) {
        return;
    }
    if (m_waterData == NULL) {
        return;
    }
    if (m_stageData == NULL) {
        return;
    }
    u32 scene = m_collisionScenePrev;
    if (scene == 0) {
        scene = m_collisionScene;
    }
    if ((u8)scene == 0 || (u8)scene >= 10) {
        m_trigger->setAreaSleep(true);
        return;
    }
    Vec3f translation;
    mtxGimmick((u8)scene)->getPosition(&translation);
    float height = dolpicLength(translation);
    switch ((u8)scene) {
    case 1:
        height += 52.036118f;
        break;
    case 2:
        height += 22.490217f;
        break;
    case 3:
        height += 15.115527f;
        break;
    case 4:
        height += 70.40888f;
        break;
    case 5:
        height += 32.928642f;
        break;
    case 6:
        height += 106.93778f;
        break;
    case 7:
        height += 30.556412f;
        break;
    case 8:
        height += 218.1322f;
        break;
    case 9:
        height += 30.556412f;
        break;
    }
    height = -height;
    m_waterData->m_swimHeight = height;
    m_waterData->m_speed = 0.0f;
    grGimmickWaterData* water = m_waterData;
    water->m_areaData.m_range.m_x = (m_limit[1].m_x - m_limit[0].m_x) + 50.0f;
    float depth = (height - m_limit[1].m_y) + 50.0f;
    water->m_areaData.m_range.m_y = depth;
    depth = nw4r::math::FSelect(depth - 0.1f, depth, 0.1f);
    float room = m_limit[0].m_y - m_limit[1].m_y;
    room = nw4r::math::FSelect(depth - room, room, depth);
    water->m_areaData.m_range.m_y = room;
    water->m_areaData.m_offsetPos.m_x = 0.0f;
    water->m_areaData.m_offsetPos.m_y = height - 0.5f * room;
    fn_27_233C70(m_trigger, m_waterData);
    switch ((u8)scene) {
    case 3:
        water->m_areaData.m_offsetPos.m_x = 0.3125f * -(m_limit[1].m_x - m_limit[0].m_x);
        water->m_areaData.m_range.m_x = 0.2f * (m_limit[1].m_x - m_limit[0].m_x);
        fn_27_233C70(m_trigger, m_waterData);
        // fall through
    case 1:
    case 2:
    case 4:
    case 5:
        switch (m_ashibaState[0]) {
        case 1:
            m_trigger->setAreaSleep(true);
            break;
        default:
            m_trigger->setAreaSleep(false);
            break;
        }
        break;
    default:
        m_trigger->setAreaSleep(true);
        break;
    }
}

// The camera positions of the stage change with the scene (and with how far the platforms have moved).
void stDolpic::updateCameraCenter(float deltaFrame) {
    if (m_stagePositions == NULL) {
        return;
    }
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data == NULL) {
        return;
    }
    u32 id;
    float rate = m_ashibaRate;
    float one = 1.0f;
    if (one == rate) {
        if (one != m_cameraRate) {
            m_cameraRate = rate;
            switch (m_state) {
            case 0xE:
            case 0x16:
            case 0x18:
            case 0x1A:
            case 0x1C:
            case 0x20:
                switch (m_ashibaLast) {
                case 1:
                    id = 100;
                    break;
                case 2:
                    id = 0x79;
                    break;
                case 3:
                    id = 0x7A;
                    break;
                case 4:
                    id = 0x7B;
                    break;
                default:
                    return;
                }
                if (m_cameraData != id) {
                    void* posData = m_fileData->getData(Data_Type_Model, id, 0xFFFE);
                    if (posData != NULL) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                    m_cameraData = id;
                }
                return;
            default:
                return;
            }
        }
        m_cameraRate = rate;
        return;
    }
    m_cameraRate = rate;
    if (m_sceneTimer - data->m_baseTime > 0.5f * data->m_waitTime) {
        switch (m_state) {
        case 0xE:
        case 0x16:
        case 0x18:
        case 0x1A:
        case 0x1C:
        case 0x20:
            return;
        }
        switch (m_ashibaLast) {
        case 1:
            id = 100;
            break;
        case 2:
            id = 0x79;
            break;
        case 3:
            id = 0x7A;
            break;
        case 4:
            id = 0x7B;
            break;
        default:
            return;
        }
    } else {
        switch (m_state) {
        case 0:
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xB:
        case 0xD:
        case 0xF:
        case 0x11:
        case 0x13:
        case 0x15:
        case 0x17:
        case 0x19:
        case 0x1B:
        case 0x1D:
        case 0x1F:
            return;
        case 2:
            id = 0x70;
            break;
        case 4:
            id = 0x71;
            break;
        case 6:
            id = 0x72;
            break;
        case 8:
            id = 0x73;
            break;
        case 10:
            id = 0x73;
            break;
        case 0xC:
            id = 0x74;
            break;
        case 0xE:
            id = 0x75;
            break;
        case 0x10:
            id = 0x75;
            break;
        case 0x12:
            id = 0x76;
            break;
        case 0x14:
            id = 0x76;
            break;
        case 0x16:
            id = 0x77;
            break;
        case 0x18:
            id = 0x78;
            break;
        case 0x1A:
            id = 0x77;
            break;
        case 0x1C:
            id = 0x78;
            break;
        case 0x1E:
            return;
        case 0x20:
        default:
            return;
        }
    }
    if (m_cameraData != id) {
        void* posData = m_fileData->getData(Data_Type_Model, id, 0xFFFE);
        if (posData != NULL) {
            nw4r::g3d::ResFile posFile(posData);
            m_stagePositions->loadPositionData(&posFile);
        }
        updateStagePositions();
        m_cameraData = id;
    }
}

// Hands the Pokemon Trainer the four positions of the group the plaza is at (the group changes with the scene and
// with how long the scene has been running).
void stDolpic::updateTrainer(float deltaFrame) {
    if (!isPokemonTrainer()) {
        return;
    }
    stDolpicParam* data = static_cast<stDolpicParam*>(m_stageData);
    if (data == NULL) {
        return;
    }
    float elapsed = data->m_waitTime - (m_sceneTimer - data->m_baseTime);
    if (elapsed < 0.0f) {
        elapsed = 0.0f;
    }
    switch (m_scene) {
    case 0:
        if (m_trainerIndex == 0x24) {
            m_trainerIndex = 0;
        } else if (elapsed > 1000.0f) {
            m_trainerIndex = 0;
        }
        break;
    case 1:
        if (m_trainerIndex == 0x24) {
            m_trainerIndex = 4;
        } else if (elapsed > 960.0f) {
            m_trainerIndex = 4;
        }
        break;
    case 2:
        if (elapsed > 260.0f) {
            m_trainerIndex = 8;
        }
        break;
    case 3:
        if (elapsed > 440.0f) {
            m_trainerIndex = 0xC;
        }
        break;
    case 4:
        if (elapsed > 700.0f) {
            m_trainerIndex = 0xC;
        }
        break;
    case 5:
        if (elapsed > 280.0f) {
            m_trainerIndex = 0x10;
        }
        break;
    case 6:
        if (elapsed > 900.0f) {
            m_trainerIndex = 0x14;
        }
        break;
    case 7:
        if (elapsed > 940.0f) {
            m_trainerIndex = 0x14;
        }
        break;
    case 8:
        if (elapsed > 900.0f) {
            m_trainerIndex = 0x18;
        }
        break;
    case 9:
        if (elapsed > 260.0f) {
            m_trainerIndex = 0x18;
        }
        break;
    case 10:
        if (elapsed > 940.0f) {
            m_trainerIndex = 0x1C;
        } else if (elapsed > 240.0f) {
            m_trainerIndex = 0x10;
        }
        break;
    case 0xB:
        if (elapsed > 930.0f) {
            m_trainerIndex = 0x20;
        } else if (elapsed > 230.0f) {
            m_trainerIndex = 0x10;
        }
        break;
    case 0xC:
        if (elapsed > 960.0f) {
            m_trainerIndex = 0x1C;
        }
        break;
    case 0xD:
        if (elapsed > 930.0f) {
            m_trainerIndex = 0x20;
        }
        break;
    case 0xE:
        if (elapsed > 250.0f) {
            m_trainerIndex = 0x10;
        }
        break;
    case 0xF:
        break;
    default:
        return;
    }
    if (m_trainerIndex != 0x24) {
        m_pokeTrainerPos[0] = m_posGimmick[m_trainerIndex];
        m_pokeTrainerPos[1] = m_posGimmick[m_trainerIndex + 1];
        m_pokeTrainerPos[2] = m_posGimmick[m_trainerIndex + 2];
        m_pokeTrainerPos[3] = m_posGimmick[m_trainerIndex + 3];
    }
}

void stDolpic::renderDebug() { }

// Loads the scene animation of the plaza (the game keeps one scene loaded at a time).
void stDolpic::setScene(u32 scene, int force) {
    if (scene < 0x10 && (m_scene != scene || force != 0)) {
        nw4r::g3d::ResFileData* sceneData = static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE));
        registScnAnim(sceneData, scene);
        m_scene = scene;
    }
}

// Starts or stops the looping sound of the platforms that are moving (volume and pitch follow the platform's rate).
void stDolpic::setSEVolAshibaSet(float volume, int index) {
    if (index < 5 && 0 <= index) {
        int handle = m_seHandle[6];
        if (handle == -1 && volume > 0.0f) {
            m_seHandle[6] = m_snd5.playSE(static_cast<SndID>(0x1B4D), 0, 0, -1);
            Vec3f pos;
            pos.m_x = 0.0f;
            pos.m_y = 0.0f;
            pos.m_z = 0.0f;
            m_snd5.setPos(&pos);
        } else if (handle != -1) {
            fn_800777DC(g_sndSystem, volume, handle, 0);
            fn_800778CC(g_sndSystem, volume, m_seHandle[6]);
            if (volume == 0.0f) {
                m_snd5.stopSE(m_seHandle[6], 0);
                m_seHandle[6] = -1;
            }
        }
    }
}

// Enables the collision of a scene (enable = 1) or schedules it to be turned off (enable = 0); scene 0 switches all of
// them.
void stDolpic::setStateGroundCollision(u32 scene, u32 enable) {
    if (scene < 10) {
        if (scene == 0) {
            u32 i = 0;
            do {
                if (m_collision[(u8)i] != NULL) {
                    fn_80110B98(m_collision[(u8)i], enable, 0);
                    if (enable == 1) {
                        m_collision[(u8)i]->setEnable();
                    } else {
                        m_collision[(u8)i]->setDisable();
                    }
                }
                i++;
            } while (i < 10);
            m_collisionScene = 0;
        } else if (enable == 1) {
            grCollision* collision = m_collision[(u8)scene];
            if (collision != NULL) {
                fn_80110B98(collision, enable, 0);
                if (m_collisionScenePrev == 0) {
                    m_collision[(u8)scene]->setEnable();
                }
            }
            m_collisionScene = scene;
        } else {
            m_collisionScenePrev = m_collisionScene;
            m_collisionTimer = 600.0f;
            m_collisionScene = 0;
        }
    }
}
