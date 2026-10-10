#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_palutena/gr_palutena.h>
#include <st_palutena/st_palutena.h>

stClassInfoImpl<Stages::Palutena, stPalutena> stPalutena::bss_loc_14;

stPalutena* stPalutena::create() {
    return new (Heaps::StageInstance) stPalutena;
}

stPalutena::stPalutena() : stMelee("stPalutena", Stages::Palutena) {
    m_resData = NULL;
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    memset(m_hp, 0, sizeof(m_hp));
    m_event = 0;
}

stPalutena::~stPalutena() {
    releaseArchive();
}

bool stPalutena::loading() {
    return true;
}

void stPalutena::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x84);
    initMovementData();
    createObjBg(0);
    createObjBg(1);
    m_resData = m_fileData->getData(Data_Type_Model, 0x14, 0xFFFE);
    createObjAshiba(2);
    createObjAshiba(3);
    createObjAshiba(4);
    createObjAshiba(5);
    createObjAshiba(6);
    createObjAshiba(7);
    createObjAshiba(8);
    createObjAshiba(9);
    createObjAshiba(10);
    createObjAshibaE(0xB);
    createObjKumo(0xC);
    createObjKumo(0xD);
    createObjKumo(0xE);
    createObjKumo(0xF);
    createCollision(m_fileData, 2, NULL);
    createObjAshiba(0x10);
    createObjAshiba(0x11);
    createObjAshiba(0x12);
    createObjAshiba(0x13);
    createObjAshiba(0x14);
    createObjAshiba(0x15);
    createObjAshiba(0x16);
    createObjAshiba(0x17);
    createObjAshiba(0x18);
    createObjAshiba(0x19);
    createObjAshiba(0x1A);
    createObjAshiba(0x1B);
    createObjAshiba(0x1C);
    createObjAshiba(0x1D);
    createObjAshiba(0x1E);
    createObjAshiba(0x1F);
    createObjAshiba(0x20);
    createObjAshiba(0x21);
    createObjChain(0x22);
    createObjAshibaBreak(0x23);
    createObjAshibaBreak(0x24);
    createObjAshibaBreak(0x25);
    createObjAshibaBreak(0x26);
    createObjAshibaBreak(0x27);
    createObjAshibaBreak(0x28);
    createObjAshibaBreak(0x29);
    createObjAshibaBreak(0x2A);
    createObjAshibaBreak(0x2B);
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
    stPalutenaData* data = static_cast<stPalutenaData*>(m_stageData);
    if (data != NULL) {
        for (int i = 0; i < 10; i++) {
            m_hp[i] = data->m_hp;
        }
    }
    if (g_GameGlobal->m_modeMelee != NULL && palutenaMeleeBytes()->m_mode == 7 && palutenaMeleeBytes()->m_event == 3) {
        m_event = 1;
    }
}

void stPalutena::createObjBg(int index) {
    grPalutena* ground;
    switch (index) {
    case 1:
        ground = grPalutena::create(0xF, "", "grPalutenaSun");
        break;
    case 0:
        ground = grPalutena::create(0xE, "", "grPalutenaMainBg");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->initStageData();
    }
}

void stPalutena::createObjAshiba(int index) {
    grPalutenaAshiba* ground;
    u8 type;
    u8 level;
    switch (index) {
    default:
        ground = NULL;
        break;
    case 2:
        ground = grPalutenaAshiba::create(0, "SCALE_A01", "grPalutenaAshibaA01");
        type = 0;
        level = 0;
        break;
    case 3:
        ground = grPalutenaAshiba::create(1, "SCALE_A02", "grPalutenaAshibaA02");
        type = 1;
        level = 0;
        break;
    case 4:
        ground = grPalutenaAshiba::create(3, "SCALE_B01", "grPalutenaAshibaB01");
        type = 2;
        level = 0;
        break;
    case 5:
        ground = grPalutenaAshiba::create(4, "SCALE_B02", "grPalutenaAshibaB02");
        type = 3;
        level = 0;
        break;
    case 6:
        ground = grPalutenaAshiba::create(6, "SCALE_C01", "grPalutenaAshibaC01");
        type = 4;
        level = 0;
        break;
    case 7:
        ground = grPalutenaAshiba::create(7, "SCALE_C02", "grPalutenaAshibaC02");
        type = 5;
        level = 0;
        break;
    case 8:
        ground = grPalutenaAshiba::create(8, "SCALE_C03", "grPalutenaAshibaC03");
        type = 6;
        level = 0;
        break;
    case 9:
        ground = grPalutenaAshiba::create(9, "SCALE_C04", "grPalutenaAshibaC04");
        type = 7;
        level = 0;
        break;
    case 10:
        ground = grPalutenaAshibaD01::create(11, "SCALE_D01", "grPalutenaAshibaD01");
        type = 8;
        level = 0;
        break;
    case 16:
        ground = grPalutenaAshiba::create(60, "SCALE_A01", "grPalutenaAshibaA01_02");
        type = 0;
        level = 1;
        break;
    case 17:
        ground = grPalutenaAshiba::create(70, "SCALE_A01", "grPalutenaAshibaA01_03");
        type = 0;
        level = 2;
        break;
    case 18:
        ground = grPalutenaAshiba::create(61, "SCALE_A02", "grPalutenaAshibaA02_02");
        type = 1;
        level = 1;
        break;
    case 19:
        ground = grPalutenaAshiba::create(71, "SCALE_A02", "grPalutenaAshibaA02_03");
        type = 1;
        level = 2;
        break;
    case 20:
        ground = grPalutenaAshiba::create(62, "SCALE_B01", "grPalutenaAshibaB01_02");
        type = 2;
        level = 1;
        break;
    case 21:
        ground = grPalutenaAshiba::create(72, "SCALE_B01", "grPalutenaAshibaB01_03");
        type = 2;
        level = 2;
        break;
    case 22:
        ground = grPalutenaAshiba::create(63, "SCALE_B02", "grPalutenaAshibaB02_02");
        type = 3;
        level = 1;
        break;
    case 23:
        ground = grPalutenaAshiba::create(73, "SCALE_B02", "grPalutenaAshibaB02_03");
        type = 3;
        level = 2;
        break;
    case 24:
        ground = grPalutenaAshiba::create(64, "SCALE_C01", "grPalutenaAshibaC01_02");
        type = 4;
        level = 1;
        break;
    case 25:
        ground = grPalutenaAshiba::create(74, "SCALE_C01", "grPalutenaAshibaC01_03");
        type = 4;
        level = 2;
        break;
    case 26:
        ground = grPalutenaAshiba::create(65, "SCALE_C02", "grPalutenaAshibaC02_02");
        type = 5;
        level = 1;
        break;
    case 27:
        ground = grPalutenaAshiba::create(75, "SCALE_C02", "grPalutenaAshibaC02_03");
        type = 5;
        level = 2;
        break;
    case 28:
        ground = grPalutenaAshiba::create(66, "SCALE_C03", "grPalutenaAshibaC03_02");
        type = 6;
        level = 1;
        break;
    case 29:
        ground = grPalutenaAshiba::create(76, "SCALE_C03", "grPalutenaAshibaC03_03");
        type = 6;
        level = 2;
        break;
    case 30:
        ground = grPalutenaAshiba::create(67, "SCALE_C04", "grPalutenaAshibaC04_02");
        type = 7;
        level = 1;
        break;
    case 31:
        ground = grPalutenaAshiba::create(77, "SCALE_C04", "grPalutenaAshibaC04_03");
        type = 7;
        level = 2;
        break;
    case 32:
        ground = grPalutenaAshibaD01::create(68, "SCALE_D01", "grPalutenaAshibaD01_02");
        type = 8;
        level = 1;
        break;
    case 33:
        ground = grPalutenaAshibaD01::create(78, "SCALE_D01", "grPalutenaAshibaD01_03");
        type = 8;
        level = 2;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        nw4r::g3d::ResFile resFile(m_resData);
        ground->setResCommon(&resFile);
        ground->setType(type);
        ground->setLevel(level);
        ground->setHPWork(&m_hp[type]);
    }
}

void stPalutena::createObjAshibaBreak(int index) {
    grPalutenaAshibaBreak* ground;
    switch (index) {
    case 0x23:
        ground = grPalutenaAshibaBreak::create(50, "BrAshibaA01", "grPalutenaAshibaA01Break");
        break;
    case 0x24:
        ground = grPalutenaAshibaBreak::create(51, "BrAshibaA02", "grPalutenaAshibaA02Break");
        break;
    case 0x25:
        ground = grPalutenaAshibaBreak::create(52, "BrAshibaB01", "grPalutenaAshibaB01Break");
        break;
    case 0x26:
        ground = grPalutenaAshibaBreak::create(53, "BrAshibaB02", "grPalutenaAshibaB02Break");
        break;
    case 0x27:
        ground = grPalutenaAshibaBreak::create(54, "BrAshibaC01", "grPalutenaAshibaC01Break");
        break;
    case 0x28:
        ground = grPalutenaAshibaBreak::create(55, "BrAshibaC02", "grPalutenaAshibaC02Break");
        break;
    case 0x29:
        ground = grPalutenaAshibaBreak::create(56, "BrAshibaC03", "grPalutenaAshibaC03Break");
        break;
    case 0x2a:
        ground = grPalutenaAshibaBreak::create(57, "BrAshibaC04", "grPalutenaAshibaC04Break");
        break;
    case 0x2b:
        ground = grPalutenaAshibaBreak::create(58, "BrAshibaD01", "grPalutenaAshibaD01Break");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->initStageData();
    }
}

void stPalutena::createObjAshibaE(int index) {
    grPalutenaAshibaE01* ground;
    switch (index) {
    case 0xB:
        ground = grPalutenaAshibaE01::create(0xD, "AshibaE01", "grPalutenaAshibaE01");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        ground->setGimmickData(&m_movement);
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        nw4r::g3d::ResFile resFile(m_resData);
        ground->setResCommon(&resFile);
    }
}

void stPalutena::createObjKumo(int index) {
    grPalutenaAshibaKumo* ground;
    switch (index) {
    case 0xE:
        ground = grPalutenaAshibaKumoC::create(10, "AshibaCKumo_kusari", "grPalutenaAshibaKumoC");
        break;
    case 0xC:
        ground = grPalutenaAshibaKumo::create(2, "gr2_AshibaAKumo", "grPalutenaAshibaKumoA");
        break;
    case 0xD:
        ground = grPalutenaAshibaKumo::create(5, "gr2_AshibaBKumo", "grPalutenaAshibaKumoB");
        break;
    case 0xF:
        ground = grPalutenaAshibaKumoD::create(0xC, "gr2_AshibaDKumo", "grPalutenaAshibaKumoD");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->initStageData();
        ground->setPosGimmickWork(m_posGimmick);
    }
}

void stPalutena::createObjChain(int index) {
    grPalutenaChain* ground;
    switch (index) {
    case 0x22:
        ground = grPalutenaChain::create(0x10, "_kusari", "grPalutenaChain");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->initStageData();
        ground->setPosGimmickWork(m_posGimmick);
    }
}

void stPalutena::update(float deltaFrame) {
    updatePos();
    updateBreak(deltaFrame);
}

// The cloud D (ground 0xF) follows the platform D01 (ground 10) and its two broken models (0x20, 0x21): the translation of
// the cloud is handed to them, and the push of a landing goes the other way.
void stPalutena::updatePos() {
    grPalutenaAshibaKumoD* cloud = static_cast<grPalutenaAshibaKumoD*>(getGround(0xF));
    if (cloud == NULL) {
        return;
    }
    grPalutenaAshibaD01* whole = static_cast<grPalutenaAshibaD01*>(getGround(10));
    if (whole == NULL) {
        return;
    }
    grPalutenaAshibaD01* broken1 = static_cast<grPalutenaAshibaD01*>(getGround(0x20));
    if (broken1 == NULL) {
        return;
    }
    grPalutenaAshibaD01* broken2 = static_cast<grPalutenaAshibaD01*>(getGround(0x21));
    if (broken2 == NULL) {
        return;
    }
    Vec3f delta;
    cloud->getTranslate_Delta(&delta);
    whole->setTranslate_Delta(delta.m_x, delta.m_y, delta.m_z);
    broken1->setTranslate_Delta(delta.m_x, delta.m_y, delta.m_z);
    broken2->setTranslate_Delta(delta.m_x, delta.m_y, delta.m_z);
    if (whole->m_isVisible == 1) {
        whole->getLanding_Delta(&delta);
        cloud->setLanding_Delta(delta.m_x, delta.m_y, delta.m_z);
    }
    if (broken1->m_isVisible == 1) {
        whole->getLanding_Delta(&delta);
        broken1->setLanding_Delta(&delta);
        cloud->setLanding_Delta(delta.m_x, delta.m_y, delta.m_z);
    }
    if (broken2->m_isVisible == 1) {
        whole->getLanding_Delta(&delta);
        broken2->setLanding_Delta(&delta);
        cloud->setLanding_Delta(delta.m_x, delta.m_y, delta.m_z);
    }
    cloud->setAshibaLevel(whole->getBreakLevel());
}

// The model of a platform that falls apart follows the platform: it breaks when the platform is broken (level 4) and is
// built again with it (level 0).
void stPalutena::updateBreak(float deltaFrame) {
    grPalutenaAshiba* ashiba;
    grPalutenaAshibaBreak* piece;
    ashiba = static_cast<grPalutenaAshiba*>(getGround(2));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x23));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(3));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x24));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(4));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x25));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(5));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x26));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(6));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x27));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(7));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x28));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(8));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x29));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    ashiba = static_cast<grPalutenaAshiba*>(getGround(9));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x2A));
    if (ashiba != NULL && piece != NULL) {
        switch (ashiba->getBreakLevel()) {
        case 0:
            ashiba->requestBuild();
            piece->requestBuild();
            break;
        case 4:
            ashiba->requestBreak();
            piece->requestBreak();
            break;
        }
    }
    grPalutenaAshibaD01* ashibaD01 = static_cast<grPalutenaAshibaD01*>(getGround(10));
    piece = static_cast<grPalutenaAshibaBreak*>(getGround(0x2B));
    if (ashibaD01 != NULL && piece != NULL) {
        switch (ashibaD01->getBreakLevel()) {
        case 0:
            ashibaD01->requestBuild();
            piece->requestBuild();
            break;
        case 4: {
            Vec3f delta;
            ashibaD01->getTranslate_Delta(&delta);
            piece->setTranslate_Delta(delta.m_x, delta.m_y, delta.m_z);
            ashibaD01->requestBreak();
            piece->requestBreak();
            break;
        }
        }
    }
}

void stPalutena::initMovementData() {
    stPalutenaData* data = static_cast<stPalutenaData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_movement.m_start.m_x = 0.0f;
    m_movement.m_start.m_y = 0.0f;
    m_movement.m_start.m_z = 0.0f;
    m_movement.m_goal.m_x = 0.0f;
    m_movement.m_goal.m_y = 0.0f;
    m_movement.m_goal.m_z = 0.0f;
    m_movement.m_speed = data->unk70;
    m_movement.m_accel = 0.0f;
    m_movement.m_kind = 2;
    m_movement.m_time = 0.0f;
    m_movement.m_ease = 0;
}

// The event of the mode ends when every platform is broken.
bool stPalutena::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_event == 0) {
        return false;
    }
    for (u8 i = 0; i < 10; i++) {
        if (i == 9) {
            break;
        }
        if (m_hp[i] != 0.0f) {
            return false;
        }
    }
    *eventState = 6;
    *eventDecision = 4;
    return true;
}
