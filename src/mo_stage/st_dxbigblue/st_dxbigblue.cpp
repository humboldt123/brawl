#define MT_VEC3F_BIKE_UTILITY_OPERATORS // the stage uses the operators with the register order of the native Utility code
#include <cm/cm_camera_controller.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_triangular.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_utility.h>
#include <string.h>
#include <types.h>

#include <st_dxbigblue/gr_dxbigblue_ashiba.h>
#include <st_dxbigblue/gr_dxbigblue_ashiba_trainer.h>
#include <st_dxbigblue/gr_dxbigblue_bg.h>
#include <st_dxbigblue/gr_dxbigblue_car.h>
#include <st_dxbigblue/gr_dxbigblue_course.h>
#include <st_dxbigblue/gr_dxbigblue_course_loop.h>
#include <st_dxbigblue/gr_dxbigblue_falcon.h>
#include <st_dxbigblue/gr_dxbigblue_tyukei.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue.h>
#include <st_dxbigblue/st_dxbigblue_data.h>

// snd3DGenerator::isPlay (main, unnamed): whether the sound of the handle is still playing
extern "C" bool fn_800797A4(snd3DGenerator* generator, s32 handle);

// HYPOTHESIS: the original clamps the random place with an inline helper (max against the low limit, then min against the high one).
static inline u8 dxbigblueClampU8(u8 value, u8 low, u8 high) {
    value = value > low ? value : low;
    value = value < high ? value : high;
    return value;
}

stClassInfoImpl<Stages::DxBigBlue, stDxBigBlue> stDxBigBlue::bss_loc_14;

stDxBigBlue* stDxBigBlue::create() {
    return new (Heaps::StageInstance) stDxBigBlue;
}

stDxBigBlue::stDxBigBlue() : stMelee("stDxBigBlue", Stages::DxBigBlue) {
    m_bgState = 0;
    m_bgTimer = 0.0f;
    memset(m_posLimit, 0, sizeof(m_posLimit));
    m_loopEnable = 0;
    memset(&m_posBg, 0, sizeof(m_posBg));
    memset(&m_posBgPrev, 0, sizeof(m_posBgPrev));
    memset(&m_rotBg, 0, sizeof(m_rotBg));
    unk214 = 0.0f;
    unk218 = 0;
    for (u8 i = 0; i < 12; i++) {
        m_course[i].m_start.m_x = 0.0f;
        m_course[i].m_start.m_y = 0.0f;
        m_course[i].m_start.m_z = 0.0f;
        m_course[i].m_end.m_x = 0.0f;
        m_course[i].m_end.m_y = 0.0f;
        m_course[i].m_end.m_z = 0.0f;
        m_courseIdTbl[i] = 0xC;
    }
    m_courseState = 0;
    m_courseTimer = 0.0f;
    m_courseIdTop = 0xC;
    for (u8 i = 0; i < 30; i++) {
        m_carData[i].m_state = 10;
        m_carData[i].unk04 = 0.0f;
        m_carData[i].unk08 = 0.0f;
        m_carData[i].unk0C = 0.0f;
        m_carData[i].unk10 = 0.0f;
        m_carData[i].unk14 = 0.0f;
        m_carData[i].unk18 = 0.0f;
        m_carData[i].unk1C = 0.0f;
        m_carData[i].unk20 = 0.0f;
        m_carData[i].unk24 = 0.0f;
        m_carData[i].unk28 = 0.0f;
        m_carData[i].unk2C = 0.0f;
        m_carData[i].unk30 = 0;
        m_carData[i].unk34 = 0.0f;
        m_carData[i].unk38 = 0;
    }
    m_carState = 0;
    m_carTimer = 0.0f;
    memset(m_carTbl, 0, sizeof(m_carTbl));
    m_carTop = 0;
    m_carBottom = 0;
    m_carDir = 0;
    m_seCar[0].m_seHandle = -1;
    m_seCar[0].m_carPlace = 0;
    m_seCar[1].m_seHandle = -1;
    m_seCar[1].m_carPlace = 0;
    m_seCar[2].m_seHandle = -1;
    m_seCar[2].m_carPlace = 0;
    m_seCar[3].m_seHandle = -1;
    m_seCar[3].m_carPlace = 0;
    m_seCar[4].m_seHandle = -1;
    m_seCar[4].m_carPlace = 0;
    m_flyReady = 0;
    for (u8 i = 0; i < 6; i++) {
        m_flyData[i].m_state = 10;
        m_flyData[i].m_pos.m_x = 0.0f;
        m_flyData[i].m_pos.m_y = 0.0f;
        m_flyData[i].m_pos.m_z = 0.0f;
        m_flyData[i].unk10.m_x = 0.0f;
        m_flyData[i].unk10.m_y = 0.0f;
        m_flyData[i].unk10.m_z = 0.0f;
        m_flyData[i].m_rot.m_x = 0.0f;
        m_flyData[i].m_rot.m_y = 0.0f;
        m_flyData[i].m_rot.m_z = 0.0f;
        m_flyData[i].unk28 = 0.0f;
        m_flyData[i].unk2C = 0.0f;
        m_flyData[i].unk30 = 0;
        m_flyData[i].m_landed = 0;
    }
    m_flyState = 0;
    m_flyTimer = 0.0f;
    m_flyAshibaNum = 0;
    m_flyTyukeiNum = 0;
    m_eventEnd = 0;
}

stDxBigBlue::~stDxBigBlue() {
    clearListAll(&m_list);
    releaseArchive();
}

bool stDxBigBlue::loading() {
    return true;
}

void stDxBigBlue::createObj() {
    CameraController* camera = CameraController::getInstance();
    reinterpret_cast<u8*>(camera)[0x44] |= 4;
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x144);
    initPosPokeTrainer(4, 1);
    createObjBg(0);
    createObjBg(1);
    createObjCourse(2);
    createObjCourse(3);
    createObjCourse(4);
    createObjCourse(5);
    createObjCourse(6);
    createObjCourse(7);
    createObjCourse(8);
    createObjCourse(9);
    createObjCourse(10);
    createObjCourse(11);
    createObjCourse(12);
    createObjCourse(13);
    createCollision(m_fileData, 2, NULL);
    createObjAshiba(0xE);
    createObjAshiba(0xF);
    createObjAshiba(0x10);
    createObjAshibaTrainer(0x11);
    createObjFalcon(0x12);
    createObjTyukei(0x13);
    createCollision(m_fileData, 3, NULL);
    createObjCar();
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData != NULL) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x46);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    if (g_GameGlobal->m_modeMelee != NULL) {
        dxbigblueMeleeView* melee = reinterpret_cast<dxbigblueMeleeView*>(g_GameGlobal->m_modeMelee);
        if (melee->m_mode == 7 && melee->m_event == 0x9F) {
            m_eventEnd = 1;
        }
    }
}

// The sea and the sky behind the track.
void stDxBigBlue::createObjBg(int index) {
    grDxBigBlueBg* ground;
    switch (index) {
    case 0:
        ground = grDxBigBlueBg::create(1, "StgDxBigBlueSea", "grDxBigBlueSea");
        break;
    case 1:
        ground = grDxBigBlueBg::create(2, "StgDxBigBlueSky", "grDxBigBlueSky");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(&m_posBg.m_x);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setRotWork(&m_rotBg.m_x);
    }
}

// The three platforms that fly over the track.
void stDxBigBlue::createObjAshiba(int index) {
    grDxBigBlueAshiba* ground;
    stDxBigBlueFlyData* fly;
    switch (index) {
    case 0xE:
        ground = grDxBigBlueAshiba::create(0xB, "StgDxBigBlueBaseA", "grDxBigBlueAshibaA");
        fly = &m_flyData[0];
        break;
    case 0xF:
        ground = grDxBigBlueAshiba::create(0xC, "StgDxBigBlueBaseB", "grDxBigBlueAshibaB");
        fly = &m_flyData[1];
        break;
    case 0x10:
        ground = grDxBigBlueAshiba::create(0xD, "StgDxBigBlueBaseC", "grDxBigBlueAshibaC");
        fly = &m_flyData[2];
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setFlyDataWork(fly);
    }
}

// The platform of the Pokemon Trainer.
void stDxBigBlue::createObjAshibaTrainer(int index) {
    grDxBigBlueAshibaTrainer* ground;
    switch (index) {
    case 0x11:
        ground = grDxBigBlueAshibaTrainer::create(0xE, "StgDxBigBluePTBase", "grDxBigBlueAshibaT");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosTrainerWork(m_pokeTrainerPos);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setFlyDataWork(&m_flyData[3]);
        ground->setVisibility(isPokemonTrainer());
    }
}

// The Blue Falcon.
void stDxBigBlue::createObjFalcon(int index) {
    grDxBigBlueFalcon* ground;
    switch (index) {
    case 0x12:
        ground = grDxBigBlueFalcon::create(0x15, "StgDxBigBlueFalcon", "grDxBigBlueFalcon");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setFlyDataWork(&m_flyData[4]);
    }
}

// The relay (Tyukeisha) platform.
void stDxBigBlue::createObjTyukei(int index) {
    grDxBigBlueTyukei* ground;
    switch (index) {
    case 0x13:
        ground = grDxBigBlueTyukei::create(0x16, "StgDxBigBlueTyukeisha", "grDxBigBlueTyukei");
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setFlyDataWork(&m_flyData[5]);
    }
}

// The twelve courses of the track.
void stDxBigBlue::createObjCourse(int index) {
    grDxBigBlueCourse* ground;
    int course;
    switch (index) {
    case 2:
        ground = grDxBigBlueCourse::create(0x1F, "StgDxBigBlueCDansa", "grDxBigBlueCDansa");
        course = 0;
        break;
    case 3:
        ground = grDxBigBlueCourse::create(0x20, "StgDxBigBlueCFlatA", "grDxBigBlueCFlatA");
        course = 1;
        break;
    case 4:
        ground = grDxBigBlueCourse::create(0x21, "StgDxBigBlueCJump1", "grDxBigBlueCJump1");
        course = 2;
        break;
    case 5:
        ground = grDxBigBlueCourse::create(0x22, "StgDxBigBlueCJump2", "grDxBigBlueCJump2");
        course = 3;
        break;
    case 6:
        ground = grDxBigBlueCourseLoop::create(0x23, "StgDxBigBlueCLoop", "grDxBigBlueCLoop");
        course = 4;
        break;
    case 7:
        ground = grDxBigBlueCourse::create(0x24, "StgDxBigBlueCPitA", "grDxBigBlueCPitA");
        course = 5;
        break;
    case 8:
        ground = grDxBigBlueCourse::create(0x25, "StgDxBigBlueCSaka4", "grDxBigBlueCSaka4");
        course = 6;
        break;
    case 9:
        ground = grDxBigBlueCourse::create(0x26, "StgDxBigBlueCSlopeDownA", "grDxBigBlueCSlopeDownA");
        course = 7;
        break;
    case 10:
        ground = grDxBigBlueCourse::create(0x27, "StgDxBigBlueCSlopeDownB", "grDxBigBlueCSlopeDownB");
        course = 8;
        break;
    case 11:
        ground = grDxBigBlueCourse::create(0x28, "StgDxBigBlueCSlopeUpA", "grDxBigBlueCSlopeUpA");
        course = 9;
        break;
    case 12:
        ground = grDxBigBlueCourse::create(0x29, "StgDxBigBlueCSlopeUpB", "grDxBigBlueCSlopeUpB");
        course = 10;
        break;
    case 13:
        ground = grDxBigBlueCourse::create(0x2A, "StgDxBigBlueCStartA", "grDxBigBlueCStartA");
        course = 11;
        break;
    default:
        ground = NULL;
        break;
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosWork(&m_course[0].m_start);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setPosBgWork(&m_posBg);
        ground->setRotWork(&m_rotBg);
        ground->setCourseID(course);
        ground->setCourseIDTopWork(&m_courseIdTop);
        ground->setAttachWork(&m_courseIdTbl[course]);
    }
}

// The cars: the places are shuffled and every place gets its car.
void stDxBigBlue::createObjCar() {
    for (u8 i = 0; i < 30; i++) {
        m_carTbl[i] = i;
    }
    int n = 29;
    for (u8 i = 0; i < 30; i++) {
        u8 other = (u8)(randf() * n);
        other = dxbigblueClampU8(other, 0, 29);
        u8 tmp = m_carTbl[i];
        m_carTbl[i] = m_carTbl[other];
        m_carTbl[other] = tmp;
    }
    for (u8 i = 0; i < 30; i++) {
        createObjCar1(i);
    }
}

void stDxBigBlue::createObjCar1(int index) {
    grDxBigBlueCar* ground;
    u8 collisionIndex = index + 0x1E;
    ground = grDxBigBlueCar::create((s16)(index + 0x82), "TopN", "grDxBigBlueCar");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setPosLimitWork(&m_posLimit[0].m_x);
        ground->setPosBgWork(&m_posBg);
        ground->setIndex(index);
        ground->setIndexTopWork(&m_carTop);
        ground->setTblWork(m_carTbl);
        ground->setCarDataWork(m_carData);
        createCollision(m_fileData, collisionIndex, ground);
    }
}

void stDxBigBlue::update(float deltaFrame) {
    updateLimit();
    updateCourse(deltaFrame);
    updateBg(deltaFrame);
    updateCar(deltaFrame);
    updateFly(deltaFrame);
}

void stDxBigBlue::updateLimit() {
    CameraController* camera = CameraController::getInstance();
    float minX;
    float minY = camera->unk160;
    minX = camera->unk158;
    m_posLimit[0].m_x = minX;
    m_posLimit[0].m_y = minY;
    m_posLimit[0].m_z = 0.0f;
    float maxX;
    float maxY = camera->unk164;
    maxX = camera->unk15C;
    m_posLimit[1].m_x = maxX;
    m_posLimit[1].m_y = maxY;
    m_posLimit[1].m_z = 0.0f;
}

void stDxBigBlue::updateCourse(float deltaFrame) {
    m_courseTimer -= deltaFrame;
    if (m_courseTimer < 0.0f) {
        m_courseTimer = 0.0f;
    }
    switch (m_courseState) {
    case 0:
        addList(0xB, &m_list);
        m_courseIdTbl[11] = 0xD;
        m_courseState = 1;
        break;
    case 1:
        m_courseState = 3;
        break;
    case 2:
        break;
    case 3:
        updateCourseClear(deltaFrame);
        updateCourseAdd(deltaFrame);
        updateCourseLoop(deltaFrame);
        break;
    }
}

void stDxBigBlue::updateCourseClear(float deltaFrame) {
    SListNode* node = getListTop(&m_list);
    if (node != NULL && m_courseIdTbl[node->m_index] == 0xC) {
        clearList(node->m_index, &m_list);
    }
}

void stDxBigBlue::updateCourseAdd(float deltaFrame) {
    SListNode* last = getListEnd(&m_list);
    if (last == NULL) {
        return;
    }
    Vec3f pos;
    pos = m_course[last->m_index].m_start;
    if (pos.m_x > m_posLimit[1].m_x) {
        return;
    }
    u8 course;
    if (last->m_index == 0xB) {
        course = 1;
    } else {
        stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
        if (data == NULL) {
            return;
        }
        course = (u8)(randf() * 11.0f);
        if (getList(course, &m_list) != NULL) {
            return;
        }
        pos = m_course[last->m_index].m_end;
        switch (course) {
        case 4:
            if (pos.m_y > 700.0f) {
                return;
            }
            m_loopEnable = 0;
            break;
        case 0:
        case 7:
        case 8:
            if (pos.m_y <= data->unk08) {
                return;
            }
            break;
        case 2:
        case 3:
        case 9:
        case 10:
            if (pos.m_y >= data->unk04) {
                return;
            }
            break;
        }
        if (last->m_index == 4 && m_loopEnable == 0) {
            return;
        }
        pos.m_y += m_course[course].m_end.m_y - m_course[course].m_start.m_y;
        if (pos.m_y < 0.0f || pos.m_y > 3000.0f) {
            goto done; // MATCH-ONLY: the original leaves the check with a plain branch to the end
        }
    }
    last = getListEnd(&m_list);
    if (last != NULL) {
        addList(course, &m_list);
        m_courseIdTbl[course] = last->m_index;
    }
done:;
}

void stDxBigBlue::updateCourseLoop(float deltaFrame) {
    SListNode* top = getListTop(&m_list);
    if (top != NULL) {
        m_courseIdTop = top->m_index;
        if (top->m_index == 4) {
            if (m_rotBg.m_z == 360.0f) {
                m_loopEnable = 1;
            }
        } else {
            m_loopEnable = 0;
            m_rotBg.m_z = 0.0f;
        }
    }
}

// The background scrolls with the track: it follows the height of the ground of the track under the camera.
void stDxBigBlue::updateBg(float deltaFrame) {
    m_bgTimer -= deltaFrame;
    if (m_bgTimer < 0.0f) {
        m_bgTimer = 0.0f;
    }
    switch (m_bgState) {
    case 0:
        m_bgState = 1;
        break;
    case 1: {
        stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
        if (data != NULL) {
            Vec3f from(0.0f, 500.0f, 0.0f);
            Vec3f dir(0.0f, -1000.0f, 0.0f);
            Vec3f hit;
            Vec3f normal;
            if (!stRayCheck(&from, &dir, &hit, &normal, true, getCollision(0), false, 1) || normal.m_y <= 0.0f) {
                if (unk218 == 0) {
                    Vec3f a(0.0f, -m_posBgPrev.m_y, 0.0f);
                    Vec3f b(data->unk6C, -m_posBg.m_y, 0.0f);
                    unk218 = 1;
                    Vec3f d;
                    d = b - a;
                    d.normalize();
                    float scale = data->unk7C;
                    d.m_x *= scale;
                    d.m_y *= scale;
                    d.m_z *= scale;
                    unk214 = d.m_y;
                }
                float v = unk214 - data->unk78 * deltaFrame;
                m_posBgPrev.m_x = m_posBg.m_x;
                m_posBgPrev.m_y = m_posBg.m_y;
                unk214 = v;
                m_posBgPrev.m_z = m_posBg.m_z;
                m_posBg.m_y = m_posBg.m_y - v * deltaFrame;
            } else {
                if (unk218 == 1) {
                    float y = m_posBg.m_y;
                    float v = unk214 - data->unk78 * deltaFrame;
                    m_posBgPrev.m_x = m_posBg.m_x;
                    m_posBgPrev.m_y = y;
                    unk214 = v;
                    float newY = m_posBg.m_y - v * deltaFrame;
                    m_posBgPrev.m_z = m_posBg.m_z;
                    m_posBg.m_y = newY;
                    if (data->unk00 < hit.m_y + (newY - y)) {
                        unk218 = 0;
                        m_posBg.m_y = y + (data->unk00 - hit.m_y);
                    }
                } else {
                    float y = m_posBg.m_y;
                    m_posBgPrev.m_x = m_posBg.m_x;
                    m_posBgPrev.m_y = y;
                    m_posBgPrev.m_z = m_posBg.m_z;
                    float newY = m_posBg.m_y + (data->unk00 - hit.m_y);
                    m_posBg.m_y = newY;
                    if (newY - y > 20.0f) {
                        unk214 = 0.0f;
                        unk218 = 1;
                        m_posBg.m_y = y;
                    }
                }
                m_flyReady = 1;
            }
        }
        break;
    }
    }
}

void stDxBigBlue::updateCar(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_carTimer -= deltaFrame;
    if (m_carTimer < 0.0f) {
        m_carTimer = 0.0f;
    }
    switch (m_carState) {
    case 0:
        if (m_flyReady == 0) {
            return;
        }
        updateCarAddInit(deltaFrame);
        m_carState = 1;
    case 1: {
        m_carTimer = data->unk10 + (data->unk14 - data->unk10) * randf();
        m_carState = 3;
    }
    case 3:
        updateCarPos(deltaFrame);
        updateCarSE(deltaFrame);
        updateCarClear(deltaFrame);
        if (m_carTimer != 0.0f) {
            updateCarAdd(deltaFrame);
        }
        break;
    case 2:
    default:
        break;
    }
}

// The cars follow the ground of the track: they drive on it and sink when a fighter lands on the roof.
void stDxBigBlue::updateCarPos(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    for (u32 i = 0, num = data->unk18; i != num; i++) {
        u32 place = m_carTop + i;
        if (place >= 0x1E) {
            place -= 0x1E;
        }
        u8 id = m_carTbl[place];
        stDxBigBlueCarData* car = &m_carData[id];
        Vec3f hit;
        Vec3f normal;
        if (!carRayCheck(&hit, &normal, id) || normal.m_y <= 0.0f) {
            if (car->unk30 == 0) {
                car->unk30 = 1;
                Vec3f b(data->unk6C, car->unk08, 0.0f);
                Vec3f a(0.0f, car->unk14, 0.0f);
                Vec3f d;
                d = b - a;
                d.normalize();
                float scale = data->unk84;
                d.m_x *= scale;
                d.m_y *= scale;
                d.m_z *= scale;
                car->unk28 = d.m_y;
            }
            car->unk28 = 0.0f;
            car->unk10 = car->unk04;
            car->unk14 = car->unk08;
            car->unk18 = car->unk0C;
            float y = car->unk08;
            if (y > 0.0f) {
                float t;
                if (car->unk04 > 0.0f) {
                    t = car->unk04 / m_posLimit[1].m_x;
                } else {
                    t = car->unk04 / m_posLimit[0].m_x;
                }
                float lo = 0.0f;
                float hi = 1.0f;
                t = dxbigblueClamp(t, lo, hi);
                float c = nw4r::math::CosFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(t * 16384.0f))));
                car->unk08 = car->unk08 - (0.15f + 0.15f * (1.0f - c)) * deltaFrame;
                if (car->unk08 < 0.0f) {
                    car->unk08 = 0.0f;
                }
            } else if (y < 0.0f) {
                car->unk08 = y + 0.25f * deltaFrame;
                if (car->unk08 > 0.0f) {
                    car->unk08 = 0.0f;
                }
            }
            car->unk24 = data->unk58;
        } else if (car->unk30 == 1) {
            car->unk28 = car->unk28 - 0.75f * data->unk80 * deltaFrame;
            if (car->unk28 < -1.0f) {
                car->unk28 = -1.0f;
            }
            car->unk10 = car->unk04;
            car->unk14 = car->unk08;
            car->unk18 = car->unk0C;
            car->unk08 = car->unk08 + car->unk28 * deltaFrame;
            if (hit.m_y + data->unk2C > car->unk08) {
                car->unk30 = 0;
                car->unk08 = hit.m_y + data->unk2C;
            }
        } else {
            car->unk10 = car->unk04;
            car->unk14 = car->unk08;
            car->unk18 = car->unk0C;
            car->unk08 = hit.m_y + data->unk2C;
            car->unk24 = 1.40625f * nw4r::math::Atan2FIdx(normal.m_y, normal.m_x) - 90.0f;
            if (nw4r::math::FAbs(car->unk08 - car->unk14) > 20.0f) {
                car->unk30 = 1;
                car->unk28 = 0.0f;
                car->unk08 = car->unk14;
            }
        }
    }
}

void stDxBigBlue::updateCarSE(float deltaFrame) {
    if (m_stageData == NULL) {
        return;
    }
    for (s32 i = 0; i != 5; i++) {
        if (m_seCar[i].m_seHandle != -1 && !fn_800797A4(&m_seCar[i].m_sound, m_seCar[i].m_seHandle)) {
            m_seCar[i].m_seHandle = -1;
        }
        if (m_seCar[i].m_seHandle == -1) {
            continue;
        }
        stDxBigBlueCarData* car = &m_carData[m_carTbl[m_seCar[i].m_carPlace]];
        switch (car->m_state) {
        case 8:
            m_seCar[i].m_sound.stopSE(m_seCar[i].m_seHandle, 0x78);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            m_seCar[i].m_sound.setPos(reinterpret_cast<Vec3f*>(&car->unk04));
            break;
        }
    }
}

void stDxBigBlue::updateCarClear(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    for (u32 i = 0, num = data->unk18; i != num; i++) {
        u32 place = m_carTop + i;
        if (place >= 0x1E) {
            place -= 0x1E;
        }
        stDxBigBlueCarData* car = &m_carData[m_carTbl[place]];
        switch (car->m_state) {
        case 7:
            break;
        case 6:
            if (!isCarEnableOut(place)) {
                car->m_state = 5;
            } else {
                car->m_state = 7;
            }
            break;
        case 8:
            if (place == m_carTop) {
                m_carTop++;
                if (m_carTop == 0x1E) {
                    m_carTop = 0;
                }
                m_carDir = 1;
                if (m_carNum != 0) {
                    m_carNum--;
                }
            } else if (place == m_carBottom) {
                if (m_carBottom == 0) {
                    m_carBottom = 0x1D;
                } else {
                    m_carBottom--;
                }
                m_carDir = 0;
                if (m_carNum != 0) {
                    m_carNum--;
                }
            }
            car->m_state = 9;
            m_carTimer = data->unk10 + (data->unk14 - data->unk10) * randf();
            break;
        }
    }
}

void stDxBigBlue::updateCarAdd(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data != NULL && m_carNum != data->unk18) {
        if (randf() < 0.5f) {
            m_carDir = 1;
        } else {
            m_carDir = 0;
        }
        u32 place;
        if (m_carDir == 1) {
            place = m_carBottom + 1;
            if (place >= 0x1E) {
                place -= 0x1E;
            }
        } else {
            place = 0x1D;
            if (m_carTop != 0) {
                place = m_carTop - 1;
            }
        }
        stDxBigBlueCarData* car = &m_carData[m_carTbl[place]];
        if (car->m_state == 10) {
            if (m_carDir == 1) {
                car->m_state = 4;
                car->unk38 = 0;
                car->unk04 = m_posLimit[1].m_x + data->unk68;
                car->unk08 = m_posBg.m_y;
                car->unk0C = 0.0f;
                m_carBottom = place;
            } else {
                car->m_state = 4;
                car->unk38 = 1;
                car->unk04 = m_posLimit[0].m_x - data->unk68;
                car->unk08 = m_posBg.m_y;
                car->unk0C = 0.0f;
                m_carTop = place;
            }
            m_carNum++;
            addCarSE(place);
        }
    }
}

// The line of cars at the start: four cars drive in from both sides and the others come at the places in between.
void stDxBigBlue::updateCarAddInit(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_carTop = (u8)(randf() * 30.0f);
    u8 start = dxbigblueClampU8(m_carTop, 0, 29);
    m_carTop = start;
    u32 place0 = start;
    if (place0 >= 0x1E) {
        place0 -= 0x1E;
    }
    stDxBigBlueCarData* car = &m_carData[m_carTbl[place0]];
    car->m_state = 5;
    car->unk38 = 1;
    car->unk04 = m_posLimit[0].m_x - data->unk68;
    car->unk08 = m_posBg.m_y;
    car->unk0C = 0.0f;
    m_seCar[0].m_seHandle = m_seCar[0].m_sound.playSE(static_cast<SndID>(0x1DC9), 0, 0, -1);
    m_seCar[0].m_carPlace = place0;
    u32 place1 = m_carTop + 1;
    if (place1 >= 0x1E) {
        place1 -= 0x1E;
    }
    car = &m_carData[m_carTbl[place1]];
    car->m_state = 5;
    car->unk38 = 1;
    car->unk04 = m_posLimit[0].m_x - data->unk68;
    car->unk08 = m_posBg.m_y;
    car->unk0C = 0.0f;
    m_seCar[1].m_seHandle = m_seCar[1].m_sound.playSE(static_cast<SndID>(0x1DCA), 0, 0, -1);
    m_seCar[1].m_carPlace = place1;
    u32 place2 = m_carTop + 2;
    if (place2 >= 0x1E) {
        place2 -= 0x1E;
    }
    car = &m_carData[m_carTbl[place2]];
    car->m_state = 5;
    car->unk38 = 0;
    car->unk04 = m_posLimit[1].m_x + data->unk68;
    car->unk08 = m_posBg.m_y;
    car->unk0C = 0.0f;
    m_seCar[2].m_seHandle = m_seCar[2].m_sound.playSE(static_cast<SndID>(0x1DCB), 0, 0, -1);
    m_seCar[2].m_carPlace = place2;
    u32 place3 = m_carTop + 3;
    if (place3 >= 0x1E) {
        place3 -= 0x1E;
    }
    car = &m_carData[m_carTbl[place3]];
    car->m_state = 5;
    car->unk38 = 0;
    car->unk04 = m_posLimit[1].m_x + data->unk68;
    car->unk08 = m_posBg.m_y;
    car->unk0C = 0.0f;
    m_seCar[3].m_seHandle = m_seCar[3].m_sound.playSE(static_cast<SndID>(0x1DCC), 0, 0, -1);
    m_seCar[3].m_carPlace = place3;
    m_carBottom = place3;
    m_carNum = 4;
    for (u32 i = 0, num = data->unk18; i != num; i++) {
        u32 p = m_carTop + i;
        if (p >= 0x1E) {
            p -= 0x1E;
        }
        stDxBigBlueCarData* c = &m_carData[m_carTbl[p]];
        c->unk04 = -0.5f * data->unk1C;
        c->unk04 = c->unk04 + data->unk1C * ((float)(i + 1) / (float)(num + 1));
        c->unk0C = 0.0f;
        Vec3f hit;
        Vec3f normal;
        if (carRayCheck(&hit, &normal, m_carTbl[p]) && normal.m_y > 0.0f) {
            c->unk08 = hit.m_y + data->unk2C;
        }
    }
}

void stDxBigBlue::updateFly(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_flyTimer -= deltaFrame;
    if (m_flyTimer < 0.0f) {
        m_flyTimer = 0.0f;
    }
    switch (m_flyState) {
    default:
        return;
    case 0:
        if (m_flyReady == 0) {
            return;
        }
        m_flyData[4].m_state = 1;
        m_flyData[4].unk2C = data->unkCC;
        if (isPokemonTrainer() == 1) {
            m_flyData[3].m_state = 1;
            m_flyData[3].unk2C = data->unkCC;
        }
        m_flyState = 1;
    case 1: {
        u32 a = data->unkDC;
        m_flyAshibaNum = (s32)((float)a + (float)(data->unkE0 - a) * randf());
        u32 b = data->unk11C;
        m_flyTyukeiNum = (s32)((float)b + (float)(data->unk120 - b) * randf());
        m_flyState = 2;
    }
    case 2:
        m_flyTimer = data->unk88 + (data->unk8C - data->unk88) * randf();
        m_flyState = 3;
    case 3:
        if (m_flyTimer == 0.0f) {
            if (m_flyData[4].m_state == 10) {
                if (m_flyAshibaNum == 0) {
                    m_flyData[4].m_state = 1;
                    m_flyData[4].unk2C = data->unkCC;
                    u32 a = data->unkDC;
                    m_flyAshibaNum = (s32)((float)a + (float)(data->unkE0 - a) * randf());
                } else if (m_flyTyukeiNum == 0) {
                    m_flyData[5].m_state = 1;
                    m_flyData[5].unk2C = data->unkF4 + (data->unkF8 - data->unkF4) * randf();
                    u32 b = data->unk11C;
                    m_flyTyukeiNum = (s32)((float)b + (float)(data->unk120 - b) * randf());
                } else {
                    float r = randf();
                    int slot;
                    if (r < 0.33333334f) {
                        slot = 0;
                    } else if (r < 0.6666667f) {
                        slot = 1;
                    } else {
                        slot = 2;
                    }
                    if (m_flyData[slot].m_state == 10) {
                        m_flyData[slot].m_state = 1;
                        m_flyData[slot].unk2C = data->unk90 + (data->unk94 - data->unk90) * randf();
                        m_flyAshibaNum--;
                        if (m_flyAshibaNum < 0) {
                            m_flyAshibaNum = 0;
                        }
                        m_flyTyukeiNum--;
                        if (m_flyTyukeiNum < 0) {
                            m_flyTyukeiNum = 0;
                        }
                    }
                }
            }
            m_flyState = 2;
        }
    }
    updateFlyPos(deltaFrame);
}

void stDxBigBlue::updateFlyPos(float deltaFrame) {
    if (m_flyData[0].m_state != 10) {
        updateAshibaPos(deltaFrame, 0);
    }
    if (m_flyData[1].m_state != 10) {
        updateAshibaPos(deltaFrame, 1);
    }
    if (m_flyData[2].m_state != 10) {
        updateAshibaPos(deltaFrame, 2);
    }
    if (m_flyData[3].m_state != 10) {
        updateAshibaPos(deltaFrame, 3);
    }
    if (m_flyData[4].m_state != 10) {
        updateFalconPos(deltaFrame);
    }
    if (m_flyData[5].m_state != 10) {
        updateTyukeiPos(deltaFrame);
    }
}

// A platform follows the ground under it, floats above the cars and the other platforms.
void stDxBigBlue::updateAshibaPos(float deltaFrame, int index) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    stDxBigBlueFlyData* fly = &m_flyData[index];
    Vec3f from(fly->m_pos.m_x, 250.0f, fly->m_pos.m_z);
    Vec3f dir(0.0f, -500.0f, 0.0f);
    Vec3f hit;
    Vec3f normal;
    float height;
    if (!stRayCheck(&from, &dir, &hit, &normal, true, getCollision(0), false, 1) || normal.m_y <= (height = 0.0f)) {
        float flyHeight;
        fly->unk30 = 1;
        fly->m_rot.m_z = 0.0f;
        fly->unk10 = fly->m_pos;
        if (isCar(&fly->m_pos.m_x) == 1) {
            height += 20.0f;
        }
        if (isFly(&flyHeight, &fly->m_pos.m_x, true) == 1) {
            height += flyHeight;
        }
        if (m_flyData[4].m_state != 10) {
            height += m_flyData[4].m_pos.m_y;
        }
        float diff = fly->m_pos.m_y - fly->unk2C;
        float speed = 0.0f;
        if (fly->m_landed == 1) {
            if (diff > speed) {
                speed -= data->unkA8 * deltaFrame;
            } else {
                speed += data->unkAC * deltaFrame;
            }
        } else if (diff > speed) {
            speed -= data->unk98 * deltaFrame;
        } else {
            speed += data->unk98 * deltaFrame;
        }
        fly->unk28 = speed;
        fly->m_pos.m_y = fly->m_pos.m_y + fly->unk28 * deltaFrame;
        if (fly->m_pos.m_y < data->unk90 + (data->unk94 - data->unk90) * 0.5f + nw4r::math::FAbs(data->unk00)) {
            fly->unk2C = fly->unk2C + data->unk98;
        } else {
            fly->unk2C = fly->unk2C - data->unk98;
        }
        return;
    }
    float flyHeight;
    if (isCar(&fly->m_pos.m_x) == 1) {
        height += 20.0f;
    }
    if (isFly(&flyHeight, &fly->m_pos.m_x, true) == 1) {
        height += flyHeight;
    }
    if (m_flyData[4].m_state != 10) {
        height += m_flyData[4].m_pos.m_y;
    }
    fly->unk10 = fly->m_pos;
    float diff = fly->m_pos.m_y - (height + (hit.m_y + fly->unk2C));
    float speed = 0.0f;
    if (fly->m_landed == 1) {
        if (diff > speed) {
            speed -= 0.5f * data->unkA8 * deltaFrame;
        } else {
            speed += 0.5f * data->unkAC * deltaFrame;
        }
    } else if (diff > speed) {
        speed -= 0.5f * data->unk98 * deltaFrame;
    } else {
        speed += 0.5f * data->unk98 * deltaFrame;
    }
    fly->unk28 = speed;
    fly->m_pos.m_y = fly->m_pos.m_y + fly->unk28 * deltaFrame;
    if (hit.m_y + fly->unk2C > fly->m_pos.m_y) {
        fly->unk30 = 0;
        fly->m_pos.m_y = hit.m_y + fly->unk2C;
    }
    float nx = normal.m_x;
    float ny = normal.m_y;
    fly->m_rot.m_z = 1.40625f * nw4r::math::Atan2FIdx(ny, nx) - 90.0f;
    if (0.0f != height) {
        fly->unk2C = fly->unk2C + nw4r::math::FAbs(fly->unk28);
    } else {
        fly->unk2C = fly->unk2C - nw4r::math::FAbs(fly->unk28);
    }
    fly->unk2C = dxbigblueClamp(fly->unk2C, data->unk90, data->unk94);
}

// The Falcon follows the ground under it.
void stDxBigBlue::updateFalconPos(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    stDxBigBlueFlyData* fly = &m_flyData[4];
    Vec3f from(fly->m_pos.m_x, 250.0f, fly->m_pos.m_z);
    Vec3f dir(0.0f, -500.0f, 0.0f);
    Vec3f hit;
    Vec3f normal;
    float zero;
    float speed;
    if (!stRayCheck(&from, &dir, &hit, &normal, true, getCollision(0), false, 1) || normal.m_y <= (zero = 0.0f)) {
        fly->unk30 = 1;
        zero = 0.0f;
        fly->m_rot.m_z = zero;
        float diff = fly->m_pos.m_y - fly->unk2C;
        if (diff > zero) {
            speed = zero - 0.5f * data->unkD0 * deltaFrame;
        } else {
            speed = zero + 0.5f * data->unkD0 * deltaFrame;
        }
        fly->unk28 = speed;
        fly->m_pos.m_y = fly->m_pos.m_y + fly->unk28 * deltaFrame;
        if (fly->m_pos.m_y < data->unkCC + nw4r::math::FAbs(data->unk00)) {
            fly->unk2C = fly->unk2C + data->unkD0 * deltaFrame;
        } else {
            fly->unk2C = fly->unk2C - data->unkD0 * deltaFrame;
        }
        return;
    }
    fly->unk10 = fly->m_pos;
    float diff = fly->m_pos.m_y - (hit.m_y + fly->unk2C);
    if (diff > zero) {
        speed = zero - 0.5f * data->unkD0 * deltaFrame;
    } else {
        speed = zero + 0.5f * data->unkD0 * deltaFrame;
    }
    fly->unk28 = speed;
    fly->m_pos.m_y = fly->m_pos.m_y + fly->unk28 * deltaFrame;
    if (hit.m_y + fly->unk2C > fly->m_pos.m_y) {
        fly->unk30 = 0;
        fly->m_pos.m_y = hit.m_y + fly->unk2C;
    }
    float nx = normal.m_x;
    float ny = normal.m_y;
    fly->m_rot.m_z = 1.40625f * nw4r::math::Atan2FIdx(ny, nx) - 90.0f;
    fly->unk2C = fly->unk2C - nw4r::math::FAbs(fly->unk28);
    if (fly->unk2C <= data->unkCC) {
        fly->unk2C = data->unkCC;
    }
    nx = normal.m_x;
    ny = normal.m_y;
    fly->m_rot.m_z = 1.40625f * nw4r::math::Atan2FIdx(ny, nx) - 90.0f;
}

// The relay platform follows the ground under it, floats above the cars and the Falcon.
void stDxBigBlue::updateTyukeiPos(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return;
    }
    stDxBigBlueFlyData* fly = &m_flyData[5];
    Vec3f from(fly->m_pos.m_x, 250.0f, fly->m_pos.m_z);
    Vec3f dir(0.0f, -500.0f, 0.0f);
    Vec3f hit;
    Vec3f normal;
    float height;
    if (!stRayCheck(&from, &dir, &hit, &normal, true, getCollision(0), false, 1) || normal.m_y <= (height = 0.0f)) {
        float flyHeight;
        fly->unk30 = 1;
        height = 0.0f;
        fly->unk10 = fly->m_pos;
        if (isCar(&fly->m_pos.m_x) == 1) {
            height += 20.0f;
        }
        if (isFly(&flyHeight, &fly->m_pos.m_x, true) == 1) {
            height += flyHeight;
        }
        if (m_flyData[4].m_state != 10) {
            height += m_flyData[4].m_pos.m_y;
        }
        float speed = 0.0f;
        if (fly->m_pos.m_y - (fly->unk2C + height) > speed) {
            speed += fly->unk28 * deltaFrame;
        } else {
            speed -= fly->unk28 * deltaFrame;
        }
        fly->m_pos.m_y = fly->m_pos.m_y + speed * deltaFrame;
        if (fly->m_pos.m_y < data->unkF4 + (data->unkF8 - data->unkF4) * 0.5f + nw4r::math::FAbs(data->unk00)) {
            fly->unk2C = fly->unk2C + deltaFrame * ((data->unk108 - data->unk104) * 0.5f);
        } else {
            fly->unk2C = fly->unk2C - deltaFrame * ((data->unk108 - data->unk104) * 0.5f);
        }
        return;
    }
    float flyHeight;
    if (isCar(&fly->m_pos.m_x) == 1) {
        height += 20.0f;
    }
    if (isFly(&flyHeight, &fly->m_pos.m_x, true) == 1) {
        height += flyHeight;
    }
    if (m_flyData[4].m_state != 10) {
        height += m_flyData[4].m_pos.m_y;
    }
    fly->unk10 = fly->m_pos;
    float speed = 0.0f;
    if (fly->m_pos.m_y - (height + (hit.m_y + fly->unk2C)) > speed) {
        speed += 0.5f * fly->unk28 * deltaFrame;
    } else {
        speed -= 0.5f * fly->unk28 * deltaFrame;
    }
    fly->m_pos.m_y = fly->m_pos.m_y + speed * deltaFrame;
    if (hit.m_y + fly->unk2C > fly->m_pos.m_y) {
        fly->unk30 = 0;
        fly->m_pos.m_y = hit.m_y + fly->unk2C;
    }
    if (0.0f != height) {
        fly->unk2C = fly->unk2C + nw4r::math::FAbs(fly->unk28);
    } else {
        fly->unk2C = fly->unk2C - nw4r::math::FAbs(fly->unk28);
    }
    fly->unk2C = dxbigblueClamp(fly->unk2C, data->unkF4, data->unkF8);
}

bool stDxBigBlue::carRayCheck(Vec3f* hitPos, Vec3f* normal, u8 index) {
    Vec3f from(m_carData[index].unk04, 500.0f, m_carData[index].unk0C);
    Vec3f dir(0.0f, -1000.0f, 0.0f);
    return stRayCheck(&from, &dir, hitPos, normal, true, getCollision(0), false, 1);
}

bool stDxBigBlue::isCarEnableOut(u32 place) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return false;
    }
    for (u32 i = 0, count = data->unk18; i != count; i++) {
        u32 p = m_carTop + i;
        if (p >= 0x1E) {
            p -= 0x1E;
        }
        if (p != place) {
            switch (m_carData[m_carTbl[p]].m_state) {
            case 6:
            case 7:
            case 8:
            case 9:
                return false;
            }
        }
    }
    return true;
}

bool stDxBigBlue::isCar(float* pos) {
    if (pos == NULL) {
        return false;
    }
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(m_stageData);
    if (data == NULL) {
        return false;
    }
    for (u32 i = 0, count = data->unk18; i != count; i++) {
        u32 p = m_carTop + i;
        if (p >= 0x1E) {
            p -= 0x1E;
        }
        float x = m_carData[m_carTbl[p]].unk04;
        float v = *pos;
        if (v > x - 75.0f && v < 75.0f + x) {
            return true;
        }
    }
    return false;
}

bool stDxBigBlue::isFly(float* height, float* pos, bool below) {
    if (height == NULL) {
        return false;
    }
    if (pos == NULL) {
        return false;
    }
    float width;
    float thick;
    float top = 0.0f;
    stDxBigBlueFlyData* fly = m_flyData;
    *height = 0.0f;
    for (int i = 0; i < 6; i++, fly++) {
        if (fly->m_state != 10) {
            switch (i) {
            case 0:
                width = 120.0f;
                thick = 40.0f;
                break;
            case 1:
                width = 100.0f;
                thick = 40.0f;
                break;
            case 2:
                width = 100.0f;
                thick = 40.0f;
                break;
            case 3:
                width = 100.0f;
                thick = 40.0f;
                break;
            case 4:
                width = 280.0f;
                thick = 100.0f;
                break;
            case 5:
                width = 80.0f;
                thick = width;
                break;
            }
            if (pos[0] > fly->m_pos.m_x - width * 1.5f && pos[0] < fly->m_pos.m_x + width * 1.5f) {
                if (below == 1) {
                    if (pos[1] > fly->m_pos.m_y && top < fly->m_pos.m_y) {
                        top = fly->m_pos.m_y + thick * 0.5f;
                    }
                } else {
                    if (pos[1] < fly->m_pos.m_y && top < fly->m_pos.m_y) {
                        top = fly->m_pos.m_y + thick * 0.5f;
                    }
                }
            }
        }
    }
    if (top == 0.0f) {
        return false;
    }
    *height = top;
    return true;
}

bool stDxBigBlue::isFlyFalcon(float* height, float* pos, bool below) {
    if (height == NULL) {
        return false;
    }
    if (pos == NULL) {
        return false;
    }
    float width;
    float thick;
    float top = 0.0f;
    *height = 0.0f;
    for (u8 i = 0; i != 6; i++) {
        stDxBigBlueFlyData* fly = &m_flyData[i];
        if (fly->m_state == 10) {
            continue;
        }
        switch (i) {
        case 4:
            width = 280.0f;
            thick = 100.0f;
            break;
        default:
            continue;
        }
        if (pos[0] > fly->m_pos.m_x - width * 1.5f && pos[0] < fly->m_pos.m_x + width * 1.5f) {
            if (below == 1) {
                if (pos[1] > fly->m_pos.m_y && top < fly->m_pos.m_y) {
                    top = fly->m_pos.m_y + thick * 0.5f;
                }
            } else {
                if (pos[1] < fly->m_pos.m_y && top < fly->m_pos.m_y) {
                    top = fly->m_pos.m_y + thick * 0.5f;
                }
            }
        }
    }
    if (top == 0.0f) {
        return false;
    }
    *height = top;
    return true;
}

void stDxBigBlue::addCarSE(u8 place) {
    SndID id = static_cast<SndID>(place);
    for (int i = 0; i < 5; i++) {
        if (m_seCar[i].m_seHandle == -1) {
            switch (i) {
            case 0:
                id = static_cast<SndID>(0x1DC9);
                break;
            case 1:
                id = static_cast<SndID>(0x1DCA);
                break;
            case 2:
                id = static_cast<SndID>(0x1DCB);
                break;
            case 3:
                id = static_cast<SndID>(0x1DCC);
                break;
            case 4:
                id = static_cast<SndID>(0x1DC9);
                break;
            }
            m_seCar[i].m_seHandle = m_seCar[i].m_sound.playSE(id, 0, 0x78, -1);
            m_seCar[i].m_carPlace = place;
            return;
        }
    }
}

void stDxBigBlue::addList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = new (Heaps::StageResource) SListNode;
    if (node != NULL) {
        node->m_index = index;
        list->PushBack(node);
    }
}

void stDxBigBlue::addListTop(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = new (Heaps::StageResource) SListNode;
    if (node != NULL) {
        node->m_index = index;
        list->Insert(list->GetBeginIter(), node);
    }
}

void stDxBigBlue::clearList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = getList(index, list);
    if (node != NULL) {
        list->Erase(node);
        delete node;
    }
}

void stDxBigBlue::clearListAll(nw4r::ut::LinkList<SListNode, 0>* list) {
    for (u32 i = 0, count = list->GetSize(); i != count; i++) {
        SListNode* node = &list->GetFront();
        list->Erase(list->GetBeginIter());
        delete node;
    }
}

stDxBigBlue::SListNode* stDxBigBlue::getList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    u32 count = list->GetSize();
    SListNode* node = &list->GetFront();
    for (u32 i = 0; i != count; i++) {
        if (index == node->m_index) {
            return node;
        }
        node = static_cast<SListNode*>(node->GetNext());
    }
    return NULL;
}

stDxBigBlue::SListNode* stDxBigBlue::getListTop(nw4r::ut::LinkList<SListNode, 0>* list) {
    return &list->GetFront();
}

stDxBigBlue::SListNode* stDxBigBlue::getListEnd(nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = &list->GetFront();
    u32 position = 0;
    u32 count = list->GetSize();
    u32 last = count - 1;
    for (; position != count; position++) {
        if (position == last) {
            return node;
        }
        node = static_cast<SListNode*>(node->GetNext());
    }
    return NULL;
}

bool stDxBigBlue::isEventEnd() {
    return m_eventEnd != 0;
}
