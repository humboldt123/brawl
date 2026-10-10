#include <cm/cm_camera_controller.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <string.h>
#include <types.h>

#include <st_dxgarden/gr_dxgarden.h>
#include <st_dxgarden/st_dxgarden.h>

// stTrigger::setWaterParam
extern "C" void fn_27_233C70(stTrigger* trigger, grGimmickWaterData* data);

stClassInfoImpl<Stages::DxGarden, stDxGarden> stDxGarden::bss_loc_14;

stDxGarden* stDxGarden::create() {
    return new (Heaps::StageInstance) stDxGarden;
}

stDxGarden::stDxGarden() : stMelee("stDxGarden", Stages::DxGarden) {
    m_waterData = NULL;
    m_trigger = NULL;
    memset(m_limit, 0, sizeof(m_limit));
}

stDxGarden::~stDxGarden() {
    if (m_waterData != NULL) {
        delete m_waterData;
    }
    releaseArchive();
}

bool stDxGarden::loading() {
    return true;
}

void stDxGarden::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x1C);
    createObjBg(0);
    createObjOther(1);
    createObjOther(2);
    createObjKrap(3);
    createObjCranky(4);
    createObjOther(5);
    createObjSuimen(6);
    createObjOther(7);
    createObjLamp(8);
    createObjLamp(9);
    createObjOther(10);
    createObjSwimArea();
    createCollision(m_fileData, 2, NULL);
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
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
}

void stDxGarden::createObjBg(int index) {
    grDxGardenBg* ground;
    switch (index) {
    case 0:
        ground = grDxGardenBg::create(6, "StgGardenBG", "grDxGardenBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

void stDxGarden::createObjKrap(int index) {
    grDxGardenKrap* ground;
    switch (index) {
    case 3:
        ground = grDxGardenKrap::create(10, "TransN", "grDxGardenKrap");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

void stDxGarden::createObjCranky(int index) {
    grDxGardenCranky* ground;
    switch (index) {
    case 4:
        ground = grDxGardenCranky::create(4, "StgGardenCranky", "grDxGardenCranky");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

void stDxGarden::createObjLamp(int index) {
    grDxGardenLamp* ground;
    int type;
    switch (index) {
    case 8:
        ground = grDxGardenLamp::create(1, "StgGardenLampA", "grDxGardenLampA");
        type = 0;
        break;
    case 9:
        ground = grDxGardenLamp::create(2, "StgGardenLampB", "grDxGardenLampB");
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
        ground->setType(type);
    }
}

void stDxGarden::createObjSuimen(int index) {
    grDxGardenSuimen* ground;
    switch (index) {
    case 6:
        ground = grDxGardenSuimen::create(8, "StgGardenSuimen", "grDxGardenSuimen");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(&m_limit[0].m_x);
    }
}

void stDxGarden::createObjOther(int index) {
    grDxGarden* ground;
    switch (index) {
    case 1:
        ground = grDxGarden::create(5, "StgGardenEnkei", "grDxGardenEnkei");
        break;
    case 2:
        ground = grDxGarden::create(7, "Birds", "grDxGardenBirds");
        break;
    case 5:
        ground = grDxGarden::create(0, "TopN", "grDxGardenHouse");
        break;
    case 7:
        ground = grDxGarden::create(9, "StgGardenRiverHikari", "grDxGardenSuimenHikari");
        break;
    case 10:
        ground = grDxGarden::create(3, "StgGardenButterfly", "grDxGardenButterfly");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
    }
}

// The area of the water: as wide as the stage (positions are set every frame by updateRiver).
void stDxGarden::createObjSwimArea() {
    float* data = static_cast<float*>(m_stageData);
    if (data != NULL) {
        m_waterData = new (Heaps::StageResource) grGimmickWaterData;
        if (m_waterData != NULL) {
            memset(m_waterData, 0, sizeof(grGimmickWaterData));
            m_waterData->m_swimHeight = data[0];
            grGimmickWaterData* water = m_waterData;
            water->m_canDrown = true;
            water->m_speed = data[1];
            grGimmickWaterData* area = m_waterData;
            area->m_areaData.m_offsetPos.m_x = 0.0f;
            area->m_areaData.m_offsetPos.m_y = data[0] - 25.0f;
            area->m_areaData.m_range.m_x = 500.0f;
            area->m_areaData.m_range.m_y = 50.0f;
            m_trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Water, -1);
            m_trigger->setWaterTrigger(m_waterData);
        }
    }
}

void stDxGarden::update(float deltaFrame) {
    switch (m_state) {
    case 0:
        g_sndSystem->playSE(snd_se_stage_Garden_junglese, 0, 0, 0, -1);
        m_state = 1;
        break;
    case 1:
        break;
    }
    if (m_isDevil == 1) {
        setCameraLimitRange(-250.0f, 250.0f, 250.0f, -20.0f);
    } else {
        resetCameraLimitRange();
    }
    updateLimit(deltaFrame);
    updateRiver(deltaFrame);
}

static inline void setLimitPoint(Vec3f* dst, float x, float y) {
    dst->m_x = x;
    dst->m_y = y;
    dst->m_z = 0.0f;
}

// Takes over the limits of the camera.
void stDxGarden::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    setLimitPoint(&m_limit[0], camera->unk158, camera->unk160);
    setLimitPoint(&m_limit[1], camera->unk15C, camera->unk164);
}

// The water area follows the limits of the camera.
void stDxGarden::updateRiver(float deltaFrame) {
    if (m_trigger == NULL) {
        return;
    }
    grGimmickWaterData* water = m_waterData;
    if (water == NULL) {
        return;
    }
    float* data = static_cast<float*>(m_stageData);
    if (data == NULL) {
        return;
    }
    water->m_areaData.m_range.m_x = 500.0f + (m_limit[1].m_x - m_limit[0].m_x);
    water->m_areaData.m_range.m_y = 50.0f + (data[0] - m_limit[1].m_y);
    water->m_areaData.m_offsetPos.m_x = 0.0f;
    water->m_areaData.m_offsetPos.m_y = data[0] - 0.5f * water->m_areaData.m_range.m_y;
    return fn_27_233C70(m_trigger, m_waterData);
}

bool stDxGarden::isBamperVector() {
    return true;
}

GXColor stDxGarden::getFinalTechniqColor() {
    u32 packed = 0x1400047D;
    return *reinterpret_cast<GXColor*>(&packed);
}
