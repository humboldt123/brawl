#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <it/it_manager.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_triangular.h>
#include <nw4r/ut/ut_Color.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>

#include <st_ice/gr_ice.h>
#include <st_ice/st_ice.h>

// ftManager::enumIncludeEntryId (sora_melee): the symbol that the original calls to count the fighters in a rectangle
// (HYPOTHESIS: its real name is another one, the map gives this one for the address)
extern "C" int enumIncludeEntryId__9ftManagerCFi(ftManager* manager, Rect2D* rect, int* entries, int unk1, int unk2);
// gmCheckGlobalItemSwitch (main, unnamed)
extern "C" bool fn_8005136C(int itemKind);

// MATCH-ONLY: the start of the settings of the match (the game mode is in the top six bits of the first byte, the byte
// at 0xE says whether items are on)
struct stIceMeleeView {
    u8 m_gameMode : 6;
    u8 _unused : 2;
    u8 _pad[0xD];
    u8 m_itemsOn;
};

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct stIceMarker {
    int m_a;
    int m_b;
    stIceMarker(int a, int b) : m_a(a), m_b(b) { }
};
static stIceMarker sMarkerA(0xFF, 0);
static stIceMarker sMarkerB(0xFF, 1);

stClassInfoImpl<Stages::Ice, stIce> stIce::bss_loc_14;

stIce* stIce::create() {
    return new (Heaps::StageInstance) stIce;
}

stIce::stIce() : stMelee("stIce", Stages::Ice) {
    m_sceneState = 0;
    m_unk1DC = 0.0f;
    m_part = 0xE;
    m_scenePrev = 0.0f;
    m_unk440 = 0.0f;
    m_posLimit[0].m_x = 0.0f;
    m_posLimit[0].m_y = 0.0f;
    m_posLimit[0].m_z = 0.0f;
    m_posLimit[1].m_x = 0.0f;
    m_posLimit[1].m_y = 0.0f;
    m_posLimit[1].m_z = 0.0f;
    for (u8 i = 0; i < 12; i++) {
        reinterpret_cast<Matrix*>(m_mtxGimmick[i])->setIdentity();
    }
    m_fishState = 0;
    m_fishTimer = 0.0f;
    memset(m_posFish, 0, sizeof(m_posFish));
    memset(m_posFishX, 0, sizeof(m_posFishX));
    m_fishIndex = -1;
    m_fishStateWork = 0xE;
    memset(m_posBear, 0, sizeof(m_posBear));
    m_bearState = 0xE;
    m_kumoState = 0;
    m_kumoTimer = 0.0f;
    m_kumoStateA = 0xE;
    m_kumoStateB = 0xE;
    m_iceState = 0;
    m_iceTimer = 0.0f;
    m_iceStateA = 0xE;
    m_iceStateB = 0xE;
    m_turaraState = 0;
    m_turaraTimer = 0.0f;
    m_turaraStateWork = 0xE;
    m_turaraHP = 0.0f;
    m_triggerWater = NULL;
    m_waterData = NULL;
    m_waterPrev = 0.0f;
    m_yasaiState = 0;
    m_yasaiTimer = 0.0f;
    m_warningTimer = 0.0f;
    m_warningState = 0xE;
    m_dangerZone = -1;
}

stIce::~stIce() {
    if (m_waterData != NULL) {
        delete m_waterData;
    }
    releaseArchive();
}

bool stIce::loading() {
    return true;
}

#define ICE_LOAD_ARCHIVE(archive, id)                                                   \
    data = m_fileData->getData(Data_Type_Misc, id, &size, 0xFFFE);                      \
    if (data != NULL) {                                                                 \
        archive.setFileImage(data, size, Heaps::StageResource);                         \
    }

void stIce::createObj() {
    CameraController* camera = CameraController::getInstance();
    // MATCH-ONLY: the camera controller is a shadow without these fields (a flag byte and an angle)
    *(reinterpret_cast<u8*>(camera) + 0x44) |= 2;
    *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x190) = 1.308997f;
    int size;
    void* data;
    ICE_LOAD_ARCHIVE(m_archiveYasai0, 0x2711)
    ICE_LOAD_ARCHIVE(m_archiveYasai1, 0x2712)
    ICE_LOAD_ARCHIVE(m_archiveYasai2, 0x2713)
    ICE_LOAD_ARCHIVE(m_archiveYasai3, 0x2714)
    ICE_LOAD_ARCHIVE(m_archiveYasai4, 0x2715)
    ICE_LOAD_ARCHIVE(m_archiveYasai5, 0x2716)
    ICE_LOAD_ARCHIVE(m_archiveYasai6, 0x2717)
    ICE_LOAD_ARCHIVE(m_archiveYasai7, 0x2718)
    ICE_LOAD_ARCHIVE(m_archiveYasai8, 0x2719)
    ICE_LOAD_ARCHIVE(m_archiveYasai9, 0x271A)
    ICE_LOAD_ARCHIVE(m_archiveYasaiParam, 0x271B)
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0xD8);
    createObjBg(0);
    createObjYama(1);
    createObjRot(2);
    createObjBreak(3);
    createObjKumo(4);
    createObjKumo(5);
    createObjIce(6);
    createObjIce(7);
    createObjTurara(8);
    createObjTurara(9);
    createObjTurara(10);
    createObjFish(0xB);
    createObjWhiteBear(0xC);
    createObjWater(0xD);
    createObjWarning(0xE);
    createCollision(m_fileData, 2, NULL);
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
    initPosPokeTrainer(4, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, NULL);
    createObjPokeTrainer(m_fileData, 0x67, "PokeTrainer02", m_pokeTrainerPos + 4, NULL);
    createObjPokeTrainer(m_fileData, 0x68, "PokeTrainer03", m_pokeTrainerPos + 6, NULL);
}

void stIce::createObjBg(int index) {
    grIceBg* ground;
    switch (index) {
    case 0:
        ground = grIceBg::create(0, "TopN", "grIceMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxGimmickWork(reinterpret_cast<Matrix*>(m_mtxGimmick));
        ground->setPosFishWork(reinterpret_cast<Vec3f*>(m_posFish));
        ground->setStateWork(&m_part);
    }
}

void stIce::createObjYama(int index) {
    grIceYama* ground;
    switch (index) {
    case 1:
        ground = grIceYama::create(1, "A_M_top", "grIceYama");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxGimmickWork(reinterpret_cast<Matrix*>(m_mtxGimmick));
        ground->setPosBearWork(m_posBear);
        ground->setStateWork(&m_part);
        ground->setStateBearWork(&m_bearState);
    }
}

void stIce::createObjBreak(int index) {
    grIceBreak* ground;
    switch (index) {
    case 3:
        ground = grIceBreak::create(3, "P_kowareyuka", "grIceKowareYuka");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(reinterpret_cast<Matrix*>(m_mtxGimmick[7]));
    }
}

void stIce::createObjRot(int index) {
    grIceRot* ground;
    switch (index) {
    case 2:
        ground = grIceRot::create(2, "P_Kaitenyuka", "grIceKaitenYuka");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(reinterpret_cast<Matrix*>(m_mtxGimmick[6]));
    }
}

void stIce::createObjKumo(int index) {
    grIceKumo* ground;
    Matrix* mtx;
    u8* state;
    switch (index) {
    case 5:
        ground = grIceKumo::create(5, "P_Kumoyuka2", "grIceKumoYuka2");
        mtx = reinterpret_cast<Matrix*>(m_mtxGimmick[2]);
        state = &m_kumoStateB;
        break;
    case 4:
        ground = grIceKumo::create(4, "P_Kumoyuka1", "grIceKumoYuka1");
        mtx = reinterpret_cast<Matrix*>(m_mtxGimmick[0]);
        state = &m_kumoStateA;
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
        ground->setLimitWork(m_posLimit);
        ground->setStateWork(state);
    }
}

void stIce::createObjIce(int index) {
    grIceIce* ground;
    u8* state;
    switch (index) {
    case 7:
        ground = grIceIce::create(7, "Ryuuhyou2", "grIceRyuhyo2");
        state = &m_iceStateB;
        break;
    case 6:
        ground = grIceIce::create(6, "Ryuuhyou1", "grIceRyuhyo1");
        state = &m_iceStateA;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(reinterpret_cast<Matrix*>(m_mtxGimmick[4]));
        ground->setMtxGimmickWork(reinterpret_cast<Matrix*>(m_mtxGimmick[8]));
        ground->setLimitWork(m_posLimit);
        ground->setStateWork(state);
    }
}

void stIce::createObjTurara(int index) {
    grIceTurara* ground;
    u8 type;
    switch (index) {
    case 9:
        ground = grIceTurara::create(9, "Turara2", "grIceTurara");
        type = 1;
        break;
    case 8:
        ground = grIceTurara::create(8, "Turara1", "grIceTurara");
        type = 0;
        break;
    case 10:
        ground = grIceTurara::create(10, "Turara3", "grIceTurara");
        type = 2;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setMtxWork(reinterpret_cast<Matrix*>(m_mtxGimmick[11]));
        ground->setPosLimitWork(m_posLimit);
        ground->setStateWork(&m_turaraStateWork);
        ground->setType(type);
        ground->setHPWork(&m_turaraHP);
    }
}

void stIce::createObjFish(int index) {
    grIceFish* ground;
    switch (index) {
    case 0xB:
        ground = grIceFish::create(0xB, "StgIceFish_TopN", "grIceFish");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(reinterpret_cast<Vec3f*>(m_posFish));
        ground->setPosXWork(m_posFishX);
        ground->setLimitWork(m_posLimit);
        ground->setStateWork(&m_fishStateWork);
    }
}

void stIce::createObjWhiteBear(int index) {
    grIceBear* ground;
    switch (index) {
    case 0xC:
        ground = grIceBear::create(0xC, "StgIceWhitebear_TopN", "grIceBear");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posBear);
        ground->setLimitWork(m_posLimit);
        ground->setStateWork(&m_bearState);
    }
}

// The water: a ground with no model that only tells where the water is, and the area of the water for the fighters.
void stIce::createObjWater(int index) {
    Ground* ground;
    switch (index) {
    case 0xD:
        ground = grIce::create(0x14, "gr2_StgIceWater", "grIceWater");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        m_waterData = new (Heaps::StageInstance) grGimmickWaterData;
        if (m_waterData != NULL) {
            memset(m_waterData, 0, sizeof(grGimmickWaterData));
            m_waterData->m_swimHeight = 0.0f;
            m_waterData->m_canDrown = true;
            m_waterData->m_speed = 1.0f;
            m_waterData->m_areaData.m_offsetPos.m_x = 0.0f;
            m_waterData->m_areaData.m_offsetPos.m_y = -25.0f;
            m_waterData->m_areaData.m_range.m_x = 500.0f;
            m_waterData->m_areaData.m_range.m_y = 25.0f;
            m_triggerWater = g_stTriggerMng->createTrigger(Gimmick::Area_Water, -1);
            m_triggerWater->setWaterTrigger(m_waterData);
        }
    }
}

void stIce::createObjWarning(int index) {
    grIceWarning* ground;
    switch (index) {
    case 0xE:
        ground = grIceWarning::create(0x1E, "gr2_StgIceCaution", "grIceWarning");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateWork(&m_warningState);
    }
}

// In the devil mode (the match is of the kind 1) the camera is limited: it shows only a part of the picture and cannot go
// below the water.
void stIce::update(float deltaFrame) {
    if (m_isDevil == true) {
        // MATCH-ONLY: the original leaves the value unset when there is no camera
        float bottom;
        cmStageParam* param = &CameraController::getInstance()->m_stageCameraParam;
        if (param != NULL) {
            float cosine = nw4r::math::CosFIdx(10.666667f);
            float sine = nw4r::math::SinFIdx(10.666667f);
            bottom = -(0.25f * (param->m_verticalRotationFactor * (sine / cosine)));
        }
        setCameraLimitRange(-200.0f, 200.0f, 180.0f, bottom);
    } else {
        resetCameraLimitRange();
    }
    updateLimit(deltaFrame);
    updateScene(deltaFrame);
    updateTurara(deltaFrame);
    updateKumo(deltaFrame);
    updateIce(deltaFrame);
    updateWater(deltaFrame);
    updateFish(deltaFrame);
    updateYasai(deltaFrame);
}

void stIce::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    m_posLimit[0].m_x = camera->unk158;
    m_posLimit[0].m_y = camera->unk160;
    m_posLimit[0].m_z = 0.0f;
    m_posLimit[1].m_x = camera->unk15C;
    m_posLimit[1].m_y = camera->unk164;
    m_posLimit[1].m_z = 0.0f;
}

// The scene (the animation of the stage model) goes through the steps of the scroll: at frame 1800 the stage shows the top
// of the mountain (the second set of places), at frame 3050 the bears' ledge, and at frame 5640 the end of the scroll; the
// sign is shown for the time of the stage data before the end.
void stIce::updateScene(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(m_stageData);
    if (data != NULL) {
        float frame = 0.0f;
        if (g_gfSceneRoot->m_anmScnRes != NULL) {
            frame = g_gfSceneRoot->m_anmScnRes->GetFrame();
        }
        u8 scene = m_sceneState;
        if (scene == 5) {
            if (3050.0f <= frame) {
                m_bearState = 5;
                m_part = 3;
                void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                zoomInCamera();
                m_sceneState = 6;
            }
        } else if (scene < 5) {
            if (scene == 0) {
                m_part = 1;
                m_sceneState = 4;
            } else if (3 < scene && 1800.0f <= frame) {
                m_bearState = 0xC;
                m_part = 2;
                void* posData = m_fileData->getData(Data_Type_Model, 0x6E, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                zoomOutCamera(280.0f, 320.0f);
                m_sceneState = 5;
            }
        } else if (scene == 7) {
            if (frame < m_scenePrev) {
                m_part = 1;
                m_sceneState = 4;
            }
        } else if (scene < 7) {
            if (frame < 5640.0f) {
                if (5640.0f - data->unk00 <= frame && m_warningState == 0xE) {
                    m_warningTimer = data->unk04;
                    m_warningState = 5;
                }
            } else {
                m_part = 4;
                m_bearState = 0;
                m_sceneState = 7;
            }
        }
        if (m_warningState == 5) {
            float timer = m_warningTimer;
            m_warningTimer = timer - deltaFrame;
            if (timer - deltaFrame < 0.0f) {
                m_warningTimer = 0.0f;
            }
            if (m_warningTimer == 0.0f) {
                m_warningState = 0xE;
            }
        }
        frame = 0.0f;
        if (g_gfSceneRoot->m_anmScnRes != NULL) {
            frame = g_gfSceneRoot->m_anmScnRes->GetFrame();
        }
        m_scenePrev = frame;
    }
}

// The icicles: after the wait they come one after the other (the first, the second, the third), fall, and the wait starts
// again.
void stIce::updateTurara(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(m_stageData);
    if (data != NULL) {
        float timer = m_turaraTimer - deltaFrame;
        m_turaraTimer = timer;
        if (timer < 0.0f) {
            m_turaraTimer = 0.0f;
        }
        u8 state = m_turaraState;
        if (state != 2) {
            if (state < 2) {
                if (state == 0) {
                    float random = randf();
                    m_turaraTimer = data->unk28 + (data->unk2C - data->unk28) * random;
                    m_turaraHP = data->unk38;
                    m_turaraState = 1;
                } else if (m_turaraHP == 0.0f) {
                    m_turaraStateWork = 0xE;
                    m_turaraState = 0;
                } else if (m_turaraTimer == 0.0f) {
                    u8 work = m_turaraStateWork;
                    if (work == 10) {
                        m_turaraStateWork = 0xB;
                        m_turaraState = 3;
                    } else if (work < 10) {
                        if (work == 8) {
                            m_turaraStateWork = 9;
                        } else if (7 < work) {
                            m_turaraStateWork = 10;
                        }
                    } else if (work == 0xE) {
                        m_turaraStateWork = 8;
                    }
                    float random = randf();
                    m_turaraTimer = data->unk30 + (data->unk34 - data->unk30) * random;
                }
            } else if (state < 4 && m_turaraStateWork == 0xE) {
                m_turaraState = 0;
            }
        }
    }
}

// The clouds come in the part 1 of the mountain after a random wait; when both are gone the wait starts again.
void stIce::updateKumo(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(m_stageData);
    if (data != NULL) {
        float timer = m_kumoTimer - deltaFrame;
        m_kumoTimer = timer;
        if (timer < 0.0f) {
            m_kumoTimer = 0.0f;
        }
        u8 state = m_kumoState;
        if (state == 2) {
            if (m_kumoTimer == 0.0f) {
                m_kumoStateA = 5;
                m_kumoStateB = 5;
                m_kumoState = 3;
            }
        } else if (state < 2) {
            if (state == 0) {
                m_kumoState = 1;
            } else if (m_part == 1) {
                float random = randf();
                float high = data->unk78;
                float low = data->unk74;
                m_kumoState = 2;
                m_kumoTimer = low + (high - low) * random;
            }
        } else if (state < 4 && m_kumoStateA == 0xE && m_kumoStateB == 0xE) {
            m_kumoState = 0;
        }
    }
}

// The ice blocks come in the part 3: one of the two, chosen by chance.
void stIce::updateIce(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(m_stageData);
    if (data != NULL) {
        float timer = m_iceTimer - deltaFrame;
        m_iceTimer = timer;
        if (timer < 0.0f) {
            m_iceTimer = 0.0f;
        }
        u8 state = m_iceState;
        if (state == 2) {
            if (m_iceTimer == 0.0f) {
                if (0.5f <= randf()) {
                    m_iceStateB = 5;
                } else {
                    m_iceStateA = 5;
                }
                m_iceState = 3;
            }
        } else if (state < 2) {
            if (state == 0) {
                m_iceState = 1;
            } else if (m_part == 3) {
                float random = randf();
                float high = data->unkBC;
                float low = data->unkB8;
                m_iceState = 2;
                m_iceTimer = low + (high - low) * random;
            }
        } else if (state < 4 && m_iceStateA == 0xE && m_iceStateB == 0xE) {
            m_iceState = 0;
        }
    }
}

// The water goes up with the stage: the area of the water follows the height of its node, and the camera is not allowed to
// go below it.
void stIce::updateWater(float deltaFrame) {
    if (m_triggerWater != NULL && m_waterData != NULL && m_stageData != NULL) {
        Matrix* mtx = reinterpret_cast<Matrix*>(m_mtxGimmick[4]);
        float waterX = mtx->m[0][3];
        float waterY = mtx->m[1][3];
        float prev = m_waterPrev;
        m_waterData->m_swimHeight = waterY;
        m_waterData->m_speed = (waterX - prev) * 0.25f;
        grGimmickWaterData* water = m_waterData;
        water->m_areaData.m_range.m_x = (m_posLimit[1].m_x - m_posLimit[0].m_x) + 500.0f;
        float height = (waterY - m_posLimit[1].m_y) + 50.0f;
        water->m_areaData.m_range.m_y = height;
        water->m_areaData.m_offsetPos.m_x = 0.0f;
        water->m_areaData.m_offsetPos.m_y = waterY - height * 0.5f;
        m_triggerWater->setWaterParam(m_waterData);
        Vec2f corner1;
        Vec2f corner2;
        corner1.m_x = m_posLimit[0].m_x;
        m_waterPrev = waterX;
        corner1.m_y = waterY;
        corner2.m_x = m_posLimit[1].m_x;
        corner2.m_y = m_posLimit[1].m_y;
        if (waterY < corner2.m_y) {
            corner1.m_y = corner2.m_y;
        }
        m_dangerZone = g_aiMgr->setDangerZone(&corner1, &corner2, m_dangerZone, 0, 0);
        float tilt = 0.0f;
        Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x148);
        if (range.m_down < waterY) {
            range.m_down = waterY;
            cmStageParam* param = &CameraController::getInstance()->m_stageCameraParam;
            if (param != NULL) {
                float cosine = nw4r::math::CosFIdx(10.666667f);
                float sine = nw4r::math::SinFIdx(10.666667f);
                range.m_down = range.m_down - param->m_verticalRotationFactor * (sine / cosine) * 0.5f;
            }
            tilt = waterY - range.m_down;
        }
        CameraController::getInstance()->setCameraRange(&range);
        *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x18C) = tilt;
    }
}

// The fish: after the wait, when a fighter is in the water (or one that is outside of the part of the stage the ledges cover
// is below the water line), the fish chooses a fighter and jumps where it is.
void stIce::updateFish(float deltaFrame) {
    if (m_triggerWater == NULL) {
        return;
    }
    stIceData* data = static_cast<stIceData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    float timer = m_fishTimer - deltaFrame;
    m_fishTimer = timer;
    if (timer < 0.0f) {
        m_fishTimer = 0.0f;
    }
    u8 state = m_fishState;
    if (state != 2) {
        if (state < 2) {
            if (state == 0) {
                if (m_part == 3 && m_fishTimer == 0.0f) {
                    int entries[9];
                    memset(entries, 0, 0x24);
                    grGimmickWaterData* water = m_waterData;
                    Rect2D rect;
                    rect.m_left = water->m_areaData.m_offsetPos.m_x - water->m_areaData.m_range.m_x * 0.5f;
                    rect.m_right = water->m_areaData.m_offsetPos.m_x + water->m_areaData.m_range.m_x * 0.5f;
                    rect.m_up = water->m_areaData.m_offsetPos.m_y + water->m_areaData.m_range.m_y * 0.5f;
                    rect.m_down = water->m_areaData.m_offsetPos.m_y - water->m_areaData.m_range.m_y * 0.5f;
                    int count = enumIncludeEntryId__9ftManagerCFi(g_ftManager, &rect, entries, 0, 1);
                    if (0 < count) {
                        m_fishState = 1;
                    }
                }
            } else if (m_part == 3) {
                if (randf() <= data->unk88) {
                    u32 count = 0;
                    int found[4];
                    memset(found, -1, sizeof(found));
                    float waterY = reinterpret_cast<Matrix*>(m_mtxGimmick[4])->m[1][3];
                    float left = reinterpret_cast<Matrix*>(m_mtxGimmick[8])->m[0][3];
                    float right = reinterpret_cast<Matrix*>(m_mtxGimmick[10])->m[0][3];
                    int* slot = found;
                    for (int i = 0; i < 4; i++) {
                        Vec3f playerPos;
                        if (getPlayerPosition(i, &playerPos) == 1 && playerPos.m_y < waterY &&
                            (playerPos.m_x < left || right < playerPos.m_x)) {
                            *slot = i;
                            slot++;
                            count++;
                        }
                    }
                    if (count == 0) {
                        float random = randf();
                        m_fishState = 0;
                        m_fishTimer = data->unk80 + (data->unk84 - data->unk80) * random;
                    } else {
                        float random = randf();
                        int pick = (int)(random * (float)count);
                        if (pick < 0) {
                            pick = 0;
                        }
                        u32 chosen = count - 1;
                        if (pick < (int)(count - 1)) {
                            chosen = pick;
                        }
                        int fighter = found[chosen];
                        m_fishIndex = fighter;
                        Vec3f target;
                        getPlayerPosition(fighter, &target);
                        m_posFishX[0] = target.m_x;
                        m_posFishX[1] = target.m_y;
                        m_posFishX[2] = target.m_z;
                        m_fishStateWork = 5;
                        m_fishState = 3;
                    }
                } else {
                    float random = randf();
                    m_fishState = 0;
                    m_fishTimer = data->unk80 + (data->unk84 - data->unk80) * random;
                }
            }
        } else if (state < 4) {
            if (m_fishIndex != -1) {
                float left = reinterpret_cast<Matrix*>(m_mtxGimmick[8])->m[0][3];
                float right = reinterpret_cast<Matrix*>(m_mtxGimmick[10])->m[0][3];
                Vec3f target;
                if (!getPlayerPosition(m_fishIndex, &target)) {
                    m_fishIndex = -1;
                }
                m_posFishX[0] = target.m_x;
                m_posFishX[1] = target.m_y;
                m_posFishX[2] = target.m_z;
                float x = m_posFishX[0];
                if (left < x && x < right) {
                    if (right - x <= x - left) {
                        m_posFishX[0] = right;
                    } else {
                        m_posFishX[0] = left;
                    }
                }
            }
            if (m_fishStateWork != 5) {
                float random = randf();
                m_fishIndex = -1;
                m_fishState = 0;
                m_fishTimer = data->unk80 + (data->unk84 - data->unk80) * random;
            }
        }
    }
}

// The vegetables: after the wait one drops at a place on the line the stage gives (if no fighter is near it the chance is
// lower), when the items are on and the vegetable is one of them.
void stIce::updateYasai(float deltaFrame) {
    stIceData* data = static_cast<stIceData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    itManager* items = itManager::getInstance();
    if (items == NULL) {
        return;
    }
    float timer = m_yasaiTimer - deltaFrame;
    m_yasaiTimer = timer;
    if (timer < 0.0f) {
        m_yasaiTimer = 0.0f;
    }
    u8 state = m_yasaiState;
    if (state != 2) {
        if (state < 2) {
            stIceMeleeView* melee = reinterpret_cast<stIceMeleeView*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 8);
            if (state == 0) {
                if (g_GameGlobal->m_modeMelee != NULL) {
                    if (melee->m_gameMode == 4) {
                        m_yasaiState = 8;
                    } else {
                        float kind = randf() * 10.0f;
                        m_yasaiKind = (u8)(int)kind;
                        float random = randf();
                        float high = data->unk5C;
                        float low = data->unk58;
                        m_yasaiState = 1;
                        m_yasaiTimer = low + (high - low) * random;
                    }
                }
            } else if (m_yasaiTimer == 0.0f && g_GameGlobal->m_modeMelee != NULL) {
                if (melee->m_itemsOn == 0 || !fn_8005136C(Item_Stage_Yasai)) {
                    m_yasaiState = 3;
                } else {
                    u8 count = getItemPosCount();
                    if (count != 0) {
                        float random = randf();
                        u32 index = (u32)((float)count * random);
                        Vec3f start;
                        Vec3f end;
                        getItemPos(&start, &end, index & 0xFF);
                        Vec3f dir;
                        bool same = false;
                        dir.m_x = start.m_x - end.m_x;
                        dir.m_y = start.m_y - end.m_y;
                        dir.m_z = start.m_z - end.m_z;
                        if (__fabs(dir.m_x) < 1e-05f && __fabs(dir.m_y) < 1e-05f && __fabs(dir.m_z) < 1e-05f) {
                            same = true;
                        }
                        Vec3f pos;
                        if (same) {
                            pos.m_x = start.m_x;
                            pos.m_y = start.m_y;
                            pos.m_z = start.m_z;
                        } else {
                            float length = grIceVecLength(&dir);
                            dir.normalize();
                            float factor = length * randf();
                            dir.m_x = dir.m_x * factor;
                            dir.m_y = dir.m_y * factor;
                            dir.m_z = dir.m_z * factor;
                            pos.m_x = end.m_x + dir.m_x;
                            pos.m_y = end.m_y + dir.m_y;
                            pos.m_z = end.m_z + dir.m_z;
                        }
                        if (g_ftManager->searchNearFighter(0.0f, data->unk68, &pos, -1, 0) == NULL) {
                            if (randf() < data->unk60 * data->unk64) {
                                BaseItem* item = items->createItem(Item_Stage_Yasai, m_yasaiKind, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
                                if (item != NULL) {
                                    item->warp(&pos);
                                    m_yasaiState = 3;
                                }
                            }
                        } else if (randf() < data->unk60) {
                            BaseItem* item = items->createItem(Item_Stage_Yasai, m_yasaiKind, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
                            if (item != NULL) {
                                item->warp(&pos);
                                m_yasaiState = 3;
                            }
                        }
                    }
                }
            }
        } else if (state < 4) {
            m_yasaiState = 0;
        }
    }
}

// The items of the stage: the vegetable (the archive of the model depends on the kind of the vegetable).
void stIce::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) {
    if (itemID == Item_Stage_Yasai) {
        switch (variantID) {
        case 0:
            *brres = &m_archiveYasai0;
            break;
        case 1:
            *brres = &m_archiveYasai1;
            break;
        case 2:
            *brres = &m_archiveYasai2;
            break;
        case 3:
            *brres = &m_archiveYasai3;
            break;
        case 4:
            *brres = &m_archiveYasai4;
            break;
        case 5:
            *brres = &m_archiveYasai5;
            break;
        case 6:
            *brres = &m_archiveYasai6;
            break;
        case 7:
            *brres = &m_archiveYasai7;
            break;
        case 8:
            *brres = &m_archiveYasai8;
            break;
        case 9:
            *brres = &m_archiveYasai9;
            break;
        }
        *param = &m_archiveYasaiParam;
    }
}

GXColor stIce::getFinalTechniqColor() {
    return nw4r::ut::Color(0x14000496);
}
