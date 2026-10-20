#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <gf/gf_archive.h>
#include <gr/collision/gr_collision.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_gw/gr_gw_alarm.h>
#include <st_gw/gr_gw_chef_etc.h>
#include <st_gw/gr_gw_fire_diver.h>
#include <st_gw/gr_gw_fire_etc.h>
#include <st_gw/gr_gw_fire_manhore.h>
#include <st_gw/gr_gw_fire_mistake.h>
#include <st_gw/gr_gw_fire_smoke.h>
#include <st_gw/gr_gw_fire_spring.h>
#include <st_gw/gr_gw_lion_keeper.h>
#include <st_gw/gr_gw_oil_guest.h>
#include <st_gw/gr_gw_scene.h>
#include <st_gw/gr_gw_scene_chef.h>
#include <st_gw/gr_gw_scene_fire.h>
#include <st_gw/gr_gw_scene_lion.h>
#include <st_gw/gr_gw_scene_oil.h>
#include <st_gw/gr_gw_step.h>
#include <st_gw/st_gw.h>

// HYPOTHESIS: tear down and validity check of a stCollisionWork (unnamed functions of sora_melee).
extern "C" void fn_27_239F6C(stCollisionWork* work);
extern "C" int fn_27_239FD8(stCollisionWork* work);

// MATCH-ONLY: the data pointers of stCollisionWork are private in the header.
struct stCollisionWorkView {
    grCollData::VtxData* m_vtxDatas;
    grCollData::LineData* m_lineDatas;
    grCollData::JointData* m_jointDatas;
};

// MATCH-ONLY: views of the node data of a model; the header names neither the name offset (+8) nor the id (+0xC).
static inline const char* gwNodeName(nw4r::g3d::ResNode& node) {
    u8* data = reinterpret_cast<u8*>(node.ptr());
    s32 offset = *reinterpret_cast<s32*>(data + 8);
    return offset != 0 ? reinterpret_cast<const char*>(data + offset) : NULL;
}

stClassInfoImpl<Stages::GW, stGW> stGW::bss_loc_14;

// MATCH-ONLY: the first word of the AI camera controller after its base holds flags of which only the top byte is used here.
static inline u32* gwCameraFlags() {
    return reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_cmAIController) + 8);
}

stGW* stGW::create() {
    return new (Heaps::StageInstance) stGW;
}

stGW::stGW() : stMelee("stGW", Stages::GW) {
    m_unk1E8 = 0;
    m_event = 1;
    m_sceneState = 0;
    m_sceneTimer = 0.0f;
    m_scene = 0;
    m_sceneQueue[0] = 4;
    m_sceneQueue[1] = 4;
    m_sceneQueue[2] = 4;
    memset(m_limit, 0, sizeof(m_limit));
    m_collWork.initialize();
    m_collWork.m_vtxLen = 2;
    m_collWork.m_isClosed = false;
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    m_ctrlStep = 0;
    m_stepManhore = 0xFF;
    m_typeManhore = 0x10;
    m_stateMistake[0] = 0xB;
    m_stateMistake[1] = 0xB;
    m_stateMistake[2] = 0xB;
    m_indexVictim = 0;
    _310[0] = 0;
    _310[1] = 0;
    memset(&m_posFighter, 0, sizeof(m_posFighter));
    m_indexFighter = 0xFF;
    memset(m_posDiver, 0, sizeof(m_posDiver));
    m_typeDiver = 0x10;
    memset(m_posKeeperL, 0, sizeof(m_posKeeperL));
    memset(m_posKeeperR, 0, sizeof(m_posKeeperR));
    memset(&m_dataGuest[0], 0, sizeof(m_dataGuest[0].unk0));
    m_dataGuest[0].unk61 = 0;
    m_dataGuest[0].unk60 = 0xD;
    memset(&m_dataGuest[1], 0, sizeof(m_dataGuest[1].unk0));
    m_dataGuest[1].unk60 = 0xD;
    m_dataGuest[1].unk61 = 0;
    m_tblStep = NULL;
    m_springData = NULL;
    if (g_cmAIController != NULL) {
        *gwCameraFlags() = (*gwCameraFlags() & 0xFFFFFF) | 0x1000000;
    }
}

stGW::~stGW() {
    if (g_cmAIController != NULL) {
        *gwCameraFlags() = *gwCameraFlags() & 0xFFFFFF;
    }
    if (m_springData != NULL) {
        delete m_springData;
    }
    m_springData = NULL;
    fn_27_239F6C(&m_collWork);
    if (m_tblStep != NULL) {
        delete m_tblStep;
    }
    m_tblStep = NULL;
    releaseArchive();
}

bool stGW::loading() {
    return true;
}

void stGW::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x8C);
    initStageDataTbl();
    createObjBg(0);
    createObjFire();
    createObjChef();
    createObjLion();
    createObjOil();
    createObjFireSub();
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
    createObjPokeTrainer(m_fileData, 0x6E, "PokeTrainer00", m_pokeTrainerPos, NULL);
}

// The model of the building that all scenes share (the Game & Watch "body").
void stGW::createObjBg(int index) {
    grGW* ground;
    switch (index) {
    case 0:
        ground = grGW::create(1, "_GAME_WATCH_BODY", "grGWMainBg");
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

// A scene: the model that has the steps of the sequence of the scene. It gets the work of the stage (the scene, the timer, the
// event, the tables); the fire scene also gets the collision of the steps.
void stGW::createObjScene(int index) {
    u8 sceneId;
    grGWScene* ground;
    switch (index) {
    case 0x13:
        ground = grGWSceneChef::create(10, "_CHIF", "grGWSceneChef");
        sceneId = 1;
        break;
    case 1:
        ground = grGWSceneFire::create(0x14, "_FIRE", "grGWSceneFire");
        sceneId = 0;
        break;
    case 0x16:
        ground = grGWSceneLion::create(0x28, "_LION", "grGWSceneLion");
        sceneId = 2;
        break;
    case 0x17:
        ground = grGWScene::create(0x2C, "gr2_LION_BUCKGROUND", "grGWSceneLionGR2");
        sceneId = 2;
        break;
    case 0x1B:
        ground = grGWSceneOil::create(0x32, "_OIL", "grGWSceneOil");
        sceneId = 3;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(sceneId);
        ground->setFrameWork(&m_sceneTimer);
        ground->setEventWork(&m_event);
        switch (index) {
        case 1: {
            grGWSceneFire* fire = static_cast<grGWSceneFire*>(ground);
            fire->setPosGimmickWork(m_posGimmick);
            fire->setCtrlStepWork(&m_ctrlStep);
            fire->setTblStepAcc(m_tblStep);
            fire->setStepManhoreWork(&m_stepManhore);
            fire->setTypeManhoreWork(&m_typeManhore);
            fire->setPosDiverWork(m_posDiver);
            fire->setTypeDiverWork(&m_typeDiver);
            fire->setPosLimitWork(m_limit);
            grCollision* collision = NULL;
            if (&m_collWork != NULL && ground != NULL) {
                grCollData* collData = new (Heaps::StageInstance) grCollData;
                if (collData != NULL) {
                    initCollDatafromCollWorkStep(collData, &m_collWork, ground);
                    collision = grCollision::create(this, collData, ground, 1);
                    setCollision(collision);
                    delete collData;
                }
            }
            break;
        }
        case 0x16:
            static_cast<grGWSceneLion*>(ground)->setPosKeeperLWork(m_posKeeperL);
            static_cast<grGWSceneLion*>(ground)->setPosKeeperRWork(m_posKeeperR);
            break;
        case 0x1B:
            static_cast<grGWSceneOil*>(ground)->setDataGuestManWork(&m_dataGuest[0]);
            static_cast<grGWSceneOil*>(ground)->setDataGuestWomanWork(&m_dataGuest[1]);
            break;
        }
    }
}

void grGW::setSceneIDWork(u8* scene) {
    m_sceneIdWork = scene;
}

void grGW::setSceneID(u8 scene) {
    m_sceneId = scene;
}

void grGWScene::setFrameWork(float* frame) {
    m_frameWork = frame;
}

void grGWScene::setEventWork(u8* event) {
    m_eventWork = event;
}

void grGWScene::setPosGimmickWork(Vec3f* positions) {
    m_posGimmickWork = positions;
}

void grGWScene::setCtrlStepWork(u8* control) {
    m_ctrlStepWork = control;
}

void grGWScene::setTblStepAcc(stDataMultiContainer* table) {
    m_tblStep = table;
}

void grGWSceneFire::setStepManhoreWork(u8* step) {
    m_stepManhoreWork = step;
}

void grGWSceneFire::setTypeManhoreWork(u8* type) {
    m_typeManhoreWork = type;
}

void grGWSceneFire::setPosDiverWork(Vec3f* pos) {
    m_posDiverWork = pos;
}

void grGWSceneFire::setTypeDiverWork(u8* type) {
    m_typeDiverWork = type;
}

void grGWSceneFire::setPosLimitWork(Vec3f* limit) {
    m_posLimitWork = limit;
}

// The alarm clock of a scene.
void stGW::createObjAlarm(int index) {
    grGWAlarm* ground;
    u8 sceneId;
    switch (index) {
    case 0x14:
        ground = grGWAlarm::create(0xB, "_CHIF_ALARM", "grGWAlarmChef");
        sceneId = 1;
        break;
    case 0x18:
        ground = grGWAlarm::create(0x29, "_LION_ALARM", "grGWAlarmLion");
        sceneId = 2;
        break;
    case 0x1C:
        ground = grGWAlarm::create(0x33, "_OIL_ALARM", "grGWAlarmOil");
        sceneId = 3;
        break;
    case 0x21:
        ground = grGWAlarm::create(0x15, "_FIRE_ALARM", "grGWAlarmFire");
        sceneId = 0;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(sceneId);
        ground->setFrameWork(&m_sceneTimer);
    }
}

void grGWAlarm::setFrameWork(float* frame) {
    m_frameWork = frame;
}

void stGW::createObjChef() {
    createObjScene(0x13);
    createObjAlarm(0x14);
    createObjChefEtc(0x15);
}

void stGW::createObjChefEtc(int index) {
    grGWChefEtc* ground;
    switch (index) {
    case 0x15:
        ground = grGWChefEtc::create(0xC, "_CHIF_ETC", "grGWChefEtc");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(1);
        ground->setPosLimitWork(m_limit);
    }
}

void grGWChefEtc::setSceneIDWork(u8* scene) {
    m_sceneWork = scene;
}

void grGWChefEtc::setSceneID(u8 scene) {
    m_sceneNo = scene;
}

void grGWChefEtc::setPosLimitWork(Vec3f* limit) {
    m_posLimitWork = limit;
}

void stGW::createObjFire() {
    createObjScene(1);
}

void stGW::createObjFireSub() {
    createObjAlarm(0x21);
    createObjFireStep();
    createObjFireManhore(0xB);
    createObjFireManhore(0xC);
    createObjFireManhore(0xD);
    createObjFireManhore(0xE);
    createObjFireSmoke(10);
    createObjFireEtc(8);
    createObjFireFighter(9);
    createObjFireMistake(0xF);
    createObjFireMistake(0x10);
    createObjFireMistake(0x11);
    createObjFireSpring();
    createObjFireDiver(2);
    createObjFireDiver(3);
    createObjFireDiver(4);
    createObjFireDiver(5);
    createObjFireDiver(6);
    createObjFireDiver(7);
}

// The twenty steps (places where the fire man may stand) of the fire scene.
void stGW::createObjFireStep() {
    for (int i = 0; i < 20; i++) {
        grGWStep* ground = grGWStep::create(0x25, "_FIRE_STEP", "grGWStepFire");
        if (ground == NULL) {
            break;
        }
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setPosWork(&m_posGimmick[i]);
        ground->setCtrlStepWork(&m_ctrlStep);
        ground->setTblStepAcc(m_tblStep);
        ground->setID(i);
        ground->setEventWork(&m_event);
    }
}

void grGWStep::setPosWork(Vec3f* pos) {
    m_posWork = pos;
}

void grGWStep::setCtrlStepWork(u8* control) {
    m_ctrlStepWork = control;
}

void grGWStep::setTblStepAcc(stDataMultiContainer* table) {
    m_tblStep = table;
}

void grGWStep::setID(u32 id) {
    m_id = id;
}

void grGWStep::setEventWork(u8* event) {
    m_eventWork = event;
}

void stGW::createObjFireManhore(int index) {
    grGWFireManhore* ground;
    u8 type;
    switch (index) {
    case 0xB:
        ground = grGWFireManhore::create(0x1E, "_FIRE_MANHORE_01", "grGWFireManhore01");
        type = 6;
        break;
    case 0xC:
        ground = grGWFireManhore::create(0x1F, "_FIRE_MANHORE_02", "grGWFireManhore02");
        type = 7;
        break;
    case 0xD:
        ground = grGWFireManhore::create(0x20, "_FIRE_MANHORE_03", "grGWFireManhore03");
        type = 8;
        break;
    case 0xE:
        ground = grGWFireManhore::create(0x21, "_FIRE_MANHORE_04", "grGWFireManhore04");
        type = 9;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setPosWork(m_posGimmick);
        ground->setStepWork(&m_stepManhore);
        ground->setCtrlStepWork(&m_ctrlStep);
        ground->setTypeWork(&m_typeManhore);
        ground->setType(type);
        ground->setEventWork(&m_event);
    }
}

void grGWFireManhore::setPosWork(Vec3f* pos) {
    m_posWork = pos;
}

void grGWFireManhore::setStepWork(u8* step) {
    m_stepWork = step;
}

void grGWFireManhore::setCtrlStepWork(u8* control) {
    m_ctrlStepWork = control;
}

void grGWFireManhore::setTypeWork(u8* type) {
    m_typeWork = type;
}

void grGWFireManhore::setType(u8 type) {
    m_type = type;
}

void grGWFireManhore::setEventWork(u8* event) {
    m_eventWork = event;
}

void stGW::createObjFireSmoke(int index) {
    grGWFireSmoke* ground;
    switch (index) {
    case 10:
        ground = grGWFireSmoke::create(0x1D, "_FIRE_GIMMICK_SMOKE", "grGWFireSmoke");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
    }
}

void stGW::createObjFireEtc(int index) {
    grGWFireEtc* ground;
    switch (index) {
    case 8:
        ground = grGWFireEtc::create(0x1C, "_FIRE_ETC", "grGWFireEtc");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setStateMistakeWork(m_stateMistake);
        ground->setEventWork(&m_event);
        ground->setIndexWork(&m_indexVictim);
        ground->setIndexFighterWork(&m_indexFighter);
    }
}

void stGW::createObjFireFighter(int index) {
    grGWFireFighter* ground;
    switch (index) {
    case 9:
        ground = grGWFireFighter::create(0x26, "_FIRE_FIREFIGHTER", "grGWFireFighter");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setEventWork(&m_event);
        ground->setPosWork(&m_posFighter);
        ground->setIndexWork(&m_indexFighter);
        ground->setIndexVictimWork(&m_indexVictim);
    }
}

void stGW::createObjFireMistake(int index) {
    grGWFireMistake* ground;
    u8* state;
    switch (index) {
    case 0xF:
        ground = grGWFireMistake::create(0x22, "_FIRE_MISTAKE_01", "grGWFireMistake01");
        state = &m_stateMistake[0];
        break;
    case 0x10:
        ground = grGWFireMistake::create(0x23, "_FIRE_MISTAKE_02", "grGWFireMistake02");
        state = &m_stateMistake[1];
        break;
    case 0x11:
        ground = grGWFireMistake::create(0x24, "_FIRE_MISTAKE_03", "grGWFireMistake03");
        state = &m_stateMistake[2];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setStateWork(state);
        ground->setEventWork(&m_event);
    }
}

void grGWFireMistake::setStateWork(u8* state) {
    m_stateWork = state;
}

void grGWFireMistake::setEventWork(u8* event) {
    m_eventWork = event;
}

// The trampoline of the fire scene: the data (a spring that bounces 3.5 and is 2 units of the model) is made here.
void stGW::createObjFireSpring() {
    m_springData = new (Heaps::StageResource) grGimmickSpringData;
    memset(m_springData, 0, sizeof(grGimmickSpringData));
    m_springData->m_pos.m_x = 0.0f;
    m_springData->m_pos.m_y = 0.0f;
    m_springData->m_bounce = 3.5f;
    m_springData->m_mdlIndex = 2;
    grGimmickSpringData* data = m_springData;
    data->m_areaData.m_offsetPos.m_x = 1.0f;
    data->m_areaData.m_offsetPos.m_y = 9.0f;
    data->m_areaData.m_range.m_x = 24.0f;
    data->m_areaData.m_range.m_y = 7.5f;
    grGWFireSpring* ground = grGWFireSpring::create(2, "nodeIndex", "grGWFireSpring");
    if (ground != NULL) {
        ground->setGimmickData(m_springData);
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setEventWork(&m_event);
        ground->setPosWork(&m_posFighter);
    }
}

void grGWFireSpring::setSceneIDWork(u8* scene) {
    m_sceneIdWork = scene;
}

void grGWFireSpring::setSceneID(u8 scene) {
    m_sceneId = scene;
}

void grGWFireSpring::setEventWork(u8* event) {
    m_eventWork = event;
}

void grGWFireSpring::setPosWork(Vec3f* pos) {
    m_posWork = pos;
}

void stGW::createObjFireDiver(int index) {
    grGWFireDiver* ground;
    u8 type;
    switch (index) {
    case 2:
        ground = grGWFireDiver::create(0x16, "_FIRE_DIVER_LEFT_01", "grGWFireDiverL01");
        type = 10;
        break;
    case 3:
        ground = grGWFireDiver::create(0x17, "_FIRE_DIVER_LEFT_02", "grGWFireDiverL02");
        type = 11;
        break;
    case 4:
        ground = grGWFireDiver::create(0x18, "_FIRE_DIVER_LEFT_03", "grGWFireDiverL03");
        type = 12;
        break;
    case 5:
        ground = grGWFireDiver::create(0x19, "_FIRE_DIVER_RIGHT_01", "grGWFireDiverR01");
        type = 13;
        break;
    case 6:
        ground = grGWFireDiver::create(0x1A, "_FIRE_DIVER_RIGHT_02", "grGWFireDiverR02");
        type = 14;
        break;
    case 7:
        ground = grGWFireDiver::create(0x1B, "_FIRE_DIVER_RIGHT_03", "grGWFireDiverR03");
        type = 15;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(0);
        ground->setPosWork(m_posDiver);
        ground->setEventWork(&m_event);
        ground->setTypeWork(&m_typeDiver);
        ground->setType(type);
    }
}

void grGWFireDiver::setPosWork(Vec3f* pos) {
    m_posWork = pos;
}

void grGWFireDiver::setEventWork(u8* event) {
    m_eventWork = event;
}

void grGWFireDiver::setTypeWork(u8* type) {
    m_typeWork = type;
}

void grGWFireDiver::setType(u8 type) {
    m_type = type;
}

void stGW::createObjLion() {
    createObjScene(0x16);
    createObjScene(0x17);
    createObjAlarm(0x18);
    createObjLionKeeper(0x19);
    createObjLionKeeper(0x1A);
}

void stGW::createObjLionKeeper(int index) {
    grGWLionKeeper* ground;
    Vec3f* pos;
    u8 type;
    switch (index) {
    case 0x19:
        ground = grGWLionKeeper::create(0x2A, "_LION_KEEPER_LEFT", "grGWLionKeeperL");
        pos = m_posKeeperL;
        type = 4;
        break;
    case 0x1A:
        ground = grGWLionKeeper::create(0x2B, "_LION_KEEPER_RIGHT", "grGWLionKeeperR");
        pos = m_posKeeperR;
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
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(2);
        ground->setPosWork(pos);
        ground->setType(type);
    }
}

void grGWLionKeeper::setSceneIDWork(u8* scene) {
    m_sceneWork = scene;
}

void grGWLionKeeper::setSceneID(u8 scene) {
    m_sceneNo = scene;
}

void grGWLionKeeper::setPosWork(Vec3f* pos) {
    m_posWork = pos;
}

void grGWLionKeeper::setType(u8 type) {
    m_type = type;
}

void stGW::createObjOil() {
    createObjScene(0x1B);
    createObjAlarm(0x1C);
    createObjOilGuest(0x1D);
    createObjOilGuest(0x1E);
    createObjOilGuest(0x1F);
    createObjOilGuest(0x20);
}

void stGW::createObjOilGuest(int index) {
    grGWOilGuest* ground;
    grGWGuestData* data;
    u8 type;
    switch (index) {
    case 0x1D:
        ground = grGWOilGuest::create(0x34, "_OIL_MAN", "grGWOilMan");
        data = &m_dataGuest[0];
        type = 0;
        break;
    case 0x1E:
        ground = grGWOilGuestAngerM::create(0x35, "_OIL_MAN_ANGER", "grGWOilManAnger");
        data = &m_dataGuest[0];
        type = 1;
        break;
    case 0x1F:
        ground = grGWOilGuest::create(0x36, "_OIL_WOMAN", "grGWOilWoman");
        data = &m_dataGuest[1];
        type = 2;
        break;
    case 0x20:
        ground = grGWOilGuestAngerW::create(0x37, "Woman__Anger", "grGWOilWomanAnger");
        data = &m_dataGuest[1];
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
        ground->setSceneIDWork(&m_scene);
        ground->setSceneID(3);
        ground->setDataGuestWork(data);
        ground->setType(type);
    }
}

void grGWOilGuest::setSceneIDWork(u8* scene) {
    m_sceneWork = scene;
}

void grGWOilGuest::setSceneID(u8 scene) {
    m_sceneNo = scene;
}

void grGWOilGuest::setDataGuestWork(grGWGuestData* data) {
    m_dataGuest = data;
}

void grGWOilGuest::setType(u8 type) {
    m_type = type;
}

void stGW::update(float deltaFrame) {
    updateLimit(deltaFrame);
    updateScene(deltaFrame);
}

void stGW::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    float minX, minY, maxX, maxY;
    minY = camera->unk160;
    minX = camera->unk158;
    m_limit[0].m_x = minX;
    m_limit[0].m_y = minY;
    m_limit[0].m_z = 0.0f;
    maxY = camera->unk164;
    maxX = camera->unk15C;
    m_limit[1].m_x = maxX;
    m_limit[1].m_y = maxY;
    m_limit[1].m_z = 0.0f;
}

// The scene state machine: it waits (the first two numbers of the stage data are the least / most frames), then changes
// the scene and loads the positions of the new scene (the model "scene + 100"), plays it (the next two numbers) and
// goes back to the first scene.
void stGW::updateScene(float deltaFrame) {
    float* data = static_cast<float*>(m_stageData);
    if (data != NULL) {
        m_sceneTimer -= deltaFrame;
        if (m_sceneTimer < 0.0f) {
            m_sceneTimer = 0.0f;
        }
        switch (m_sceneState) {
        case 0:
            changeScene(0);
            m_typeManhore = 0x10;
            m_sceneState = 1;
            break;
        case 1: {
            float r = randf();
            m_sceneState = 2;
            m_sceneTimer = data[0] + (data[1] - data[0]) * r;
            break;
        }
        case 2:
            if (m_sceneTimer == 0.0f) {
                selectScene();
                void* posData = m_fileData->getData(Data_Type_Model, m_scene + 100, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                m_event = 1;
                m_sceneState = 3;
            }
            break;
        case 3: {
            float r = randf();
            m_sceneState = 4;
            m_sceneTimer = data[2] + (data[3] - data[2]) * r;
            break;
        }
        case 4:
            if (m_sceneTimer == 0.0f || m_scene == 4) {
                changeScene(0);
                m_typeManhore = 0x10;
                void* posData = m_fileData->getData(Data_Type_Model, m_scene + 100, 0xFFFE);
                if (posData != NULL) {
                    nw4r::g3d::ResFile posFile(posData);
                    m_stagePositions->loadPositionData(&posFile);
                }
                updateStagePositions();
                m_sceneState = 1;
            }
            break;
        }
    }
}

void stGW::changeScene(u32 scene) {
    if (scene >= 4) {
        return;
    }
    if (m_scene == scene) {
        return;
    }
    m_scene = scene;
}

// Picks the next scene: the three scenes of the queue are used first; when it is empty a new order is drawn.
void stGW::selectScene() {
    if (m_sceneQueue[1] == 4 && m_sceneQueue[2] == 4) {
        float r = randf();
        if (r < 1.0f / 3.0f) {
            m_sceneQueue[0] = 1;
        } else if (r < 2.0f / 3.0f) {
            m_sceneQueue[0] = 2;
        } else {
            m_sceneQueue[0] = 3;
        }
        r = randf();
        switch (m_sceneQueue[0]) {
        case 1:
            if (r < 0.5f) {
                m_sceneQueue[1] = 2;
                m_sceneQueue[2] = 3;
            } else {
                m_sceneQueue[1] = 3;
                m_sceneQueue[2] = 2;
            }
            break;
        case 2:
            if (r < 0.5f) {
                m_sceneQueue[1] = 1;
                m_sceneQueue[2] = 3;
            } else {
                m_sceneQueue[1] = 3;
                m_sceneQueue[2] = 1;
            }
            break;
        case 3:
            if (r < 0.5f) {
                m_sceneQueue[1] = 1;
                m_sceneQueue[2] = 2;
            } else {
                m_sceneQueue[1] = 2;
                m_sceneQueue[2] = 1;
            }
            break;
        }
    } else {
        m_sceneQueue[0] = m_sceneQueue[1];
        m_sceneQueue[1] = m_sceneQueue[2];
        m_sceneQueue[2] = 4;
    }
    m_scene = m_sceneQueue[0];
}

// The table of the steps of the scenes (from the stage data file).
void stGW::initStageDataTbl() {
    if (m_fileData != NULL) {
        stDataContainerData* containerData = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, 0x15, 0xFFFE));
        if (containerData != NULL) {
            m_tblStep = stDataMultiContainer::create(containerData, Heaps::StageInstance);
        }
    }
}

// MATCH-ONLY: the name of a model is at the offset 0x3c of its data.
static inline const char* gwModelName(nw4r::g3d::ResMdl& model) {
    u8* data = reinterpret_cast<u8*>(model.ptr());
    s32 offset = *reinterpret_cast<s32*>(data + 0x3C);
    return offset != 0 ? reinterpret_cast<const char*>(data + offset) : NULL;
}

// Builds the collision of the steps of the fire scene: 20 steps, every step is one line between two points (the points are
// a left and a right one: -11 and 11 units, 1.5 high; the real places come from the joints, which are the nodes of the model
// named "_FIRE_STEP_POSITION__"), each with its own joint.
void stGW::initCollDatafromCollWorkStep(grCollData* data, stCollisionWork* work, Ground* ground) {
    if (data == NULL || work == NULL || ground == NULL || !fn_27_239FD8(work)) {
        return;
    }
    // MATCH-ONLY: the three data pointers are private in the header; this view reads them at their offsets.
    stCollisionWorkView* wv = reinterpret_cast<stCollisionWorkView*>(work);
    u32 vtxNum = work->m_vtxLen * 20;
    double minX = 0.0;
    double maxX = 0.0;
    double minY = 0.0;
    double maxY = 0.0;
    wv->m_vtxDatas = reinterpret_cast<grCollData::VtxData*>(new (Heaps::StageInstance) u8[(work->m_vtxLen * 0xA0) & 0x7FFF8]);
    wv->m_lineDatas = reinterpret_cast<grCollData::LineData*>(new (Heaps::StageInstance) u8[0x140]);
    wv->m_jointDatas = reinterpret_cast<grCollData::JointData*>(new (Heaps::StageInstance) u8[0x870]);
    memset(wv->m_vtxDatas, 0, (vtxNum & 0xFFFF) << 3);
    memset(wv->m_lineDatas, 0, 0x140);
    memset(wv->m_jointDatas, 0, 0x870);
    u8 side = 0;
    u32 index = 0;
    for (u32 left = vtxNum & 0xFFFF; left != 0; left--) {
        float* vtx = reinterpret_cast<float*>(reinterpret_cast<u8*>(wv->m_vtxDatas) + (index & 0xFFFF) * 8);
        if (side == 1) {
            vtx[0] = 11.0f;
            vtx[1] = 1.5f;
        } else if (side == 0) {
            vtx[0] = -11.0f;
            vtx[1] = 1.5f;
        }
        if (vtx[0] < minX) {
            minX = vtx[0];
        }
        if (maxX < vtx[0]) {
            maxX = vtx[0];
        }
        if (vtx[1] < minY) {
            minY = vtx[1];
        }
        if (maxY < vtx[1]) {
            maxY = vtx[1];
        }
        side++;
        if (side == 2) {
            side = 0;
        }
        index++;
    }
    u32 line = 0;
    for (int n = 4; n != 0; n--) {
        for (int k = 0; k < 5; k++) {
            u16* entry = reinterpret_cast<u16*>(reinterpret_cast<u8*>(wv->m_lineDatas) + ((line + k) & 0xFFFF) * 0x10);
            entry[0] = (line + k) * 2;
            entry[1] = (line + k) * 2 + 1;
            entry[2] = 0xFFFF;
            entry[3] = 0xFFFF;
            entry[4] = 0xFFFF;
            entry[5] = 0xFFFF;
            entry[6] = 1;
            entry[7] = entry[7] | 0x8100;
        }
        line += 5;
    }
    nw4r::g3d::ResMdl model;
    if (ground->m_sceneModels[0] != NULL) {
        model = ground->m_sceneModels[0]->m_resMdl;
        if (model.IsValid()) {
            u32 numNodes = model.GetResNodeNumEntries();
            short jointCount = 0;
            short lineStart = 0;
            u32 jointOffset = 0;
            for (u32 i = 0; i != numNodes; i++) {
                nw4r::g3d::ResNode resNode = model.GetResNode(i);
                const char* name = gwNodeName(resNode);
                if (strstr(name, "_FIRE_STEP_POSITION__") != NULL) {
                    u8* joint = reinterpret_cast<u8*>(wv->m_jointDatas) + jointOffset;
                    *reinterpret_cast<short*>(joint) = jointCount;
                    *reinterpret_cast<u16*>(joint + 2) = 1;
                    *reinterpret_cast<float*>(joint + 0x14) = 0.0f;
                    *reinterpret_cast<float*>(joint + 0x18) = 0.0f;
                    *reinterpret_cast<float*>(joint + 0x1C) = 1.0f;
                    *reinterpret_cast<float*>(joint + 0x20) = 1.0f;
                    *reinterpret_cast<short*>(joint + 0x24) = lineStart;
                    *reinterpret_cast<u16*>(joint + 0x26) = 2;
                    strcpy(reinterpret_cast<char*>(joint + 0x2C), gwModelName(model));
                    strcpy(reinterpret_cast<char*>(joint + 0x4C), gwNodeName(resNode));
                    jointOffset += 0x6C;
                    lineStart += 2;
                    jointCount++;
                }
            }
            data->m_vtxLen = vtxNum;
            data->m_lineLen = 20;
            data->m_jointLen = 20;
            data->m_unk1 = 0;
            data->m_vtxDatas = wv->m_vtxDatas;
            data->m_lineDatas = wv->m_lineDatas;
            data->m_jointDatas = wv->m_jointDatas;
        }
    }
}
