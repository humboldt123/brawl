#define SO_ANIM_CMD_ADDRESS_PACK_ARGS
#define SO_LINK_PHYSICS_SLOTS
#define SO_GLOW_MODULE_ATTACK_API
#define SO_LINK_MODULE_IMPL_FULL
#include <so/link/so_link_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <types.h>
#include <so/color/so_color_blend_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/glow/so_glow_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/shake/so_shake_module_simple.h>
#include <so/slow/so_slow_module_impl.h>
#include <so/camera/so_camera_module_impl.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/so_module_hyp.h>

soLinkModule::~soLinkModule() {
}

soLinkModuleImpl::soLinkModuleImpl(soModuleAccesser* moduleAccesser, soArray<soLinkConnection>* connections)
    : soAnimCmdEventObserver(5, static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId()) {
    m_modelConstraintNo = -1;
    m_modelConstraintFlag.m_mask = 0;
    m_isModelConstraintMutual = false;
    m_anmFileTransactorLinkNo = -1;
    m_moduleAccesser = moduleAccesser;
    m_connections = connections;
    m_modelConstraintNode = -1;
    m_modelConstraintTargetNode = -1;
    m_serverNode.m_next = NULL;
    m_serverNode.m_prev = NULL;
    m_serverNodeOwner = this;
    for (s32 i = 0; i < m_connections->size(); i++) {
        soLinkConnection& connection = m_connections->at(i);
        connection.m_id = i;
        connection.reset();
    }
    m_nodes.m_tail = NULL;
    m_nodes.m_head = NULL;
    m_nodes.m_length = 0;
    m_parents.m_tail = NULL;
    m_parents.m_head = NULL;
    m_parents.m_length = 0;
}

soLinkModuleImpl::~soLinkModuleImpl() {
    deactivate();
}

void soLinkModuleImpl::adjustModelConstraintPosture(Vec3f* rot, Vec3f* pos) {
    static_cast<soLinkModule*>(this)->adjustModelConstraintPosture(m_modelConstraintFlag.m_mask, rot, pos);
}

void soLinkModuleImpl::adjustModelConstraintPosture(u16 flags, Vec3f* rot, Vec3f* pos) {
    Matrix mtx(true);
    static_cast<soLinkModule*>(this)->getModelConstraintTargetNodeWorldMtx(&mtx, false);
    if ((flags & 1) != 0) {
        Vec3f outPos;
        mtx.mulPos(rot, &outPos);
        m_moduleAccesser->getPostureModule().setPos(&outPos);
    }
    if ((flags & 2) != 0) {
        Vec3f outRot;
        mtx.getRotate(&outRot);
        outRot.m_x = 57.29578f * outRot.m_x + pos->m_x;
        outRot.m_y = 57.29578f * outRot.m_y + pos->m_y;
        outRot.m_z = 57.29578f * outRot.m_z + pos->m_z;
        m_moduleAccesser->getPostureModule().setRot(&outRot, 0);
    }
}

Vec3f soLinkModuleImpl::getModelConstraintTargetNodePosition() {
    return m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(m_modelConstraintTargetNode, false);
}

Vec3f soLinkModuleImpl::getModelConstraintTargetNodeRotation() {
    return m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalRotation(m_modelConstraintTargetNode, false);
}

bool soLinkModuleImpl::getNodeRotation(u32 index, u32 nodeIndex, Vec3f* rotation) {
    rotation->m_x = rotation->m_y = rotation->m_z = 0.0f;
    Vec3f rot = m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalRotation(nodeIndex, false);
    rotation->m_x = rot.m_x;
    rotation->m_y = rot.m_y;
    rotation->m_z = rot.m_z;
    return true;
}

bool soLinkModuleImpl::setUpModelConstraintTargetMtx() {
    soLinkConnection& connection = m_connections->at(m_modelConstraintNo);
    if (connection.m_parent.getLinkModule()->m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        connection.m_parent.getLinkModule()->m_moduleAccesser->getLinkModule().setUpModelConstraintTargetMtx();
    }
    connection.m_parent.getLinkModule()->m_moduleAccesser->getModelModule().setUpMtx(m_modelConstraintTargetNode);
    return true;
}

void soLinkModuleImpl::getCmdAddressPackParents(u32 linkConnectionIndex, soArray<soAnimCmdAddressPack>* pack, int motionKind, short* ids) {
    soLinkModuleImpl* parent = m_connections->at(linkConnectionIndex).m_parent.getLinkModule();
    int linkNo = parent->m_anmFileTransactorLinkNo;
    if (linkNo != -1) {
        static_cast<soLinkModule*>(parent)->getCmdAddressPackParents(linkNo, pack, motionKind, ids);
    } else {
        parent->m_moduleAccesser->getAnimCmdModule().setAddressPackList(pack, motionKind, ids);
    }
}

bool soLinkModuleImpl::correctParent2ndBoth(u32 index, Vec3f* pos, float value) {
    soLinkModuleImpl* parent = m_connections->at(index).m_parent.getLinkModule();
    return static_cast<soHyp::soPhysicsModuleHyp*>(parent->m_moduleAccesser->m_enumerationStart->m_physicsModule)->unk38(pos, value);
}

void soLinkModuleImpl::setConstraint2ndWind(u8 wind) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (static_cast<soLinkModule*>(node->getLinkModule())->isModelConstraint() == true) {
            static_cast<soHyp::soPhysicsModuleHyp*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_physicsModule)->unkD8(wind);
        }
    }
}

void soLinkModuleImpl::getLinkParentCameraInfo(u32 index, u8* playerNo, u8* pri, u8* advPri) {
    soLinkConnection& connection = m_connections->at(index);
    *playerNo = connection.m_parent.getLinkModule()->m_moduleAccesser->getCameraModule().getPlayerNo(0);
    *pri = connection.m_parent.getLinkModule()->m_moduleAccesser->getCameraModule().getPri(0);
    *advPri = connection.m_parent.getLinkModule()->m_moduleAccesser->getCameraModule().getAdvPri(0);
}

StageObject* soLinkModuleImpl::getOwner() {
    return m_moduleAccesser->m_stageObject;
}

void soLinkModuleImpl::activate() {
    lbl_27_bss_110.entry(&m_serverNode);
    unlinkAll();
    unlinkNodeAll();
}

void soLinkModuleImpl::unlinkAll() {
    lbl_27_bss_110.disconnectAll(this);
}

void soLinkModuleImpl::unlinkNodeAll() {
    lbl_27_bss_110.disconnectNodeAll(this);
}

void soLinkModuleImpl::unlink(u32 index) {
    lbl_27_bss_110.disconnect(&m_connections->at(index));
}

bool soLinkModuleImpl::isLink(int index) {
    if (index != -1) {
        return m_connections->at(index).m_isLinked;
    }
    return m_parents.m_length != 0;
}

bool soLinkModuleImpl::isLinked(int index) {
    if (index != -1) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (index == static_cast<int>(node->m_connection->m_id)) {
                return true;
            }
        }
        return false;
    }
    return m_nodes.m_length != 0;
}

void soLinkModuleImpl::onUnlink(int id) {
    if (isModelConstraint() && m_modelConstraintNo == id) {
        static_cast<soLinkModule*>(this)->removeModelConstraint(true);
    }
}

bool soLinkModuleImpl::isModelConstraint() {
    return m_modelConstraintNo != -1;
}

StageObject* soLinkModuleImpl::getNode(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

s8 soLinkModuleImpl::getParentModelLoupeNo(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getLoupeNo();
}

StageObject* soLinkModuleImpl::getParent(u32 index) {
    soLinkConnection& connection = m_connections->at(index);
    if (connection.m_isLinked) {
        return connection.m_parent.getLinkModule()->getOwner();
    }
    return NULL;
}

StageObject* soLinkModuleImpl::searchParent(int taskId) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (taskId == node->m_taskId) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

StageObject* soLinkModuleImpl::searchNode(int taskId) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (taskId == node->m_taskId) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

float soLinkModuleImpl::getParentScale(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getPostureModule().getScale();
}

GXColor soLinkModuleImpl::getParentSubColor(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getColorBlendModule().getSubColor();
}

float soLinkModuleImpl::getParentMotionFrame(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getMotionModule().getFrame();
}

SituationKind soLinkModuleImpl::getParentSituationKind(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getSituationModule().getKind();
}

float soLinkModuleImpl::getParentLr(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getPostureModule().getLr();
}

int soLinkModuleImpl::getParentTaskId(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->getOwner()->m_taskId;
}

float soLinkModuleImpl::getParentDamage(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getDamage(0);
}

float soLinkModuleImpl::getParentDamageReaction(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getReaction(0);
}

float soLinkModuleImpl::getParentDamagePowerMax(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getPowerMax(0);
}

bool soLinkModuleImpl::isParentDamageReaction(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().isReaction();
}

bool soLinkModuleImpl::isParentDamageCatchCut(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().isCatchCut();
}

int soLinkModuleImpl::getParentTotalHitStatus(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getCollisionHitModule().getTotalStatus(0);
}

float soLinkModuleImpl::getOwnerAttackPowerMulArticle(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getParamCustomizeModule().getAttackPowerMulArticle();
}

bool soLinkModuleImpl::isParentRefFB(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().isRefFB();
}

bool soLinkModuleImpl::isParentEffectSyncScale(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getEffectModule().isSyncScale();
}

u8 soLinkModuleImpl::getParentAttachItemVisibility(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getItemManageModule().getAttachItemVisibility();
}

void soLinkModuleImpl::setAnmFileTransactorLinkNo(int no) {
    m_anmFileTransactorLinkNo = no;
}

u32 soLinkModuleImpl::getModelConstraintNo() {
    return m_modelConstraintNo;
}

void soLinkModuleImpl::setModelConstraintNode(int node) {
    m_modelConstraintNode = node;
}

void soLinkModuleImpl::setModelConstraintTargetNode(int node) {
    m_modelConstraintTargetNode = node;
}

bool soLinkModuleImpl::isObserv(char group) {
    return group == 15;
}

bool soLinkModuleImpl::isModelConstraintMutual() {
    return m_isModelConstraintMutual;
}

int soLinkModuleImpl::getModelConstraintNode() {
    return m_modelConstraintNode;
}

void soLinkModuleImpl::offModelConstraintFlag(u16 flag) {
    m_modelConstraintFlag.m_mask &= ~flag;
}

void soLinkModuleImpl::setModelConstraintFlag(u16 flag) {
    m_modelConstraintFlag.m_mask = flag;
}

u16 soLinkModuleImpl::getModelConstraintFlag() {
    return m_modelConstraintFlag.m_mask;
}

// The deactivation events sent to both ends of every connection (soLinkEventArgs keeps its constructor protected).
struct soLinkDeactivateEvent : public soLinkEventArgs {
    soLinkDeactivateEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

void soLinkModuleImpl::deactivate() {
    soLinkDeactivateEvent nodeEvent(StageObject::Link::Event_Deactivate_Node);
    sendEventParents(-1, nodeEvent);
    soLinkDeactivateEvent parentEvent(StageObject::Link::Event_Deactivate_Parent);
    sendEventNodes(-1, parentEvent, 0);
    unlinkAll();
    unlinkNodeAll();
    lbl_27_bss_110.exit(&m_serverNode);
}

bool soLinkModuleImpl::link(u32 index, int taskId) {
    return lbl_27_bss_110.link(this, taskId, &m_connections->at(index));
}

void soLinkModuleImpl::unlinkNode(u32 index) {
    soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0));
    while (node != NULL) {
        soLinkNode* next = static_cast<soLinkNode*>(node->m_next);
        if (static_cast<int>(index) == static_cast<int>(node->m_connection->m_id)) {
            node->getLinkModule()->unlink(index);
        }
        node = next;
    }
}

void soLinkModuleImpl::setAttribute(u32 index, soLinkConnection::Attribute attribute, bool enable) {
    soLinkConnection& connection = m_connections->at(index);
    if (enable == true) {
        connection.m_attribute |= 1 << attribute;
    } else {
        connection.m_attribute &= ~(1 << attribute);
    }
}

bool soLinkModuleImpl::chkLinkedAttribute(soLinkConnection::Attribute attribute) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << attribute)) {
            return true;
        }
    }
    return false;
}

int soLinkModuleImpl::getNodeTaskId(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->getOwner()->m_taskId;
        }
    }
    return -1;
}

bool soLinkModuleImpl::isNodeDamageReaction(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->m_moduleAccesser->getDamageModule().isReaction();
        }
    }
    return false;
}

bool soLinkModuleImpl::isNodeDamageCaptureCut(u8 id) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (id == static_cast<int>(node->m_connection->m_id)) {
            return node->getLinkModule()->m_moduleAccesser->getDamageModule().isCaptureCut();
        }
    }
    return false;
}

StageObject* soLinkModuleImpl::searchParentAttr(soLinkConnection::Attribute attribute) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << attribute)) {
            return node->getLinkModule()->getOwner();
        }
    }
    return NULL;
}

bool soLinkModuleImpl::chkLinkStop() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
                return node->getLinkModule()->m_moduleAccesser->getStopModule().isStop();
            }
        }
    }
    return false;
}

void soLinkModuleImpl::toLinkStop() {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
            node->getLinkModule()->m_moduleAccesser->getStopModule().setLinkStop(1);
        }
    }
}

void soLinkModuleImpl::leaveLinkStop() {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
            node->getLinkModule()->m_moduleAccesser->getStopModule().setLinkStop(0);
        }
    }
}

bool soLinkModuleImpl::isValidLinkStop() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Stop)) {
                return true;
            }
        }
    }
    return false;
}

soColorBlend* soLinkModuleImpl::getLinkColorBlend() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Color_Blend)) {
                return node->getLinkModule()->m_moduleAccesser->getColorBlendModule().getColorBlend();
            }
        }
    }
    return NULL;
}

s8 soLinkModuleImpl::getLinkLightSet() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Light_Set)) {
                return node->getLinkModule()->m_moduleAccesser->getColorBlendModule().getLightSet();
            }
        }
    }
    return 0;
}

bool soLinkModuleImpl::chkLinkVisibility() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Visibility)) {
                return node->getLinkModule()->m_moduleAccesser->getVisibilityModule().isVisible();
            }
        }
    }
    return true;
}

soController* soLinkModuleImpl::getLinkController() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Unknown_4)) {
                return node->getLinkModule()->m_moduleAccesser->getControllerModule().getController();
            }
        }
    }
    return &g_soControllerNull;
}

float soLinkModuleImpl::getLinkScale() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Scale)) {
                return node->getLinkModule()->m_moduleAccesser->getPostureModule().getScale();
            }
        }
    }
    return 1.0f;
}

void soLinkModuleImpl::adjustLinkGroundCollision(int unk1, float* unk2) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Ground_Collision)) {
            node->getLinkModule()->getOwner()->adjustParentGroundCollision(unk1, unk2);
        }
    }
}

Vec3f soLinkModuleImpl::getParentModelNodeGlobalPosition(u32 index, u32 correctNodeId, bool p3) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(correctNodeId, p3);
}

Vec3f soLinkModuleImpl::getParentModelNodeGlobalPosition(u32 index, const char* nodeName, bool p3) {
    soLinkConnection& connection = m_connections->at(index);
    int nodeId = connection.m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeId(nodeName);
    return connection.m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeId, p3);
}

Vec3f soLinkModuleImpl::getParentRot(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getPostureModule().getRot(0);
}

nw4r::g3d::ResFile soLinkModuleImpl::getParentAnmFile(u32 index, u16 fileIndex) {
    return m_connections->at(index).m_parent.getLinkModule()->getAnmFile(fileIndex);
}

bool soLinkModuleImpl::isParentEnableEnvMap(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().isEnvMap();
}

void soLinkModuleImpl::sendEnvMapNodes(bool enable) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Env_Map)) {
            node->getLinkModule()->m_moduleAccesser->getModelModule().switchEnvMap(enable);
        }
    }
}

bool soLinkModuleImpl::isParentEnableSubColor(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getColorBlendModule().isEnableSubColor();
}

nw4r::g3d::ResFile soLinkModuleImpl::getAnmFile(u16 fileIndex) {
    if (m_anmFileTransactorLinkNo != -1) {
        return getParentAnmFile(m_anmFileTransactorLinkNo, fileIndex);
    }
    return m_moduleAccesser->getResourceModule().getAnmFile(fileIndex, 0xFFFF, 0);
}

bool soLinkModuleImpl::getLinkSlopeRotation(int index, Vec3f* rotation) {
    rotation->m_x = rotation->m_y = rotation->m_z = 0.0f;
    if (index >= 0) {
        return getNodeRotation(index, 0, rotation);
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Slope)) {
                Vec3f rot = node->m_connection->m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalRotation(0, false);
                rotation->m_x = rot.m_x;
                rotation->m_y = rot.m_y;
                rotation->m_z = rot.m_z;
                return true;
            }
        }
    }
    return false;
}

float soLinkModuleImpl::getConstraintModelScale() {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Model_Scale)) {
                return node->getLinkModule()->m_moduleAccesser->getModelModule().getScale();
            }
        }
    }
    return 1.0f;
}

void soLinkModuleImpl::setNodeScalePowerMul(float mul) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Scale_Power_Mul)) {
            node->getLinkModule()->m_moduleAccesser->getParamCustomizeModule().setAttackPowerMulScale(mul);
        }
    }
}

soArticle* soLinkModuleImpl::generateArticleLinkParents(u32 connectionIndex, int id, soModuleAccesser* moduleAccesser) {
    return m_connections->at(connectionIndex).m_parent.getLinkModule()->m_moduleAccesser->getGenerateArticleManageModule().generate(id, moduleAccesser, false);
}

bool soLinkModuleImpl::isValidParentShape(u32 index) {
    return !m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().isNull();
}

Vec2f soLinkModuleImpl::getParentShapeCenterPos(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().getCenterPos(0);
}

bool soLinkModuleImpl::isNodeGroundCrush(float p1) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_nodes.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Ground_Crush)) {
            if (node->getLinkModule()->m_moduleAccesser->getGroundModule().isCrush(p1, 0) == true) {
                return true;
            }
        }
    }
    return false;
}

bool soLinkModuleImpl::getParentShapeCenterPosTaskId(int taskId, Vec2f* outCenterPos) {
    for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
        soLinkNode& parent = node->m_connection->m_parent;
        if (taskId == parent.m_taskId) {
            *outCenterPos = parent.getLinkModule()->m_moduleAccesser->getGroundModule().getCenterPos(0);
            return true;
        }
        if (parent.getLinkModule()->m_moduleAccesser->getLinkModule().getParentShapeCenterPosTaskId(taskId, outCenterPos) == true) {
            return true;
        }
    }
    return false;
}

// HYPOTHESIS: glow module slots past isGlow, named from the soLinkModuleImpl callers.
class soGlowModuleLinkView : public soGlowModule {
public:
    virtual int getWeaponGlowDefense();    // 0x24
    virtual int unk28();
    virtual int unk2C();
    virtual float getWeaponGlowEff();      // 0x30
    virtual float getWeaponGlowDamageMin(); // 0x34
    virtual float getWeaponGlowDamageMax(); // 0x38
    virtual int unk3C();
    virtual int unk40();
    virtual int unk44();
    virtual int unk48();
    virtual int unk4C();
    virtual int unk50();
    virtual int unk54();
    virtual int getGlowAttackAdd(soCollisionAttackData* data); // 0x58
    virtual int getGlowAttackData(soCollisionAttackData* data); // 0x5C
    virtual int unk60();
    virtual int getWeaponGlowAttack();     // 0x64
};

#define SO_PARENT_GLOW(index) (*reinterpret_cast<soGlowModuleLinkView*>(m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->m_enumerationStart->m_glowModule))

bool soLinkModuleImpl::isParentGlow(u32 index) {
    return SO_PARENT_GLOW(index).isGlow();
}

int soLinkModuleImpl::getParentWeaponGlowDefense(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowDefense();
}

int soLinkModuleImpl::getParentGlowAttack(u32 index, soCollisionAttackData* data) {
    return SO_PARENT_GLOW(index).getGlowAttackData(data);
}

int soLinkModuleImpl::getParentWeaponGlowAttack(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowAttack();
}

int soLinkModuleImpl::getParentGlowAttackAdd(soCollisionAttackData* data) {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Power_Up)) {
                return reinterpret_cast<soGlowModuleLinkView*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_glowModule)->getGlowAttackAdd(data);
            }
        }
    }
    return 0;
}

float soLinkModuleImpl::getParentWeaponGlowEff(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowEff();
}

float soLinkModuleImpl::getParentWeaponGlowDamageMin(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowDamageMin();
}

float soLinkModuleImpl::getParentWeaponGlowDamageMax(u32 index) {
    return SO_PARENT_GLOW(index).getWeaponGlowDamageMax();
}

Vec2f soLinkModuleImpl::getParentSumSpeed(u32 index) {
    soLinkConnection& connection = m_connections->at(index);
    soKineticEnergy::AttributeFlag flag(1);
    return connection.m_parent.getLinkModule()->m_moduleAccesser->getKineticModule().getSumSpeed(flag);
}

int soLinkModuleImpl::getParentTexResourceId(u32 index) {
    return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getResourceModule().getResourceIdAccesser()->getTexResId();
}

bool soLinkModuleImpl::chkLinkShake(Vec2f* offset) {
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Shake)) {
                return reinterpret_cast<soShakeModule*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_shakeModule)->getOffset(offset);
            }
        }
    }
    return false;
}

void soLinkModuleImpl::setParentHitStopFrame(u32 index, u32 frame, bool unk) {
    m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getStopModule().setHitStop(static_cast<s32>(frame), unk);
}

void soLinkModuleImpl::setParentAttackerInfo(u32 index, StageObject* attacker) {
    soLinkConnection& connection = m_connections->at(index);
    soDamageAttackerInfo info;
    info.clear();
    connection.m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().getAttackerInfo(&info);
    info.set(connection.m_parent.m_taskId, attacker);
    connection.m_parent.getLinkModule()->m_moduleAccesser->getDamageModule().restoreAttackerInfo(&info);
}

bool soLinkModuleImpl::chkAttribute(int index, soLinkConnection::Attribute attribute) {
    if (index >= 0) {
        u32 bit = 1 << attribute;
        u32 attr = m_connections->at(index).m_attribute;
        return (attr & bit) != 0;
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << attribute)) {
                return true;
            }
        }
    }
    return false;
}

soSlopeStatusParam::Status soLinkModuleImpl::getLinkSlopeStatus(int index) {
    if (index >= 0) {
        return static_cast<soSlopeModule*>(m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->m_enumerationStart->m_slopeModule)->getStatus();
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << soLinkConnection::Attribute_Reference_Parent_Slope)) {
                return static_cast<soSlopeModule*>(node->getLinkModule()->m_moduleAccesser->m_enumerationStart->m_slopeModule)->getStatus();
            }
        }
    }
    return static_cast<soSlopeStatusParam::Status>(0);
}

Vec3f soLinkModuleImpl::getModelConstraintNodeGlobalPosition(u32 nodeIndex) {
    return m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeIndex, false);
}

void soLinkModuleImpl::getModelConstraintTargetNodeWorldMtx(Matrix* mtx, bool unk) {
    m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeGlobalMtx(m_modelConstraintTargetNode, mtx, unk);
}

// HYPOTHESIS: attribute 0xD is the slow-motion reference (no other query tests it).
bool soLinkModuleImpl::isLinkParentSlow(int index) {
    if (index >= 0) {
        return m_connections->at(index).m_parent.getLinkModule()->m_moduleAccesser->getSlowModule().isSlow();
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << 13)) {
                return node->getLinkModule()->m_moduleAccesser->getSlowModule().isSlow();
            }
        }
    }
    return false;
}

bool soLinkModuleImpl::getLinkParentSlowInfo(float* rate, bool* isSkip, int index) {
    if (index >= 0) {
        soLinkConnection& connection = m_connections->at(index);
        *rate = connection.m_parent.getLinkModule()->m_moduleAccesser->getSlowModule().getRate();
        *isSkip = connection.m_parent.getLinkModule()->m_moduleAccesser->getSlowModule().isSkip();
        return true;
    }
    if (isLink(-1)) {
        for (soLinkNode* node = static_cast<soLinkNode*>(m_parents.getNode(0)); node != NULL; node = static_cast<soLinkNode*>(node->m_next)) {
            if (node->m_connection->m_attribute & (1 << 13)) {
                *rate = node->getLinkModule()->m_moduleAccesser->getSlowModule().getRate();
                *isSkip = node->getLinkModule()->m_moduleAccesser->getSlowModule().isSkip();
                return true;
            }
        }
    }
    return false;
}

Vec3f soLinkModuleImpl::getModelConstraintNodeGlobalPositionRecursive(u32 nodeIndex) {
    soModuleAccesser* accesser = m_connections->at(m_modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser;
    soLinkModule& link = accesser->getLinkModule();
    if (link.isModelConstraint() == true && !link.isModelConstraintMutual()) {
        return link.getModelConstraintNodeGlobalPositionRecursive(nodeIndex);
    }
    return accesser->getModelModule().getNodeGlobalPosition(nodeIndex, false);
}

void soLinkModuleImpl::removeModelConstraint(bool resetParent) {
    if (!static_cast<soLinkModule*>(this)->isModelConstraint()) {
        return;
    }
    soLinkConnection& connection = m_connections->at(m_modelConstraintNo);
    static_cast<soLinkModule*>(this)->setModelConstraintAttribute(m_modelConstraintNo, false);
    m_modelConstraintNo = -1;
    m_modelConstraintNode = -1;
    m_modelConstraintTargetNode = -1;
    m_modelConstraintFlag.m_mask = 0;
    m_isModelConstraintMutual = false;
    if (resetParent == true) {
        if (!connection.m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().isNull()) {
            Vec2f center = connection.m_parent.getLinkModule()->m_moduleAccesser->getGroundModule().getCenterPos(0);
            Vec3f pos;
            pos.m_x = center.m_x;
            pos.m_y = center.m_y;
            pos.m_z = 0.0f;
            m_moduleAccesser->getGroundModule().relocate(reinterpret_cast<Vec2f*>(&pos), 0);
        }
    }
    m_moduleAccesser->getModelModule().clearSetUpMtx();
}

void soLinkModuleImpl::setModelConstraintAttribute(u32 index, bool enable) {
    soLinkConnection& connection = m_connections->at(index);
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Stop;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Stop);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Shake;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Shake);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Color_Blend;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Color_Blend);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << soLinkConnection::Attribute_Reference_Parent_Visibility;
    } else {
        connection.m_attribute &= ~(1 << soLinkConnection::Attribute_Reference_Parent_Visibility);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << 13;
    } else {
        connection.m_attribute &= ~(1 << 13);
    }
    if (enable == true) {
        connection.m_attribute |= 1 << 14;
    } else {
        connection.m_attribute &= ~(1 << 14);
    }
}

void soLinkModuleImpl::setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, int modelConstraintTargetNode, u16 modelConstraintFlag, bool modelConstraintAttribute) {
    soLinkConnection& connection = m_connections->at(modelConstraintNo);
    int correctedNode = m_moduleAccesser->getModelModule().getCorrectNodeId(modelConstraintNode);
    m_moduleAccesser->getModelModule().getNodeGlobalPosition(correctedNode, false);
    m_moduleAccesser->getModelModule().clearSetUpMtx();
    soLinkModuleImpl* parentImpl = connection.m_parent.getLinkModule();
    soLinkModule* parent = parentImpl;
    m_modelConstraintNo = modelConstraintNo;
    m_modelConstraintNode = correctedNode;
    m_modelConstraintTargetNode = parentImpl->m_moduleAccesser->getModelModule().getCorrectNodeId(modelConstraintTargetNode);
    m_modelConstraintFlag.m_mask = modelConstraintFlag;
    if (parent->isModelConstraint() == true && parent->getParentTaskId(parent->getModelConstraintNo()) == getOwner()->m_taskId) {
        m_isModelConstraintMutual = true;
    } else {
        m_isModelConstraintMutual = false;
    }
    static_cast<soLinkModule*>(this)->setModelConstraintAttribute(modelConstraintNo, modelConstraintAttribute);
}

void soLinkModuleImpl::setModelConstraintPosOrt(u32 modelConstraintNo, int modelConstraintNode, const char* modelConstraintTargetNode, u16 modelConstraintFlag, bool modelConstraintAttribute) {
    u32 nodeId = m_connections->at(modelConstraintNo).m_parent.getLinkModule()->m_moduleAccesser->getModelModule().getNodeId(modelConstraintTargetNode);
    if (nodeId != 0xFFFFFFFF) {
        static_cast<soLinkModule*>(this)->setModelConstraintPosOrt(modelConstraintNo, modelConstraintNode, nodeId, modelConstraintFlag, modelConstraintAttribute);
    }
}
