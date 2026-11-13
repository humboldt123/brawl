#include <gf/gf_heap_manager.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_data.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_manager.h>
#include <gr/ground.h>
#include <memory.h>
#include <st/stage.h>

// The collision construction helpers that sora_melee owns; the rest of grCollision lives in main.dol.

// BYTES: g_grCollisionList is declared as a list of grCollisionHandle, but its nodes are the grCollision links at
// +0x30 (same view as gr_collision.cpp).
typedef nw4r::ut::LinkList<grCollision, 0x30> grCollisionNodeList;
static inline grCollisionNodeList& collisionList() {
    return reinterpret_cast<grCollisionNodeList&>(g_grCollisionList);
}

extern grCollisionManager* lbl_805A0360;
// grCollision::createLine (main.dol 0x801107CC); the last argument is passed as the raw halfword here.
extern "C" grCollisionLine* fn_801107CC(grCollision* collision, u16 count, grCollData::LineData* lineDatas, u16 unk);

grCollision* grCollision::create(Stage* stage, void* colData, Ground* ground, int enable) {
    grCollision* collision = new (Heaps::StageInstance) grCollision;
    if (collision != NULL) {
        collision->m_isEnabled = enable & 1; // MATCH-ONLY: the bit is copied without a bool conversion
        collision->m_isDynamic = false;
        collision->m_next = NULL;
        collision->m_prev = NULL;
        collision->initialize(static_cast<grCollData*>(colData));
        collision->setupJointRef(stage, ground, static_cast<grCollData*>(colData));
        collisionList().PushBack(collision);
        collision->m_unk2D_7 = false;
    }
    return collision;
}

grCollision* grCollision::createHeap(Stage* stage, void* colData, Ground* ground, int enable, Heaps::HeapType heapType) {
    grCollision* collision = new (heapType) grCollision;
    if (collision != NULL) {
        collision->m_isEnabled = enable & 1; // MATCH-ONLY: the bit is copied without a bool conversion
        collision->m_isDynamic = false;
        collision->m_next = NULL;
        collision->m_prev = NULL;
        collision->initialize(static_cast<grCollData*>(colData));
        collision->setupJointRef(stage, ground, static_cast<grCollData*>(colData));
        collisionList().PushBack(collision);
        collision->m_unk2D_7 = false;
    }
    return collision;
}

grCollision::grCollision(void* colData, int enable, int dynamic) {
    m_isEnabled = enable & 1; // MATCH-ONLY: the bit is copied without a bool conversion
    m_isDynamic = false;
    m_next = NULL;
    m_prev = NULL;
    initialize(static_cast<grCollData*>(colData));
    m_isDynamic = dynamic & 1; // MATCH-ONLY: as above
    collisionList().PushBack(this);
}

void grCollision::initialize(grCollData* colData) {
    m_disablePending = false;
    m_hasActiveJoint = false;
    m_unk2C_3 = true;
    m_noUpdate = false;
    m_lineStatusDone = false;
    m_hasHangJoint = false;
    m_isDestroying = false;
    lbl_805A0360->regist(this);
    u16 unk = colData->m_unk1;
    m_collData = colData;
    u16 vtxLen = colData->m_vtxLen;
    m_vtxLen = vtxLen;
    m_lineLen = colData->m_lineLen;
    m_jointLen = colData->m_jointLen;
    m_vtxDatas = colData->m_vtxDatas;
    m_vtxPositions = static_cast<Vec2f*>(gfHeapManager::alloc(Heaps::StageInstance, vtxLen * 16));
    m_lines = fn_801107CC(this, m_lineLen, colData->m_lineDatas, unk);
    m_joints = createJoint(m_jointLen, colData->m_jointDatas);
    m_aabBox.m_unk1 = -10.0f;
    m_aabBox.m_unk2 = -10.0f;
    m_aabBox.m_unk3 = 10.0f;
    m_aabBox.m_unk4 = 10.0f;
}

// HYPOTHESIS: an inline helper (both node values are read before any of the joint stores).
static inline void bindJoint(grCollisionJoint* joint, Ground* ground, int mdlIndex, u16 nodeIndex, grCollision* collision) {
    joint->m_ground = ground;
    joint->_0x4E = mdlIndex;
    joint->m_nodeIndex = nodeIndex;
    ground->m_collision = collision;
    joint->m_0x49 = 1;
}

// Binds each joint to the ground model node named by its joint data: the given ground when there is one, otherwise
// the first of the stage's grounds that has the node.
void grCollision::setupJointRef(Stage* stage, Ground* ground, grCollData* colData) {
    grCollData::JointData* jointDatas = colData->m_jointDatas;
    u32 groundNum = stage->getGroundNum();
    for (u32 i = 0; i < m_jointLen; i++) {
        grCollisionJoint* joint = &m_joints[i];
        grCollData::JointData* data = &jointDatas[i];
        if (static_cast<u8>(data->m_name[0]) != '\0') {
            const char* mdlName = data->_spacer2C;
            const char* nodeName = data->m_name;
            if (ground == NULL) {
                for (u32 j = 0; j < groundNum; j++) {
                    Ground* g = stage->getGround(j);
                    int mdlIndex;
                    u16 nodeIndex;
                    if (g->searchNode(mdlName, nodeName, &mdlIndex, &nodeIndex)) {
                        bindJoint(joint, g, mdlIndex, nodeIndex, this);
                        break;
                    }
                }
            } else {
                int mdlIndex;
                u16 nodeIndex;
                if (ground->searchNode(mdlName, nodeName, &mdlIndex, &nodeIndex)) {
                    bindJoint(joint, ground, mdlIndex, nodeIndex, this);
                }
            }
        }
    }
    if (ground != NULL) {
        ground->m_collision = this;
    }
}
