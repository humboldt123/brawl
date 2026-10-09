#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <ft/ft_manager.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

#include <st_dxrcruise/st_dxrcruise.h>

// HYPOTHESIS: the scene-wide object at 0x805A0360 has a flag byte at +0x40 (bit 0x20) that the stage sets while it exists.
struct stDxCruiseSceneFlags {
    char _0[0x40];
    u8 pad : 2;
    u8 stageActive : 1;
    u8 pad2 : 5;
};
extern stDxCruiseSceneFlags* lbl_805A0360;

// MATCH-ONLY: the original reads a few bytes of the melee init data directly.
struct stDxCruiseMeleeView {
    char _0[8];
    u8 gameMode : 6;   // 0x08
    u8 pad8 : 2;
    char _9[5];        // 0x09
    u8 speedClass : 3; // 0x0E
    u8 padE : 5;
    char _f[1];        // 0x0F
    u8 eventId;        // 0x10
};

// Unnamed helpers of sora_melee the stage calls directly.
// HYPOTHESIS: sets a ground's blend/speed constant (on = 1)
extern "C" void fn_27_279228(Ground* ground, bool on);
// HYPOTHESIS: gives the position of the fighter with this entry id (false when it cannot be found)
extern "C" bool fn_27_2397E4(Stage* stage, int entryId, Vec3f* pos);

// Reads the "landed on" weight of a madein ground (grMadein::m_304 at +0x170 is protected).
class grCruiseMadein : public grMadein {
public:
    float getLandWeight() { return m_304; }
    bool isLandWeightAtLeast(float weight) { return m_304 >= weight; }
};

static inline grCruiseMadein* cruiseGround(stDxCruise* stage, int index) {
    return static_cast<grCruiseMadein*>(stage->getGround(index));
}

static inline float stCruiseClamp(float lo, float hi, float value) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

stClassInfoImpl<Stages::DxCruise, stDxCruise> stDxCruise::bss_loc_14;

stDxCruise* stDxCruise::create() {
    return new (Heaps::StageInstance) stDxCruise;
}

stDxCruise::stDxCruise() : stMelee("stDxCruise", Stages::DxCruise) {
    initStageData();
    memset(&m_unkFD4, 0, 0x28);
    m_seesawAngle = 0.0f;
    m_unkFD4 = 0.0f;
    m_seesawDir = 0.0f;
    m_seesawSpeed = 0.0f;
    m_count = 0;
    m_prevCount = 0;
    m_weightRight = 0.0f;
    m_weightLeft = 0.0f;
    m_settleFrames = 0;
    m_seesawState = 0;
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
    m_unk1008 = 0;
    m_speedInit = 0;
    m_speedScale = 1.0f;
    m_lastMotionFrame = 0.0f;
    lbl_805A0360->stageActive = 1;
}

stDxCruise::~stDxCruise() {
    lbl_805A0360->stageActive = 0;
    releaseArchive();
}

bool stDxCruise::loading() {
    return true;
}

void stDxCruise::createObj() {
    testStageParamInit(m_fileData, 10);
    addGround(grMadein::create(7, "", "enkei", Heaps::StageInstance));
    addGround(grMadein::create(1, "", "base", Heaps::StageInstance));
    addGround(grMadein::create(3, "", "CarpetRouteB", Heaps::StageInstance));
    addGround(grMadein::create(4, "", "CarpetRouteC", Heaps::StageInstance));
    addGround(grMadein::create(5, "", "CarpetRouteF", Heaps::StageInstance));
    addGround(grMadein::create(0, "", "Ashiba", Heaps::StageInstance));
    addGround(grMadein::create(2, "", "Carpet", Heaps::StageInstance));
    addGround(grMadein::create(2, "", "Carpet", Heaps::StageInstance));
    addGround(grMadein::create(2, "", "Carpet", Heaps::StageInstance));
    addGround(grMadein::create(9, "", "SeasawBodyC", Heaps::StageInstance));
    addGround(grMadein::create(10, "", "SeasawBodyL", Heaps::StageInstance));
    addGround(grMadein::create(11, "", "SeasawBodyR", Heaps::StageInstance));
    addGround(grMadein::create(12, "", "SeasawCore", Heaps::StageInstance));
    addGround(grMadein::create(8, "", "null", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(6, "", "chikuwa", Heaps::StageInstance));
    addGround(grMadein::create(13, "", "chikuwa", Heaps::StageInstance));
    Ground* ground;
    for (u32 i = 0, groundNum = getGroundNum(); i != groundNum; i++) {
        ground = getGround(i);
        if (ground != NULL) {
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
        }
    }
    float zero = 0.0f;
    for (int i = 0; i < 17; i++) {
        createCollision(m_fileData, 4, getGround(i + 14));
        m_blockEvent[i].set(0.0f, 0.0f);
        m_blockHeight[i] = zero;
    }
    for (int i = 0; i < 3; i++) {
        createCollision(m_fileData, 3, getGround(i + 6));
    }
    createCollision(m_fileData, 2, NULL);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    float scaleValue = 0.66f;
    for (u32 i = 0; i < getGroundNum(); i++) {
        Vec3f scale;
        scale.m_x = scaleValue;
        scale.m_y = scaleValue;
        scale.m_z = scaleValue;
        static_cast<grMadein*>(getGround(i))->initializeEntity();
        if (i - 2 <= 2) {
            static_cast<grMadein*>(getGround(i))->startEntity();
        } else {
            static_cast<grMadein*>(getGround(i))->startEntityAutoLoop();
        }
        static_cast<grGimmick*>(getGround(i))->setScale(&scale);
    }
    for (int i = 0; i < 3; i++) {
        fn_27_279228(getGround(i + 2), true);
        u8* flags = reinterpret_cast<u8*>(getGround(i + 6)) + 0x16C;
        *flags |= 1;
        m_carpetFlag[i] = 0;
    }
    for (int i = 0; i < 17; i++) {
        u8* flags = reinterpret_cast<u8*>(getGround(i + 14)) + 0x16C;
        *flags |= 1;
    }
    *(reinterpret_cast<u8*>(getGround(10)) + 0x16C) |= 1;
    *(reinterpret_cast<u8*>(getGround(11)) + 0x16C) |= 1;
    loadStageAttrParam(m_fileData, 30);
    initPosPokeTrainer(4, 1);
    if (!isPokemonTrainer()) {
        static_cast<grMadein*>(getGround(31))->endEntity();
    }
    stDxCruiseMeleeView* melee = reinterpret_cast<stDxCruiseMeleeView*>(g_GameGlobal->m_modeMelee);
    if (melee != NULL) {
        if (melee->gameMode == 7) {
            if (melee->eventId == 0x8C) {
                m_unk1008 = 1;
            }
        }
    }
}

void stDxCruise::update(float deltaFrame) {
    if (m_unk1008 == 1) {
        if (m_speedInit == 0) {
            stDxCruiseMeleeView* melee = reinterpret_cast<stDxCruiseMeleeView*>(g_GameGlobal->m_modeMelee);
            if (melee == NULL) {
                return;
            }
            if (melee->gameMode != 7) {
                return;
            }
            switch (melee->speedClass) {
                case 0:
                    m_speedScale = 0.5f;
                    break;
                case 1:
                    m_speedScale = 0.75f;
                    break;
                case 2:
                    m_speedScale = 0.9f;
                    break;
                default:
                    m_speedScale = 1.0f;
                    break;
            }
            m_speedInit = 1;
        }
        if (m_speedInit == 1) {
            *reinterpret_cast<float*>(reinterpret_cast<u8*>(g_GameGlobal->m_stageData) + 8) = m_speedScale;
        }
    }
    // The chikuwa blocks: a block falls away once a fighter has stood on it, then flickers back in.
    for (int i = 0; i < 17; i++) {
        switch (m_blockEvent[i].getPhase()) {
            case 0:
                if (cruiseGround(this, i + 14)->isLandWeightAtLeast(30.0f) == true) {
                    m_blockEvent[i].setPhase(1);
                    m_blockEvent[i].m_manualFramesLeft = 0.0f;
                }
                break;
            case 1:
                m_blockEvent[i].m_manualFramesLeft += deltaFrame;
                if (m_blockEvent[i].m_manualFramesLeft > 60.0f) {
                    m_blockEvent[i].setPhase(2);
                    m_blockEvent[i].m_manualFramesLeft = 0.0f;
                    m_blockSpeed[i] = 0.0f;
                }
                m_blockHeight[i] = -0.25f + (0.5f - 1.0f * randf());
                break;
            case 2: {
                m_blockSpeed[i] -= 0.03f;
                if (m_blockSpeed[i] < -2.0f) {
                    m_blockSpeed[i] = -2.0f;
                }
                m_blockHeight[i] += deltaFrame * m_blockSpeed[i];
                int block = i + 14;
                Vec3f pos;
                pos = static_cast<grGimmick*>(getGround(block))->getPos();
                if (pos.m_y <= 2.0f * *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x154)) {
                    m_blockEvent[i].setPhase(3);
                    static_cast<grMadein*>(getGround(block))->endEntity();
                    getGround(block)->setEnableCollisionStatus(false);
                }
                break;
            }
            case 3:
                m_blockEvent[i].m_manualFramesLeft += deltaFrame;
                if (m_blockEvent[i].m_manualFramesLeft >= 60.0f) {
                    m_blockEvent[i].m_manualFramesLeft = 0.0f;
                    m_blockEvent[i].setPhase(4);
                }
                break;
            case 4: {
                int block = i + 14;
                static_cast<grMadein*>(getGround(block))->startEntity();
                getGround(block)->setEnableCollisionStatus(true);
                m_blockEvent[i].setPhase(5);
            }
                // FALL-THROUGH
            case 5: {
                int block = i + 14;
                m_blockHeight[i] = 0.0f;
                m_blockEvent[i].m_manualFramesLeft += deltaFrame;
                if (m_blockEvent[i].m_manualFramesLeft - 10.0f * (float)(int)(m_blockEvent[i].m_manualFramesLeft / 10.0f) < 3.0f) {
                    getGround(block)->setVisibility(0);
                } else {
                    getGround(block)->setVisibility(1);
                }
                if (m_blockEvent[i].m_manualFramesLeft > 120.0f) {
                    getGround(block)->setVisibility(1);
                    m_blockEvent[i].setPhase(0);
                }
                break;
            }
        }
    }
    // The carpets: they fly away when landed on, flicker and come back after a while.
    for (int i = 0; i < 3; i++) {
        switch (m_carpetEvent[i].getPhase()) {
            case 0:
                if (cruiseGround(this, i + 6)->isLandWeightAtLeast(1.0f) == true) {
                    fn_27_279228(getGround(i + 2), false);
                    m_carpetEvent[i].setPhase(1);
                } else {
                    fn_27_279228(getGround(i + 2), true);
                }
                break;
            case 1:
                if (static_cast<grMadein*>(getGround(i + 2))->isEndEntity() == true) {
                    m_carpetEvent[i].setPhase(2);
                    m_carpetEvent[i].m_manualFramesLeft = 0.0f;
                }
                if (cruiseGround(this, i + 6)->isLandWeightAtLeast(1.0f) == true) {
                    fn_27_279228(getGround(i + 2), false);
                    m_carpetFlag[i] = 1;
                    m_carpetEvent[i].m_manualFramesLeft = 0.0f;
                } else {
                    fn_27_279228(getGround(i + 2), true);
                    m_carpetEvent[i].m_manualFramesLeft += deltaFrame;
                    if (m_carpetEvent[i].m_manualFramesLeft > 200.0f) {
                        m_carpetEvent[i].setPhase(2);
                        m_carpetEvent[i].m_manualFramesLeft = 0.0f;
                    }
                }
                break;
            case 2:
                if (static_cast<grMadein*>(getGround(i + 2))->isEndEntity() == false) {
                    if (cruiseGround(this, i + 6)->isLandWeightAtLeast(1.0f) == true) {
                        getGround(i + 6)->setVisibility(1);
                        m_carpetEvent[i].setPhase(1);
                        break;
                    }
                }
                m_carpetEvent[i].m_manualFramesLeft += deltaFrame;
                if (m_carpetEvent[i].m_manualFramesLeft - 10.0f * (float)(int)(m_carpetEvent[i].m_manualFramesLeft / 10.0f) < 4.0f) {
                    getGround(i + 6)->setVisibility(0);
                } else {
                    getGround(i + 6)->setVisibility(1);
                }
                if (m_carpetEvent[i].m_manualFramesLeft > 120.0f) {
                    getGround(i + 6)->setVisibility(1);
                    static_cast<grMadein*>(getGround(i + 6))->endEntity();
                    getGround(i + 6)->setEnableCollisionStatus(false);
                    m_carpetEvent[i].setPhase(3);
                    m_carpetEvent[i].m_manualFramesLeft = 0.0f;
                }
                break;
            case 3:
                m_carpetEvent[i].m_manualFramesLeft += deltaFrame;
                if (m_carpetEvent[i].m_manualFramesLeft > 200.0f) {
                    static_cast<grMadein*>(getGround(i + 2))->endEntity();
                    static_cast<grMadein*>(getGround(i + 2))->startEntity();
                    fn_27_279228(getGround(i + 2), true);
                    m_carpetEvent[i].setPhase(4);
                    m_carpetEvent[i].m_manualFramesLeft = 0.0f;
                    m_carpetFlag[i] = 0;
                }
                break;
            case 4:
                getGround(i + 6)->setEnableCollisionStatus(true);
                static_cast<grMadein*>(getGround(i + 6))->startEntityAutoLoop();
                m_carpetEvent[i].setPhase(0);
                break;
        }
    }
    Vec3f pos;
    getGround(1)->getNodePosition(&pos, 0, "ctrN");
    static_cast<grGimmick*>(getGround(5))->setPos(&pos);
    static_cast<grGimmick*>(getGround(0))->setPos(&pos);
    static_cast<grGimmick*>(getGround(2))->setPos(&pos);
    static_cast<grGimmick*>(getGround(3))->setPos(&pos);
    static_cast<grGimmick*>(getGround(4))->setPos(&pos);
    static_cast<grGimmick*>(getGround(5))->setPos(&pos);
    getGround(5)->updateG3dProcCalcWorld();
    getGround(5)->getNodePosition(&pos, 0, "SeesawN");
    static_cast<grGimmick*>(getGround(9))->setPos(&pos);
    static_cast<grGimmick*>(getGround(10))->setPos(&pos);
    static_cast<grGimmick*>(getGround(11))->setPos(&pos);
    static_cast<grGimmick*>(getGround(12))->setPos(&pos);
    // Fighters standing on the seesaw tip it towards the heavier side.
    if (cruiseGround(this, 11)->isLandWeightAtLeast(1.0f) == true) {
        int instance = 0;
        int entryId = g_ftManager->getEntryIdFromTaskId(cruiseGround(this, 11)->getLanderTaskId(), &instance);
        Vec3f fighterPos;
        if (entryId != -1) {
            if (fn_27_2397E4(this, entryId, &fighterPos)) {
                Vec3f seesawPos = static_cast<grGimmick*>(getGround(9))->getPos();
                float distance = mtSqrtf((seesawPos.m_x - fighterPos.m_x) * (seesawPos.m_x - fighterPos.m_x)
                                         + (seesawPos.m_y - fighterPos.m_y) * (seesawPos.m_y - fighterPos.m_y));
                if (distance >= 4.0f) {
                    m_weightLeft += 0.03f * distance;
                }
                m_count++;
            }
        }
    }
    if (cruiseGround(this, 10)->isLandWeightAtLeast(1.0f) == true) {
        int instance = 0;
        int entryId = g_ftManager->getEntryIdFromTaskId(cruiseGround(this, 10)->getLanderTaskId(), &instance);
        Vec3f fighterPos;
        if (entryId != -1) {
            if (fn_27_2397E4(this, entryId, &fighterPos)) {
                Vec3f seesawPos = static_cast<grGimmick*>(getGround(9))->getPos();
                float distance = mtSqrtf((seesawPos.m_x - fighterPos.m_x) * (seesawPos.m_x - fighterPos.m_x)
                                         + (seesawPos.m_y - fighterPos.m_y) * (seesawPos.m_y - fighterPos.m_y));
                if (distance >= 4.0f) {
                    m_weightRight += 0.03f * distance;
                }
                m_count++;
            }
        }
    }
    Seasaw();
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock1N");
    pos.m_y += m_blockHeight[0];
    static_cast<grGimmick*>(getGround(14))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock2N");
    pos.m_y += m_blockHeight[1];
    static_cast<grGimmick*>(getGround(15))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock3N");
    pos.m_y += m_blockHeight[2];
    static_cast<grGimmick*>(getGround(16))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock4N");
    pos.m_y += m_blockHeight[3];
    static_cast<grGimmick*>(getGround(17))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock5N");
    pos.m_y += m_blockHeight[4];
    static_cast<grGimmick*>(getGround(18))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock6N");
    pos.m_y += m_blockHeight[5];
    static_cast<grGimmick*>(getGround(19))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock7N");
    pos.m_y += m_blockHeight[6];
    static_cast<grGimmick*>(getGround(20))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock8N");
    pos.m_y += m_blockHeight[7];
    static_cast<grGimmick*>(getGround(21))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock9N");
    pos.m_y += m_blockHeight[8];
    static_cast<grGimmick*>(getGround(22))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock10N");
    pos.m_y += m_blockHeight[9];
    static_cast<grGimmick*>(getGround(23))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock11N");
    pos.m_y += m_blockHeight[10];
    static_cast<grGimmick*>(getGround(24))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock12N");
    pos.m_y += m_blockHeight[11];
    static_cast<grGimmick*>(getGround(25))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock13N");
    pos.m_y += m_blockHeight[12];
    static_cast<grGimmick*>(getGround(26))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock14N");
    pos.m_y += m_blockHeight[13];
    static_cast<grGimmick*>(getGround(27))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock15N");
    pos.m_y += m_blockHeight[14];
    static_cast<grGimmick*>(getGround(28))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock16N");
    pos.m_y += m_blockHeight[15];
    static_cast<grGimmick*>(getGround(29))->setPos(&pos);
    getGround(5)->getNodePosition(&pos, 0, "ChikuwaBlock17N");
    pos.m_y += m_blockHeight[16];
    static_cast<grGimmick*>(getGround(30))->setPos(&pos);
    getGround(2)->getNodePosition(&pos, 0, "CarpetBRouteN");
    static_cast<grGimmick*>(getGround(6))->setPos(&pos);
    getGround(3)->getNodePosition(&pos, 0, "CarpetCRouteN");
    static_cast<grGimmick*>(getGround(7))->setPos(&pos);
    getGround(4)->getNodePosition(&pos, 0, "CarpetFRouteN");
    static_cast<grGimmick*>(getGround(8))->setPos(&pos);
    if (isPokemonTrainer()) {
        getGround(5)->getNodePosition(&pos, 0, "PTCarpetN");
        static_cast<grGimmick*>(getGround(31))->setPos(&pos);
        getGround(31)->getNodePosition(&pos, 0, "PokemonTrainer1N");
        m_pokeTrainerPos[0] = pos;
        getGround(31)->getNodePosition(&pos, 0, "PokemonTrainer2N");
        m_pokeTrainerPos[1] = pos;
        getGround(31)->getNodePosition(&pos, 0, "PokemonTrainer3N");
        m_pokeTrainerPos[2] = pos;
        getGround(31)->getNodePosition(&pos, 0, "PokemonTrainer4N");
        m_pokeTrainerPos[3] = pos;
    }
    updateAI(deltaFrame);
}

// Reports the moving hazards of the course (the sweeping walls) to the AI as danger zones, timed by the motion frame
// of ground 1.
void stDxCruise::updateAI(float deltaFrame) {
    Ground* ground = getGround(1);
    if (ground != NULL) {
        CameraController* camera = CameraController::getInstance();
        Vec3f cornerB;
        Vec3f cornerA;
        cornerA.m_x = camera->unk158;
        cornerA.m_y = camera->unk160;
        cornerA.m_z = 0.0f;
        cornerB.m_x = camera->unk15C;
        cornerB.m_y = camera->unk164;
        cornerB.m_z = 0.0f;
        float frame = ground->getMotionFrame(0);
        Vec2f zoneMax;
        Vec2f zoneMin;
        if (frame > 1800.0f && frame < 2450.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, (frame - 1800.0f) / 400.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMax.m_x = cornerB.m_x;
            zoneMax.m_y = cornerB.m_y;
            zoneMin.m_y = cornerB.m_y + 0.5f * (cornerA.m_y - cornerB.m_y);
            zoneMin.m_x = cornerB.m_x - s * (0.4f * (cornerB.m_x - cornerA.m_x));
            m_dangerZone[0] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[0], false, false);
        } else if (m_dangerZone[0] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[0]);
            m_dangerZone[0] = -1;
        }
        if (frame > 2900.0f && frame < 3000.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, (frame - 2900.0f) / 100.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMax.m_y = cornerB.m_y;
            zoneMin.m_y = cornerA.m_y;
            zoneMax.m_x = cornerA.m_x + s * (0.2f * (cornerB.m_x - cornerA.m_x));
            m_dangerZone[1] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[1], false, false);
        } else if (frame >= 3000.0f && frame < 4100.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, 1.0f - (frame - 3000.0f) / 1000.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMin.m_y = cornerA.m_y;
            zoneMax.m_y = cornerB.m_y;
            zoneMax.m_x = cornerA.m_x + (0.1f + 0.1f * s) * (cornerB.m_x - cornerA.m_x);
            m_dangerZone[1] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[1], false, false);
        } else if (frame >= 4100.0f && frame < 4400.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, 1.0f - (frame - 4100.0f) / 100.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMin.m_y = cornerA.m_y;
            zoneMax.m_y = cornerB.m_y;
            zoneMax.m_x = cornerA.m_x + s * (0.1f * (cornerB.m_x - cornerA.m_x));
            m_dangerZone[1] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[1], false, false);
        } else if (frame >= 4400.0f && frame < 5200.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, (frame - 4400.0f) / 300.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMin.m_y = cornerA.m_y;
            zoneMax.m_y = cornerB.m_y;
            zoneMax.m_x = cornerA.m_x + (0.2f + 0.1f * s) * (cornerB.m_x - cornerA.m_x);
            m_dangerZone[1] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[1], false, false);
        } else if (m_dangerZone[1] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[1]);
            m_dangerZone[1] = -1;
        }
        if (frame > 2000.0f && frame < 3000.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, (frame - 2000.0f) / 500.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMax.m_x = cornerB.m_x;
            zoneMax.m_y = cornerB.m_y;
            zoneMin.m_y = cornerB.m_y + s * (0.3f * (cornerA.m_y - cornerB.m_y));
            m_dangerZone[2] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[2], false, false);
        } else if (frame >= 3000.0f && frame < 3600.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, 1.0f - (frame - 3000.0f) / 150.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMax.m_x = cornerB.m_x;
            zoneMax.m_y = cornerB.m_y;
            zoneMin.m_y = cornerB.m_y + s * (0.3f * (cornerA.m_y - cornerB.m_y));
            m_dangerZone[2] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[2], false, false);
        } else if (frame > 6800.0f && frame < 6970.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, (frame - 6800.0f) / 60.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMin.m_y = cornerA.m_y;
            zoneMax.m_x = cornerB.m_x;
            zoneMax.m_y = cornerA.m_y - s * (0.5f * (cornerA.m_y - cornerB.m_y));
            m_dangerZone[2] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[2], false, false);
        } else if (frame > 6970.0f && frame < 7070.0f) {
            float t = stCruiseClamp(0.0f, 1.0f, 1.0f - (frame - 6970.0f) / 100.0f);
            float s = nw4r::math::SinIdx((u16)(16384.0f * t));
            zoneMin.m_x = cornerA.m_x;
            zoneMin.m_y = cornerA.m_y;
            zoneMax.m_x = cornerB.m_x;
            zoneMax.m_y = cornerA.m_y - s * (0.5f * (cornerA.m_y - cornerB.m_y));
            m_dangerZone[2] = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone[2], false, false);
        } else if (m_dangerZone[2] != -1) {
            g_aiMgr->delDangerZone(m_dangerZone[2]);
            m_dangerZone[2] = -1;
        }
    }
}

void stDxCruise::initStageData() { }

// The seesaw: tips towards the side with more weight on it, swings back to level when nobody stands on it.
void stDxCruise::Seasaw() {
    float angle = m_seesawAngle;
    float absAngle = __fabsf(angle);
    int turns = (int)(absAngle / 360.0f);
    float remainder = absAngle - 360.0f * (float)turns;
    switch (m_seesawState) {
        case 0:
            if (m_count == 0) {
                int dir;
                if (angle < 0.0f) {
                    dir = 1;
                } else {
                    dir = -1;
                }
                m_seesawDir = dir;
                m_seesawSpeed = 0.5f * dir;
                if (remainder <= 0.2f) {
                    m_seesawState = 0;
                    m_seesawSpeed = 0.0f;
                }
            } else if (m_weightLeft < m_weightRight) {
                m_seesawDir = 1.0f;
                m_seesawSpeed = 0.3f * (m_weightRight - m_weightLeft);
                if (m_seesawSpeed >= 3.0f) {
                    m_seesawSpeed = 3.0f;
                }
            } else {
                m_seesawDir = -1.0f;
                m_seesawSpeed = -0.3f * (m_weightLeft - m_weightRight);
                if (m_seesawSpeed <= -3.0f) {
                    m_seesawSpeed = -3.0f;
                }
            }
            break;
        case 1:
            if (m_settleFrames == 0) {
                if (m_count == 0) {
                    int dir;
                    if (angle < 0.0f) {
                        dir = 1;
                    } else {
                        dir = -1;
                    }
                    m_seesawDir = dir;
                    m_seesawSpeed = 0.5f * dir;
                    if (remainder <= 0.2f) {
                        m_seesawState = 0;
                        m_seesawSpeed = 0.0f;
                    }
                } else {
                    m_seesawState = 0;
                }
            } else {
                m_settleFrames--;
                m_seesawSpeed = m_seesawSpeed + 0.008f * -m_seesawDir;
                if (__fabsf(m_seesawSpeed) <= 0.008f) {
                    m_seesawSpeed = 0.0f;
                }
            }
            break;
    }
    float newAngle = m_seesawAngle + m_seesawSpeed;
    m_seesawAngle = newAngle;
    Vec3f rot;
    rot.m_x = 0.0f;
    rot.m_y = 0.0f;
    rot.m_z = newAngle;
    static_cast<grGimmick*>(getGround(9))->setRot(&rot);
    static_cast<grGimmick*>(getGround(10))->setRot(&rot);
    static_cast<grGimmick*>(getGround(11))->setRot(&rot);
    m_weightRight = 0.0f;
    m_prevCount = m_count;
    m_count = 0;
    m_weightLeft = 0.0f;
}

bool stDxCruise::isEventEnd(int unk1, int* unk2, int* unk3) {
    if (m_unk1008 == 0) {
        return false;
    }
    Ground* ground = getGround(0);
    if (ground == NULL) {
        return false;
    }
    if (ground->getMotionFrame(0) < m_lastMotionFrame) {
        *unk2 = 6;
        *unk3 = 3;
        return true;
    }
    m_lastMotionFrame = ground->getMotionFrame(0);
    return false;
}
