#include <ec/ec_mgr.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_positions.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>

#include <st_earth/gr_earth.h>
#include <st_earth/st_earth.h>

// sndSystem::stopSE (main, unnamed)
extern "C" void fn_80075F1C(sndSystem* system, s32 handle, int fade);
// stCollisionWork::destroy (sora_melee, unnamed)
extern "C" void fn_27_239F6C(stCollisionWork* work);
// stTrigger::setBeltConveyorTrigger for the triangle belt (sora_melee, unnamed)
extern "C" void fn_27_233808(stTrigger* trigger, stEarthBeltData* belt);
// stTrigger::setBeltConveyorParam for the triangle belt (sora_melee, unnamed)
extern "C" void fn_27_233B58(stTrigger* trigger, stEarthBeltData* belt);

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct stEarthMarker {
    int m_a;
    int m_b;
    stEarthMarker(int a, int b) : m_a(a), m_b(b) { }
};
static stEarthMarker sMarkerA(0xFF, 0);
static stEarthMarker sMarkerB(0xFF, 1);

stClassInfoImpl<Stages::Earth, stEarth> stEarth::bss_loc_14;

stEarth* stEarth::create() {
    return new (Heaps::StageInstance) stEarth;
}

stEarth::stEarth() : stMelee("stEarth", Stages::Earth) {
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    memset(m_posLeaf, 0, sizeof(m_posLeaf));
    m_stateOniyon = 0;
    m_timerOniyon = 0.0f;
    m_colorOniyon = 0x80;
    m_firstOniyon = 1;
    m_leafOniyon = 3;
    m_stateWeather = 5;
    m_timerWeather = 0.0f;
    m_timerSun = 0.0f;
    m_riverSpeed = 0.0f;
    m_firstWeather = 1;
    m_sunOn = 0;
    m_seHandle = -1;
    m_stateChappy = 0;
    m_timerChappy = 0.0f;
    m_firstChappy = 1;
    m_collWork.initialize();
    m_collWork.m_vtxLen = 0x34;
    m_collWork.m_isClosed = 1;
    m_statePellet = 0;
    m_timerPellet = 0.0f;
    m_pelletData = NULL;
    m_firstPellet = 1;
    m_lastColor = 3;
    m_triggerBelt = NULL;
    m_beltData = NULL;
}

stEarth::~stEarth() {
    fn_27_239F6C(&m_collWork);
    if (m_pelletData != NULL) {
        delete[] m_pelletData;
    }
    if (m_beltData != NULL) {
        delete m_beltData;
    }
    releaseArchive();
}

bool stEarth::loading() {
    return true;
}

#define EARTH_LOAD_ARCHIVE(archive, id)                                                 \
    data = m_fileData->getData(Data_Type_Misc, id, &size, 0xFFFE);                      \
    if (data != NULL) {                                                                 \
        archive.setFileImage(data, size, Heaps::StageResource);                         \
    }

void stEarth::createObj() {
    int size;
    void* data;
    EARTH_LOAD_ARCHIVE(m_archivePelletBrres, 0x2711)
    EARTH_LOAD_ARCHIVE(m_archivePelletParam, 0x2712)
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0xDC);
    stEarthData* stageData = static_cast<stEarthData*>(m_stageData);
    if (stageData != NULL) {
        u8 count = stageData->unk3C;
        m_pelletData = new (Heaps::StageInstance) stEarthPelletData[count];
        if (m_pelletData != NULL) {
            count = stageData->unk3C;
            for (u8 i = 0; i != count; i++) {
                initPelletData(&m_pelletData[i]);
            }
            Ground* ground = grEarthMainBg::create(4, "", "grEarthMainBg");
            addGround(ground);
            ground = grEarthSun::create(8, "", "grEarthSun");
            addGround(ground);
            u32 groundNum = getGroundNum();
            for (u32 i = 0; i != groundNum; i++) {
                Ground* each = getGround(i);
                if (each != NULL) {
                    each->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                    each->setStageData(m_stageData);
                }
            }
            createObjOniyon(2);
            createObjStump(3);
            createObjBranch(4);
            createObjLeaf(5);
            createObjLeaf(6);
            createObjLeaf(7);
            createCollision(m_fileData, 2, NULL);
            createObjRiver(8);
            createObjRain(9);
            createObjHaneMizu(10);
            createObjHaneMizu(11);
            createObjHaneMizu(12);
            createObjHaneMizu(13);
            createObjChappy();
            createObjPellet();
            createObjBeltConv();
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
    }
}

void stEarth::createObjOniyon(int index) {
    grEarthOniyon* ground;
    switch (index) {
    case 2:
        ground = grEarthOniyon::create(9, "TopN", "grEarthOniyon");
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

void stEarth::createObjStump(int index) {
    grEarthStump* ground;
    switch (index) {
    case 3:
        ground = grEarthStump::create(7, "", "grEarthStump");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPelletData(m_pelletData);
        ground->setPosGimmickWork(m_posGimmick);
    }
}

// The small setters of the grounds are defined here, as in the original (the stage unit is the first one that needs them).
void grEarthStump::setPelletData(stEarthPelletData* pelletData) {
    m_pelletData = pelletData;
}

void grEarthStump::setPosGimmickWork(Vec3f* posGimmickWork) {
    m_posGimmickWork = posGimmickWork;
}

void stEarth::createObjBranch(int index) {
    grEarthBranch* ground;
    switch (index) {
    case 4:
        ground = grEarthBranch::create(0, "tsutaYuka", "grEarthBranch");
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

// The three leaves: the left one (5), the center one (6) and the right one (7); each one gets the place of its move, the
// lowest it can go and the pellets.
void stEarth::createObjLeaf(int index) {
    grEarthLeaf* ground;
    int type;
    float downLimit;
    switch (index) {
    case 5:
        ground = grEarthLeaf::create(1, "happaLeft", "grEarthLeafLeft");
        downLimit = -15.0f;
        type = 0;
        break;
    case 6:
        ground = grEarthLeaf::create(2, "happaCenter", "grEarthLeafCenter");
        downLimit = -20.0f;
        type = 1;
        break;
    case 7:
        ground = grEarthLeaf::create(3, "happaRight", "grEarthLeafRight");
        downLimit = -25.0f;
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
        ground->setType(type);
        ground->setDownLimit(downLimit);
        ground->setPosOffsetWork(&m_posLeaf[type]);
        ground->setPelletData(m_pelletData);
    }
}

void grEarthLeaf::setType(u8 type) {
    m_type = type;
}

void grEarthLeaf::setDownLimit(float downLimit) {
    m_downLimit = downLimit;
}

void grEarthLeaf::setPosOffsetWork(Vec3f* posOffsetWork) {
    m_posOffsetWork = posOffsetWork;
}

void grEarthLeaf::setPelletData(stEarthPelletData* pelletData) {
    m_pelletData = pelletData;
}

void stEarth::createObjChappy() {
    grEarthChappy* ground = grEarthChappy::create(10, "TopN", "grEarthChappy");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        createCollisionSelf(&m_collWork, ground, "StgEarthChappy", "TopN", 0x409);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setEnableCollisionStatus(true);
        ground->disableCalcCollision();
    }
}

void grEarthChappy::setPosGimmickWork(Vec3f* posGimmickWork) {
    m_posGimmickWork = posGimmickWork;
}

void stEarth::createObjPellet() {
    if (m_stageData != NULL) {
        u8 count = static_cast<stEarthData*>(m_stageData)->unk3C;
        for (u8 i = 0; i != count; i++) {
            createObjPelletFlower(i);
            createObjPellet1(i);
        }
    }
}

void stEarth::createObjPelletFlower(int index) {
    grEarthPelletFlower* ground = grEarthPelletFlower::create(0xB, "TopN", "grEarthPelletFlower");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPelletData(&m_pelletData[index]);
    }
}

void grEarthPelletFlower::setPelletData(stEarthPelletData* pelletData) {
    m_pelletData = pelletData;
}

void stEarth::createObjPellet1(int index) {
    grEarthPellet* ground = grEarthPellet::create(5, "Pellet00_white1", "grEarthPellet");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPelletData(&m_pelletData[index]);
    }
}

void grEarthPellet::setPelletData(stEarthPelletData* pelletData) {
    m_pelletData = pelletData;
}

void stEarth::createObjRain(int index) {
    grEarthRain* ground = grEarthRain::create(0x18, "gr2_EffAme", "grEarthRain");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setWeatherWork(&m_weather);
    }
}

void grEarthRain::setWeatherWork(u8* weatherWork) {
    m_weatherWork = weatherWork;
}

void stEarth::createObjRiver(int index) {
    grEarthRiver* ground = grEarthRiver::create(6, "", "grEarthRiver");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setSpeedWork(&m_riverSpeed);
    }
}

void grEarthRiver::setSpeedWork(float* speedWork) {
    m_speedWork = speedWork;
}

// The drops that fly up from the water: one in the middle of the stage and three on the leaves.
void stEarth::createObjHaneMizu(int index) {
    grEarthHaneMizu* ground;
    Vec3f* posOffset;
    switch (index) {
    case 10:
        ground = grEarthHaneMizu::create(0x14, "EffAmeHanemizu", "grEarthRainHaneMizu");
        posOffset = NULL;
        break;
    case 11:
        ground = grEarthHaneMizu::create(0x15, "hanemizuA03", "grEarthRainHaneMizuL");
        posOffset = &m_posLeaf[0];
        break;
    case 12:
        ground = grEarthHaneMizu::create(0x16, "hanemizuA02", "grEarthRainHaneMizuR");
        posOffset = &m_posLeaf[2];
        break;
    case 13:
        ground = grEarthHaneMizu::create(0x17, "hanemizuA01", "grEarthRainHaneMizuC");
        posOffset = &m_posLeaf[1];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setWeatherWork(&m_weather);
        ground->setPosOffsetWork(posOffset);
    }
}

void grEarthHaneMizu::setWeatherWork(u8* weatherWork) {
    m_weatherWork = weatherWork;
}

void grEarthHaneMizu::setPosOffsetWork(Vec3f* posOffsetWork) {
    m_posOffsetWork = posOffsetWork;
}

// The area of the belt conveyor that carries the fighters on the water of the river (a triangle that follows the stump).
void stEarth::createObjBeltConv() {
    m_beltData = new (Heaps::StageResource) stEarthBeltData;
    if (m_beltData != NULL) {
        memset(m_beltData, 0, sizeof(stEarthBeltData));
        m_beltData->m_points[0].m_x = -175.0f;
        m_beltData->m_points[0].m_y = 60.0f;
        m_beltData->m_points[1].m_x = -175.0f;
        m_beltData->m_points[1].m_y = -17.0f;
        m_beltData->m_points[2].m_x = -24.0f;
        m_beltData->m_points[2].m_y = -17.0f;
        m_beltData->m_speed = 0.0f;
        m_beltData->m_isRight = true;
        m_beltData->m_areaData.m_offsetPos.m_x = 0.0f;
        m_beltData->m_areaData.m_offsetPos.m_y = 0.0f;
        m_beltData->m_areaData.m_range.m_x = 0.0f;
        m_beltData->m_areaData.m_range.m_y = 0.0f;
        m_beltData->m_areaData.m_shapeType = static_cast<gfArea::ShapeType>(2);
        m_triggerBelt = g_stTriggerMng->createTrigger(Gimmick::Area_BeltConveyor, -1);
        fn_27_233808(m_triggerBelt, m_beltData);
        m_triggerBelt->setAreaSleep(true);
    }
}

void stEarth::update(float deltaFrame) {
    // HYPOTHESIS: the flag at 0xEB of the stage (m_isDevil in the header) says which camera range is used
    if (m_isDevil == 1) {
        setCameraLimitRange(-200.0f, 160.0f, 160.0f, -120.0f);
    } else {
        resetCameraLimitRange();
    }
    updateOniyon(deltaFrame);
    updateWeather(deltaFrame);
    updateChappy(deltaFrame);
    updatePellet(deltaFrame);
    updateBelt(deltaFrame);
}

// The oniyon: it waits (1) and comes (2) to a leaf where no pellet lies, stays there (3) and goes (4).
void stEarth::updateOniyon(float deltaFrame) {
    m_timerOniyon = m_timerOniyon - deltaFrame;
    if (m_timerOniyon < 0.0f) {
        m_timerOniyon = 0.0f;
    }
    grEarthOniyon* oniyon = static_cast<grEarthOniyon*>(getGround(2));
    if (oniyon != NULL) {
        grEarthLeaf* leafLeft = static_cast<grEarthLeaf*>(getGround(5));
        if (leafLeft != NULL) {
            grEarthLeaf* leafCenter = static_cast<grEarthLeaf*>(getGround(6));
            if (leafCenter != NULL) {
                grEarthLeaf* leafRight = static_cast<grEarthLeaf*>(getGround(7));
                stEarthData* data = static_cast<stEarthData*>(m_stageData);
                if (leafRight != NULL && data != NULL) {
                    switch (m_stateOniyon) {
                    case 0:
                        if (m_firstOniyon == 1) {
                            float r = randf();
                            float most = data->unk44;
                            float least = data->unk40;
                            m_firstOniyon = 0;
                            m_timerOniyon = least + (most - least) * r;
                        } else {
                            float r = randf();
                            m_timerOniyon = data->unk48 + (data->unk4C - data->unk48) * r;
                        }
                        m_stateOniyon = 1;
                        break;
                    case 1:
                        if (m_timerOniyon == 0.0f) {
                            if (data->unk50 <= randf()) {
                                bool found = false;
                                float r = randf();
                                u32 leaf;
                                if (0.33333334f <= r) {
                                    if (0.6666667f <= r) {
                                        leaf = 2;
                                    } else {
                                        leaf = 1;
                                    }
                                } else {
                                    leaf = 0;
                                }
                                int offset = 0;
                                for (u32 count = data->unk3C; count != 0; count--) {
                                    u8 pelletLeaf = reinterpret_cast<u8*>(m_pelletData)[offset + 9];
                                    if (pelletLeaf == 1) {
                                        if (leaf == 1) {
                                            found = true;
                                        }
                                    } else if (pelletLeaf == 0) {
                                        if (leaf == 0) {
                                            found = true;
                                        }
                                    } else if (pelletLeaf < 3 && leaf == 2) {
                                        found = true;
                                    }
                                    if (found) {
                                        break;
                                    }
                                    offset += 0x58;
                                }
                                if (found) {
                                    m_timerOniyon = (data->unk48 + (data->unk4C - data->unk48) * randf()) * 0.5f;
                                } else {
                                    m_colorOniyon = oniyon->selectColor(m_colorOniyon);
                                    m_stateOniyon = 2;
                                    if (leaf == 1) {
                                        m_leafOniyon = 1;
                                    } else if (leaf == 0) {
                                        m_leafOniyon = 0;
                                    } else if (leaf < 3) {
                                        m_leafOniyon = 2;
                                    }
                                }
                            } else {
                                m_timerOniyon = (data->unk48 + (data->unk4C - data->unk48) * randf()) * 0.5f;
                            }
                        }
                        break;
                    case 2: {
                        oniyon->requestLanding(m_leafOniyon);
                        bool notPre = !oniyon->isPreLanding();
                        u8 leaf = oniyon->getLandingLeaf();
                        Vec3f pos;
                        switch (leaf) {
                        case 0:
                            leafLeft->getPos(&pos, notPre);
                            break;
                        case 1:
                            leafCenter->getPos(&pos, notPre);
                            break;
                        case 2:
                            leafRight->getPos(&pos, notPre);
                            break;
                        default:
                            pos.m_x = 0.0f;
                            pos.m_y = 0.0f;
                            pos.m_z = 0.0f;
                            break;
                        }
                        oniyon->setPosLeaf(pos.m_x, pos.m_y, pos.m_z);
                        if (oniyon->isLanding() == 1) {
                            float r = randf();
                            float most = data->unk6C;
                            float least = data->unk68;
                            m_stateOniyon = 3;
                            m_timerOniyon = least + (most - least) * r;
                        }
                        break;
                    }
                    case 3: {
                        u8 leaf = oniyon->getLandingLeaf();
                        Vec3f pos;
                        switch (leaf) {
                        case 0:
                            leafLeft->getPos(&pos, 0);
                            break;
                        case 1:
                            leafCenter->getPos(&pos, 0);
                            break;
                        case 2:
                            leafRight->getPos(&pos, 0);
                            break;
                        default:
                            pos.m_x = 0.0f;
                            pos.m_y = 0.0f;
                            pos.m_z = 0.0f;
                            break;
                        }
                        oniyon->setPosLeaf(pos.m_x, pos.m_y, pos.m_z);
                        if (m_timerOniyon == 0.0f) {
                            m_stateOniyon = 4;
                        }
                        break;
                    }
                    case 4:
                        oniyon->requestTakeOff();
                        if (oniyon->isTakeOff() == 1) {
                            m_stateOniyon = 0;
                        }
                        break;
                    case 5:
                        break;
                    }
                }
            }
        }
    }
}

u8 grEarthOniyon::getLandingLeaf() {
    return m_landingLeaf;
}

void grEarthOniyon::setPosLeaf(float x, float y, float z) {
    m_posLeaf.m_x = x;
    m_posLeaf.m_y = y;
    m_posLeaf.m_z = z;
}

// The weather: fine (0) for a while, then maybe the rain comes (the sun turns to it (2), the rain runs (3 - 4), the sun
// turns back (5)). The river runs while it rains.
void stEarth::updateWeather(float deltaFrame) {
    grEarthSun* sun = static_cast<grEarthSun*>(getGround(1));
    stEarthData* data = static_cast<stEarthData*>(m_stageData);
    if (sun != NULL && data != NULL) {
        m_timerWeather = m_timerWeather - deltaFrame;
        if (m_timerWeather < 0.0f) {
            m_timerWeather = 0.0f;
        }
        m_timerSun = m_timerSun - deltaFrame;
        if (m_timerSun < 0.0f) {
            m_timerSun = 0.0f;
        }
        switch (m_stateWeather) {
        case 5:
            if (sun->isChangeWeather()) {
                m_weather = 3;
                registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), m_weather);
                sun->requestToFine();
                nw4r::g3d::ResFile scnFile(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE));
                if (scnFile.IsValid() && scnFile.HasResAnmScn()) {
                    m_timerSun = 0.0f;
                    m_timerWeather = scnFile.GetResAnmScn(m_weather).ptr()->m_animLength;
                }
                if (m_firstWeather != 1) {
                    if (m_seHandle >= 0) {
                        fn_80075F1C(g_sndSystem, m_seHandle, 0x3C);
                    }
                    m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x1CD9), 0, 0x3C, 0, -1);
                }
                m_stateWeather = 0;
            }
            break;
        case 0:
            if (m_timerWeather == 0.0f) {
                m_weather = 0;
                registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), m_weather);
                if (m_firstWeather == 1) {
                    float r = randf();
                    float most = data->unkB8;
                    float least = data->unkB4;
                    m_firstWeather = 0;
                    m_timerWeather = least + (most - least) * r;
                } else {
                    float r = randf();
                    m_timerWeather = data->unkBC + (data->unkC0 - data->unkBC) * r;
                }
                grEarthStump* stump = static_cast<grEarthStump*>(getGround(3));
                if (stump == NULL) {
                    return;
                }
                stump->setAILowPriority(0);
                stump->setCollisionAttr(4);
                m_stateWeather = 1;
            }
            if (m_timerSun == 0.0f && m_sunOn == 1) {
                grEarthRiver* river = static_cast<grEarthRiver*>(getGround(8));
                if (river != NULL) {
                    river->endRiver();
                }
                m_sunOn = 0;
                g_ecMgr->endEffect(m_efRain[0]);
                g_ecMgr->endEffect(m_efRain[1]);
                g_ecMgr->endEffect(m_efRain[2]);
                g_ecMgr->endEffect(m_efRain[3]);
            }
            break;
        case 1:
            if (m_timerWeather == 0.0f) {
                if (randf() > data->unkC4) {
                    m_stateWeather = 2;
                } else {
                    float r = randf();
                    float most = data->unkC0;
                    float least = data->unkBC;
                    m_stateWeather = 0;
                    m_timerWeather = least + (most - least) * r;
                }
            }
            break;
        case 2:
            if (sun->isChangeWeather()) {
                m_weather = 1;
                registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), m_weather);
                sun->requestToRain();
                nw4r::g3d::ResFile scnFile(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE));
                if (scnFile.IsValid() && scnFile.HasResAnmScn()) {
                    m_timerWeather = scnFile.GetResAnmScn(m_weather).ptr()->m_animLength;
                }
                m_timerSun = data->unkC8;
                m_sunOn = 0;
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x1CD7), 0, 0, 0, -1);
                m_stateWeather = 3;
            }
            break;
        case 3:
            if (m_timerWeather == 0.0f) {
                m_weather = 2;
                registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), m_weather);
                m_timerWeather = data->unkD4 + (data->unkD8 - data->unkD4) * randf();
                if (m_seHandle >= 0) {
                    fn_80075F1C(g_sndSystem, m_seHandle, 0x3C);
                }
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x1CD8), 0, 0x3C, 0, -1);
                Vec3f effectPos;
                effectPos.m_x = 0.0f;
                effectPos.m_y = 0.0f;
                effectPos.m_z = 0.0f;
                m_efRain[0] = g_ecMgr->setEffect(static_cast<EfID>(0x4E0001), &effectPos);
                Vec3f effectPos1;
                effectPos1.m_x = 0.0f;
                effectPos1.m_y = 0.0f;
                effectPos1.m_z = 0.0f;
                m_efRain[1] = g_ecMgr->setEffect(static_cast<EfID>(0x4E0002), &effectPos1);
                grEarthStump* stump = static_cast<grEarthStump*>(getGround(3));
                if (stump != NULL) {
                    stump->setAILowPriority(1);
                    stump->setCollisionAttr(0x1E);
                    m_stateWeather = 4;
                }
            }
            break;
        case 4:
            if (m_timerWeather == 0.0f) {
                m_stateWeather = 5;
            }
            if (m_timerSun == 0.0f && m_sunOn == 0) {
                grEarthRiver* river = static_cast<grEarthRiver*>(getGround(8));
                if (river != NULL) {
                    river->startRiver();
                    m_sunOn = 1;
                }
            }
            break;
        }
    }
}

// The chappy: it waits (0, 1), comes in (2), stays (3) and goes out (4).
void stEarth::updateChappy(float deltaFrame) {
    grEarthChappy* chappy = static_cast<grEarthChappy*>(getGround(0xE));
    if (chappy != NULL && getGround(3) != NULL) {
        stEarthData* data = static_cast<stEarthData*>(m_stageData);
        if (data != NULL) {
            m_timerChappy = m_timerChappy - deltaFrame;
            if (m_timerChappy < 0.0f) {
                m_timerChappy = 0.0f;
            }
            switch (m_stateChappy) {
            case 0:
                if (m_firstChappy == 1) {
                    float r = randf();
                    float most = data->unk8C;
                    float least = data->unk88;
                    m_firstChappy = 0;
                    m_timerChappy = least + (most - least) * r;
                } else {
                    float r = randf();
                    m_timerChappy = data->unk90 + (data->unk94 - data->unk90) * r;
                }
                m_stateChappy = 1;
                break;
            case 1:
                if (m_timerChappy == 0.0f && chappy->requestIn() == 1) {
                    m_timerChappy = data->unk98 + (data->unk9C - data->unk98) * randf();
                    void* posData = m_fileData->getData(Data_Type_Model, 0x6E, 0xFFFE);
                    if (posData != NULL) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                    m_stateChappy = 2;
                }
                break;
            case 2:
                if (chappy->isInEnd() == 1) {
                    m_stateChappy = 3;
                }
                break;
            case 3:
                if (m_timerChappy == 0.0f && chappy->requestOut() == 1) {
                    m_stateChappy = 4;
                }
                if (chappy->isOutEndForce() == 1) {
                    m_stateChappy = 0;
                }
                break;
            case 4:
                if (chappy->isOutEnd() == 1) {
                    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
                    if (posData != NULL) {
                        nw4r::g3d::ResFile posFile(posData);
                        m_stagePositions->loadPositionData(&posFile);
                    }
                    updateStagePositions();
                    m_stateChappy = 0;
                }
                break;
            }
        }
    }
}

// The pellets: a pellet is chosen from the data of the stage after a wait, and it gets the leaf (and the color and the
// size) that it will lie on.
void stEarth::updatePellet(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(m_stageData);
    if (data != NULL) {
        m_timerPellet = m_timerPellet - deltaFrame;
        if (m_timerPellet < 0.0f) {
            m_timerPellet = 0.0f;
        }
        switch (m_statePellet) {
        case 0:
            if (m_firstPellet == 1) {
                float r = randf();
                float most = data->unk04;
                float least = data->unk00;
                m_firstPellet = 0;
                m_timerPellet = least + (most - least) * r;
            } else {
                float r = randf();
                m_timerPellet = data->unk08 + (data->unk0C - data->unk08) * r;
            }
            m_statePellet = 1;
            break;
        case 1:
            if (m_timerPellet == 0.0f) {
                m_timerPellet = data->unk08 + (data->unk0C - data->unk08) * randf();
                if (!(randf() > data->unk10)) {
                    u8 count = data->unk3C;
                    for (u8 i = 0; i != count; i++) {
                        bool placed = false;
                        stEarthPelletData* pellet = &m_pelletData[i];
                        switch (pellet->m_state) {
                        case 0:
                            initPelletData(pellet);
                            pellet->m_state = 1;
                            // fall through
                        case 1: {
                            int riding = getCountPelletRideLeaf();
                            float r = randf();
                            if (r < 0.25f) {
                                if (riding == 0 && m_leafOniyon != 0) {
                                    pellet->m_leaf = 0;
                                    pellet->unk24 = 0.2f + 0.6f * randf();
                                place:
                                    m_lastColor = selectPelletColor(m_lastColor);
                                    placed = true;
                                    pellet->m_color = m_lastColor;
                                    pellet->m_state = 2;
                                }
                            } else if (r < 0.5f) {
                                if (riding == 0 && m_leafOniyon != 1) {
                                    pellet->m_leaf = 1;
                                    pellet->unk24 = 0.2f + 0.6f * randf();
                                    goto place;
                                }
                            } else if (r < 0.75f) {
                                if (riding == 0 && m_leafOniyon != 2) {
                                    pellet->m_leaf = 2;
                                    pellet->unk24 = 0.2f + 0.6f * randf();
                                    goto place;
                                }
                            } else {
                                float scale = 0.7f + 0.15f * randf();
                                if (isEnableSpacePellet(scale)) {
                                    pellet->m_leaf = 3;
                                    pellet->unk24 = scale;
                                    goto place;
                                }
                            }
                            break;
                        }
                        }
                        if (placed) {
                            break;
                        }
                    }
                }
            }
            break;
        }
    }
}

// The belt conveyor on the water follows the stump: the points of the triangle are taken from the nodes of the stump, and
// it is on only while the river runs.
void stEarth::updateBelt(float deltaFrame) {
    grEarthStump* stump = static_cast<grEarthStump*>(getGround(3));
    if (stump != NULL) {
        Vec3f start;
        Vec3f end;
        stump->getNodePosition(&start, 0, "taibokuPelletP1");
        stump->getNodePosition(&end, 0, "taibokuPelletP2");
        Vec3f dir = end - start;
        Vec3f scaled;
        grEarthVec3Scale(&scaled, &dir, 1.12f);
        end = start + scaled;
        end.m_y = end.m_y + 5.0f;
        m_beltData->m_points[0].m_x = start.m_x;
        m_beltData->m_points[0].m_y = (start.m_y + 5.0f) - 3.0f;
        m_beltData->m_points[1].m_x = end.m_x;
        m_beltData->m_points[1].m_y = end.m_y - 3.0f;
        m_beltData->m_points[2].m_x = start.m_x;
        m_beltData->m_points[2].m_y = end.m_y - 3.0f;
        m_beltData->m_speed = m_riverSpeed;
        fn_27_233B58(m_triggerBelt, m_beltData);
        if (m_riverSpeed == 0.0f) {
            m_triggerBelt->setAreaSleep(true);
        } else {
            m_triggerBelt->setAreaSleep(false);
        }
    }
}

void stEarth::initPelletData(stEarthPelletData* pellet) {
    if (pellet != NULL && m_stageData != NULL) {
        stEarthData* data = static_cast<stEarthData*>(m_stageData);
        pellet->m_state = 0;
        pellet->unk04 = 0.0f;
        pellet->unk08 = 0;
        pellet->m_leaf = 4;
        pellet->m_color = 3;
        pellet->unk0B = 0;
        pellet->unk0C = data->unk34;
        pellet->unk10 = data->unk30;
        pellet->m_pos.m_x = 0.0f;
        pellet->m_pos.m_y = 0.0f;
        pellet->m_pos.m_z = 0.0f;
        pellet->unk24 = 0.5f;
        pellet->unk20 = randf() * 90.0f + 45.0f;
        pellet->m_mtx.setIdentity();
    }
}

// The color of the pellet is chosen by the weights of the stage (the one that was dropped last is less likely).
int stEarth::selectPelletColor(int lastColor) {
    if (m_stageData == NULL) {
        return 0;
    }
    stEarthData* data = static_cast<stEarthData*>(m_stageData);
    float weight0 = data->unk14;
    float weight1 = data->unk18;
    float weight2 = data->unk1C;
    switch (lastColor) {
    case 1:
        weight1 = weight1 * data->unk20;
        break;
    case 0:
        weight0 = weight0 * data->unk20;
        break;
    case 2:
        weight2 = weight2 * data->unk20;
        break;
    }
    float value = (weight2 + (weight0 + weight1)) * randf();
    if (value < weight0) {
        return 0;
    }
    if (value < weight0 + weight1) {
        return 1;
    }
    return 2;
}

// How many pellets lie on one of the three leaves.
int stEarth::getCountPelletRideLeaf() {
    if (m_pelletData == NULL) {
        return 0;
    }
    if (m_stageData != NULL) {
        int count = 0;
        for (u8 i = 0; i != static_cast<stEarthData*>(m_stageData)->unk3C; i++) {
            if (m_pelletData[i].m_state == 3) {
                switch (m_pelletData[i].m_leaf) {
                case 0:
                case 1:
                case 2:
                    count++;
                    break;
                }
            }
        }
        return count;
    }
    return 0;
}

// Whether a pellet can be dropped on the stump: it must be far enough from the pellets that lie there already.
bool stEarth::isEnableSpacePellet(float rate) {
    if (m_pelletData == NULL) {
        return false;
    }
    grEarthStump* stump = static_cast<grEarthStump*>(getGround(3));
    if (stump == NULL) {
        return false;
    }
    stEarthData* data = static_cast<stEarthData*>(m_stageData);
    if (data == NULL) {
        return false;
    }
    Vec3f pos;
    stump->getPosPellet(rate, &pos);
    for (u8 i = 0; i != data->unk3C; i++) {
        stEarthPelletData* pellet = &m_pelletData[i];
        if (pellet->m_state == 3) {
            Vec3f pelletPos = pellet->m_pos;
            if (__fabsf(pos.m_x - pelletPos.m_x) < data->unk38) {
                return false;
            }
        }
    }
    return true;
}

void stEarth::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) {
    if (itemID == Item_Stage_Pellet) {
        *brres = &m_archivePelletBrres;
        *param = &m_archivePelletParam;
    }
}
