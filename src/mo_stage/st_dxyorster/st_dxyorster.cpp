#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_dxyorster/gr_dxyorster.h>
#include <st_dxyorster/st_dxyorster.h>

stClassInfoImpl<Stages::DxYorster, stDxYorster> stDxYorster::bss_loc_14;

// MATCH-ONLY: keeps the stage name first in the data section although the constructor is inlined.
static char sStageName[] = "stDxYorster";

inline stDxYorster::stDxYorster() : stMelee(sStageName, Stages::DxYorster) {
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    m_stateL = 1;
    m_stateR = 1;
    m_unk246 = 0;
    memset(m_blockDmg, 0, sizeof(m_blockDmg));
}

stDxYorster* stDxYorster::create() {
    return new (Heaps::StageInstance) stDxYorster;
}

stDxYorster::~stDxYorster() {
    releaseArchive();
}

bool stDxYorster::loading() {
    return true;
}

void stDxYorster::createObj() {
    createObjBg(0);
    createCollision(m_fileData, 2, NULL);
    createObjOther(1);
    createObjOther(2);
    createObjBlockPos();
    createObjBlock(0);
    createObjBlock(1);
    createObjBlock(2);
    createObjBlock(3);
    createObjBlock(4);
    createObjBlock(5);
    createObjBlock(6);
    createObjBlock(7);
    createObjBlock(8);
    testStageParamInit(m_fileData, 10);
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
    initPosPokeTrainer(2, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, NULL);
}

void stDxYorster::createObjBg(int index) {
    grDxYorsterBg* ground;
    switch (index) {
    case 0:
        ground = grDxYorsterBg::create(0, "StgDxYorster", "grDxYorsterMainBg");
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
        ground->setStateLWork(&m_stateL);
        ground->setStateRWork(&m_stateR);
    }
}

// The clouds and Lakitu behind the island.
void stDxYorster::createObjOther(int index) {
    grDxYorster* ground;
    switch (index) {
    case 1:
        ground = grDxYorster::create(1, "", "grDxYorsterCloud");
        break;
    case 2:
        ground = grDxYorster::create(2, "", "grDxYorsterJugem");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        switch (index) {
        case 2:
            ground->setMotionLoop(true, 0);
            break;
        }
    }
}

void stDxYorster::createObjBlockPos() {
    grDxYorsterBlockPos* ground = grDxYorsterBlockPos::create(3, "StgYorsterBlockPoint", "grDxYorsterBlockPos");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setDmgWork(m_blockDmg);
    }
}

void stDxYorster::createObjBlock(int index) {
    grDxYorsterBlock* ground = grDxYorsterBlock::create(4, "StgDxYorsterKuru2Blk", "grDxYorsterBlock");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setIndex(index);
        ground->setPos(&m_posGimmick[index]);
        ground->setDmg(&m_blockDmg[index]);
        switch (index) {
        case 3:
            ground->setStateWork(&m_stateL);
            break;
        case 5:
            ground->setStateWork(&m_stateR);
            break;
        }
        createCollision(m_fileData, 3, ground);
    }
}

void stDxYorster::update(float deltaFrame) { }
