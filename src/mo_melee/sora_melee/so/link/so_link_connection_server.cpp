#define SO_LINK_MODULE_IMPL_FULL
#include <so/link/so_link_connection_server.h>
#include <types.h>

soLinkModuleImpl* soLinkNode::getLinkModule() {
    return m_linkModule;
}

void soLinkConnection::reset() {
    m_node.m_taskId = 0;
    m_node.m_linkModule = NULL;
    m_node.m_connection = this;
    m_parent.m_taskId = 0;
    m_parent.m_linkModule = NULL;
    m_parent.m_connection = this;
    m_attribute = 0;
    m_isLinked = false;
}

soLinkConnectionServer::~soLinkConnectionServer() { }

void soLinkConnectionServer::entry(utListNode* node) {
    if (isExistNode(node) == -1) {
        addTail(node);
    }
}

void soLinkConnectionServer::exit(utListNode* node) {
    if (isExistNode(node) != -1) {
        remove(node);
    }
}

soLinkConnectionServer lbl_27_bss_110;
