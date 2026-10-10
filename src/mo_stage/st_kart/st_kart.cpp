#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_kart/gr_kart.h>
#include <st_kart/st_kart.h>

stClassInfoImpl<Stages::Kart, stKart> stKart::bss_loc_14;

stKart* stKart::create() {
    return new (Heaps::StageInstance) stKart;
}

stKart::stKart() : stMelee("stKart", Stages::Kart) {
    m_path = NULL;
    memset(m_limit, 0, sizeof(m_limit));
    m_unk1F4 = 10;
    m_unk1F5 = 10;
    memset(m_kart, 0, sizeof(m_kart));
    m_zoom = 0;
}

stKart::~stKart() {
    releaseArchive();
}

bool stKart::loading() {
    return true;
}

void stKart::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x4C);
    createObjPath();
    createObjBg(0);
    createCollision(m_fileData, 2, NULL);
    createObjMap(1);
    createObjWarning(2);
    createObjKart();
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData == NULL) {
        createStagePositions();
    } else {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x1E);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(2, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, NULL);
}

// The paths of the karts are in the first misc file of the stage.
void stKart::createObjPath() {
    m_path = NULL;
    grFixedPathCollection* path = static_cast<grFixedPathCollection*>(m_fileData->getData(Data_Type_Misc, 4, 0xFFFE));
    if (path != NULL) {
        path->relocation();
        m_path = path;
    }
}

void stKart::createObjBg(int index) {
    grKartBg* ground;
    switch (index) {
    case 0:
        ground = grKartBg::create(0, "TopN", "grKartBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateAIWork(&m_unk1F5);
    }
}

void stKart::createObjMap(int index) {
    grKart* ground;
    switch (index) {
    case 1:
        ground = grKart::create(0x14, "", "grKartMap");
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

void stKart::createObjWarning(int index) {
    grKartWarning* ground;
    switch (index) {
    case 2:
        ground = grKartWarning::create(0x1E, "gr2_StgKartCaution", "grKartWarning");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStateWork(&m_unk1F4);
    }
}

// Makes a kart, its attack and its icon for every player (the models of the karts are 1 - 4, the icons 11 - 14).
void stKart::createObjKart() {
    stKartData* data = static_cast<stKartData*>(m_stageData);
    if (data != NULL) {
        if (data->m_kartNum > 8) {
            data->m_kartNum = 8;
        }
        u8 kartNum = data->m_kartNum;
        s16 mdlIndex = 1;
        for (u8 team = 0; team != kartNum; team++) {
            createObjKart1(team, mdlIndex);
            createObjAttack(team, 0x28);
            createObjIcon(team, (s16)(mdlIndex + 10));
            mdlIndex++;
            if (mdlIndex == 5) {
                mdlIndex = 1;
            }
        }
    }
}

void stKart::createObjKart1(u8 team, int mdlIndex) {
    if (m_stageData != NULL) {
        grKartKart* ground = grKartKart::create(mdlIndex, "TopN", "grKartKart");
        if (ground != NULL) {
            addGround(ground);
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
            ground->setPathHeader(m_path);
            ground->setTeam(team);
            ground->setPosLimitWork(m_limit);
            ground->setKartData(m_kart);
        }
    }
}

void stKart::createObjAttack(int team, int mdlIndex) {
    grKartAttack* ground = grKartAttack::create(mdlIndex, "nodeIndex", "grKartAttack");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setKartData(&m_kart[team]);
    }
}

void stKart::createObjIcon(int team, int mdlIndex) {
    if (m_stageData == NULL) {
        return;
    }
    grKartIcon* ground;
    switch (mdlIndex) {
    case 0xB:
        ground = grKartIcon::create(0xB, "StgKartIcon01", "grKartIcon");
        break;
    case 0xC:
        ground = grKartIcon::create(mdlIndex, "StgKartIcon02", "grKartIcon");
        break;
    case 0xD:
        ground = grKartIcon::create(0xD, "StgKartIcon03", "grKartIcon");
        break;
    case 0xE:
        ground = grKartIcon::create(mdlIndex, "StgKartIcon04", "grKartIcon");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPathHeader(m_path);
        ground->setKartData(&m_kart[team]);
    }
}

void stKart::update(float deltaFrame) {
    if (m_isDevil == true) {
        // MATCH-ONLY: the original leaves the value unset when there is no camera
        float bottom;
        cmStageParam* param = &CameraController::getInstance()->m_stageCameraParam;
        if (param != NULL) {
            float cosine = nw4r::math::CosFIdx(10.666667f);
            float sine = nw4r::math::SinFIdx(10.666667f);
            bottom = -(0.25f * (param->m_verticalRotationFactor * (sine / cosine)));
        }
        setCameraLimitRange(-200.0f, 200.0f, 200.0f, bottom);
    } else {
        resetCameraLimitRange();
    }
    updateLimit(deltaFrame);
    updateKart(deltaFrame);
}

// The area the camera shows (the karts are put out of the picture when they leave it).
void stKart::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    float top = camera->unk160;
    m_limit[0].m_x = camera->unk158;
    m_limit[0].m_y = top;
    m_limit[0].m_z = 0.0f;
    float right = camera->unk164;
    m_limit[1].m_x = camera->unk15C;
    m_limit[1].m_y = right;
    m_limit[1].m_z = 0.0f;
}

// Ranks the karts, tells the grounds what the road and the warning do and zooms the camera.
void stKart::updateKart(float deltaFrame) {
    stKartData* data = static_cast<stKartData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    // MATCH-ONLY: the original reserves stack space for every kart here (nothing uses it)
    __alloca(data->m_kartNum * 4);
    bool allAhead = true;
    m_unk1F5 = 10;
    u8 kartNum = data->m_kartNum;
    for (u8 i = 0; i != kartNum; i++) {
        m_kart[i].m_rank = 0;
        if (m_kart[i].m_lap < 10) {
            allAhead = false;
        }
        if (m_kart[i].m_point > 0x14 && m_kart[i].m_point < 0x28 && m_kart[i].m_state == 1) {
            if (m_unk1F4 == 10) {
                m_unk1F4 = 0;
            }
            m_unk1F5 = 9;
        }
        if (m_kart[i].m_point > 0x5A && m_kart[i].m_state == 1) {
            m_unk1F5 = 8;
        }
    }
    kartNum = data->m_kartNum;
    for (u8 i = 0; i != kartNum; i++) {
        if (allAhead) {
            m_kart[i].m_lap -= 10;
        }
    }
    kartNum = data->m_kartNum;
    for (u8 i = 0; i != kartNum; i++) {
        u8 otherNum = data->m_kartNum;
        for (u8 j = 0; j != otherNum; j++) {
            if (i != j) {
                bool behind = false;
                if (m_kart[i].m_lap < m_kart[j].m_lap) {
                    behind = true;
                } else if (m_kart[i].m_lap == m_kart[j].m_lap) {
                    if (m_kart[i].m_point < m_kart[j].m_point) {
                        behind = true;
                    } else if (m_kart[i].m_point == m_kart[j].m_point && m_kart[i].m_rate < m_kart[j].m_rate) {
                        behind = true;
                    }
                }
                if (behind) {
                    m_kart[i].m_rank++;
                }
            }
        }
    }
    if (m_zoom == 1 && m_unk1F4 != 0) {
        zoomInCamera();
        m_zoom = 0;
    } else if (m_zoom == 0 && m_unk1F4 == 0) {
        zoomOutCamera(225.0f, 275.0f);
        m_zoom = 1;
    }
}

void stKart::renderDebug() { }

// HYPOTHESIS: the same fsel based clamp helper the glide statuses use.
static inline float kartClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// The light set depends on where the position is: over the road (or the gaps in it) it is dark, in the dips it is lit.
int stKart::getZoneLightSetIndex(Vec3f* position) {
    if (position == NULL) {
        return 0x14;
    }
    float x = position->m_x;
    float y = position->m_y;
    if (y < 0.0f) {
        return 0x14;
    }
    if (x < -250.0f) {
        return 0x14;
    }
    if (x > -45.0f && x < -21.0f) {
        return 0x14;
    }
    if (x > 21.0f && x < 45.0f) {
        return 0x14;
    }
    if (x > 250.0f) {
        return 0x14;
    }
    if (x >= -250.0f && x <= -100.0f && y <= 25.0f) {
        return 0x15;
    }
    if (x >= -100.0f && x <= -45.0f) {
        if (y <= 25.0f) {
            return 0x15;
        }
        if (y <= kartClamp((x - -100.0f) / 55.0f, 0.0f, 1.0f) * 15.0f + 25.0f) {
            return 0x15;
        }
    }
    if (x >= -21.0f && x <= 21.0f && y <= 50.0f) {
        return 0x15;
    }
    if (x >= 45.0f && x <= 100.0f) {
        if (y <= 25.0f) {
            return 0x15;
        }
        if (y <= kartClamp(1.0f - (x - 45.0f) / 55.0f, 0.0f, 1.0f) * 15.0f + 25.0f) {
            return 0x15;
        }
    }
    if (x >= 100.0f && x <= 250.0f && y <= 25.0f) {
        return 0x15;
    }
    return 0x14;
}
