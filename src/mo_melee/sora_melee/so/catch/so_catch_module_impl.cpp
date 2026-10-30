#include <so/catch/so_catch_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

soCatchModule::~soCatchModule() { }

void soCatchModuleImpl::activate() {
    m_sendCutEvent = true;
    m_catchFlag = false;
}

void soCatchModuleImpl::deactivate() { }

void soCatchModuleImpl::setNodes(int node1, int node2, float unk) {
    m_node1 = node1;
    m_node2 = node2;
    unk1C = unk;
}

void soCatchModuleImpl::initInfo() {
    unk54 = 3.4028235e38f;
}

bool soCatchModuleImpl::isCatch() {
    if (m_catchFlag == true) {
        if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
            return true;
        }
        if (m_moduleAccesser->getLinkModule().isLink(0) == true) {
            return true;
        }
    }
    return false;
}

void soCatchModuleImpl::onCatchFlag() {
    m_catchFlag = true;
}

void soCatchModuleImpl::offCatchFlag() {
    m_catchFlag = false;
}

void soCatchModuleImpl::setSendCutEvent(bool sendCutEvent) {
    m_sendCutEvent = sendCutEvent;
}
