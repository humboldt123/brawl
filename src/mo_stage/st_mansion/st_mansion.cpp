#include <cm/cm_camera_controller.h>
#include <cm/cm_quake.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

#include <st_mansion/gr_mansion.h>
#include <st_mansion/gr_mansion_area_break.h>
#include <st_mansion/gr_mansion_area_down.h>
#include <st_mansion/gr_mansion_area_up.h>
#include <st_mansion/gr_mansion_floor.h>
#include <st_mansion/gr_mansion_front.h>
#include <st_mansion/gr_mansion_ground.h>
#include <st_mansion/gr_mansion_pillar.h>
#include <st_mansion/gr_mansion_util.h>
#include <st_mansion/st_mansion.h>

stClassInfoImpl<Stages::Mansion, stMansion> stMansion::bss_loc_14;

stMansion* stMansion::create() {
    return new (Heaps::StageInstance) stMansion;
}

stMansion::stMansion() : stMelee("stMansion", Stages::Mansion) {
    memset(m_posLimit, 0, sizeof(m_posLimit));
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    m_phase = 0;
    m_timer = 0.0f;
    m_sceneFrame = 0.0f;
    m_scaleRate = 1.0f;
    m_eventState = 5;
    memset(m_posArea, 0, sizeof(m_posArea));
    memset(m_stateArea, 0, sizeof(m_stateArea));
    memset(m_posFloor, 0, sizeof(m_posFloor));
    memset(m_stateFloor, 0, sizeof(m_stateFloor));
    m_isEvent = 0;
}

stMansion::~stMansion() {
    releaseArchive();
}

bool stMansion::loading() {
    return true;
}

void stMansion::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x6C);
    createObjBg(0);
    createObjGround(1);
    createObjAshiba(2);
    createObjAshiba(3);
    createObjArea(4);
    createObjArea(5);
    createObjArea(6);
    createObjArea(7);
    createObjArea(8);
    createObjArea(9);
    createObjArea(10);
    createObjPillar(11);
    createObjPillar(12);
    createObjPillar(13);
    createObjPillar(14);
    createObjFront(15);
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
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    if (g_GameGlobal->m_modeMelee != NULL && mansionMeleeInitData()->m_gameMode == MANSION_MODE_EVENT && mansionMeleeInitData()->m_event == MANSION_EVENT_ID) {
        m_isEvent = 1;
    }
}

// The sky behind the house.
void stMansion::createObjBg(int index) {
    Ground* ground;
    switch (index) {
    case 0:
        ground = grMansion::create(0, "StgManshon00AmoonSky", "grMansionMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setDontMoveGround();
    }
}

// The house itself: its model has the two locators that the front reads.
void stMansion::createObjGround(int index) {
    grMansionGround* ground;
    switch (index) {
    case 1:
        ground = grMansionGround::create(1, "StgManshon00Ground", "grMansionGround");
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
        ground->setDontMoveGround();
    }
}

// The two ghost floors (left: 2, right: 3) that move up, down and to the side by themselves.
void stMansion::createObjAshiba(int index) {
    grMansionFloor* ground;
    Vec3f* pos;
    u8* state;
    int type;
    switch (index) {
    case 2:
        ground = grMansionFloor::create(3, "StgManshon00GhostFloorL", "grMansionAshibaL");
        pos = &m_posFloor[0];
        state = &m_stateFloor[0];
        type = 4;
        break;
    case 3:
        ground = grMansionFloor::create(4, "StgManshon00GhostFloorR", "grMansionAshibaR");
        pos = &m_posFloor[1];
        state = &m_stateFloor[1];
        type = 5;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setPosLimitWork(m_posLimit);
        ground->setStateWork(state);
        ground->setType(type);
    }
}

// The seven grounds of the four areas of the house: for each side an area that is up (the part above), one that is down (the
// part below) and one that breaks, and one that breaks all of them (index 10).
void stMansion::createObjArea(int index) {
    grMansionArea* ground;
    Vec3f* pos;
    u8* state;
    u8* stateSub;
    int type;
    int typeLR;
    int scaleRateFlg;
    switch (index) {
    case 4:
        ground = grMansionAreaUp::create(5, "StgManshon00LUClash", "grMansionL_Up");
        pos = &m_posArea[0];
        state = &m_stateArea[0];
        stateSub = NULL;
        type = 1;
        typeLR = 0;
        scaleRateFlg = 0;
        break;
    case 5:
        ground = grMansionAreaDown::create(7, "StgManshon00LDClash", "grMansionL_Down");
        pos = &m_posArea[1];
        state = &m_stateArea[1];
        stateSub = NULL;
        type = 0;
        typeLR = 0;
        scaleRateFlg = 0;
        break;
    case 6:
        ground = grMansionAreaBreak::create(8, "StgManshon00LDCollapse", "grMansionL_Break");
        pos = NULL;
        state = &m_stateArea[1];
        stateSub = &m_stateArea[3];
        type = 2;
        typeLR = 0;
        scaleRateFlg = 0;
        break;
    case 7:
        ground = grMansionAreaUp::create(9, "StgManshon00RUClash", "grMansionR_Up");
        pos = &m_posArea[2];
        state = &m_stateArea[2];
        stateSub = NULL;
        type = 1;
        typeLR = 1;
        scaleRateFlg = 0;
        break;
    case 8:
        ground = grMansionAreaDown::create(0xB, "StgManshon00RDClash", "grMansionR_Down");
        pos = &m_posArea[3];
        state = &m_stateArea[3];
        stateSub = NULL;
        type = 0;
        typeLR = 1;
        scaleRateFlg = 0;
        break;
    case 9:
        ground = grMansionAreaBreak::create(0xC, "StgManshon00RDCollapse", "grMansionR_Break");
        pos = NULL;
        state = &m_stateArea[3];
        stateSub = &m_stateArea[1];
        type = 2;
        typeLR = 1;
        scaleRateFlg = 0;
        break;
    case 10:
        ground = grMansionAreaBreak::create(0xD, "StgManshon00LRAllCollapse", "grMansionBreak");
        pos = NULL;
        state = &m_stateArea[0];
        stateSub = NULL;
        type = 3;
        typeLR = 2;
        scaleRateFlg = 1;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setStateWork(state);
        ground->setStateSubWork(stateSub);
        ground->setScaleRateWork(&m_scaleRate);
        ground->setType(type);
        ground->setTypeLR(typeLR);
        ground->setScaleRateFlg(scaleRateFlg);
        u32 node;
        switch (index) {
        case 4:
            ground->getNodeIndex(&node, 0, "pillarLU");
            ground->setPillarNode(node);
            break;
        case 5:
            ground->getNodeIndex(&node, 0, "pillarLD");
            ground->setPillarNode(node);
            break;
        case 6:
            break;
        case 7:
            ground->getNodeIndex(&node, 0, "pillarRU");
            ground->setPillarNode(node);
            break;
        case 8:
            ground->getNodeIndex(&node, 0, "pillarRD");
            ground->setPillarNode(node);
            break;
        }
    }
}

// The four pillars that hold the areas (11: up left, 12: down left, 13: up right, 14: down right).
void stMansion::createObjPillar(int index) {
    grMansionPillar* ground;
    Vec3f* pos;
    u8* state;
    int type;
    switch (index) {
    case 11:
        ground = grMansionPillar::create(0xE, "StgManshon00Pillar", "grMansionPillarLUp");
        pos = &m_posArea[0];
        state = &m_stateArea[0];
        type = 1;
        break;
    case 12:
        ground = grMansionPillar::create(0xE, "StgManshon00Pillar", "grMansionPillarLDown");
        pos = &m_posArea[1];
        state = &m_stateArea[1];
        type = 0;
        break;
    case 13:
        ground = grMansionPillar::create(0xE, "StgManshon00Pillar", "grMansionPillarRUp");
        pos = &m_posArea[2];
        state = &m_stateArea[2];
        type = 1;
        break;
    case 14:
        ground = grMansionPillar::create(0xE, "StgManshon00Pillar", "grMansionPillarRDown");
        pos = &m_posArea[3];
        state = &m_stateArea[3];
        type = 0;
        break;
    default:
        ground = NULL;
        pos = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
        ground->setStateWork(state);
        ground->setScaleRateWork(&m_scaleRate);
        ground->setType(type);
        ground->setScaleRateFlg(0);
    }
}

// The front of the house (the part that hides the fighters inside): it reads the places of the house and the steps of the
// areas.
void stMansion::createObjFront(int index) {
    grMansionFront* ground;
    switch (index) {
    case 15:
        ground = grMansionFront::create(2, "StgManshon00Front", "grMansionFront");
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
        ground->setStateWork(m_stateArea);
        ground->setStateAreaWork(&m_eventState);
    }
}

void stMansion::update(float deltaFrame) {
    updateLimit();
    updatePillar(deltaFrame);
    updateArea(deltaFrame);
}

// The left/top and right/bottom limit of the camera.
void stMansion::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    // BYTES: the x limits are declared before the y loads so their float registers match.
    float left, right;
    float top = camera->unk160;
    left = camera->unk158;
    m_posLimit[0].m_x = left;
    m_posLimit[0].m_y = top;
    m_posLimit[0].m_z = 0.0f;
    float bottom = camera->unk164;
    right = camera->unk15C;
    m_posLimit[1].m_x = right;
    m_posLimit[1].m_y = bottom;
    m_posLimit[1].m_z = 0.0f;
}

void stMansion::updatePillar(float deltaFrame) { }

// The collapse of the house. All four areas are in place (phase 0), then they wait until they all fell (phase 1: the pillars
// break them one by one), then the house shakes for a while (phase 2), and the areas come back (phase 3). The light of the
// scene follows the areas that are down.
void stMansion::updateArea(float deltaFrame) {
    stMansionData* data = static_cast<stMansionData*>(m_stageData);
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        bool setScene = true;
        switch (m_phase) {
        case 0:
            m_stateArea[0] = 0;
            m_stateArea[1] = 0;
            m_stateArea[2] = 0;
            m_stateArea[3] = 0;
            m_scaleRate = 1.0f;
            m_phase = 1;
            break;
        case 1:
            if (m_stateArea[0] == 2 && m_stateArea[1] == 2 && m_stateArea[2] == 2 && m_stateArea[3] == 2) {
                m_stateArea[0] = 3;
                m_stateArea[1] = 3;
                m_stateArea[2] = 3;
                m_stateArea[3] = 3;
                m_scaleRate = 0.0f;
                m_timer = data->respawnFrames;
                m_phase = 2;
            }
            break;
        case 2:
            if (m_timer == 0.0f) {
                m_stateArea[0] = 4;
                setScene = false;
                m_stateArea[1] = 4;
                m_stateArea[2] = 4;
                m_stateArea[3] = 4;
                m_timer = data->respawnAnimFrames;
                g_gfSceneRoot->setCurrentFrame(10.0f);
                m_phase = 3;
            }
            break;
        case 3:
            if (m_timer == 0.0f) {
                m_phase = 0;
            } else {
                float rate = 1.0f - m_timer / data->respawnAnimFrames;
                if (rate < 0.0f) {
                    rate = 0.0f;
                }
                if (rate > 1.0f) {
                    rate = 1.0f;
                }
                m_scaleRate = rate;
                setScene = false;
                Vec3f offset(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_M, &offset);
            }
            break;
        }
        if (setScene == 1) {
            // which of the nine pictures of the scene shows the areas that are down
            float frame = 0.0f;
            if (m_stateArea[0] == 2 && m_stateArea[2] == 2) {
                frame = 3.0f;
            } else if (m_stateArea[0] == 2 && m_stateArea[1] == 2) {
                frame = 7.0f;
            } else if (m_stateArea[2] == 2 && m_stateArea[3] == 2) {
                frame = 8.0f;
            } else if (m_stateArea[0] == 2) {
                frame = 1.0f;
            } else if (m_stateArea[2] == 2) {
                frame = 2.0f;
            }
            if (frame == 3.0f && m_stateArea[1] == 2) {
                frame = 4.0f;
            } else if (frame == 3.0f && m_stateArea[3] == 2) {
                frame = 5.0f;
            }
            if (m_stateArea[1] == 3 && m_stateArea[3] == 3) {
                frame = 6.0f;
            }
            g_gfSceneRoot->setCurrentFrame(frame);
            m_sceneFrame = 0.0f;
        } else {
            float now = 0.0f;
            if (g_gfSceneRoot->m_anmScnRes != NULL) {
                now = g_gfSceneRoot->m_anmScnRes->GetFrame();
            }
            if (now < m_sceneFrame) {
                g_gfSceneRoot->setCurrentFrame(m_sceneFrame);
            } else {
                now = 0.0f;
                if (g_gfSceneRoot->m_anmScnRes != NULL) {
                    now = g_gfSceneRoot->m_anmScnRes->GetFrame();
                }
                m_sceneFrame = now;
            }
        }
    }
}

void stMansion::notifyEventInfoGo() {
    m_eventState = 0;
}

// The light set of the zone: inside the house the light is the dark one, unless the area of that corner fell.
u32 stMansion::getZoneLightSetIndex(Vec3f* position) {
    if (position == NULL) {
        return 0x15;
    }
    float x = position->m_x;
    float y = position->m_y;
    if (x < -75.0f) {
        return 0x15;
    }
    if (x > 75.0f) {
        return 0x15;
    }
    if (y < -10.0f) {
        return 0x15;
    }
    if (y > 99.0f) {
        return 0x15;
    }
    if (x <= 0.0f) {
        if (y >= 33.0f) {
            if (static_cast<u8>(m_stateArea[0] + 0xFE) <= 1) {
                return 0x15;
            }
        } else if (static_cast<u8>(m_stateArea[1] + 0xFE) <= 1) {
            return 0x15;
        }
    } else if (y >= 33.0f) {
        if (static_cast<u8>(m_stateArea[2] + 0xFE) <= 1) {
            return 0x15;
        }
    } else if (static_cast<u8>(m_stateArea[3] + 0xFE) <= 1) {
        return 0x15;
    }
    return 0x14;
}

// The event ends when one of the areas came back (its state is 1).
bool stMansion::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_isEvent == 0) {
        return false;
    }
    for (u8 i = 0; i < 4; i++) {
        if (m_stateArea[i] == 1) {
            *eventState = 6;
            *eventDecision = 3;
            return true;
        }
    }
    return false;
}
