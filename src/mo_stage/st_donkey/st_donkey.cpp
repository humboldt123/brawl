#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_donkey/gr_donkey.h>
#include <st_donkey/st_donkey.h>

stClassInfoImpl<Stages::Donkey, stDonkey> stDonkey::bss_loc_14;

stDonkey::stDonkey() : stMelee("stDonkey", Stages::Donkey) {
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    memset(&m_limitMin, 0, 0x18);
    m_ladderData = NULL;
    m_stateKong = 6;
    m_jackState = 0;
    m_jackTimer1 = 0.0f;
    m_jackTimer2 = 0.0f;
    m_stateJack[0] = 6;
    m_stateJack[1] = 6;
    m_jackFirst = 0;
    m_stateItem[0] = 6;
    m_stateItem[1] = 6;
    m_stateItem[2] = 6;
    m_stateFireBall[0] = 6;
    m_stateFireBall[1] = 6;
    m_score = 0;
    m_hiScore = 0;
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
}

stDonkey* stDonkey::create() {
    return new (Heaps::StageInstance) stDonkey;
}

stDonkey::~stDonkey() {
    if (m_ladderData != NULL) {
        delete[] m_ladderData;
    }
    releaseArchive();
}

bool stDonkey::loading() {
    return true;
}

void stDonkey::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x6C);
    createObjBg(0);
    createCollision(m_fileData, 2, NULL);
    createObjAshiba(1);
    createObjAshiba(2);
    createObjAshiba(3);
    createObjAshiba(4);
    createObjAshiba(5);
    createObjAshiba(6);
    createObjKong(7);
    createObjFireBall(0);
    createObjFireBall(1);
    createObjJack(10);
    createObjJack(11);
    createObjItem(12);
    createObjItem(13);
    createObjItem(14);
    createObjNumber(15);
    createObjNumber(16);
    createObjNumber(17);
    createObjNumber(18);
    createObjNumber(19);
    createObjNumber(20);
    createObjNumber(21);
    createObjNumber(22);
    createObjNumber(23);
    createObjNumber(24);
    createObjNumber(25);
    createObjNumber(26);
    createObjHashigo();
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

void stDonkey::createObjBg(int index) {
    grDonkeyMainBg* ground;
    switch (index) {
    case 0:
        ground = grDonkeyMainBg::create(0, "", "grDonkeyMainBg");
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
    }
}

// The six elevators: A to C go up at a third, two thirds and the full lift speed, D to F come down.
void stDonkey::createObjAshiba(int index) {
    grDonkeyLift* ground;
    Vec3f* pos;
    float rate;
    switch (index) {
    case 1:
        ground = grDonkeyLift::create(1, "StgDonkey_Elevator", "grDonkeyAshibaA");
        rate = 1.0f / 3.0f;
        pos = &m_posGimmick[2];
        break;
    case 2:
        ground = grDonkeyLift::create(1, "StgDonkey_Elevator", "grDonkeyAshibaB");
        rate = 2.0f / 3.0f;
        pos = &m_posGimmick[2];
        break;
    case 3:
        ground = grDonkeyLift::create(1, "StgDonkey_Elevator", "grDonkeyAshibaC");
        rate = 1.0f;
        pos = &m_posGimmick[2];
        break;
    case 4:
        ground = grDonkeyLift::create(1, "StgDonkey_Elevator", "grDonkeyAshibaD");
        rate = -1.0f / 3.0f;
        pos = &m_posGimmick[4];
        break;
    case 5:
        ground = grDonkeyLift::create(1, "StgDonkey_Elevator", "grDonkeyAshibaE");
        rate = -2.0f / 3.0f;
        pos = &m_posGimmick[4];
        break;
    case 6:
        ground = grDonkeyLift::create(1, "StgDonkey_Elevator", "grDonkeyAshibaF");
        rate = -1.0f;
        pos = &m_posGimmick[4];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        createCollision(m_fileData, 3, ground);
        ground->setPosWork(pos);
        ground->setRate(rate);
    }
}

void stDonkey::createObjKong(int index) {
    grDonkeyKong* ground;
    switch (index) {
    case 7:
        ground = grDonkeyKong::create(8, "DonkeyKong", "grDonkeyKong");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(m_posGimmick);
        ground->setStateWork(&m_stateKong);
        ground->setStateJackWork(m_stateJack);
    }
}

void stDonkey::createObjFireBall(int index) {
    grDonkeyFireBall* ground;
    Vec3f* pos;
    u8* state;
    int type;
    switch (index) {
    case 0:
        ground = grDonkeyFireBall::create(2, "StgDonkey_Fireball", "grDonkeyFireBallA");
        pos = &m_posGimmick[28];
        state = &m_stateFireBall[0];
        type = 0;
        break;
    case 1:
        ground = grDonkeyFireBall::create(2, "StgDonkey_Fireball", "grDonkeyFireBallB");
        pos = &m_posGimmick[33];
        state = &m_stateFireBall[1];
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
        ground->setPosWork(pos);
        ground->setType(type);
        ground->setStateWork(state);
    }
}

void stDonkey::createObjJack(int index) {
    grDonkeyJack* ground;
    u8* state;
    switch (index) {
    case 10:
        ground = grDonkeyJack::create(3, "Jack", "grDonkeyJackA");
        state = &m_stateJack[0];
        break;
    case 11:
        ground = grDonkeyJack::create(3, "Jack", "grDonkeyJackB");
        state = &m_stateJack[1];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(&m_posGimmick[1]);
        ground->setStateWork(state);
    }
}

// The three bonus items, each with an "800" that pops up where it was taken.
void stDonkey::createObjItem(int index) {
    grDonkeyItem* ground;
    int itemIndex;
    int posIndex;
    switch (index) {
    case 12:
        ground = grDonkeyItem::create(5, "StgDonkey_Parasol", "grDonkeyParasol");
        itemIndex = 0;
        posIndex = 0x28;
        break;
    case 13:
        ground = grDonkeyItem::create(6, "StgDonkey_Handbag", "grDonkeyBag");
        itemIndex = 1;
        posIndex = 0x29;
        break;
    case 14:
        ground = grDonkeyItem::create(7, "StgDonkey__Hat", "grDonkeyHat");
        itemIndex = 2;
        posIndex = 0x2A;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setType(itemIndex);
        ground->setPosWork(&m_posGimmick[posIndex]);
        ground->setStateWork(&m_stateItem[itemIndex]);
        grDonkeyItemScore* score = grDonkeyItemScore::create(10, "StgDonkey_Score_800", "grDonkeyItemScore");
        if (score != NULL) {
            addGround(score);
            score->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            score->setStageData(m_stageData);
            score->setPosWork(&m_posGimmick[posIndex]);
            score->setStateWork(&m_stateItem[itemIndex]);
        }
    }
}

// The digits of the score (15 to 20) and of the high score (21 to 26), 1s at the right.
void stDonkey::createObjNumber(int index) {
    grDonkeyScore* ground;
    Vec3f* pos;
    u32* score;
    int type;
    switch (index) {
    case 15:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyScore0");
        pos = &m_posGimmick[6];
        score = &m_score;
        type = 7;
        break;
    case 16:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyScore1");
        pos = &m_posGimmick[7];
        score = &m_score;
        type = 6;
        break;
    case 17:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyScore2");
        pos = &m_posGimmick[8];
        score = &m_score;
        type = 5;
        break;
    case 18:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyScore3");
        pos = &m_posGimmick[9];
        score = &m_score;
        type = 4;
        break;
    case 19:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyScore4");
        pos = &m_posGimmick[10];
        score = &m_score;
        type = 3;
        break;
    case 20:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyScore5");
        pos = &m_posGimmick[11];
        score = &m_score;
        type = 2;
        break;
    case 21:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyHiScore0");
        pos = &m_posGimmick[12];
        score = &m_hiScore;
        type = 7;
        break;
    case 22:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyHiScore1");
        pos = &m_posGimmick[13];
        score = &m_hiScore;
        type = 6;
        break;
    case 23:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyHiScore2");
        pos = &m_posGimmick[14];
        score = &m_hiScore;
        type = 5;
        break;
    case 24:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyHiScore3");
        pos = &m_posGimmick[15];
        score = &m_hiScore;
        type = 4;
        break;
    case 25:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyHiScore4");
        pos = &m_posGimmick[16];
        score = &m_hiScore;
        type = 3;
        break;
    case 26:
        ground = grDonkeyScore::create(4, "StgDonkey_Suuji", "grDonkeyHiScore5");
        pos = &m_posGimmick[17];
        score = &m_hiScore;
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
        ground->setPosWork(pos);
        ground->setType(type);
        ground->setScoreWork(reinterpret_cast<int*>(score));
    }
}

// MATCH-ONLY: the ladder entries are set up by hand (the motion path model of the first entry is cleared each time).
static inline void donkeyInitLadder(grGimmickLadderData* data, grGimmickLadderData* first, u8 mdlIndex, const char* nodeName,
                                    float height) {
    MEMINIT(data);
    data->m_mdlIndex = mdlIndex;
    data->m_49 = 0;
    data->m_restrictUpExit = false;
    data->m_51 = 1;
    strcpy(data->m_nodeName, nodeName);
    first->m_motionPathData.m_mdlIndex = -1;
    data->m_areaData.m_offsetPos.m_x = 0.0f;
    data->m_areaData.m_offsetPos.m_y = 4.0f;
    data->m_areaData.m_range.m_x = 10.0f;
    data->m_areaData.m_range.m_y = height;
}

// The ten ladders (ids 0x3C to 0x45), each created right after its data.
void stDonkey::createObjHashigo() {
    m_ladderData = new (Heaps::StageResource) grGimmickLadderData[10];
    if (m_ladderData != NULL) {
        donkeyInitLadder(&m_ladderData[0], m_ladderData, 0x3C, "hasigo_09", 40.0f);
        createObjHashigo1(0x1B);
        donkeyInitLadder(&m_ladderData[1], m_ladderData, 0x3D, "hasigo_07", 48.0f);
        createObjHashigo1(0x1C);
        donkeyInitLadder(&m_ladderData[2], m_ladderData, 0x3E, "hasigo_08", 64.0f);
        createObjHashigo1(0x1D);
        donkeyInitLadder(&m_ladderData[3], m_ladderData, 0x3F, "hasigo_06", 64.0f);
        createObjHashigo1(0x1E);
        donkeyInitLadder(&m_ladderData[4], m_ladderData, 0x40, "hasigo_02", 32.0f);
        createObjHashigo1(0x1F);
        donkeyInitLadder(&m_ladderData[5], m_ladderData, 0x41, "hasigo_01", 16.0f);
        createObjHashigo1(0x20);
        donkeyInitLadder(&m_ladderData[6], m_ladderData, 0x42, "hasigo_05", 32.0f);
        createObjHashigo1(0x21);
        donkeyInitLadder(&m_ladderData[7], m_ladderData, 0x43, "hasigo_11", 40.0f);
        createObjHashigo1(0x22);
        donkeyInitLadder(&m_ladderData[8], m_ladderData, 0x44, "hasigo_10", 32.0f);
        createObjHashigo1(0x23);
        donkeyInitLadder(&m_ladderData[9], m_ladderData, 0x45, "hasigo_04", 24.0f);
        createObjHashigo1(0x24);
    }
}

void stDonkey::createObjHashigo1(int index) {
    grDonkeyLadder* ground;
    Vec3f* pos;
    grGimmickLadderData* data;
    switch (index) {
    case 0x1B:
        ground = grDonkeyLadder::create(0x3C, "HASIGO_01", "grDonkeyLadder01");
        data = &m_ladderData[0];
        pos = &m_posGimmick[18];
        break;
    case 0x1C:
        ground = grDonkeyLadder::create(0x3D, "HASIGO_02", "grDonkeyLadder02");
        data = &m_ladderData[1];
        pos = &m_posGimmick[19];
        break;
    case 0x1D:
        ground = grDonkeyLadder::create(0x3E, "HASIGO_03", "grDonkeyLadder03");
        data = &m_ladderData[2];
        pos = &m_posGimmick[20];
        break;
    case 0x1E:
        ground = grDonkeyLadder::create(0x3F, "HASIGO_04", "grDonkeyLadder04");
        data = &m_ladderData[3];
        pos = &m_posGimmick[21];
        break;
    case 0x1F:
        ground = grDonkeyLadder::create(0x40, "HASIGO_05", "grDonkeyLadder05");
        data = &m_ladderData[4];
        pos = &m_posGimmick[22];
        break;
    case 0x20:
        ground = grDonkeyLadder::create(0x41, "HASIGO_06", "grDonkeyLadder06");
        data = &m_ladderData[5];
        pos = &m_posGimmick[23];
        break;
    case 0x21:
        ground = grDonkeyLadder::create(0x42, "HASIGO_07", "grDonkeyLadder07");
        data = &m_ladderData[6];
        pos = &m_posGimmick[24];
        break;
    case 0x22:
        ground = grDonkeyLadder::create(0x43, "HASIGO_08", "grDonkeyLadder08");
        data = &m_ladderData[7];
        pos = &m_posGimmick[25];
        break;
    case 0x23:
        ground = grDonkeyLadder::create(0x44, "HASIGO_09", "grDonkeyLadder09");
        data = &m_ladderData[8];
        pos = &m_posGimmick[26];
        break;
    case 0x24:
        ground = grDonkeyLadder::create(0x45, "HASIGO_10", "grDonkeyLadder10");
        data = &m_ladderData[9];
        pos = &m_posGimmick[27];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->setGimmickData(data);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(pos);
    }
}

void stDonkey::update(float deltaFrame) {
    updateLimit();
    updateAI();
    updateScore();
    updateJack(deltaFrame);
}

// Keeps the camera limit of the stage (the AI uses it to know where the screen is).
void stDonkey::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    m_limitMin.m_x = camera->unk158;
    m_limitMin.m_y = camera->unk160;
    m_limitMin.m_z = 0.0f;
    m_limitMax.m_x = camera->unk15C;
    m_limitMax.m_y = camera->unk164;
    m_limitMax.m_z = 0.0f;
}

// Tells the AI where the danger is: the area below Donkey Kong while he stands at his throwing position and the area around
// the lift end.
void stDonkey::updateAI() {
    Vec2f zoneMin;
    Vec2f zoneMax;
    if (m_stateKong == 3) {
        zoneMin.m_x = m_limitMin.m_x;
        zoneMin.m_y = m_limitMin.m_y;
        zoneMax.m_x = m_posGimmick[26].m_x;
        zoneMax.m_y = m_posGimmick[0].m_y - 30.0f;
        m_dangerZone[0] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[0], false, false);
    } else if (m_dangerZone[0] != -1) {
        g_aiMgr->delDangerZone(m_dangerZone[0]);
        m_dangerZone[0] = -1;
    }
    zoneMin.m_x = m_posGimmick[5].m_x - 15.0f;
    zoneMin.m_y = m_posGimmick[5].m_y + 40.0f;
    zoneMax.m_x = m_posGimmick[5].m_x + 15.0f;
    zoneMax.m_y = m_posGimmick[5].m_y - 20.0f;
    m_dangerZone[2] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[2], false, false);
}

// Every item that was taken (state 4) is worth 800 points; the score stops at 999999 and carries the high score.
void stDonkey::updateScore() {
    if (m_stateItem[0] == 4) {
        m_stateItem[0] = 5;
        m_score += 800;
        if (m_score > 999999) {
            m_score = 999999;
        }
        if (m_hiScore < m_score) {
            m_hiScore = m_score;
        }
    }
    if (m_stateItem[1] == 4) {
        m_stateItem[1] = 5;
        m_score += 800;
        if (m_score > 999999) {
            m_score = 999999;
        }
        if (m_hiScore < m_score) {
            m_hiScore = m_score;
        }
    }
    if (m_stateItem[2] == 4) {
        m_stateItem[2] = 5;
        m_score += 800;
        if (m_score > 999999) {
            m_score = 999999;
        }
        if (m_hiScore < m_score) {
            m_hiScore = m_score;
        }
    }
}

// Decides when the two Jacks appear. The stage data holds the ranges of the waits (floats at 0x18 to 0x38).
void stDonkey::updateJack(float deltaFrame) {
    float* data = static_cast<float*>(m_stageData);
    if (m_stateKong != 3 || data == NULL) {
        return;
    }
    m_jackTimer1 -= deltaFrame;
    if (m_jackTimer1 < 0.0f) {
        m_jackTimer1 = 0.0f;
    }
    m_jackTimer2 -= deltaFrame;
    if (m_jackTimer2 < 0.0f) {
        m_jackTimer2 = 0.0f;
    }
    switch (m_jackState) {
    case 0:
        m_stateJack[0] = 6;
        m_stateJack[1] = 6;
        if (m_jackFirst == 1) {
            m_jackFirst = 0;
            m_jackTimer1 = data[6] + (data[7] - data[6]) * randf();
        } else {
            m_jackTimer1 = data[8] + (data[9] - data[8]) * randf();
        }
        m_jackState = 1;
        break;
    case 1:
        if (m_jackTimer1 == 0.0f) {
            if (randf() < data[10]) {
                m_jackState = 2;
                m_jackTimer2 = 0.0f;
                m_jackTimer1 = data[11] + (data[12] - data[11]) * randf();
            } else {
                m_jackState = 0;
            }
        }
        break;
    case 2:
        if (m_jackTimer1 == 0.0f) {
            m_jackState = 3;
        } else if (m_jackTimer2 == 0.0f) {
            if (m_stateJack[0] == 6) {
                m_stateJack[0] = 3;
            } else {
                if (m_stateJack[1] != 6) {
                    return;
                }
                m_stateJack[1] = 3;
            }
            m_jackTimer2 = data[13] + (data[14] - data[13]) * randf();
        }
        break;
    case 3:
        if (m_stateJack[0] == 6 && m_stateJack[1] == 6) {
            m_jackState = 0;
        }
        break;
    }
}

void stDonkey::notifyEventInfoGo() {
    m_stateFireBall[0] = 3;
    m_stateFireBall[1] = 3;
}
