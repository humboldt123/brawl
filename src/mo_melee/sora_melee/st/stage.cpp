#include <ai/ai_mgr.h>
#include <cm/cm_controller_ai.h>
#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <gf/gf_3d_scene.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_poke_trainer.h>
#include <memory.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_arithmetic.h>
#include <st/st_area_manager.h>
#include <st/stage.h>
#include <types.h>

// gfSceneRoot::setResAnmScn (main.dol 0x8000D2E0), still called by this name from mu_title_sunset.
extern "C" void fn_8000D2E0(gfSceneRoot* root, nw4r::g3d::ResAnmScn* anmScn, int unk);
// ecMgr::killEffectGroup (main.dol 0x80060E20, map name; parameters from the call).
extern "C" void fn_80060E20(ecMgr* mgr, int group, int unk2, int unk3, int unk4, int unk5);

// BYTES: g_grCollisionList is declared as a list of grCollisionHandle, but its nodes are the grCollision links at
// +0x30 (same view as gr_collision.cpp).
typedef nw4r::ut::LinkList<grCollision, 0x30> grCollisionNodeList;
static inline grCollisionNodeList& collisionList() {
    return reinterpret_cast<grCollisionNodeList&>(g_grCollisionList);
}

// The stage area manager singleton (stAreaManager's instance pointer, created on first use).
extern stAreaManager* lbl_27_bss_5780;

static inline stAreaManager* getAreaManager() {
    if (lbl_27_bss_5780 == NULL) {
        lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
    }
    return lbl_27_bss_5780;
}

static inline void updateGameFrame() {
    g_stGameFrame.m_frameDelta = g_GameGlobal->getGameFrameDelta();
    g_stIsJustGameFrame = g_GameGlobal->isJustGameFrame();
}

// HYPOTHESIS: the magic words of the stage parameter (STPM) and stage data (STDT) blocks are string literals
// compared as words; the original keeps them in .data like the other literals of this unit.
static inline bool isBlock(stDataContainer* container, const char* magic) {
    return container->m_filedata->m_MAGIC == *reinterpret_cast<const u32*>(magic);
}

Stage::Stage(const char* name, srStageKind stageKind)
    : gfTask(name, Category_Stage, 1, 2, true),
      m_stageKind(stageKind),
      unk4E(0),
      unk4F(0),
      unk50(0),
      unk54(0),
      m_collConv(NULL),
      m_shadowTex(NULL),
      m_stageParamContainer(NULL),
      m_stageDataContainer(NULL),
      unkAC(false),
      m_fighterDeadEffectSizeRate(1.0f),
      m_enemyDeadEffectSizeRate(0.5f),
      unkB8(0),
      unkE9(true),
      unkEA(true),
      m_isDevil(false),
      unk194(0),
      unk199(false),
      unk19A(false),
      unk19B(false),
      unk19C(false),
      unk19D(false),
      unk19E(false),
      unk19F(false) {
    g_Stage = this;
    initializeStage();
}

void Stage::initializeStage() {
    unk40 = 0;
    m_cameraParam1 = NULL;
    unk7C = false;
    unk7D = 0;
    unk4D = 0;
    unk49 = 0;
    unk4A = 0;
    unk4B = 0;
    unk4C = 0;
    unk4E = 0;
    unk4F = 0;
    unk50 = 0;
    unk54 = 0;
    m_cameraParam2 = NULL;
    unk84 = 0;
    m_headGround = NULL;
    m_stagePositions = NULL;
    m_shadow = NULL;
    m_stageParam = NULL;
    m_stageData = NULL;
    m_unk2 = false;
    m_unk3 = 0;
    m_unk4 = 0.0f;
    m_frameRuleTime = 0.0f;
    m_deadRange.m_left = -4096.0f;
    m_deadRange.m_right = 4096.0f;
    m_deadRange.m_up = 4096.0f;
    m_deadRange.m_down = -4096.0f;
    m_aiRange.m_left = -4096.0f;
    m_aiRange.m_right = 4096.0f;
    m_aiRange.m_up = 4096.0f;
    m_aiRange.m_down = -4096.0f;
    updateGameFrame();
    stShadow* shadow = stShadow::create(0x10, 0x18);
    m_shadow = shadow;
    shadow->m_connectedTask = this;
    if (m_attachedTask == NULL) {
        m_attachedTask = shadow;
    } else {
        gfTask* last = m_attachedTask;
        while (last->m_nextTask != NULL) {
            last = last->m_nextTask;
        }
        last->m_nextTask = shadow;
    }
    m_triggerMng = new (Heaps::StageInstance) stTriggerMng("stTriggerManager");
    getAreaManager()->initMng(0x10);
    m_pokeTrainerPos = NULL;
    m_pokeTrainerPosIndex = 0;
    m_pokeTrainerPosCount = 0;
    m_pokeTrainerPosType = 0;
    m_motionRatioBlend.m_start = 0.0f;
    m_motionRatioBlend.m_target = 0.0f;
    m_motionRatioBlend.m_frame = 0.0f;
    m_motionRatioBlend.m_endFrame = 0.0f;
    m_motionSubRatioBlend.m_start = 0.0f;
    m_motionSubRatioBlend.m_target = 0.0f;
    m_motionSubRatioBlend.m_frame = 0.0f;
    m_motionSubRatioBlend.m_endFrame = 0.0f;
    unkE8 = 0;
    m_devilOffset.m_x = 0.0f;
    m_devilOffset.m_y = 0.0f;
    unkF4 = 0.0f;
}

void Stage::closeStage() {
    g_gfSceneRoot->removeResAnmScn();
    fn_80060E20(g_ecMgr, 0, 0, 1, 1, 0);
    if (m_pokeTrainerPos != NULL) {
        delete[] m_pokeTrainerPos;
        m_pokeTrainerPos = NULL;
    }
    u32 numCollisions = collisionList().GetSize();
    for (u32 i = 0; i < numCollisions; i++) {
        delete &collisionList().GetFront();
    }
    if (m_collConv != NULL) {
        delete m_collConv;
        m_collConv = NULL;
    }
    for (int i = 0;; i++) {
        Ground* ground = getGround(i);
        if (ground == NULL) {
            break;
        }
        ground->preExit();
        ground->exit();
    }
    m_headGround = NULL;
    if (m_cameraParam2 != NULL) {
        delete m_cameraParam2;
        m_cameraParam2 = NULL;
    }
    if (m_stagePositions != NULL) {
        delete m_stagePositions;
        m_stagePositions = NULL;
    }
    if (m_stageParamContainer != NULL) {
        m_stageParam = NULL;
        delete m_stageParamContainer;
        m_stageParamContainer = NULL;
    }
    if (m_stageDataContainer != NULL) {
        m_stageData = NULL;
        delete m_stageDataContainer;
        m_stageDataContainer = NULL;
    }
    g_cmAIController->unk94_20 = false;
    g_aiMgr->clearDangerZone(0);
    g_efScreen->m_notify = NULL;
}

void Stage::processBegin() {
    updateGameFrame();
    if (m_shadow) {
        m_shadow->clearShadowObject();
    }
    switch (unk40) {
        case 0:
            unk40 = 1;
        case 1:
            if (loading()) {
                createObj();
                unk40 = 2;
            }
            break;
        case 3:
            unk40 = 4;
            break;
        case 4:
            process();
            break;
    }
}

static inline float clampRatio(float x, float low, float high) {
    x = nw4r::math::FSelect(x - low, x, low);
    return nw4r::math::FSelect(x - high, high, x);
}

// HYPOTHESIS: cleared at the start of every stage animation pass (main.dol .sbss, no other users known).
extern u32 lbl_805A0368;

void Stage::processAnim() {
    lbl_805A0368 = 0;
    // HYPOTHESIS: the stage's motion ratios blend towards their targets, see MotionRatioBlend; the clamp is
    // an FSelect clamp.
    if (m_motionRatioBlend.m_endFrame != 0.0f) {
        m_motionRatioBlend.m_frame += g_GameGlobal->getGameFrameDelta();
        float t = clampRatio(m_motionRatioBlend.m_frame / m_motionRatioBlend.m_endFrame, 0.0f, 1.0f);
        g_GameGlobal->m_stageData->m_motionRatio = m_motionRatioBlend.m_start + t * (m_motionRatioBlend.m_target - m_motionRatioBlend.m_start);
        if (1.0f == t) {
            m_motionRatioBlend.m_start = 0.0f;
            m_motionRatioBlend.m_target = 0.0f;
            m_motionRatioBlend.m_frame = 0.0f;
            m_motionRatioBlend.m_endFrame = 0.0f;
        }
    }
    if (m_motionSubRatioBlend.m_endFrame != 0.0f) {
        m_motionSubRatioBlend.m_frame += g_GameGlobal->getGameFrameDelta();
        float t = clampRatio(m_motionSubRatioBlend.m_frame / m_motionSubRatioBlend.m_endFrame, 0.0f, 1.0f);
        g_GameGlobal->m_stageData->m_motionSubRatio = m_motionSubRatioBlend.m_start + t * (m_motionSubRatioBlend.m_target - m_motionSubRatioBlend.m_start);
        if (1.0f == t) {
            m_motionSubRatioBlend.m_start = 0.0f;
            m_motionSubRatioBlend.m_target = 0.0f;
            m_motionSubRatioBlend.m_frame = 0.0f;
            m_motionSubRatioBlend.m_endFrame = 0.0f;
        }
    }
    if (unkE8 == 1) {
        updateStagePositions();
    }
    if (m_stagePositions != NULL) {
        // HYPOTHESIS: the copy swaps the two depth limits (+0x18/+0x1C) on the way into the camera.
        cmStageParam param;
        param.m_range = m_cameraParam1->m_range;
        param.m_verticalRotationFactor = m_cameraParam1->m_verticalRotationFactor;
        param.m_horizontalRotationFactor = m_cameraParam1->m_horizontalRotationFactor;
        param.m_maxZ = m_cameraParam1->m_minZ;
        param.m_minZ = m_cameraParam1->m_maxZ;
        param.m_centerPos = m_cameraParam1->m_centerPos;
        param.m_fighterBubbleBufferMultiplier = m_cameraParam1->m_fighterBubbleBufferMultiplier;
        param.m_48 = m_cameraParam1->m_48;
        param.m_speed = m_cameraParam1->m_speed;
        param.m_tiltThreshold = m_cameraParam1->m_tiltThreshold;
        param.m_fov = m_cameraParam1->m_fov;
        CameraController::getInstance()->setCameraRange(&m_cameraParam1->m_range);
        CameraController::getInstance()->setStageParam(&param);
    }
}

void Stage::processUpdate() { }

void Stage::processMapCorrection() { }

void Stage::processFixCamera() {
    if (m_stagePositions != NULL) {
        m_stagePositions->getDeadRange(&m_deadRange);
        CameraController::getInstance()->setDeadRange(&m_deadRange);
    }
}

void Stage::processEnd() {
    if (unk40 == 4) {
        updateGameFrame();
    }
}

void Stage::renderPre() {
    if (m_shadow) {
        m_shadow->clearShadowObject();
    }
}

bool Stage::loading() {
    return true;
}

void Stage::startLoadLocalData() { }

bool Stage::isLoadLocalData() {
    return true;
}

int Stage::entryLocalData() {
    return 1;
}

void Stage::removeLocalData() { }

void Stage::createObj() { }

void Stage::initPosPokeTrainer(int count, int type) {
    if (count != 0) {
        switch (type) {
            case 0:
                m_pokeTrainerPos = new (Heaps::StageInstance) Vec3f[(u8)count * 2];
                memset(m_pokeTrainerPos, 0, count * sizeof(Vec3f) * 2);
                break;
            case 1:
                m_pokeTrainerPos = new (Heaps::StageInstance) Vec3f[count];
                memset(m_pokeTrainerPos, 0, count * sizeof(Vec3f));
                break;
            default:
                return;
        }
        m_pokeTrainerPosCount = count;
        m_pokeTrainerPosType = type;
    }
}

void Stage::createObjPokeTrainer(gfArchive* filedata, int fileIndex, const char* name, Vec3f* pokeTrainerPos, Vec3f* unk2) {
    if (pokeTrainerPos != NULL) {
        grPokeTrainer* ground = grPokeTrainer::create(fileIndex, name, "grPokeTrainer");
        if (ground != NULL) {
            addGround(ground);
            ground->startup(filedata, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
            ground->setPosWork(unk2);
            ground->setPosDstWork(pokeTrainerPos);
        }
    }
}

void Stage::process() { }

void Stage::addGround(Ground* ground) {
    if (m_headGround == NULL) {
        m_headGround = ground;
    } else {
        Ground* last = m_headGround;
        while (last->m_nextGround != NULL) {
            last = last->m_nextGround;
        }
        last->m_nextGround = ground;
    }
}

int Stage::getZoneLightSetIndex() {
    return 0x14;
}

void Stage::removeGround(Ground* ground) {
    Ground* cur = m_headGround;
    Ground* prev = NULL;
    while (cur != NULL) {
        if (cur == ground) {
            break;
        }
        prev = cur;
        cur = cur->m_nextGround;
    }
    if (cur != NULL) {
        Ground* next = cur->m_nextGround;
        if (prev == NULL) {
            m_headGround = next;
        } else {
            prev->m_nextGround = next;
        }
    }
}

u32 Stage::getGroundNum() {
    u32 num = 0;
    for (Ground* ground = m_headGround; ground; ground = ground->m_nextGround) {
        num++;
    }
    return num;
}

Ground* Stage::getGround(int index) {
    Ground* ground = m_headGround;
    while (index != 0 && ground) {
        ground = ground->m_nextGround;
        index--;
    }
    return ground;
}

void Stage::setCollision(grCollision* coll) { }

grCollision* Stage::getCollision(int index) {
    grCollisionNodeList::Iterator it = collisionList().GetBeginIter();
    for (int i = 0; i < index; i++) {
        it++;
    }
    return &*it;
}

u32 Stage::getCollisionNum() {
    return collisionList().GetSize();
}

bool Stage::registScnAnim(nw4r::g3d::ResFileData* data, u32 index) {
    bool result = false;
    nw4r::g3d::ResFile file(data);
    if (data != NULL && file.HasResAnmScn()) {
        nw4r::g3d::ResAnmScn scn = file.GetResAnmScn(index);
        scn.Bind(scn);
        fn_8000D2E0(g_gfSceneRoot, &scn, 0xF);
        result = true;
    }
    return result;
}

bool Stage::registShadowTexture(void* data, u32 index) {
    bool result = false;
    nw4r::g3d::ResFile file(data);
    nw4r::g3d::ResFile::Init(&file);
    if (file.ptr() != NULL && file.HasResTex()) {
        m_shadowTex = file.GetResTex(index);
        result = true;
    }
    return result;
}

void Stage::clearShadow() {
    if (m_shadow) {
        m_shadow->clearShadowObject();
    }
}

void Stage::renderDebug() { }

void Stage::renderDebugPositions() { }

grCollision* Stage::createCollision(gfArchive* archive, int index, Ground* ground) {
    grCollision* collision;
    if (archive == NULL) {
        return NULL;
    }
    grCollData* collData = new (Heaps::StageInstance) grCollData;
    if (collData == NULL) {
        return NULL;
    }
    testCollDataInit(archive, collData, index);
    collision = grCollision::create(this, collData, ground, 1);
    delete collData;
    return collision;
}

void Stage::clearCameraParam() {
    if (m_cameraParam2 != NULL) {
        delete m_cameraParam2;
        m_cameraParam2 = NULL;
    }
}

// The stage's debug report (compiled out in this build: it only spills its arguments; name from the map).
static void srReportLessMastar(const char* format, ...) { }

void Stage::testCollDataInit(gfArchive* archive, grCollData* collData, u32 index) {
    grCollData* data = static_cast<grCollData*>(archive->getData(Data_Type_Misc, index, 0xFFFE));
    memset(collData, 0, sizeof(grCollData));
    if (data != NULL) {
        collData->m_vtxLen = data->m_vtxLen;
        collData->m_lineLen = data->m_lineLen;
        collData->m_jointLen = data->m_jointLen;
        collData->m_unk1 = data->m_unk1;
        // The file stores the three tables as offsets from its start.
        collData->m_vtxDatas = reinterpret_cast<grCollData::VtxData*>(reinterpret_cast<u32>(data->m_vtxDatas) + reinterpret_cast<u32>(data));
        collData->m_lineDatas = reinterpret_cast<grCollData::LineData*>(reinterpret_cast<u32>(data->m_lineDatas) + reinterpret_cast<u32>(data));
        collData->m_jointDatas = reinterpret_cast<grCollData::JointData*>(reinterpret_cast<u32>(data->m_jointDatas) + reinterpret_cast<u32>(data));
    } else {
        srReportLessMastar("\x1B[31m Collision binary is not found! \x1B[0m \n");
        if (m_collConv == NULL) {
            m_collConv = grCollConv::create();
            m_collConv->process(NULL);
        }
        collData->m_vtxLen = m_collConv->getVtxLen();
        collData->m_lineLen = m_collConv->getLineLen();
        collData->m_jointLen = m_collConv->getJointLen();
        collData->m_unk1 = m_collConv->m_unk6;
        collData->m_vtxDatas = reinterpret_cast<grCollData::VtxData*>(m_collConv->getVtxData());
        collData->m_lineDatas = reinterpret_cast<grCollData::LineData*>(m_collConv->getLineData());
        collData->m_jointDatas = reinterpret_cast<grCollData::JointData*>(m_collConv->getJointData());
    }
}

bool Stage::stageParamChange(u32 index) {
    stDataMultiContainer* container = m_stageParamContainer;
    if (container != NULL && isBlock(container, "STPM")) {
        stParam* param = reinterpret_cast<stParam*>(container->getData(index));
        if (param != NULL) {
            m_stageParam = param;
            return true;
        }
    }
    return false;
}

void Stage::testStageDataInit(gfArchive* archive, int fileIndex, int unk2) {
    stDataContainerData* data = static_cast<stDataContainerData*>(archive->getData(Data_Type_Misc, fileIndex, 0xFFFE));
    if (data != NULL) {
        m_stageDataContainer = stDataMultiContainer::create(data, Heaps::StageInstance);
        if (isBlock(m_stageDataContainer, "STDT")) {
            m_stageData = m_stageDataContainer->getData(0);
        }
    }
}

void Stage::getFighterStartPos(Vec3f* startPos, int fighterIndex) {
    startPos->m_z = 0.0f;
    startPos->m_y = 0.0f;
    startPos->m_x = 0.0f;
    if ((u32)fighterIndex < 4 && m_stagePositions != NULL) {
        m_stagePositions->getPlayerStartPos(fighterIndex, startPos);
    }
}

void Stage::getFighterReStartPos(Vec3f* startPos, int fighterIndex) {
    startPos->m_z = 0.0f;
    startPos->m_y = 0.0f;
    startPos->m_x = 0.0f;
    if ((u32)fighterIndex < 4 && m_stagePositions != NULL) {
        m_stagePositions->getPlayerReStartPos(fighterIndex, startPos);
    }
}

bool Stage::isReStartSamePoint() {
    return false;
}

void Stage::getPokeTrainerStartPos(Vec3f* pos, u32 index) {
    pos->m_z = 0.0f;
    pos->m_y = 0.0f;
    pos->m_x = 0.0f;
    if (index < 4 && m_stagePositions != NULL) {
        m_stagePositions->getTrainerStartPos(index, pos);
    }
}

u8 Stage::getItemPosCount() {
    if (m_stagePositions == NULL) {
        return 0;
    }
    return m_stagePositions->m_itemPosCount;
}

void Stage::getItemPos(Vec3f* pos, Vec3f* safePos, u32 index) {
    pos->m_z = 0.0f;
    pos->m_y = 0.0f;
    pos->m_x = 0.0f;
    safePos->m_z = 0.0f;
    safePos->m_y = 0.0f;
    safePos->m_x = 0.0f;
    if (m_stagePositions != NULL) {
        m_stagePositions->getItemPos(index, pos, safePos);
    }
}

void Stage::getRandItemPos(Vec3f* pos) {
    pos->m_z = 0.0f;
    pos->m_y = 0.0f;
    pos->m_x = 0.0f;
    if (m_stagePositions != NULL) {
        m_stagePositions->getRandItemPos(pos);
    }
}

void Stage::getKirifudaPos(Vec3f* pos, u32 index) {
    pos->m_z = 0.0f;
    pos->m_y = 0.0f;
    pos->m_x = 0.0f;
    switch (index) {
        case 5:
            if (m_stageParam != NULL) {
                pos->m_x = m_stageParam->m_iceClimbersFinalPos.m_x;
                pos->m_y = m_stageParam->m_iceClimbersFinalPos.m_y;
                pos->m_z = m_stageParam->m_iceClimbersFinalPos.m_z;
            }
            break;
        default:
            if (m_stagePositions != NULL) {
                m_stagePositions->getKirifudaPos(index, pos);
            }
            break;
    }
}

float Stage::getKirifudaAngle(u32 index) {
    if (m_stageParam == NULL) {
        return 30.0f;
    }
    if (index >= 7) {
        return 30.0f;
    }
    switch (index) {
        case 2:
            return m_stageParam->m_olimarFinalCamAngle;
        default:
            return 30.0f;
    }
}

void Stage::getKirifudaScale(Vec3f* scale, u32 index) {
    scale->m_x = 1.0f;
    scale->m_y = 1.0f;
    scale->m_z = 1.0f;
    if (m_stageParam != NULL && index < 7) {
        switch (index) {
            case 5:
                scale->m_x = m_stageParam->m_iceClimbersFinalScale.m_x;
                scale->m_y = m_stageParam->m_iceClimbersFinalScale.m_y;
                break;
            case 6:
                scale->m_x = m_stageParam->m_pitFinalPalutenaScale.m_x;
                scale->m_y = m_stageParam->m_pitFinalPalutenaScale.m_y;
                break;
        }
    }
}

u8 Stage::getKirifudaModelType(u32 index) {
    if (m_stageParam == NULL) {
        return 0;
    }
    if (index >= 7) {
        return 0;
    }
    switch (index) {
        case 6:
            return m_stageParam->m_kirifudaModelType;
        default:
            return 0;
    }
}

u8 Stage::getPokeTrainerPosCount() {
    return m_pokeTrainerPosCount;
}

void Stage::getPokeTrainerPos(Vec3f* pos, Vec3f* dstPos, u32 index) {
    pos->m_z = 0.0f;
    pos->m_y = 0.0f;
    pos->m_x = 0.0f;
    dstPos->m_z = 0.0f;
    dstPos->m_y = 0.0f;
    dstPos->m_x = 0.0f;
    switch (m_pokeTrainerPosType) {
        case 0: {
            u8 i = index;
            *pos = m_pokeTrainerPos[i * 2];
            *dstPos = m_pokeTrainerPos[i * 2 + 1];
            break;
        }
        case 1:
            *pos = m_pokeTrainerPos[index];
            *dstPos = m_pokeTrainerPos[index];
            break;
    }
}

float Stage::getFighterDeadEffectSizeRate() {
    return m_fighterDeadEffectSizeRate;
}

float Stage::getEnemyDeadEffectSizeRate() {
    return m_enemyDeadEffectSizeRate;
}

float Stage::getEnableZ() {
    if (m_stageParam == NULL) {
        return 0.0f;
    }
    return m_stageParam->m_enableZ;
}

void Stage::loadStageAttrParam(gfArchive* filedata, int fileIndex) {
    stDataContainerData* data = static_cast<stDataContainerData*>(filedata->getData(Data_Type_Misc, fileIndex, 0xFFFE));
    if (data != NULL) {
        m_collisionAttrParam.setCollisionAttrParamSet(data);
    }
}

void Stage::setStageAttackData(grGimmickDamageFloor* attackData, u32 index) {
    m_collisionAttrParam.setCollisionAttackData(attackData);
}

void Stage::createStagePositions(nw4r::g3d::ResFile* resFile) {
    m_stagePositions = stPositions::create(*resFile);
    updateStagePositions();
}

void Stage::createStagePositions() {
    m_stagePositions = stPositions::create();
    updateStagePositions();
}

void Stage::updateStagePositions() {
    if (m_stagePositions != NULL) {
        m_stagePositions->update();
        m_stagePositions->getCameraRange(&m_cameraParam1->m_range);
        m_stagePositions->getCameraCenter(&m_cameraParam1->m_centerPos);
        if (m_stageParam != NULL) {
            m_cameraParam1->m_range.m_right *= m_stageParam->m_camRight;
            m_cameraParam1->m_range.m_left *= m_stageParam->m_camLeft;
            m_cameraParam1->m_range.m_up *= m_stageParam->m_starKOCamTilt;
            m_cameraParam1->m_range.m_down *= m_stageParam->m_finalSmashCamTilt;
            g_gfSceneRoot->m_shadowPitch = m_stageParam->m_shadowPitch;
            g_gfSceneRoot->m_shadowYaw = m_stageParam->m_shadowYaw;
            g_gfSceneRoot->m_1156 = m_stageParam->m_shadowRoll;
        }
    }
}

void Stage::enableDevil() {
    m_isDevil = true;
    g_cmAIController->unk94_80 = true;
}

void Stage::disableDevil() {
    m_isDevil = false;
    g_cmAIController->unk94_80 = false;
}

bool Stage::isDevil() {
    return m_isDevil;
}

void Stage::setDevilScrool(float x, float y) {
    if (x == 0.0f && y == 0.0f) {
        m_devilOffset.m_x = 0.0f;
        m_devilOffset.m_y = 0.0f;
        g_cmAIController->resetDevilOffset();
    } else {
        m_devilOffset.m_x += x;
        m_devilOffset.m_y += y;
        Vec2f offset(x, y);
        g_cmAIController->accDevilOffset(&offset);
    }
}

void Stage::getLucarioFinalTechniquePosition(Vec3f* pos) {
    pos->m_x = 0.0f;
    pos->m_y = 100.0f;
    pos->m_z = 0.0f;
}

bool Stage::isEnd() {
    return false;
}

bool Stage::isEventEnd(int param1, int* eventState, int* eventDecision) {
    return false;
}

GXColor Stage::getFinalTechniqColor() {
    GXColor color = { 0x14, 0x00, 0x04, 0xB8 };
    return color;
}

void Stage::setMotionRatio(float ratio, float frames) {
    if (frames == 0.0f) {
        g_GameGlobal->m_stageData->m_motionRatio = ratio;
        m_motionRatioBlend.m_start = 0.0f;
        m_motionRatioBlend.m_target = 0.0f;
        m_motionRatioBlend.m_frame = 0.0f;
        m_motionRatioBlend.m_endFrame = 0.0f;
    } else {
        m_motionRatioBlend.m_start = g_GameGlobal->m_stageData->m_motionRatio;
        m_motionRatioBlend.m_target = ratio;
        m_motionRatioBlend.m_frame = 0.0f;
        m_motionRatioBlend.m_endFrame = frames;
    }
}

void Stage::saveMotionRatio(MotionRatioBlend* save) {
    *save = m_motionRatioBlend;
}

void Stage::restoreMotionRatio(MotionRatioBlend* save) {
    m_motionRatioBlend = *save;
}

void Stage::setMotionSubRatio(float ratio, float frames) {
    if (frames == 0.0f) {
        g_GameGlobal->m_stageData->m_motionSubRatio = ratio;
        m_motionSubRatioBlend.m_start = 0.0f;
        m_motionSubRatioBlend.m_target = 0.0f;
        m_motionSubRatioBlend.m_frame = 0.0f;
        m_motionSubRatioBlend.m_endFrame = 0.0f;
    } else {
        m_motionSubRatioBlend.m_start = g_GameGlobal->m_stageData->m_motionSubRatio;
        m_motionSubRatioBlend.m_target = ratio;
        m_motionSubRatioBlend.m_frame = 0.0f;
        m_motionSubRatioBlend.m_endFrame = frames;
    }
}

void Stage::saveMotionSubRatio(MotionRatioBlend* save) {
    *save = m_motionSubRatioBlend;
}

void Stage::restoreMotionSubRatio(MotionRatioBlend* save) {
    m_motionSubRatioBlend = *save;
}

void Stage::notifyEventInfoReady() { }

void Stage::notifyEventInfoGo() { }

// HYPOTHESIS: state 3 is an empty player slot.
static inline bool isPokemonTrainerSlot(gmPlayerInitData& player) {
    if (player.m_state != 3) {
        if (player.m_characterKind == Character_PokeLizardon_Trainer) {
            return true;
        }
        if (player.m_characterKind == Character_PokeZenigame_Trainer) {
            return true;
        }
        if (player.m_characterKind == Character_PokeFushigisou_Trainer) {
            return true;
        }
    }
    return false;
}

bool Stage::isPokemonTrainer() {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    for (int i = 0; i < 4; i++) {
        if (isPokemonTrainerSlot(melee->m_playersInitData[i])) {
            return true;
        }
    }
    return false;
}

void Stage::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) { }

void Stage::getItemGenPac(gfArchive** archive) { }

void Stage::getItemPacEnemyFigure(gfArchive** archive) { }

void Stage::getEnemyPac(gfArchive** brres, gfArchive** param, gfArchive** enmCommon, gfArchive** primFaceBrres, EnemyKind enemyKind) { }

void Stage::getAdvRadarData(gfArchive**, gfArchive**) { }

stDestroyBossParamCommon Stage::getDestroyBossParamCommon(u32) {
    stDestroyBossParamCommon param = { 60, 60, 5.0f, 180, 240, 120 };
    return param;
}

void Stage::stAdventureEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) { }

bool Stage::startAppear() {
    return false;
}

bool Stage::isAppear() {
    return false;
}

bool Stage::isStartAppearTimming() {
    return true;
}

void Stage::endAppear() { }

void Stage::setAppearKind(u8 kind) { }

void* Stage::getMadeinAiData() {
    return NULL;
}

bool Stage::isBamperVector() {
    return false;
}

void Stage::getBamperVector(Vec3f* vec) {
    if (vec) {
        vec->m_x = 0.0f;
        vec->m_y = 0.0f;
        vec->m_z = 0.0f;
    }
}

void Stage::setBlendColorEnable() {
    u32 i = 0;
    u32 num = getGroundNum();
    for (; i != num; i++) {
        Ground* ground = getGround(i);
        if (ground != NULL) {
            ground->setBlendColorEnable();
        }
    }
}

void Stage::setBlendColorDisable() {
    u32 i = 0;
    u32 num = getGroundNum();
    for (; i != num; i++) {
        Ground* ground = getGround(i);
        if (ground != NULL) {
            ground->setBlendColorDisable();
        }
    }
}

void Stage::setBlendColor(GXColor color) {
    u32 i = 0;
    u32 num = getGroundNum();
    for (; i != num; i++) {
        Ground* ground = getGround(i);
        if (ground != NULL) {
            ground->setBlendColor(color);
        }
    }
}

int Stage::getZoneState() {
    return 0;
}

void Stage::getZonePos(Vec3f* pos) { }

float Stage::getMagmaHeight() {
    return 0.0f;
}

float Stage::getAcidHeight() {
    return 0.0f;
}

u8 Stage::getIteamDropStatus() {
    return 1;
}

void Stage::setStageOutEffectInit() {
    unkE9 = false;
    unkEA = false;
}

void Stage::setStageInEffectInit() {
    unkE9 = true;
    unkEA = true;
}

void Stage::setVision(u8) { }

IfSmashAppearTask* Stage::getAppearTask() {
    return NULL;
}

void Stage::forceStopAppear() { }

void Stage::updateWind2ndOnly() { }

grGimmickWindData2nd* Stage::getWind2ndOnlyData() {
    return NULL;
}

bool Stage::createWind2ndOnly() {
    return false;
}
