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
        removeModelConstraint(true);
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
