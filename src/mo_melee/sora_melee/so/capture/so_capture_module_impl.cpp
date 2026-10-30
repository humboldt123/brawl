#define SO_CAPTURE_MODULE_IMPL_FULL
#include <so/capture/so_capture_module_impl.h>
#include <so/catch/so_catch_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

soCaptureModule::~soCaptureModule() { }

soCaptureModuleImpl::~soCaptureModuleImpl() { }

void soCaptureModuleImpl::activate() {
    initInfo();
    m_sendCutEvent = true;
    unk2D = true;
    m_isThrown = false;
    unk31 = false;
    m_isReaction = false;
    unk28 = 0.0f;
}

void soCaptureModuleImpl::deactivate() { }

void soCaptureModuleImpl::setNodes(int node1, int node2, float unk) {
    m_node1 = node1;
    m_node2 = node2;
    unk20 = unk;
}

void soCaptureModuleImpl::initInfo() {
    m_isReaction = false;
}

bool soCaptureModuleImpl::isCapture() {
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == false) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return true;
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return true;
        }
    }
    return false;
}

int soCaptureModuleImpl::getCatchTaskId() {
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == false) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return m_moduleAccesser->getLinkModule().getNodeTaskId(0);
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return m_moduleAccesser->getLinkModule().getParentTaskId(0);
        }
    }
    return -1;
}

float soCaptureModuleImpl::getCatchPosY() {
    return m_moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(0, m_catchNode, true).m_y;
}

bool soCaptureModuleImpl::isIgnoreCatching() {
    return m_ignoreCatching;
}

void soCaptureModuleImpl::setIgnoreCatching(bool ignoreCatching) {
    m_ignoreCatching = ignoreCatching;
}

int soCaptureModuleImpl::getMotionOffset() {
    return m_motionOffset;
}

bool soCaptureModuleImpl::isReaction() {
    return m_isReaction;
}

void soCaptureModuleImpl::setUpdatePos(bool updatePos) {
    m_updatePos = updatePos;
}

void soCaptureModuleImpl::setCatchNode(int node) {
    m_catchNode = node;
}
