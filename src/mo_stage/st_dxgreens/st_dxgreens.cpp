#include <cm/cm_camera_controller.h>
#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <string.h>
#include <types.h>

#include <st_dxgreens/gr_dxgreens.h>
#include <st_dxgreens/st_dxgreens.h>

stClassInfoImpl<Stages::DxGreens, stDxGreens> stDxGreens::bss_loc_14;

stDxGreens* stDxGreens::create() {
    return new (Heaps::StageInstance) stDxGreens;
}

stDxGreens::stDxGreens() : stMelee("stDxGreens", Stages::DxGreens) {
    memset(m_limit, 0, sizeof(m_limit));
    memset(m_block, 0, sizeof(m_block));
    memset(m_posBlock, 0, sizeof(m_posBlock));
    m_blockInit = 1;
    m_dropTimer = 0.0f;
    m_windTrigger = NULL;
    m_windData = NULL;
    m_joint[0] = NULL;
    m_joint[1] = NULL;
    m_joint[2] = NULL;
}

stDxGreens::~stDxGreens() {
    for (u8 i = 0; i < 6; i++) {
        clearListAll(&m_list[i]);
    }
    if (m_windData != NULL) {
        delete m_windData;
    }
    releaseArchive();
}

bool stDxGreens::loading() {
    return true;
}

void stDxGreens::createObj() {
    int size;
    void* data = m_fileData->getData(Data_Type_Model, 0x2711, &size, 0xFFFE);
    if (data != NULL) {
        m_archiveAppleParam.setFileImage(data, size, Heaps::StageResource);
    }
    data = m_fileData->getData(Data_Type_Model, 0x2712, &size, 0xFFFE);
    if (data != NULL) {
        m_archiveApple.setFileImage(data, size, Heaps::StageResource);
    }
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 0x78);
    createObjWind();
    createObjBg(1);
    createObjBg(0);
    createCollision(m_fileData, 2, NULL);
    createObjWhispy();
    createObjBlockPos();
    createObjBlock();
    initCameraParam();

    void* posData = m_fileData->getData(Data_Type_Model, 100, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    loadStageAttrParam(m_fileData, 0x1E);
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);
    initPosPokeTrainer(2, 0);
    createObjPokeTrainer(m_fileData, 0x65, "PokeTrainer00", m_pokeTrainerPos, NULL);
    createObjPokeTrainer(m_fileData, 0x66, "PokeTrainer01", m_pokeTrainerPos + 2, NULL);
}

void stDxGreens::createObjBg(int index) {
    Ground* ground;
    switch (index) {
    case 0:
        ground = grDxGreens::create(0, "StgDxGreensStage", "grDxGreensStage");
        break;
    case 1:
        ground = grDxGreens::create(1, "StgDxGreensEnkei", "grDxGreensEnkei");
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

void stDxGreens::createObjWhispy() {
    grDxGreensWhispy* ground = grDxGreensWhispy::create(2, "StgDxGreensWhispy", "grDxGreensWhispy");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setTrigger(m_windTrigger);
        ground->setWindDataWork(m_windData);
    }
}

void stDxGreens::createObjBlockPos() {
    grDxGreensBlockPos* ground = grDxGreensBlockPos::create(3, "StgDxGreensBlockPosition", "grDxGreensBlockPos");
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->setStageData(m_stageData);
        ground->setBlockDataWork(&m_block[0][0]);
        ground->setBlockPosWork(&m_posBlock[0][0]);
    }
}

// Creates the thirty blocks and sets the time of the first drop (the stage data holds the start, the range and the factor
// of every level).
void stDxGreens::createObjBlock() {
    for (u8 column = 0; column < 6; column++) {
        for (u8 row = 0; row < 5; row++) {
            createObjBlock1(column, row);
        }
    }
    float* data = static_cast<float*>(m_stageData);
    if (data != NULL) {
        u8 level = getBlockLevel();
        float range = data[1] * data[level + 2];
        m_dropTimer = data[0] + randf() * range;
    }
}

void stDxGreens::createObjBlock1(u32 column, u32 row) {
    if (column < 6 && row < 5) {
        float* data = static_cast<float*>(m_stageData);
        if (data != NULL) {
            stDxGreensBlockData* block = &m_block[column][row];
            grDxGreensBlock* ground = grDxGreensBlock::create(4, "StgDxGreensBlock", "grDxGreensBlock");
            if (ground != NULL) {
                addGround(ground);
                ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
                ground->setStageData(m_stageData);
                ground->setBlockDataWork(block);
                ground->setPosLimitWork(m_limit);
                if (data[7] > randf()) {
                    block->m_hard = 1;
                } else {
                    block->m_hard = 0;
                }
                block->m_state = 7;
                createCollision(m_fileData, 3, ground);
            }
        }
    }
}

// The wind of Whispy: an area without size (the ground gives it its size when he blows) and a trigger that starts asleep.
void stDxGreens::createObjWind() {
    m_windData = new (Heaps::StageInstance) grGimmickWindData;
    if (m_windData != NULL) {
        memset(m_windData, 0, sizeof(grGimmickWindData));
        Vec3f pos(0.0f, 0.0f, 0.0f);
        Vec2f areaPos(0.0f, 0.0f);
        Vec2f areaRange(0.0f, 0.0f);
        m_windData->m_pos = pos;
        m_windData->m_speed = 10.0f;
        m_windData->m_vector = 0.0f;
        m_windData->m_areaData.set(&areaPos, &areaRange);
        m_windTrigger = g_stTriggerMng->createTrigger(Gimmick::Area_Wind, -1);
        m_windTrigger->setWindTrigger(m_windData);
        m_windTrigger->setAreaSleep(true);
    }
}

void stDxGreens::update(float deltaFrame) {
    updateLimit(deltaFrame);
    updateBlockInit(deltaFrame);
    updateBlockDrop(deltaFrame);
    updateBlockDamage(deltaFrame);
    updateJoint(deltaFrame);
}

static inline void setLimitPoint(Vec3f* dst, float x, float y) {
    dst->m_x = x;
    dst->m_y = y;
    dst->m_z = 0.0f;
}

// Takes over the limits of the camera.
void stDxGreens::updateLimit(float deltaFrame) {
    CameraController* camera = CameraController::getInstance();
    setLimitPoint(&m_limit[0], camera->unk158, camera->unk160);
    setLimitPoint(&m_limit[1], camera->unk15C, camera->unk164);
}

// Once the positions of the blocks are known, the starting position of the blocks of every column is set (the columns
// start with 4, 3, 2, 2, 3 and 4 blocks).
void stDxGreens::updateBlockInit(float deltaFrame) {
    if (m_blockInit != 0) {
        float* data = static_cast<float*>(m_stageData);
        if (m_posBlock[0][0].m_x != 0.0f && data != NULL) {
            for (u8 column = 0; column < 6; column++) {
                for (u8 row = 0; row < 5; row++) {
                    stDxGreensBlockData* block = &m_block[column][row];
                    block->m_pos = m_posBlock[column][row];
                    block->m_posTgt = m_posBlock[column][row];
                    block->m_index = row;
                    if (data[7] > randf()) {
                        block->m_hard = 1;
                    } else {
                        block->m_hard = 0;
                    }
                }
            }
            m_block[0][0].m_state = 1;
            m_block[0][1].m_state = 1;
            m_block[0][2].m_state = 1;
            m_block[0][3].m_state = 1;
            addList(0, &m_list[0]);
            addList(1, &m_list[0]);
            addList(2, &m_list[0]);
            addList(3, &m_list[0]);
            m_block[1][0].m_state = 1;
            m_block[1][1].m_state = 1;
            m_block[1][2].m_state = 1;
            addList(0, &m_list[1]);
            addList(1, &m_list[1]);
            addList(2, &m_list[1]);
            m_block[2][0].m_state = 1;
            m_block[2][1].m_state = 1;
            addList(0, &m_list[2]);
            addList(1, &m_list[2]);
            m_block[3][0].m_state = 1;
            m_block[3][1].m_state = 1;
            addList(0, &m_list[3]);
            addList(1, &m_list[3]);
            m_block[4][0].m_state = 1;
            m_block[4][1].m_state = 1;
            m_block[4][2].m_state = 1;
            addList(0, &m_list[4]);
            addList(1, &m_list[4]);
            addList(2, &m_list[4]);
            m_block[5][0].m_state = 1;
            m_block[5][1].m_state = 1;
            m_block[5][2].m_state = 1;
            m_block[5][3].m_state = 1;
            addList(0, &m_list[5]);
            addList(1, &m_list[5]);
            addList(2, &m_list[5]);
            addList(3, &m_list[5]);
            m_blockInit = 0;
        }
    }
}

// From time to time a new block drops into a random column that has room: it appears at the top of the column and falls
// to the next free row.
void stDxGreens::updateBlockDrop(float deltaFrame) {
    if (m_blockInit != 1) {
        m_dropTimer -= deltaFrame;
        if (m_dropTimer < 0.0f) {
            m_dropTimer = 0.0f;
        }
        if (m_dropTimer == 0.0f) {
            float* data = static_cast<float*>(m_stageData);
            if (data != NULL) {
                u8 level = getBlockLevel();
                float range = data[1] * (&data[2])[level];
                m_dropTimer = data[0] + randf() * range;
                u8 column = (u8)(randf() * 6.0f);
                nw4r::ut::LinkList<SListNode, 0>* list = &m_list[column];
                if (list->GetSize() != 5) {
                    SListNode* last = getListEnd(list);
                    if (last != NULL) {
                        if (m_posBlock[column][4].m_y == m_block[column][last->m_index].m_posTgt.m_y) {
                            return;
                        }
                    }
                    stDxGreensBlockData* blocks = m_block[column];
                    u32 slot;
                    for (slot = 0; slot < 5; slot++) {
                        if (blocks[slot].m_state != 7) {
                            break;
                        }
                    }
                    if (slot != 5) {
                        stDxGreensBlockData* block = &blocks[slot];
                        u8 row;
                        if (last == NULL) {
                            row = 0;
                        } else {
                            row = blocks[last->m_index].m_index + 1;
                        }
                        block->m_pos = m_posBlock[column][0];
                        block->m_posTgt = m_posBlock[column][row];
                        block->m_index = row;
                        block->m_state = 0;
                        if (data[7] > randf()) {
                            block->m_hard = 1;
                        } else {
                            block->m_hard = 0;
                        }
                        addList(slot, list);
                    }
                }
            }
        }
    }
}

// Takes the broken blocks out of the lists of their columns and lets the blocks above them fall down one row.
void stDxGreens::updateBlockDamage(float deltaFrame) {
    if (m_blockInit != 1) {
        for (u8 column = 0; column < 6; column++) {
            int lowest = -1;
            for (u8 row = 0; row < 5; row++) {
                stDxGreensBlockData* block = &m_block[column][row];
                if (block->m_state == 5) {
                    if (lowest == -1) {
                        lowest = (s8)block->m_index;
                    }
                    if ((int)row < lowest) {
                        lowest = (s8)block->m_index;
                    }
                    clearList(row, &m_list[column]);
                    block->m_state = 7;
                }
            }
            if (lowest >= 0) {
                SListNode* node = getListTop(&m_list[column]);
                for (u32 count = m_list[column].GetSize() & 0xFF; count != 0; count--) {
                    stDxGreensBlockData* block = &m_block[column][node->m_index];
                    u8 state = block->m_state;
                    if (state != 1) {
                        if (state == 0) {
                            s8 target = lowest;
                            if ((int)target < (int)block->m_index) {
                                block->m_posTgt = m_posBlock[column][target];
                                block->m_index = target;
                                lowest = lowest + 1;
                            }
                        } else if (state < 3) {
                            lowest = (s8)(block->m_index + 1);
                        }
                    }
                    node = static_cast<SListNode*>(node->GetNext());
                }
            }
        }
    }
}

// The blocks that lie at rest close the ledges of the platforms they touch: the outer ones (column 0 and 5) on the outside,
// the others towards the middle.
void stDxGreens::updateJoint(float deltaFrame) {
    if (m_joint[0] == NULL || m_joint[1] == NULL || m_joint[2] == NULL) {
        grCollision* collision = getCollision(0);
        if (collision != NULL) {
            m_joint[0] = collision->getJoint(0);
            m_joint[1] = collision->getJoint(1);
            m_joint[2] = collision->getJoint(2);
        }
    } else {
        u16 flags0 = 0;
        u16 flags1 = 0;
        u16 flags2 = 0;
        for (u8 column = 0; column < 6; column++) {
            u8 row = 0;
            while (row != 5) {
                stDxGreensBlockData* block = &m_block[column][row];
                if (block->m_state == 2) {
                    if (column == 3) {
                        if (block->m_index < 2) {
                            flags1 |= 0x4000;
                        }
                    } else if (column < 3) {
                        if (column != 1) {
                            if (column == 0) {
                                if (block->m_index == 0) {
                                    flags0 = 0x4000;
                                }
                            } else if (block->m_index < 2) {
                                flags1 |= 0x2000;
                            }
                        }
                    } else if (column == 5 && block->m_index == 0) {
                        flags2 = 0x2000;
                    }
                }
                row++;
            }
        }
        m_joint[0]->m_0x52 = flags0;
        m_joint[1]->m_0x52 = flags1;
        m_joint[2]->m_0x52 = flags2;
    }
}

// The highest number of blocks that stand in a column.
u8 stDxGreens::getBlockLevel() {
    u32 level = 0;
    for (u8 i = 0; i < 6; i++) {
        if (level < m_list[i].GetSize()) {
            level = m_list[i].GetSize() & 0xFF;
        }
    }
    return level;
}

void stDxGreens::getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID) {
    if (itemID != Item_Stage_Apple) {
        return;
    }
    *brres = &m_archiveAppleParam;
    *param = &m_archiveApple;
}

void stDxGreens::addList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = new (Heaps::StageResource) SListNode;
    if (node != NULL) {
        node->m_index = index;
        list->PushBack(node);
    }
}

void stDxGreens::clearList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = getList(index, list);
    if (node != NULL) {
        list->Erase(node);
        if (node != NULL) {
            delete node;
        }
    }
}

void stDxGreens::clearListAll(nw4r::ut::LinkList<SListNode, 0>* list) {
    for (u32 count = list->GetSize(), i = 0; i != count; i++) {
        SListNode* node = &list->GetFront();
        list->Erase(node);
        if (node != NULL) {
            delete node;
        }
    }
}

stDxGreens::SListNode* stDxGreens::getList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list) {
    u32 count = list->GetSize();
    SListNode* node = &list->GetFront();
    while (true) {
        if (count == 0) {
            return NULL;
        }
        if (index == node->m_index) {
            break;
        }
        node = static_cast<SListNode*>(node->GetNext());
        count--;
    }
    return node;
}

stDxGreens::SListNode* stDxGreens::getListTop(nw4r::ut::LinkList<SListNode, 0>* list) {
    return &list->GetFront();
}

stDxGreens::SListNode* stDxGreens::getListEnd(nw4r::ut::LinkList<SListNode, 0>* list) {
    SListNode* node = &list->GetFront();
    u32 position = 0;
    u32 count = list->GetSize();
    u32 last = count - 1;
    while (true) {
        if (count == 0) {
            return NULL;
        }
        if (position == last) {
            break;
        }
        node = static_cast<SListNode*>(node->GetNext());
        position++;
        count--;
    }
    return node;
}

bool stDxGreens::isBamperVector() {
    return true;
}
