#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_village/gr_village.h>
#include <st_village/st_village.h>

stClassInfoImpl<Stages::Village, stVillage> stVillage::bss_loc_14;

stVillage* stVillage::create() {
    return new (Heaps::StageInstance) stVillage;
}

stVillage::stVillage() : stMelee("stVillage", Stages::Village) {
    m_guestRes = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_scene = 5;
    m_area = 6;
    memset(m_posGuest, 0, sizeof(m_posGuest));
    m_perioState = 0;
    m_perioMotion = 6;
    m_perioTimer = 0.0f;
    m_taxiState = 0;
    m_taxiTimer = 0.0f;
    m_taxiMotion = 6;
    m_ufoState = 0;
    m_ufoTimer = 0.0f;
    for (u8 i = 0; i < 12; i++) {
        m_motion[i] = 6;
    }
    m_guestList = NULL;
    m_guestPos = NULL;
}

stVillage::~stVillage() {
    if (m_guestList != NULL) {
        delete m_guestList;
    }
    m_guestList = NULL;
    if (m_guestPos != NULL) {
        delete m_guestPos;
    }
    m_guestPos = NULL;
    releaseArchive();
}

bool stVillage::loading() {
    return true;
}

void stVillage::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x48);
    initStageDataTbl();
    selectScene();
    createObjBg(0);
    createObjBg(1);
    createObjBg(2);
    createObjBg(3);
    createObjBg(4);
    createObjSky(5);
    createObjAshiba(6);
    createObjAshiba(7);
    createObjGuestPathMove(8);
    createObjGuestPathMove(9);
    createObjGuestPathMove(10);
    createObjGuestPathMove(11);
    createObjLiveDeco(12);
    createObjClock(13);
    createObjBalloon(14);
    createObjGuest();
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
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), m_scene);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    switch (m_scene) {
    case 3:
        g_sndSystem->playSE(snd_se_stage_Village_night, 0, 0, 0, -1);
        break;
    case 0:
        g_sndSystem->playSE(snd_se_stage_Village_morning, 0, 0, 0, -1);
        break;
    case 4:
        g_sndSystem->playSE(snd_se_stage_Village_midnight, 0, 0, 0, -1);
        break;
    }
}

// The scenery: every ground is shown only in the times of day whose bit is in the bit mask it gets.
void stVillage::createObjBg(int index) {
    grVillage* ground;
    u8 sceneBit = 0;
    switch (index) {
    case 0:
        ground = grVillage::create(0, "yStgVillageChikeAll", "grVillageMainBg");
        break;
    case 1:
        ground = grVillage::create(1, "StgVillageMadoHikari", "grVillageMainBgLight");
        sceneBit |= 0xC;
        break;
    case 2:
        ground = grVillageStage::create(2, "StgVillageMainStage", "grVillageMainStage");
        break;
    case 3:
        ground = grVillage::create(3, "zStgVillageHikari", "grVillageMainStageLight");
        sceneBit |= 0x1C;
        break;
    case 4:
        ground = grVillage::create(0xE, "aStgVillageSky00Star", "grVillageStar");
        sceneBit |= 0x18;
        break;
    case 5:
        ground = grVillage::create(0x11, "aStgVillageSky01", "grVillageSky");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneWork(&m_scene);
        ground->setSceneBit(sceneBit);
        ground->setStateWork(&m_area);
        ground->setPosGuestWork(m_posGuest);
    }
}

void stVillage::createObjSky(int index) {
    grVillageSky* ground;
    switch (index) {
    case 5:
        ground = grVillageSky::create(0x11, "aStgVillageSky01", "grVillageSky");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneWork(&m_scene);
        ground->setStateWork(&m_area);
    }
}

// The two floating platforms: which of the two models is used depends on a chance of the stage data.
void stVillage::createObjAshiba(int index) {
    grVillageAshiba* ground;
    u8 sceneBit = 0;
    switch (index) {
    case 6: {
        ground = grVillageAshiba::create(4, "StgVillageFuyuuAshiba", "grVillageAshiba");
        stVillageData* data = static_cast<stVillageData*>(m_stageData);
        if (data == NULL) {
            return;
        }
        if (randf() < data->unk00) {
            m_ashibaScene = 3;
        } else {
            m_ashibaScene = 2;
        }
        break;
    }
    case 7:
        ground = grVillageAshiba::create(5, "zStgVillageFuyuuAshibaHikari", "grVillageAshibaLight");
        sceneBit |= 0x1C;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneWork(&m_scene);
        ground->setSceneBit(sceneBit);
        ground->setStateWork(&m_ashibaScene);
    }
}

// The guests of this time of day: a random list of the stage data tells which guest stands in which of the places.
void stVillage::createObjGuest() {
    if (m_guestList != NULL && m_guestPos != NULL) {
        u32 type = (u32)(randf() * 5.0f);
        type = type ? type : 0;
        u32 list = 4;
        if (type < 4) {
            list = type;
        }
        switch (m_scene) {
        case 1:
            list += 5;
            break;
        case 2:
            list += 10;
            break;
        case 3:
            list += 0xF;
            break;
        case 4:
            list += 0x14;
            break;
        }
        m_guestRes = nw4r::g3d::ResFile(m_fileData->getData(Data_Type_Model, 0x1E, 0xFFFE));
        createObjGuest1(0, list);
        createObjGuest1(1, list);
        createObjGuest1(2, list);
        createObjGuest1(3, list);
        createObjGuest1(4, list);
        createObjGuest1(5, list);
        createObjGuest1(6, list);
        createObjGuest1(7, list);
        switch (m_area) {
        case 0:
        case 1:
            createObjGuest1(8, list);
            break;
        }
    }
}

// One guest: the list says which kind stands in the place, the definition of the kind says how it behaves.
void stVillage::createObjGuest1(int index, int type) {
    u8* list = reinterpret_cast<u8*>(m_guestList->getData(type));
    if (list != NULL) {
        u32 count;
        u32 i = 0;
        count = m_guestPos->m_filedata->m_numFiles;
        u8* entry;
        u32 id;
        for (; i != count; i++) {
            entry = reinterpret_cast<u8*>(m_guestPos->getData(i));
            if (entry != NULL) {
                if ((u32)index == 8) {
                    if (*entry == 0x24u) {
                        break;
                    }
                } else if (*entry == list[index]) {
                    break;
                }
            }
        }
        if (i != count) {
            switch (index) {
            case 8:
                id = 0x24;
                break;
            default: {
                u32 j = 0;
                u32 n = m_guestPos->m_filedata->m_numFiles;
                for (; j != n; j++) {
                    entry = reinterpret_cast<u8*>(m_guestPos->getData(j));
                    if (entry != NULL && *entry == list[index]) {
                        break;
                    }
                }
                if (j == n) {
                    return;
                }
                id = *entry;
                break;
            }
            }
            grVillageGuest* ground;
            switch (id) {
            case 0:
                ground = grVillageGuest::create(31, "TopN", "grVillageGuestAsami");
                break;
            case 1:
                ground = grVillageGuestAyasiNeko::create(32, "TopN", "grVillageGuestAyasiNeko");
                break;
            case 2:
                ground = grVillageGuest::create(33, "TopN", "grVillageGuestBoy01");
                break;
            case 3:
                ground = grVillageGuest::create(34, "TopN", "grVillageGuestBoy02");
                break;
            case 4:
                ground = grVillageGuest::create(35, "TopN", "grVillageGuestBoy03");
                break;
            case 5:
                ground = grVillageGuest::create(36, "TopN", "grVillageGuestDonguri");
                break;
            case 6:
                ground = grVillageGuestFuta::create(37, "TopN", "grVillageGuestFuko");
                break;
            case 7:
                ground = grVillageGuestFuta::create(38, "TopN", "grVillageGuestFuta");
                break;
            case 8:
                ground = grVillageGuest::create(39, "TopN", "grVillageGuestGirl01");
                break;
            case 9:
                ground = grVillageGuest::create(40, "TopN", "grVillageGuestGirl02");
                break;
            case 10:
                ground = grVillageGuest::create(41, "TopN", "grVillageGuestGrace");
                break;
            case 11:
                ground = grVillageGuest::create(8, "obj_gcs", "grVillageGuestGraceCar");
                break;
            case 12:
                ground = grVillageGuest::create(42, "TopN", "grVillageGuestHakkemi");
                break;
            case 13:
                ground = grVillageGuest::create(43, "TopN", "grVillageGuestHonma");
                break;
            case 14:
                ground = grVillageGuest::create(44, "TopN", "grVillageGuestKaburiba");
                break;
            case 15:
                ground = grVillageGuest::create(45, "TopN", "grVillageGuestKatorinu");
                break;
            case 16:
                ground = grVillageGuest::create(46, "TopN", "grVillageGuestKinuyo");
                break;
            case 17:
                ground = grVillageGuest::create(47, "TopN", "grVillageGuestMaigo");
                break;
            case 18:
                ground = grVillageGuest::create(48, "TopN", "grVillageGuestMameTubu");
                break;
            case 19:
                ground = grVillageGuestMaster::create(49, "TopN", "grVillageGuestMaster");
                break;
            case 20:
                ground = grVillageGuestHatonosu::create(9, "StgVillageHatoNoSu", "grVillageHatonosu");
                break;
            case 21:
                ground = grVillageGuest::create(50, "TopN", "grVillageGuestMisiranuNeko");
                break;
            case 22:
                ground = grVillageGuestMonban::create(51, "TopN", "grVillageGuestMonbanA");
                break;
            case 23:
                ground = grVillageGuestMonban::create(52, "TopN", "grVillageGuestMonbanB");
                break;
            case 24:
                ground = grVillageGuest::create(53, "TopN", "grVillageGuestMaigoMama");
                break;
            case 25:
                ground = grVillageGuest::create(54, "TopN", "grVillageGuestPeriko");
                break;
            case 26:
                ground = grVillageGuest::create(55, "TopN", "grVillageGuestPerimi");
                break;
            case 27:
                ground = grVillageGuest::create(56, "TopN", "grVillageGuestRakosuke");
                break;
            case 28:
                ground = grVillageGuest::create(57, "TopN", "grVillageGuestRouran");
                break;
            case 29:
                ground = grVillageGuest::create(58, "TopN", "grVillageGuestSeiiti");
                break;
            case 30:
                ground = grVillageGuest::create(59, "TopN", "grVillageGuestSisyou");
                break;
            case 31:
                ground = grVillageGuest::create(60, "TopN", "grVillageGuestKotobuki");
                break;
            case 32:
                ground = grVillageGuest::create(61, "TopN", "grVillageGuestTanukiD");
                break;
            case 33:
                ground = grVillageGuest::create(62, "TopN", "grVillageGuestTanukiK");
                break;
            case 34:
                ground = grVillageGuest::create(63, "TopN", "grVillageGuestTanukiS");
                break;
            case 35:
                ground = grVillageGuest::create(64, "TopN", "grVillageGuestTanukiZ");
                break;
            case 36:
                ground = grVillageGuestTotakeke::create(65, "TopN", "grVillageGuestTotakeke");
                break;
            case 37:
                ground = grVillageGuest::create(66, "TopN", "grVillageGuestTunekiti");
                break;
            case 38:
                ground = grVillageGuest::create(67, "TopN", "grVillageGuestUntensyu");
                break;
            default:
                ground = NULL;
                break;
            }
            if (ground != NULL) {
                addGround(ground);
                ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                ground->setStageData(m_stageData);
                ground->setSceneWork(&m_scene);
                ground->setStateWork(&m_area);
                ground->setStateSubWork(&m_motion[index + 1]);
                ground->setPosWork(&m_posGuest[index]);
                ground->setPosGimmickWork(m_posGuest);
                ground->setID(id);
                nw4r::g3d::ResFile res = m_guestRes;
                ground->setResCommon(&res);
                ground->setMotionRatio(*reinterpret_cast<float*>(entry + 4));
                ground->setGuestListData(entry);
                switch (index) {
                case 0:
                    ground->setRotY(12.0f);
                    break;
                case 1:
                    ground->setRotY(8.0f);
                    break;
                case 2:
                    ground->setRotY(1.0f);
                    break;
                case 3:
                    ground->setRotY(-1.0f);
                    break;
                case 4:
                    ground->setRotY(-5.0f);
                    break;
                case 5:
                    ground->setRotY(-10.0f);
                    break;
                case 6:
                    ground->setRotY(-7.0f);
                    break;
                case 7:
                    ground->setRotY(-12.0f);
                    break;
                }
                if (index == 5) {
                    if (list[6] == 0x14) {
                        m_motion[6] = 5;
                        ground->setPosWork(&m_posGuest[10]);
                    }
                } else if (index < 5 && index > 3 && list[6] == 0x14) {
                    m_motion[5] = 5;
                    ground->setPosWork(&m_posGuest[9]);
                }
            }
        }
    }
}

// The bird, the taxi and the UFO move along a path of the stage model.
void stVillage::createObjGuestPathMove(int index) {
    stVillageData* data = static_cast<stVillageData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    grVillageGuestPathMove* ground;
    float ratio = 1.0f;
    u8* motion = NULL;
    u8 sceneBit = 0;
    u8 type;
    switch (index) {
    case 8:
        ground = grVillageGuestPathMove::create(6, "perio1", "grVillagePerio");
        sceneBit |= 0xF;
        motion = &m_perioMotion;
        type = 0;
        break;
    case 9:
        ground = grVillageGuestPathMove::create(0xF, "taransTaxi", "grVillageTaxi");
        motion = &m_taxiMotion;
        type = 1;
        break;
    case 10:
        ground = grVillageGuestPathMove::create(0x10, "taransTaxi", "grVillageTaxiLight");
        sceneBit |= 0x18;
        motion = &m_taxiMotion;
        type = 2;
        break;
    case 11:
        ground = grVillageGuestPathMove::create(0xC, "StgVillageUFO", "grVilageUFO");
        ratio = data->unk10;
        motion = &m_motion[0];
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
        ground->setSceneWork(&m_scene);
        ground->setSceneBit(sceneBit);
        ground->setStateWork(motion);
        ground->setType(type);
        ground->setMotionRatio(ratio);
    }
}

void stVillage::createObjLiveDeco(int index) {
    grVillageLiveDeco* ground;
    switch (index) {
    case 12:
        ground = grVillageLiveDeco::create(10, "yStgVillageLiveKazari", "grVillageLiveDeco");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneWork(&m_scene);
        ground->setStateWork(&m_area);
    }
}

void stVillage::createObjClock(int index) {
    grVillageClock* ground;
    switch (index) {
    case 13:
        ground = grVillageClock::create(13, "StgVillageTokei", "grVillageClock");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneWork(&m_scene);
        ground->setStateWork(&m_area);
    }
}

void stVillage::createObjBalloon(int index) {
    grVillageBalloon* ground;
    switch (index) {
    case 14:
        ground = grVillageBalloon::create(11, "StgVillageBalloon", "grVillageBalloon");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSceneWork(&m_scene);
        ground->setType(4);
    }
}

void stVillage::update(float deltaFrame) {
    updatePerio(deltaFrame);
    updateTaxi(deltaFrame);
    updateUFO(deltaFrame);
}

// The bird: it waits, then a chance decides whether it comes (and starts the motion of the ground) or waits again.
void stVillage::updatePerio(float deltaFrame) {
    stVillageData* data = static_cast<stVillageData*>(m_stageData);
    if (data != NULL) {
        m_perioTimer -= deltaFrame;
        if (m_perioTimer < 0.0f) {
            m_perioTimer = 0.0f;
        }
        switch (m_perioState) {
        case 0:
            m_perioTimer = data->unk30;
            m_perioState = 1;
            break;
        case 1:
            if (m_perioTimer == 0.0f) {
                if (randf() < data->unk38) {
                    m_perioMotion = 4;
                    m_perioState = 2;
                } else {
                    m_perioTimer = data->unk34;
                }
            }
            break;
        case 2:
            if (m_perioMotion == 6) {
                m_perioTimer = data->unk34;
                m_perioState = 1;
            }
            break;
        }
    }
}

void stVillage::updateTaxi(float deltaFrame) {
    stVillageData* data = static_cast<stVillageData*>(m_stageData);
    if (data != NULL) {
        m_taxiTimer -= deltaFrame;
        if (m_taxiTimer < 0.0f) {
            m_taxiTimer = 0.0f;
        }
        switch (m_taxiState) {
        case 0:
            m_taxiTimer = data->unk3C;
            m_taxiState = 1;
            break;
        case 1:
            if (m_taxiTimer == 0.0f) {
                if (randf() < data->unk44) {
                    m_taxiMotion = 4;
                    m_taxiState = 2;
                } else {
                    m_taxiTimer = data->unk40;
                }
            }
            break;
        case 2:
            if (m_taxiMotion == 6) {
                m_taxiTimer = data->unk40;
                m_taxiState = 1;
            }
            break;
        }
    }
}

// The UFO only comes in the later areas.
void stVillage::updateUFO(float deltaFrame) {
    switch (m_area) {
    case 0:
    case 1:
        break;
    default: {
        stVillageData* data = static_cast<stVillageData*>(m_stageData);
        if (data != NULL) {
            m_ufoTimer -= deltaFrame;
            if (m_ufoTimer < 0.0f) {
                m_ufoTimer = 0.0f;
            }
            switch (m_ufoState) {
            case 0:
                m_ufoTimer = data->unk04;
                m_ufoState = 1;
                break;
            case 1:
                if (m_ufoTimer == 0.0f) {
                    if (randf() < data->unk0C) {
                        m_motion[0] = 4;
                        m_ufoState = 2;
                    } else {
                        m_ufoTimer = data->unk08;
                    }
                }
                break;
            case 2:
                if (m_motion[0] == 6) {
                    m_ufoTimer = data->unk08;
                    m_ufoState = 1;
                }
                break;
            }
        }
        break;
    }
    }
}

void stVillage::initStageDataTbl() {
    if (m_fileData != NULL) {
        stDataContainerData* data = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, 0x15, 0xFFFE));
        if (data != NULL) {
            m_guestList = stDataMultiContainer::create(data, Heaps::StageInstance);
        }
        data = static_cast<stDataContainerData*>(m_fileData->getData(Data_Type_Misc, 0x16, 0xFFFE));
        if (data != NULL) {
            m_guestPos = stDataMultiContainer::create(data, Heaps::StageInstance);
        }
    }
}

// The time of day comes from the sub stage of the match settings.
void stVillage::selectScene() {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    if (melee != NULL) {
        switch (melee->m_meleeInitData.m_subStageKind) {
        case 0:
            m_scene = 0;
            break;
        case 1:
            m_scene = 1;
            break;
        case 2:
            m_scene = 2;
            break;
        case 3:
            m_scene = 3;
            break;
        case 4:
            m_scene = 3;
            break;
        case 5:
            m_scene = 3;
            break;
        case 6:
            m_scene = 4;
            break;
        case 7:
            m_scene = 4;
            break;
        }
        switch (melee->m_meleeInitData.m_subStageKind) {
        case 4:
            m_area = 0;
            break;
        case 5:
            m_area = 1;
            break;
        case 6:
            break;
        case 7:
            m_area = 1;
            break;
        }
    }
}

GXColor stVillage::getFinalTechniqColor() {
    u32 packed = 0x14000496;
    return *reinterpret_cast<GXColor*>(&packed);
}
