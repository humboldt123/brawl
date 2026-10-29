#include <so/posture/so_posture_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <types.h>

void soPostureModuleImpl::initRot() {
    int count = m_interpolationArray->size();
    for (int i = 0; i < count; i++) {
        initRot(i);
    }
}

void soPostureModuleImpl::initRotYLr() {
    m_rotYLr = 0.0f;
    updateRotYLr();
}

void soPostureModuleImpl::updateRotYLr() {
    m_rotYLr = 90.0f * m_lr;
}

void soPostureModuleSimple::setLr(float lr) {
    m_lr = lr;
}

bool soPostureModuleImpl::setStickLR(float stickLR) {
    float stickX = m_moduleAccesser->getControllerModule().getStickX();
    float lr = getLr();
    if (stickX > stickLR) {
        setLr(1.0f);
    } else if (stickX < -stickLR) {
        setLr(-1.0f);
    }
    return lr != getLr();
}

float soPostureModuleSimple::getLr() {
    return m_lr;
}

void soPostureModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    updateRotYLr();
    m_syncConstraintNode = -1;
}

bool soPostureModuleImpl::isObserv(char unk1) {
    return unk1 == 5;
}

void soPostureModuleSimple::reverseLr() {
    setLr(-m_lr);
}

void soPostureModuleSimple::setScale(float scale) {
    m_baseScale = scale;
}

void soPostureModuleImpl::reverseRotYLr() {
    m_rotYLr = -m_rotYLr;
}

void soPostureModuleImpl::setSyncConstraintNode(int node) {
    m_syncConstraintNode = node;
}

void soPostureModuleSimple::initScale() {
    m_baseScale = 1.0f;
}

float soPostureModuleSimple::getBaseScale() {
    return m_baseScale;
}

void soPostureModuleImpl::setOwnerScale(float ownerScale) {
    m_ownerScale = ownerScale;
}

float soPostureModuleImpl::getOwnerScale() {
    return m_ownerScale;
}

float soPostureModuleImpl::getRotYLr() {
    return m_rotYLr;
}

