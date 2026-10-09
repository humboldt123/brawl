#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <gm/gm_stage_data.h>
#include <gm/gm_global_mode_melee.h>
#include <ft/ft_manager.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <revolution/GX.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_mariopast/gr_mariopast.h>
#include <st_mariopast/st_mariopast.h>

// HYPOTHESIS: tear down and validity check of a stCollisionWork (unnamed functions of sora_melee).
extern "C" void fn_27_239F6C(stCollisionWork* work);
extern "C" int fn_27_239FD8(stCollisionWork* work);

// MATCH-ONLY: the data pointers of stCollisionWork are private in the header.
struct stCollisionWorkView {
    grCollData::VtxData* m_vtxDatas;
    grCollData::LineData* m_lineDatas;
    grCollData::JointData* m_jointDatas;
};


// MATCH-ONLY: views of the node data of a model; the header names neither the name offset (+8), the id (+0xC) nor the offset
// to the parent node (+0x5C).
static inline const char* mpNodeName(nw4r::g3d::ResNode& node) {
    u8* data = reinterpret_cast<u8*>(node.ptr());
    s32 offset = *reinterpret_cast<s32*>(data + 8);
    return offset != 0 ? reinterpret_cast<const char*>(data + offset) : NULL;
}
static inline int mpNodeId(u8* data) {
    return *reinterpret_cast<int*>(data + 0xC);
}
static inline u8* mpNodeParent(nw4r::g3d::ResNode& node) {
    u8* data = reinterpret_cast<u8*>(node.ptr());
    s32 offset = *reinterpret_cast<s32*>(data + 0x5C);
    return offset != 0 ? data + offset : NULL;
}

stClassInfoImpl<Stages::MarioPast, stMarioPast> stMarioPast::bss_loc_14;

// MATCH-ONLY: the bytes of the match settings the stage reads (their names are unknown): byte 0 holds the game mode in its
// upper six bits, byte 6 the setting that picks the speed of the lifts in its upper three bits and byte 8 the event.
static inline u8* mpMeleeBytes() {
    return reinterpret_cast<u8*>(&g_GameGlobal->m_modeMelee->m_meleeInitData);
}

// MATCH-ONLY: the first word of the AI camera controller after its base holds flags of which only the top byte is used here.
static inline u32* mpCameraFlags() {
    return reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_cmAIController) + 8);
}

stMarioPast* stMarioPast::create() {
    return new (Heaps::StageInstance) stMarioPast;
}

stMarioPast::stMarioPast() : stMelee("stMarioPast", Stages::MarioPast) {
    m_frame = 0.0f;
    memset(m_limit, 0, sizeof(m_limit));
    memset(&m_unk204, 0, sizeof(m_unk204));
    memset(m_posGimmick, 0, sizeof(m_posGimmick));
    memset(&m_posPrev, 0, sizeof(m_posPrev));
    m_stage = 0;
    memset(&m_blockWork, 0, sizeof(m_blockWork));
    for (int i = 0; i < 2; i++) {
        m_collWork[i].initialize();
        m_collWork[i].m_vtxLen = 4;
        m_collWork[i].m_isClosed = true;
    }
    memset(m_liftTimer, 0, sizeof(m_liftTimer));
    memset(m_liftId, 0, sizeof(m_liftId));
    m_archive = NULL;
    m_trainerState = 0;
    m_trainerAway[0] = 0;
    m_trainerAway[1] = 0;
    m_trainerAway[2] = 0;
    m_trainerAway[3] = 0;
    m_event = 0;
    m_liftSet = 0;
    m_liftSpeed = 0.0f;
    m_eventPrev = 0.0f;
    if (g_cmAIController != NULL) {
        *mpCameraFlags() = (*mpCameraFlags() & 0xFFFFFF) | 0x1000000;
    }
}

stMarioPast::~stMarioPast() {
    if (g_cmAIController != NULL) {
        *mpCameraFlags() = *mpCameraFlags() & 0xFFFFFF;
    }
    for (int i = 0; i < 2; i++) {
        fn_27_239F6C(&m_collWork[i]);
    }
    if (m_blockWork.m_data != NULL) {
        delete[] m_blockWork.m_data;
        m_blockWork.m_data = NULL;
    }
    memset(&m_blockWork, 0, sizeof(m_blockWork));
    if (m_archive != NULL) {
        m_archive->~gfArchive();
    }
    releaseArchive();
}

bool stMarioPast::loading() {
    return true;
}

void stMarioPast::createObj() {
    testStageDataInit(m_fileData, 0x14, 0x28);
    if (g_GameGlobal->m_modeMelee != NULL) {
        switch (mpMeleeBytes()[0x14]) {
        case 1:
            m_stage = 1;
            break;
        case 0:
            m_stage = 0;
            break;
        default:
            m_stage = 0;
            break;
        }
        testStageParamInit(m_fileData, 10);
        createObjBgLocator(0);
        createObjBg(1);
        createCollision(m_fileData, 2, NULL);
        createObjLift();
        createObjBlockPos();
        initCameraParam();

        void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
        if (posData) {
            nw4r::g3d::ResFile posFile(posData);
            createStagePositions(&posFile);
        } else {
            createStagePositions();
        }
        createWind2ndOnly();
        initPosPokeTrainer(4, 0);
        if (m_stage == 1) {
            createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, &m_posGimmick[0]);
            createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, &m_posGimmick[2]);
            createObjPokeTrainer(m_fileData, 0x67, "PokeTrainer02", m_pokeTrainerPos + 4, &m_posGimmick[2]);
            createObjPokeTrainer(m_fileData, 0x68, "PokeTrainer03", m_pokeTrainerPos + 6, &m_posGimmick[0]);
        } else if (m_stage == 0) {
            createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, &m_posGimmick[0]);
            createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, &m_posGimmick[0]);
            createObjPokeTrainer(m_fileData, 0x67, "PokeTrainer02", m_pokeTrainerPos + 4, &m_posGimmick[2]);
            createObjPokeTrainer(m_fileData, 0x68, "PokeTrainer03", m_pokeTrainerPos + 6, &m_posGimmick[2]);
        }
        loadStageAttrParam(m_fileData, 0x1E);
        registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
        if (g_GameGlobal->m_modeMelee != NULL && (mpMeleeBytes()[0] >> 2) == 7 && mpMeleeBytes()[8] == 8) {
            m_event = 1;
        }
    }
}

void stMarioPast::createObjBgLocator(int index) {
    grMarioPastBgLocator* ground = NULL;
    if (m_stage == 1) {
        if (index == 0) {
            ground = grMarioPastBgLocator::create(2, "StgMarioPast01_Ashiba_locator", "grMarioPastBgLocator");
        }
    } else if (m_stage == 0) {
        if (index == 0) {
            ground = grMarioPastBgLocator::create(2, "StgMarioPast00_Ashiba_locator", "grMarioPastBgLocator");
        }
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setStage(m_stage);
        ground->setFrameLocatorWork(&m_frame);
        ground->setPosGimmickWork(m_posGimmick);
    }
}

void stMarioPast::createObjBg(int index) {
    grMarioPastBg* ground = NULL;
    if (m_stage == 1) {
        if (index == 1) {
            ground = grMarioPastBg::create(0, "StgMarioPast01", "grMarioPastMainBg");
        }
    } else if (m_stage == 0) {
        if (index == 1) {
            ground = grMarioPastBg::create(0, "StgMarioPast00", "grMarioPastMainBg");
        }
    }
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setFrameLocatorWork(&m_frame);
        ground->setPosGimmickWork(m_posGimmick);
        ground->setStage(m_stage);
    }
}

void stMarioPast::createObjLift() {
    if (m_stage == 1) {
        for (u8 i = 0; i < 3; i++) {
            grMarioPastLift* ground = grMarioPastLift::create(4, "StgMarioPast01Lift", "grMarioPastLiftLeft");
            if (ground == NULL) {
                return;
            }
            addGround(ground);
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
            ground->setPosWork(&m_posGimmick[7]);
            ground->setPosLimitWork(m_limit);
            ground->setCtrlIDWork(&m_liftId[0]);
            ground->setID(i);
            createCollision(m_fileData, 5, ground);
        }
        for (u8 i = 0; i < 3; i++) {
            grMarioPastLift* ground = grMarioPastLift::create(4, "StgMarioPast01Lift", "grMarioPastLiftRight");
            if (ground == NULL) {
                return;
            }
            addGround(ground);
            ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            ground->setStageData(m_stageData);
            ground->setPosWork(&m_posGimmick[9]);
            ground->setPosLimitWork(m_limit);
            ground->setCtrlIDWork(&m_liftId[1]);
            ground->setID(i);
            createCollision(m_fileData, 5, ground);
        }
    }
}

void stMarioPast::update(float deltaFrame) {
    if (m_stageData != NULL) {
        if (m_posGimmick[11].m_x < m_posPrev.m_x) {
            m_scroll.m_x = m_posGimmick[11].m_x - m_posPrev.m_x;
            m_scroll.m_y = m_posGimmick[11].m_y - m_posPrev.m_y;
            m_scroll.m_z = m_posGimmick[11].m_z - m_posPrev.m_z;
        }
        m_posPrev.m_x = m_posGimmick[11].m_x;
        m_posPrev.m_y = m_posGimmick[11].m_y;
        m_posPrev.m_z = m_posGimmick[11].m_z;
        updateLimit(deltaFrame);
        updateLift(deltaFrame);
        updateTrainer(deltaFrame);
    }
}

void stMarioPast::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    m_limit[0].m_x = camera->unk158;
    m_limit[0].m_y = camera->unk160;
    m_limit[0].m_z = 0.0f;
    m_limit[1].m_x = camera->unk15C;
    m_limit[1].m_y = camera->unk164;
    m_limit[1].m_z = 0.0f;
}

// The lifts of the second stage run at a speed the stage data gives for the rate of the match (HYPOTHESIS: the field of the
// match mode is the rate); it also tells the match how fast the stage scrolls.
void stMarioPast::updateLift(float deltaFrame) {
    if (m_event == 1) {
        if (m_liftSet == 0) {
            float* data = static_cast<float*>(m_stageData);
            if (data == NULL) {
                return;
            }
            if (g_GameGlobal->m_modeMelee == NULL) {
                return;
            }
            if ((mpMeleeBytes()[0] >> 2) != 7) {
                return;
            }
            switch (mpMeleeBytes()[6] >> 5) {
            case 0:
                m_liftSpeed = data[7];
                break;
            case 1:
                m_liftSpeed = data[8];
                break;
            case 2:
                m_liftSpeed = data[9];
                break;
            }
            m_liftSet = 1;
        }
        if (m_liftSet == 1) {
            g_GameGlobal->m_stageData->m_motionSubRatio = m_liftSpeed;
        }
    }
    updateLift1(deltaFrame, 0);
    updateLift1(deltaFrame, 1);
}

void stMarioPast::updateLift1(float deltaFrame, u8 index) {
    m_liftTimer[index] -= deltaFrame;
    if (m_liftTimer[index] < 0.0f) {
        m_liftTimer[index] = 0.0f;
    }
    if (m_liftTimer[index] != 0.0f) {
        return;
    }
    float* data = static_cast<float*>(m_stageData);
    if (data == NULL) {
        return;
    }
    m_liftId[index]++;
    if (m_liftId[index] == 3) {
        m_liftId[index] = 0;
    }
    m_liftTimer[index] = data[5];
}

// Keeps track of the Pokemon Trainer: he may only be replaced when his position is out of the camera area to the left.
void stMarioPast::updateTrainer(float deltaFrame) {
    if (m_trainerState == 1) {
        float limit = m_limit[1].m_x + 10.0f;
        if (m_pokeTrainerPos[m_pokeTrainerPosIndex * 2].m_x < limit || m_pokeTrainerPos[m_pokeTrainerPosIndex * 2 + 1].m_x < limit) {
            g_ftManager->notifyReplacePokeTrainer(m_pokeTrainerPosIndex);
            m_trainerState = 0;
        }
    } else if (m_trainerState == 0) {
        u32 index = m_pokeTrainerPosIndex;
        if (m_trainerAway[index] == 0) {
            float limit = m_limit[0].m_x - 10.0f;
            if (m_pokeTrainerPos[index * 2].m_x < limit && m_pokeTrainerPos[index * 2 + 1].m_x < limit) {
                m_trainerAway[index] = 1;
                m_pokeTrainerPosIndex++;
                if (m_pokeTrainerPosIndex > 3) {
                    m_pokeTrainerPosIndex = 0;
                }
                m_trainerState++;
            }
        }
    }
    if (m_trainerAway[0] == 1) {
        if (m_limit[1].m_x < m_pokeTrainerPos[0].m_x && m_limit[1].m_x < m_pokeTrainerPos[1].m_x) {
            m_trainerAway[0] = 0;
        }
    }
    if (m_trainerAway[1] == 1) {
        if (m_limit[1].m_x < m_pokeTrainerPos[2].m_x && m_limit[1].m_x < m_pokeTrainerPos[3].m_x) {
            m_trainerAway[1] = 0;
        }
    }
    if (m_trainerAway[2] == 1) {
        if (m_limit[1].m_x < m_pokeTrainerPos[4].m_x && m_limit[1].m_x < m_pokeTrainerPos[5].m_x) {
            m_trainerAway[2] = 0;
        }
    }
    if (m_trainerAway[3] == 1) {
        if (m_limit[1].m_x < m_pokeTrainerPos[6].m_x && m_limit[1].m_x < m_pokeTrainerPos[7].m_x) {
            m_trainerAway[3] = 0;
        }
    }
}

int stMarioPast::getScrollDir(Vec3f* dir) {
    dir->m_x = m_scroll.m_x;
    dir->m_y = m_scroll.m_y;
    dir->m_z = m_scroll.m_z;
    return 1;
}

bool stMarioPast::isEventEnd(int param1, int* eventState, int* eventDecision) {
    if (m_event == 0) {
        return false;
    }
    if (m_eventPrev > 0.0f && m_posGimmick[12].m_x <= 0.0f) {
        *eventState = 6;
        *eventDecision = 4;
        return true;
    }
    m_eventPrev = m_posGimmick[12].m_x;
    return false;
}

void stMarioPast::getBamperVector(Vec3f* vec) {
    if (vec == NULL) {
        return;
    }
    vec->m_x = m_scroll.m_x;
    vec->m_y = m_scroll.m_y;
    vec->m_z = m_scroll.m_z;
}

// Builds the two block grounds (one per model node group): the blocks are read from the model nodes, their collision is
// built from the nodes' positions and the ground gets pointers to both.
void stMarioPast::createObjBlockPos() {
    if (m_stageData != NULL) {
        nw4r::g3d::ResMdl model;
        void* data = m_fileData->getData(Data_Type_Model, 1, 0xFFFE);
        bool found = false;
        if (data != NULL) {
            nw4r::g3d::ResFile file(data);
            nw4r::g3d::ResMdl mdl = file.GetResMdl(static_cast<unsigned long>(0));
            if (mdl.IsValid()) {
                found = true;
                model = mdl;
            }
        }
        if (found) {
            float scale = static_cast<float*>(m_stageData)[3];
            if (m_stage == 1) {
                setupBlockWork(scale, &m_blockWork, &model, 0x107);
            } else if (m_stage == 0) {
                setupBlockWork(scale, &m_blockWork, &model, 0x2C);
            }
            for (u32 i = 0; i < 2; i++) {
                grMarioPastBlockPosLite* ground = grMarioPastBlockPosLite::create(0x5A, "grMarioPastBlockPos");
                if (ground != NULL) {
                    addGround(ground);
                    ground->m_stage = m_stage;
                    ground->m_group = i;
                    ground->setStageData(m_stageData);
                    ground->m_posGimmick = &m_posGimmick[i * 4 / 4];
                    ground->m_limit = m_limit;
                    ground->m_num = m_blockWork.m_num[i];
                    ground->m_blocks = m_blockWork.m_start[i];
                    ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                    grCollision* collision = NULL;
                    stCollisionWork* work = &m_collWork[i];
                    if (work != NULL && ground != NULL) {
                        grCollData* collData = new (Heaps::StageInstance) grCollData;
                        if (collData != NULL) {
                            initCollDatafromCollWorkBlock(collData, work, ground, m_blockWork.m_num[i], m_blockWork.m_start[i]);
                            collision = grCollision::create(this, collData, ground, 1);
                            delete collData;
                        }
                    }
                    if (collision != NULL) {
                        ground->m_joints = collision->getJoint(0);
                        ground->initCollisionJoint();
                    }
                    ground->initYaku();
                }
            }
        }
    }
}


// Builds the collision of the blocks of one model: every block gets a closed square of 8.5 units around its position (four
// points, four lines, one joint).
void stMarioPast::initCollDatafromCollWorkBlock(grCollData* data, stCollisionWork* work, Ground* ground, u32 count,
                                                stMarioPastBlockDt* blocks) {
    if (data == NULL || work == NULL || ground == NULL || count == 0 || blocks == NULL || !fn_27_239FD8(work)) {
        return;
    }
    // MATCH-ONLY: the three data pointers are private in the header; this view reads them at their offsets.
    stCollisionWorkView* wv = reinterpret_cast<stCollisionWorkView*>(work);
    u32 vtxNum = work->m_vtxLen * count;
    double minX = 0.0;
    double maxX = 0.0;
    double minY = 0.0;
    double maxY = 0.0;
    wv->m_vtxDatas = reinterpret_cast<grCollData::VtxData*>(new (Heaps::StageInstance) u8[(vtxNum * 8) & 0x7FFF8]);
    wv->m_lineDatas = reinterpret_cast<grCollData::LineData*>(new (Heaps::StageInstance) u8[(vtxNum & 0xFFFF) << 4]);
    wv->m_jointDatas = reinterpret_cast<grCollData::JointData*>(new (Heaps::StageInstance) u8[(count & 0xFFFF) * 0x6C]);
    memset(wv->m_vtxDatas, 0, (vtxNum & 0xFFFF) << 3);
    memset(wv->m_lineDatas, 0, (vtxNum & 0xFFFF) << 4);
    memset(wv->m_jointDatas, 0, (count & 0xFFFF) * 0x6C);
    float* vtx = reinterpret_cast<float*>(wv->m_vtxDatas);
    for (u32 i = 0; i < count; i++) {
        float x0 = blocks[i].m_pos.m_x - 4.25f;
        float y1 = blocks[i].m_pos.m_y + 4.25f;
        float x1 = blocks[i].m_pos.m_x + 4.25f;
        float y0 = blocks[i].m_pos.m_y - 4.25f;
        vtx[0] = x0;
        vtx[1] = y1;
        vtx[2] = x1;
        vtx[3] = y1;
        vtx[4] = x1;
        vtx[5] = y0;
        vtx[6] = x0;
        vtx[7] = y0;
        if (i != 0) {
            if (x0 < minX) {
                minX = x0;
            }
            if (maxX < x1) {
                maxX = x1;
            }
            if (y0 < minY) {
                minY = y0;
            }
            if (maxY < y1) {
                maxY = y1;
            }
        }
        vtx += 8;
    }
    short* line = reinterpret_cast<short*>(wv->m_lineDatas);
    short base = 0;
    for (u32 i = 0; i < count; i++) {
        static const short sLineKind[4] = { 0x11, 0x14, 0x12, 0x18 };
        short v = base;
        for (int k = 0; k < 4; k++) {
            line[0] = v;
            line[1] = (k == 3) ? base : v + 1;
            line[2] = (k == 0) ? base + 3 : v - 1;
            line[3] = (k == 3) ? base : v + 1;
            line[4] = -1;
            line[5] = -1;
            line[6] = sLineKind[k];
            line[7] = 0;
            line += 8;
            v++;
        }
        base += 4;
    }
    short* joint = reinterpret_cast<short*>(wv->m_jointDatas);
    base = 0;
    for (u32 i = 0; i < count; i++) {
        joint[0] = base;
        joint[1] = 4;
        *reinterpret_cast<float*>(joint + 10) = 0.0f;
        *reinterpret_cast<float*>(joint + 12) = 0.0f;
        *reinterpret_cast<float*>(joint + 14) = 1.0f;
        *reinterpret_cast<float*>(joint + 16) = 1.0f;
        joint[18] = base;
        joint[19] = 4;
        base += 4;
        joint += 0x36;
    }
    data->m_vtxLen = vtxNum;
    data->m_lineLen = vtxNum;
    data->m_jointLen = count;
    data->m_unk1 = 0;
    data->m_vtxDatas = wv->m_vtxDatas;
    data->m_lineDatas = wv->m_lineDatas;
    data->m_jointDatas = wv->m_jointDatas;
}

// Counts the blocks (nodes that contain "block_dummy" or "hatena_dummy" in their name) below the given node.
int stMarioPast::countBlockNum(nw4r::g3d::ResMdl* model, int node) {
    int count = 0;
    u32 num = model->GetResNodeNumEntries();
    for (u32 i = 0; i < num; i++) {
        nw4r::g3d::ResNode resNode = model->GetResNode(i);
        u8* parent = mpNodeParent(resNode);
        if (parent != NULL && node == mpNodeId(parent)) {
            const char* name = mpNodeName(resNode);
            if (strstr(name, "block_dummy") != NULL || strstr(name, "hatena_dummy") != NULL) {
                count++;
            }
        }
    }
    return count;
}

// Reads the blocks from the nodes of the model ("blockLocatorA" and "blockLocatorB" are the two groups) into one array.
// A "hatena_dummy" is a ? block, "block_dummy" a brick; "_item" and "_hide" tell that it holds an item or is hidden, and the
// rest of the name which item ("pwup", "1up", "10coin", "star").
bool stMarioPast::setupBlockWork(float scale, stMarioPastBlockWork* work, nw4r::g3d::ResMdl* model, int count) {
    static const char* const sGroupNames[2] = { "blockLocatorA", "blockLocatorB" };
    if (work->m_total != 0) {
        if (work->m_data != NULL) {
            delete[] work->m_data;
            work->m_data = NULL;
        }
        memset(work, 0, 0x18);
    }
    if (model->ptr() == NULL || count == 0) {
        return false;
    }
    int groupId[2];
    int total = 0;
    for (int i = 0; i < 2; i++) {
        nw4r::g3d::ResNode group = model->GetResNode(sGroupNames[i]);
        if (group.ptr() == NULL) {
            return false;
        }
        groupId[i] = mpNodeId(reinterpret_cast<u8*>(group.ptr()));
        int num = countBlockNum(model, groupId[i]);
        work->m_num[i] = num;
        total += num;
    }
    work->m_total = total;
    work->m_data = new (Heaps::StageInstance) stMarioPastBlockDt[total];
    memset(work->m_data, 0, count << 6);
    work->m_start[0] = work->m_data;
    work->m_start[1] = work->m_data + work->m_num[0];
    stMarioPastBlockDt* cursor[2];
    cursor[0] = work->m_start[0];
    cursor[1] = work->m_start[1];
    u32 num = model->GetResNodeNumEntries();
    for (u32 i = 0; i < num; i++) {
        nw4r::g3d::ResNode node = model->GetResNode(i);
        const char* name = mpNodeName(node);
        int group = -1;
        u8* parent = mpNodeParent(node);
        if (parent != NULL) {
            if (mpNodeId(parent) == groupId[0]) {
                group = 0;
            } else if (mpNodeId(parent) == groupId[1]) {
                group = 1;
            }
            if (group >= 0) {
                float bump = scale;
                u8 kind;
                if (strstr(name, "block_dummy") == NULL) {
                    if (strstr(name, "hatena_dummy") == NULL) {
                        continue;
                    }
                    bump = 0.0f;
                    kind = 2;
                    if (strstr(name, "hatena_dummy_hide") != NULL) {
                        kind = 3;
                    }
                } else {
                    kind = 0;
                    if (strstr(name, "block_dummy_item") != NULL) {
                        kind = 1;
                    }
                }
                u8 item = 9;
                u8 hits = 0;
                if (kind != 0) {
                    hits = 1;
                    if (strstr(name, "pwup") != NULL) {
                        item = 4;
                    } else if (strstr(name, "1up") != NULL) {
                        item = 5;
                    } else if (strstr(name, "10coin") != NULL) {
                        item = 6;
                        hits = 3;
                    } else {
                        item = 7 + (strstr(name, "star") != NULL);
                    }
                }
                Vec3f pos;
                pos.m_x = node.ptr()->m_translation.m_x;
                pos.m_y = node.ptr()->m_translation.m_y;
                pos.m_z = node.ptr()->m_translation.m_z;
                stMarioPastBlockDt* block = cursor[group];
                block->m_posOrg.m_x = pos.m_x;
                block->m_posOrg.m_y = pos.m_y;
                block->m_posOrg.m_z = pos.m_z;
                block->m_pos.m_x = pos.m_x;
                block->m_pos.m_y = pos.m_y;
                block->m_pos.m_z = pos.m_z;
                block->m_group = group;
                block->m_zone = 0;
                block->m_kind = kind;
                block->m_item = item;
                block->m_state = 1;
                block->m_count = hits;
                block->m_timer = 0.0f;
                block->m_bump = bump;
                block->m_index = 0;
                block->m_row = 0;
                cursor[group] = block + 1;
            }
        }
    }
    return true;
}

GXColor stMarioPast::getFinalTechniqColor() {
    u32 packed = 0x140004B8;
    if (m_stage != 0) {
        packed = 0x14000496;
    }
    return *reinterpret_cast<GXColor*>(&packed);
}

bool stMarioPast::isBamperVector() {
    return true;
}
