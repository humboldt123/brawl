// The constructor of soGroundModuleImpl is declared only for the module builders.
#define FT_MODULE_BUILDER
// soGroundModuleImpl declares its anim cmd handler with an int index; the observer base must take the same one for the
// thunk of the observer vtable to be the module's.
#define WN_ARTICLE_ANIMCMD_INT
#include <em/enemy.h>
#include <em/em_info.h>
#include <em/ground/em_ground_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// HYPOTHESIS: the default ground condition checker of sora_melee (.bss 0x2AC, 0x10 bytes; its class is not known, the
// interface is abstract), which every enemy ground module uses.
extern char lbl_27_bss_2AC[];
#define EM_DEFAULT_GROUND_CONDITION_CHECKER reinterpret_cast<soGroundConditionChecker*>(lbl_27_bss_2AC)

static const u32 WorkFlag_Front = 0x12000003; // the enemy faces the camera (see Enemy::getEmTopNRot)

// HYPOTHESIS: the last parameter of the soGroundModuleImpl constructor is the ground tracer (the shared header calls it
// a soEventObserverRegistrationDesc*); the module keeps it as m_groundTracer.
emGroundModuleImpl::emGroundModuleImpl(soModuleAccesser* acc, soArray<soGroundShapeImpl>* shapes, void* groundData)
    : soGroundModuleImpl(acc, shapes, EM_DEFAULT_GROUND_CONDITION_CHECKER,reinterpret_cast<soEventObserverRegistrationDesc*>(&m_tracer)),
      m_vtxNodeData(static_cast<emGroundVtxNodeData*>(groundData)) { }

emGroundModuleImpl::~emGroundModuleImpl() { }

void emGroundModuleImpl::updateVtxFromNodeSub(u32 nodeId, u32 vtxIndex, bool init) {
    if (vtxIndex >= getGrCollisionVertexNum()) {
        return;
    }
    EnemyKind kind = static_cast<EnemyKind>(m_moduleAccesser->getStageObject().soGetSubKind());
    if (nodeId >= emInfo::getInstance()->getNodeNum(kind)) {
        return;
    }
    Vec3f nodePos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeId, false);
    Vec2f pos(nodePos.m_x, nodePos.m_y);
    if (init == true) {
        initGrCollisionVertexPos(vtxIndex, &pos);
    } else {
        setGrCollisionVertexPos(vtxIndex, &pos);
    }
}

void emGroundModuleImpl::updateVtxFromNode(bool init) {
    emGroundVtxNodeData* data = m_vtxNodeData;
    if (data == NULL) {
        return;
    }
    u32 num = data->m_num;
    float rotYLr = m_moduleAccesser->getPostureModule().getRotYLr();
    if (m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front) != true && rotYLr == -90.0f) {
        // Turned away: pair each vertex with the node of the mirrored entry.
        for (u32 i = 0; i < num; i++) {
            updateVtxFromNodeSub(getVtxNode(num - 1 - i).m_nodeId, getVtxNode(i).m_vtxIndex, init);
        }
    } else {
        for (u32 i = 0; i < num; i++) {
            updateVtxFromNodeSub(getVtxNode(i).m_nodeId, getVtxNode(i).m_vtxIndex, init);
        }
    }
}

void emGroundModuleImpl::update(u32 index) {
    soGroundModuleImpl::update(index);
    updateVtxFromNode(false);
}

float emGroundModuleImpl::getRotateYForGrCollisionMatrix() {
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Enemy& enemy = dynamic_cast<Enemy&>(m_moduleAccesser->getStageObject());
    Vec3f rot = enemy.getEmTopNRot(posture.getRotYLr(), m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front));
    return rot.m_y;
}

bool emGroundModuleImpl::isReferenceModelNode() {
    if (m_vtxNodeData != NULL) {
        return true;
    }
    return soGroundModuleImpl::isReferenceModelNode();
}

// The inline methods of soGroundModuleImpl (weak copies of this unit, which the vtable points at).

bool soGroundModuleImpl::isObserv(char group) {
    return group == 8;
}

void soGroundModuleImpl::setDetachChangeStatus(bool detach) {
    m_detachChangeStatus = detach;
}

void soGroundModuleImpl::setIgnoreFriction(bool ignore) {
    m_ignoreFriction = ignore;
}

soGroundConditionChecker* soGroundModuleImpl::getConditionChecker() {
    return m_groundConditionChecker;
}

float soGroundModuleImpl::getZ() {
    return m_Z;
}

void soGroundModuleImpl::setZ(float z) {
    m_Z = z;
}

void soGroundModuleImpl::setCliffBackRangeXMul(float cliffBackRangeXMul) {
    m_cliffBackRangeXMul = cliffBackRangeXMul;
}

void soGroundModuleImpl::setCliffHangData(void* cliffHangData) {
    m_cliffHangData = cliffHangData;
}

void soGroundModuleImpl::pause(bool pause) {
    m_pause = pause;
}
