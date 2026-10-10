#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <it/it_create.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/ut/ut_Color.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>

#include <st_famicom/gr_famicom.h>
#include <st_famicom/st_famicom.h>

// stCollisionWork::destroy (sora_melee, unnamed)
extern "C" void fn_27_239F6C(stCollisionWork* work);
// stTrigger::setPressureAreaTrigger (sora_melee, unnamed)
extern "C" void fn_27_233898(stTrigger* trigger, stFamicomPressureData* data);
// stTrigger::setPressureParam (sora_melee, unnamed)
extern "C" void fn_27_233BE4(stTrigger* trigger, stFamicomPressureData* data);

// MATCH-ONLY: the first word of the AI camera controller after its vtable pointer: the original sets its top byte (a flag that
// is set while the stage is played) with one read-modify-write of the word.
struct famicomCameraFlag {
    u32 m_flag : 8;
    u32 _unused : 24;
};

stClassInfoImpl<Stages::Famicom, stFamicom> stFamicom::bss_loc_14;

stFamicom* stFamicom::create() {
    return new (Heaps::StageInstance) stFamicom;
}

stFamicom::stFamicom() : stMelee("stFamicom", Stages::Famicom) {
    m_collWork[0].initialize();
    m_collWork[0].m_vtxLen = 0x18;
    m_collWork[0].m_isClosed = true;
    m_collWork[1].initialize();
    m_collWork[1].m_isClosed = true;
    m_collWork[1].m_vtxLen = 0x1E;
    m_collWork[2].initialize();
    m_collWork[2].m_vtxLen = 0x1E;
    m_collWork[2].m_isClosed = true;
    m_collWork[3].initialize();
    m_collWork[3].m_isClosed = true;
    m_collWork[3].m_vtxLen = 0x12;
    m_collWork[4].initialize();
    m_collWork[4].m_vtxLen = 0x12;
    m_collWork[4].m_isClosed = true;
    m_collWork[5].initialize();
    m_collWork[5].m_isClosed = true;
    m_collWork[5].m_vtxLen = 0x1D;
    m_collWork[6].initialize();
    m_collWork[6].m_vtxLen = 0x1D;
    m_collWork[6].m_isClosed = true;
    memset(m_posYuka, 0, sizeof(m_posYuka));
    for (int i = 0; i < 7; i++) {
        m_yukaToss[i].m_x = 0.0f;
        m_yukaToss[i].m_y = 0.0f;
        m_yukaToss[i].m_z = 0.0f;
        m_yukaToss[i].m_value = 0.0f;
        m_yukaToss[i].m_state = 0xD;
    }
    m_triggerYuka[0] = NULL;
    m_triggerYuka[1] = NULL;
    m_triggerYuka[2] = NULL;
    m_triggerYuka[3] = NULL;
    m_pressureYuka[0] = NULL;
    m_pressureYuka[1] = NULL;
    m_pressureYuka[2] = NULL;
    m_pressureYuka[3] = NULL;
    m_collWork[7].initialize();
    m_collWork[7].m_isClosed = true;
    m_collWork[7].m_vtxLen = 4;
    memset(&m_posPow, 0, sizeof(m_posPow));
    m_powState = 0xD;
    m_ballState = 0;
    m_ballTimer = 0.0f;
    m_ballWait = 0.0f;
    memset(m_posBall, 0, sizeof(m_posBall));
    m_ballKind = 0xD;
    m_ballKindPrev = 0xD;
    m_ballLtoR = 1;
    m_ballLtoRPrev = 0;
    m_enemyState = 0;
    m_enemyTimer = 0.0f;
    m_enemyData = NULL;
    memset(m_posEnemy, 0, sizeof(m_posEnemy));
    m_triggerPow = NULL;
    m_pressurePow = NULL;
    m_posLimit[0].m_x = 0.0f;
    m_posLimit[0].m_y = 0.0f;
    m_posLimit[0].m_z = 0.0f;
    m_posLimit[1].m_x = 0.0f;
    m_posLimit[1].m_y = 0.0f;
    m_posLimit[1].m_z = 0.0f;
    memset(m_landingWork, 0, sizeof(m_landingWork));
    memset(m_landingStock, 0, sizeof(m_landingStock));
    memset(m_landingFlg, 0, sizeof(m_landingFlg));
    m_stateArea = 0xD;
    if (g_cmAIController != NULL) {
        reinterpret_cast<famicomCameraFlag*>(reinterpret_cast<u8*>(g_cmAIController) + 8)->m_flag = 1;
    }
}

stFamicom::~stFamicom() {
    if (g_cmAIController != NULL) {
        reinterpret_cast<famicomCameraFlag*>(reinterpret_cast<u8*>(g_cmAIController) + 8)->m_flag = 0;
    }
    if (m_pressureYuka[0] != NULL) {
        delete m_pressureYuka[0];
    }
    if (m_pressureYuka[1] != NULL) {
        delete m_pressureYuka[1];
    }
    if (m_pressureYuka[2] != NULL) {
        delete m_pressureYuka[2];
    }
    if (m_pressureYuka[3] != NULL) {
        delete m_pressureYuka[3];
    }
    if (m_pressurePow != NULL) {
        delete m_pressurePow;
    }
    fn_27_239F6C(&m_collWork[0]);
    fn_27_239F6C(&m_collWork[1]);
    fn_27_239F6C(&m_collWork[2]);
    fn_27_239F6C(&m_collWork[3]);
    fn_27_239F6C(&m_collWork[4]);
    fn_27_239F6C(&m_collWork[5]);
    fn_27_239F6C(&m_collWork[6]);
    fn_27_239F6C(&m_collWork[7]);
    if (m_enemyData != NULL) {
        delete[] m_enemyData;
        m_enemyData = NULL;
    }
    releaseArchive();
}

bool stFamicom::loading() {
    return true;
}

void stFamicom::createObj() {
    int size;
    void* data = m_fileData->getData(Data_Type_Misc, 0x2711, &size, 0xFFFE);
    if (data != NULL) {
        m_archiveShellBrres.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2712, &size, 0xFFFE);
    if (data != NULL) {
        m_archiveShellParam.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2713, &size, 0xFFFE);
    if (data != NULL) {
        m_archiveCrabBrres.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Misc, 0x2714, &size, 0xFFFE);
    if (data != NULL) {
        m_archiveCrabParam.setFileImage(data, size, Heaps::StageResource);
    }
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 100);
    createObjEnemy();
    createObjBg(0);
    createObjYukaCenter(1);
    createObjYukaSideA(2);
    createObjYukaSideA(3);
    createObjYukaSideB(4);
    createObjYukaSideB(5);
    createObjYukaSideC(6);
    createObjYukaSideC(7);
    createObjYukaTrigger();
    createObjPow(8);
    createObjBall(9);
    createObjBall(10);
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
}

void stFamicom::createObjBg(int index) {
    grFamicomBg* ground = grFamicomBg::create(0, "StgFamicomMain", "grFamicomMainBg");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosYukaWork(m_posYuka);
        ground->setPosPowWork(&m_posPow);
        ground->setPosEnemyWork(m_posEnemy);
        ground->setPosBallWork(m_posBall);
        ground->setPosLimitWork(m_posLimit);
        ground->setLandingWork(m_landingWork);
        ground->setLandingFlgWork(m_landingFlg);
        ground->setStateAreaWork(&m_stateArea);
        createCollision(m_fileData, 2, ground);
    }
}

void stFamicom::createObjYukaCenter(int index) {
    grFamicomYuka* ground;
    stCollisionWork* collWork;
    switch (index) {
    case 1:
        ground = grFamicomYuka::create(1, "YukaBlockLocCenter", "grFamicomYukaCenter");
        collWork = &m_collWork[0];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->setCtrl(0x15);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posYuka);
        ground->setEnemyDataWork(m_enemyData);
        ground->setYukaTossData(m_yukaToss);
        ground->setType(0);
        createCollisionSelf(collWork, ground, "YukaBlockLocCenter", "YukaBlockLocCenter", 0x41A);
        ground->disableCalcCollision();
    }
}

// The floors on the sides: the two parts of a side (the left one and the right one) are the grounds A and D / B and E / C and F
// of the model. Their pressure areas tell the stage which fighters hit them from below.
#define FAMICOM_SIDE_YUKA(CTRL, LEFT_CASE, LEFT_SIZE, LEFT_NODE, LEFT_NAME, LEFT_COLL, LEFT_INDEX, RIGHT_SIZE, RIGHT_NODE, RIGHT_NAME, \
                          RIGHT_COLL, RIGHT_INDEX)                                                                                    \
    grFamicomYuka* ground;                                                                                                            \
    stCollisionWork* collWork;                                                                                                        \
    u8 rotFlg;                                                                                                                        \
    u8 type;                                                                                                                          \
    Vec3f* posWork;                                                                                                                   \
    stFamicomYukaToss* tossWork;                                                                                                      \
    if (index == (LEFT_CASE) + 1) {                                                                                                   \
        ground = grFamicomYuka::create(RIGHT_SIZE, RIGHT_NODE, RIGHT_NAME);                                                           \
        collWork = &m_collWork[RIGHT_COLL];                                                                                           \
        posWork = &m_posYuka[RIGHT_INDEX];                                                                                            \
        tossWork = &m_yukaToss[RIGHT_INDEX];                                                                                          \
        rotFlg = 1;                                                                                                                   \
        type = RIGHT_INDEX;                                                                                                           \
    } else if (index < (LEFT_CASE) + 1 && index > (LEFT_CASE) - 1) {                                                                  \
        ground = grFamicomYuka::create(LEFT_SIZE, LEFT_NODE, LEFT_NAME);                                                              \
        collWork = &m_collWork[LEFT_COLL];                                                                                            \
        posWork = &m_posYuka[LEFT_INDEX];                                                                                             \
        tossWork = &m_yukaToss[LEFT_INDEX];                                                                                           \
        rotFlg = 0;                                                                                                                   \
        type = LEFT_INDEX;                                                                                                            \
    } else {                                                                                                                          \
        ground = NULL;                                                                                                                \
        collWork = NULL;                                                                                                              \
    }                                                                                                                                 \
    if (ground != NULL) {                                                                                                             \
        addGround(ground);                                                                                                            \
        ground->setCtrl(CTRL);                                                                                                        \
        ground->setRotFlg(rotFlg);                                                                                                    \
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);                                                                    \
        ground->setStageData(m_stageData);                                                                                            \
        ground->setPosWork(posWork);                                                                                                  \
        ground->setEnemyDataWork(m_enemyData);                                                                                        \
        ground->setYukaTossData(tossWork);                                                                                            \
        ground->setType(type);                                                                                                        \
        if (index == (LEFT_CASE) + 1) {                                                                                               \
            createCollisionSelf(collWork, ground, RIGHT_NODE, RIGHT_NODE, 0x41A);                                                     \
        } else if (index < (LEFT_CASE) + 1 && index > (LEFT_CASE) - 1) {                                                              \
            createCollisionSelf(collWork, ground, LEFT_NODE, LEFT_NODE, 0x41A);                                                       \
        }                                                                                                                             \
        ground->disableCalcCollision();                                                                                               \
    }

void stFamicom::createObjYukaSideA(int index) {
    FAMICOM_SIDE_YUKA(0x1B, 2, 6, "YukaBlockLocSideA", "grFamicomYukaSideA_L", 1, 1, 9, "YukaBlockLocSideD", "grFamicomYukaSideA_R", 2, 2)
}

void stFamicom::createObjYukaSideB(int index) {
    FAMICOM_SIDE_YUKA(0xF, 4, 7, "YukaBlockLocSideB", "grFamicomYukaSideB_L", 3, 3, 10, "YukaBlockLocSideE", "grFamicomYukaSideB_R", 4, 4)
}

void stFamicom::createObjYukaSideC(int index) {
    FAMICOM_SIDE_YUKA(0x1A, 6, 8, "YukaBlockLocSideC", "grFamicomYukaSideC_L", 5, 5, 11, "YukaBlockLocSideF", "grFamicomYukaSideC_R", 6, 6)
}

// The area under a floor that the fighters press from below: the size of the area is the same (20 x 20) and a trigger of the
// kind 0xF watches it.
#define FAMICOM_PRESSURE_DATA(member, data, size, flag)                                              \
    memset(member, 0, sizeof(stFamicomPressureData));                                                \
    {                                                                                                \
        stFamicomPressureData* pressure = member;                                                    \
        pressure->unk28 = 0.0f;                                                                      \
        pressure->unk2C = 0.0f;                                                                      \
        pressure->unk30 = 0.0f;                                                                      \
    }                                                                                                \
    member->unk34 = data->unk10;                                                                     \
    member->unk38 = 90.0f;                                                                           \
    member->unk40 = flag;                                                                            \
    {                                                                                                \
        stFamicomPressureData* pressure = member;                                                    \
        pressure->unk18 = 0.0f;                                                                      \
        pressure->unk1C = 0.0f;                                                                      \
        pressure->unk20 = size;                                                                      \
        pressure->unk24 = size;                                                                      \
    }

#define FAMICOM_YUKA_TRIGGER(i)                                                                       \
    m_pressureYuka[i] = new (Heaps::StageInstance) stFamicomPressureData;                              \
    if (m_pressureYuka[i] != NULL) {                                                                  \
        FAMICOM_PRESSURE_DATA(m_pressureYuka[i], data, 20.0f, 0)                                       \
        m_triggerYuka[i] = g_stTriggerMng->createTrigger(Gimmick::Area_Yuka, -1);                      \
        fn_27_233898(m_triggerYuka[i], m_pressureYuka[i]);                                            \
        m_triggerYuka[i]->setAreaSleep(true);

void stFamicom::createObjYukaTrigger() {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data != NULL) {
        FAMICOM_YUKA_TRIGGER(0)
        FAMICOM_YUKA_TRIGGER(1)
        FAMICOM_YUKA_TRIGGER(2)
        FAMICOM_YUKA_TRIGGER(3)
        }}}}
    }
}

void stFamicom::createObjPow(int index) {
    grFamicomPow* ground = grFamicomPow::create(2, "StgFamicomPowBlock", "grFamicomPow");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(&m_posPow);
        ground->setStateWork(&m_powState);
        createCollisionSelf(&m_collWork[7], ground, "StgFamicomPowBlock", "StgFamicomPowBlock", 0x41A);
        ground->disableCalcCollision();
        stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
        if (data != NULL) {
            m_pressurePow = new (Heaps::StageInstance) stFamicomPressureData;
            if (m_pressurePow != NULL) {
                FAMICOM_PRESSURE_DATA(m_pressurePow, data, 500.0f, 1)
                m_triggerPow = g_stTriggerMng->createTrigger(Gimmick::Area_Pow, -1);
                fn_27_233898(m_triggerPow, m_pressurePow);
                m_triggerPow->setAreaSleep(true);
            }
        }
    }
}

void stFamicom::createObjBall(int index) {
    grFamicomBall* ground;
    u8* state;
    u8* ltoR;
    if (index == 10) {
        ground = grFamicomBall::create(5, "stgFamicomGreenBall", "grFamicomBall_B");
        state = &m_ballKindPrev;
        ltoR = &m_ballLtoRPrev;
    } else if (index < 10 && index > 8) {
        ground = grFamicomBall::create(5, "stgFamicomGreenBall", "grFamicomBall_A");
        state = &m_ballKind;
        ltoR = &m_ballLtoR;
    } else {
        ground = NULL;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posBall);
        ground->setStateWork(state);
        ground->setLtoRWork(ltoR);
    }
}

void stFamicom::createObjEnemy() {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data != NULL) {
        m_enemyData = new (Heaps::StageResource) stFamicomEnemyData[data->m_enemyMax];
        if (m_enemyData != NULL) {
            u8 count = data->m_enemyMax;
            for (u8 i = 0; i != count; i++) {
                initEnemyData(&m_enemyData[i]);
            }
        }
    }
}

void stFamicom::update(float deltaFrame) {
    updateLimit();
    updateEnemy(deltaFrame);
    updateBall(deltaFrame);
    updatePow(deltaFrame);
    updateYuka(deltaFrame);
}

void stFamicom::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    m_posLimit[0].m_x = camera->unk158;
    m_posLimit[0].m_y = camera->unk160;
    m_posLimit[0].m_z = 0.0f;
    m_posLimit[1].m_x = camera->unk15C;
    m_posLimit[1].m_y = camera->unk164;
    m_posLimit[1].m_z = 0.0f;
}

// The enemies come one after the other: the first one after the wait of the stage data, then after a random wait.
void stFamicom::updateEnemy(float deltaFrame) {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data != NULL) {
        m_enemyTimer = m_enemyTimer - deltaFrame;
        if (m_enemyTimer < 0.0f) {
            m_enemyTimer = 0.0f;
        }
        if (m_enemyState == 1) {
            if (m_enemyTimer == 0.0f) {
                makeEnemy();
                m_enemyTimer = data->unk18 + (data->unk1C - data->unk18) * randf();
            }
        } else if (m_enemyState == 0) {
            m_enemyTimer = data->unk14;
            m_enemyState = 1;
        }
    }
}

// The green balls: the floors that were not hit for the longest time (the part of the wait that the fighters stood on them)
// choose where the next one comes from.
void stFamicom::updateBall(float deltaFrame) {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data != NULL) {
        m_ballTimer = m_ballTimer - deltaFrame;
        if (m_ballTimer < 0.0f) {
            m_ballTimer = 0.0f;
        }
        if (m_ballState == 1) {
            if (m_ballTimer <= 0.0f) {
                bool found = false;
                float limit = data->unk5C;
                u32 index = 0;
                for (index = 0; index < 4; index++) {
                    if (m_landingStock[index] > limit) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    limit = m_ballWait;
                    for (index = 0; index < 4; index++) {
                        if (m_landingWork[index] > limit) {
                            break;
                        }
                    }
                }
                if (index != 4) {
                    switch (index) {
                    case 2:
                        m_ballKind = 0xB;
                        break;
                    case 0:
                        m_ballKind = 9;
                        break;
                    case 1:
                        m_ballKind = 10;
                        break;
                    case 3:
                        m_ballKind = 0xC;
                        break;
                    }
                    if (!found || data->unk60 <= randf()) {
                        if (m_landingFlg[index] == 1) {
                            m_ballLtoR = 0;
                        } else {
                            m_ballLtoR = 1;
                        }
                        if (found) {
                            m_landingStock[index] = m_landingWork[index];
                        } else {
                            m_landingStock[index] = m_landingStock[index] + m_landingWork[index];
                        }
                    } else {
                        m_ballKindPrev = m_ballKind;
                        m_ballLtoR = 1;
                        m_ballLtoRPrev = 0;
                        m_landingStock[index] = 0.0f;
                    }
                    m_landingWork[index] = 0.0f;
                    m_ballState = 2;
                }
            }
        } else if (m_ballState == 0) {
            m_ballTimer = data->unk4C;
            float random = randf();
            m_ballState = 1;
            m_ballWait = data->unk50 + (data->unk54 - data->unk50) * random;
        } else if (m_ballState < 3 && m_ballKind == 0xD && m_ballKindPrev == 0xD) {
            float random = randf();
            m_ballState = 1;
            m_ballWait = data->unk50 + (data->unk54 - data->unk50) * random;
        }
    }
}

// The POW block was hit: the enemies that stand on the floor are turned over.
void stFamicom::updatePow(float deltaFrame) {
    if (m_powState == 8) {
        m_triggerPow->setAreaSleep(false);
        stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
        if (data != NULL) {
            u8 count = data->m_enemyMax;
            for (u8 i = 0; i != count; i++) {
                stFamicomEnemyData* enemy = &m_enemyData[i];
                if (enemy->m_grounded == 1) {
                    enemy->m_state = 8;
                }
            }
            m_powState = 0xD;
        }
    } else {
        m_triggerPow->setAreaSleep(true);
    }
}

// What the fighters did to the floors this frame is given to the pressure areas (up to four at the same time).
void stFamicom::updateYuka(float deltaFrame) {
    m_triggerYuka[0]->setAreaSleep(true);
    m_triggerYuka[1]->setAreaSleep(true);
    m_triggerYuka[2]->setAreaSleep(true);
    m_triggerYuka[3]->setAreaSleep(true);
    u8 count = 0;
    u32 index = 0;
    do {
        stFamicomYukaToss* toss = &m_yukaToss[index & 0xFF];
        if (toss->m_state == 0) {
            m_pressureYuka[count]->unk3C = toss->m_value;
            stFamicomPressureData* pressure = m_pressureYuka[count];
            pressure->unk18 = toss->m_x;
            pressure->unk1C = toss->m_y;
            fn_27_233BE4(m_triggerYuka[count], m_pressureYuka[count]);
            m_triggerYuka[count]->setAreaSleep(false);
            toss->m_state = 0xD;
            count++;
        }
    } while (count != 4 && ++index != 7);
}

void stFamicom::initEnemyData(stFamicomEnemyData* data) {
    if (data == NULL) {
        return;
    }
    data->m_pos.m_x = 0.0f;
    data->m_pos.m_y = 0.0f;
    data->m_pos.m_z = 0.0f;
    data->m_speed.m_x = 0.0f;
    data->m_speed.m_y = 0.0f;
    data->m_speed.m_z = 0.0f;
    data->m_dir = 1;
    data->m_grounded = 0;
    data->m_timer = 0.0f;
    data->m_hitDir = -1.0f;
    data->m_state = 0xD;
}

u8 stFamicom::getCountEnemy() {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data == NULL) {
        return 0;
    }
    u8 count = 0;
    for (u8 i = 0; i != data->m_enemyMax; i++) {
        if (m_enemyData[i].m_state != 0xD) {
            count++;
        }
    }
    return count;
}

u8 stFamicom::getEmptyIndexEnemyTbl() {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data == NULL) {
        return 0xFF;
    }
    u8 i = 0;
    while (true) {
        if (i == data->m_enemyMax) {
            return 0xFF;
        }
        if (m_enemyData[i].m_state == 0xD) {
            break;
        }
        i++;
    }
    return i;
}

// A new enemy: a sidestepper or a shellcreeper (the chance of the first one is in the stage data).
void stFamicom::makeEnemy() {
    stFamicomData* data = static_cast<stFamicomData*>(m_stageData);
    if (data != NULL) {
        u8 index = getEmptyIndexEnemyTbl();
        if (index != 0xFF) {
            Ground* ground;
            if (data->unk20 <= (data->unk20 + data->unk24) * randf()) {
                ground = grFamicomKani::create(4, "StgFamicomKani", "grFamicomKani");
            } else {
                ground = grFamicomKame::create(3, "StgFamicomKame", "grFamicomKame");
            }
            if (ground != NULL) {
                grFamicomEnemy* enemy = static_cast<grFamicomEnemy*>(ground);
                addGround(enemy);
                enemy->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                enemy->setStageData(m_stageData);
                initEnemyData(&m_enemyData[index]);
                m_enemyData[index].m_state = 0;
                enemy->setEnemyDataWork(m_enemyData);
                enemy->setYukaTossData(m_yukaToss);
                enemy->setPosCtrlWork(m_posEnemy);
                enemy->setPosLimitWork(m_posLimit);
                enemy->setIndex(index);
            }
        }
    }
}

void stFamicom::notifyEventInfoGo() {
    m_stateArea = 0;
}

void stFamicom::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) {
    if (itemID == Item_Stage_Shell) {
        *brres = &m_archiveShellBrres;
        *param = &m_archiveShellParam;
    } else if (itemID == Item_Stage_Crab) {
        *brres = &m_archiveCrabBrres;
        *param = &m_archiveCrabParam;
    }
}

GXColor stFamicom::getFinalTechniqColor() {
    return nw4r::ut::Color(0x14000496);
}
