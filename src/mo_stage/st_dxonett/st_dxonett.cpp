#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_triangular.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <string.h>
#include <types.h>

#include <st_dxonett/gr_dxonett.h>
#include <st_dxonett/st_dxonett.h>

stClassInfoImpl<Stages::DxOnett, stDxOnett> stDxOnett::bss_loc_14;

stDxOnett* stDxOnett::create() {
    return new (Heaps::StageInstance) stDxOnett;
}

stDxOnett::stDxOnett() : stMelee("stDxOnett", Stages::DxOnett), m_subject(0, 1) {
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    memset(m_limit, 0, sizeof(m_limit));
    m_carCur = 4;
    m_carState = 0;
    m_carTimer = 0.0f;
    m_carPrev = 4;
    m_carRev = 4;
    m_carDriveState = 2;
    m_carAttackState = 2;
    m_subject.clear();
    m_subjectState = 2;
    m_subject.m_state = 1;
    m_kanbanState = 0;
    m_kanbanTimer = 0.0f;
    m_kanbanLevel = 0;
    m_kanbanCount = 0;
    m_kanbanMotionFlg = 0;
    m_beltTrigger[0] = NULL;
    m_beltTrigger[1] = NULL;
    m_beltData[0] = NULL;
    m_beltData[1] = NULL;
}

stDxOnett::~stDxOnett() {
    if (m_beltData[0] != NULL) {
        delete m_beltData[0];
    }
    if (m_beltData[1] != NULL) {
        delete m_beltData[1];
    }
    releaseArchive();
}

bool stDxOnett::loading() {
    return true;
}

void stDxOnett::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x4C);
    createObjBg(0);
    createObjOther(1);
    createObjKanban(2);
    createObjKanban(3);
    createObjAshiba(4);
    createObjAshiba(5);
    createCollision(m_fileData, 2, NULL);
    createObjCar(6);
    createObjCar(7);
    createObjCar(8);
    createObjCar(9);
    createObjAttack(10);
    createObjWarning(11);
    createObjBeltConv();
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
    cmAIController* ai = g_cmAIController;
    *((u8*)ai + 0x94) |= 4; // HYPOTHESIS: sets a camera flag of the AI controller
    *(float*)((u8*)ai + 0x184) = 0.30543262f;
}

// The clouds that float behind the stage.
void stDxOnett::createObjOther(int index) {
    grDxOnett* ground;
    switch (index) {
    case 1:
        ground = grDxOnett::create(1, "", "grDxOnettCloud");
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

void stDxOnett::createObjBg(int index) {
    grDxOnettBg* ground;
    switch (index) {
    case 0:
        ground = grDxOnettBg::create(0, "", "grDxOnettMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setStateWork(&m_carDriveState);
    }
}

void stDxOnett::createObjKanban(int index) {
    grDxOnettKanban* ground;
    switch (index) {
    case 2:
        ground = grDxOnettKanban::create(2, "StgDxOnettKanban", "grDxOnettKanban");
        break;
    case 3:
        ground = grDxOnettKanban::create(9, "StgDxOnettHisashiB", "grDxOnettHisashiB");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setLevelWork(&m_kanbanLevel);
        ground->setCountWork(&m_kanbanCount);
        ground->setMotionFlgWork(&m_kanbanMotionFlg);
        switch (index) {
        case 3:
            ground->setHisashiB(1);
            break;
        }
    }
}

void stDxOnett::createObjAshiba(int index) {
    grDxOnettAshiba* ground;
    u8 type;
    switch (index) {
    case 4:
        ground = grDxOnettAshiba::create(3, "StgDxOnettAshibaTreeA", "grDxOnettAshibaA");
        type = 0;
        break;
    case 5:
        ground = grDxOnettAshiba::create(4, "StgDxOnettAshibaTreeB", "grDxOnettAshibaB");
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

void stDxOnett::createObjCar(int index) {
    grDxOnettCar* ground;
    u8 type;
    switch (index) {
    case 6:
        ground = grDxOnettCar::create(5, "StgDxOnettBluecar", "grDxOnettBlueCar");
        type = 0;
        break;
    case 7:
        ground = grDxOnettCar::create(6, "StgDxOnettPinkcar", "grDxOnettPinkCar");
        type = 1;
        break;
    case 8:
        ground = grDxOnettCar::create(7, "StgDxOnettTaxi", "grDxOnettTaxi");
        type = 2;
        break;
    case 9:
        ground = grDxOnettCar::create(8, "StgDxOnettTonzurabus", "grDxOnettTonzuraBus");
        type = 3;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setPosLimitWork(m_limit);
        ground->setCurWork(&m_carCur);
        ground->setRevWork(&m_carRev);
        ground->setStateWork(&m_carDriveState);
        ground->setStateAttackWork(&m_carAttackState);
        ground->setType(type);
    }
}

void stDxOnett::createObjAttack(int index) {
    grDxOnettAttack* ground;
    switch (index) {
    case 10:
        ground = grDxOnettAttack::create(0x1E, "nodeIndex", "grDxOnettBlueAttack");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setStateWork(&m_carAttackState);
        ground->setCurWork(&m_carCur);
    }
}

void stDxOnett::createObjWarning(int index) {
    grDxOnettWarning* ground;
    switch (index) {
    case 11:
        ground = grDxOnettWarning::create(0x15, "grf_StgDxOnettCautionR", "grDxOnettWarningR");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setStateWork(&m_subjectState);
    }
}

// The two conveyor belts of the road (left and right lane).
void stDxOnett::createObjBeltConv() {
    m_beltData[0] = new (Heaps::StageResource) grGimmickBeltConveyorData;
    if (m_beltData[0] != NULL) {
        memset(m_beltData[0], 0, sizeof(grGimmickBeltConveyorData));
        grGimmickBeltConveyorData* pos = m_beltData[0];
        pos->m_pos.m_x = -74.925f;
        pos->m_pos.m_y = 0.0f;
        pos->m_pos.m_z = 0.0f;
        m_beltData[0]->m_speed = 6.0f;
        m_beltData[0]->m_isRight = false;
        grGimmickBeltConveyorData* area = m_beltData[0];
        area->m_areaData.m_offsetPos.m_x = 0.0f;
        area->m_areaData.m_offsetPos.m_y = 0.0f;
        area->m_areaData.m_range.m_x = 25.515f;
        area->m_areaData.m_range.m_y = 10.0f;
        area->m_areaData.m_shapeType = gfArea::Shape_Rectangle;
        m_beltTrigger[0] = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
        m_beltTrigger[0]->setBeltConveyorTrigger(m_beltData[0]);

        m_beltData[1] = new (Heaps::StageResource) grGimmickBeltConveyorData;
        if (m_beltData[1] != NULL) {
            memset(m_beltData[1], 0, sizeof(grGimmickBeltConveyorData));
            grGimmickBeltConveyorData* pos2 = m_beltData[1];
            pos2->m_pos.m_x = 68.85f;
            pos2->m_pos.m_y = 0.0f;
            pos2->m_pos.m_z = 0.0f;
            m_beltData[1]->m_speed = 6.0f;
            m_beltData[1]->m_isRight = true;
            grGimmickBeltConveyorData* area2 = m_beltData[1];
            area2->m_areaData.m_offsetPos.m_x = 0.0f;
            area2->m_areaData.m_offsetPos.m_y = 0.0f;
            area2->m_areaData.m_range.m_x = 43.74f;
            area2->m_areaData.m_range.m_y = 10.0f;
            area2->m_areaData.m_shapeType = gfArea::Shape_Rectangle;
            m_beltTrigger[1] = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
            m_beltTrigger[1]->setBeltConveyorTrigger(m_beltData[1]);
        }
    }
}

void stDxOnett::update(float deltaFrame) {
    if (m_isDevil == 1) {
        float down;
        CameraController* camera = CameraController::getInstance();
        cmStageParam* param = &camera->m_stageCameraParam;
        if (param != NULL) {
            float cosine = nw4r::math::CosFIdx(10.666667f);
            float sine = nw4r::math::SinFIdx(10.666667f);
            down = -(0.25f * (param->m_verticalRotationFactor * (sine / cosine)));
        }
        setCameraLimitRange(-150.0f, 140.0f, 150.0f, down);
    } else {
        resetCameraLimitRange();
    }
    updateLimit(deltaFrame);
    updateCar(deltaFrame);
    updateKanban(deltaFrame);
    updateBelt(deltaFrame);
}

static inline void setLimitPoint(Vec3f* dst, float x, float y) {
    dst->m_x = x;
    dst->m_y = y;
    dst->m_z = 0.0f;
}

// Takes over the limits of the camera.
void stDxOnett::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    Rect2D* range = reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
    setLimitPoint(&m_limit[0], range->m_left, range->m_up);
    setLimitPoint(&m_limit[1], range->m_right, range->m_down);
}

// Lets a car drive by every few seconds (the timers come from the stage data).
void stDxOnett::updateCar(float deltaFrame) {
    float* data = static_cast<float*>(m_stageData);
    if (data != NULL) {
        m_carTimer -= deltaFrame;
        if (m_carTimer < 0.0f) {
            m_carTimer = 0.0f;
        }
        switch (m_carState) {
        case 0:
            m_carTimer = data[9];
            m_carState = 1;
            break;
        case 1:
            if (m_carTimer == 0.0f) {
                m_carTimer = data[10];
                m_carState = 2;
            }
            break;
        case 2:
            if (m_carTimer == 0.0f && selectCar(0) == 1) {
                m_carState = 3;
            }
            break;
        case 3:
            if (m_carCur == 4) {
                m_carTimer = data[9];
                m_carState = 1;
                m_subject.m_state = 1;
                if (randf() < data[15]) {
                    selectCar(1);
                }
            }
            break;
        }
    }
}

// Lets the signs above the road bend now and then.
void stDxOnett::updateKanban(float deltaFrame) {
    float* data = static_cast<float*>(m_stageData);
    if (data != NULL) {
        switch (m_kanbanState) {
        case 0:
            m_kanbanState = 1;
            // fall through
        case 1:
            if (m_kanbanLevel == 10) {
                m_kanbanTimer = data[5] + (data[6] - data[5]) * randf();
                m_kanbanState = 3;
            }
            break;
        case 3:
            m_kanbanTimer -= deltaFrame;
            if (m_kanbanTimer < 0.0f) {
                m_kanbanTimer = 0.0f;
            }
            if (m_kanbanTimer == 0.0f) {
                m_kanbanLevel = 100;
                m_kanbanState = 0;
            }
            break;
        }
    }
}

void stDxOnett::updateBelt(float deltaFrame) { }

// Picks the next car. It is never the car that drove last; with mode 1 it is the one that waits for its turn back.
bool stDxOnett::selectCar(u32 mode) {
    float rnd = randf();
    int prev;
    if (mode == 1) {
        prev = 0xFF;
    } else {
        prev = m_carPrev;
    }
    u8 pick;
    switch (prev) {
    case 0:
        if (rnd < 1.0f / 3.0f) {
            pick = 1;
        } else if (rnd < 2.0f / 3.0f) {
            pick = 2;
        } else {
            pick = 3;
        }
        break;
    case 1:
        if (rnd < 1.0f / 3.0f) {
            pick = 0;
        } else if (rnd < 2.0f / 3.0f) {
            pick = 2;
        } else {
            pick = 3;
        }
        break;
    case 2:
        if (rnd < 1.0f / 3.0f) {
            pick = 0;
        } else if (rnd < 2.0f / 3.0f) {
            pick = 1;
        } else {
            pick = 3;
        }
        break;
    case 3:
        if (rnd < 1.0f / 3.0f) {
            pick = 0;
        } else if (rnd < 2.0f / 3.0f) {
            pick = 1;
        } else {
            pick = 2;
        }
        break;
    default:
        if (rnd < 0.25f) {
            pick = 0;
        } else if (rnd < 0.5f) {
            pick = 1;
        } else if (rnd < 0.75f) {
            pick = 2;
        } else {
            pick = 3;
        }
        break;
    }
    if (mode == 1) {
        if (pick == m_carCur) {
            return false;
        }
        m_carRev = pick;
    } else {
        if (pick == m_carRev) {
            return false;
        }
        m_carCur = pick;
        m_carPrev = pick;
        Rect2D range;
        Vec3f pos;
        range.m_up = -20.0f;
        range.m_down = 20.0f;
        range.m_left = -40.0f;
        range.m_right = 40.0f;
        pos.m_x = 0.0f;
        pos.m_y = 0.0f;
        pos.m_z = 0.0f;
        m_subject.m_state = 0;
        m_subject.setPos(&pos);
        m_subject.m_60 = range;
        m_subject.m_stateB = 0;
        m_subjectState = 0;
    }
    return true;
}

bool stDxOnett::isBamperVector() {
    return true;
}
