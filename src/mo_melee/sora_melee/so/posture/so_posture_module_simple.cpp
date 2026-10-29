#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <types.h>

soPostureModuleSimple::soPostureModuleSimple(soModuleAccesser* moduleAccesser, soArray<soInterpolation<Vec3f> >* interpolations, soEventObserverRegistrationDesc* registrationDesc)
    : m_moduleAccesser(moduleAccesser), m_baseScale(1.0f), m_lr(0.0f) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    m_pos = zero;
    m_prevPos = zero;
    _36 = zero;
    _48 = Vec3f(0.0f, 0.0f, 0.0f);
}

soPostureModuleSimple::~soPostureModuleSimple() { }

void soPostureModuleSimple::init(Vec3f* pos, float lr) {
    initPos(pos);
    setLr(lr);
    initScale();
}

void soPostureModuleSimple::begin() {
    m_prevPos = m_pos;
}

Vec3f soPostureModuleSimple::getPrevRoughPos() {
    return getPos();
}

void soPostureModuleSimple::turnBack() {
    m_pos = _36;
}

void soPostureModuleSimple::fixRot() { }

void soPostureModuleSimple::fixConstraintNodePosture(int, void*) { }

void soPostureModuleSimple::setSyncConstraintNode(int) { }

bool soPostureModuleSimple::setStickLR(float stickLR) {
    return false;
}

void soPostureModuleSimple::setOwnerScale(float ownerScale) { }

float soPostureModuleSimple::getOwnerScale() {
    return 1.0f;
}

void soPostureModuleSimple::reverseRotYLr() { }

void soPostureModuleSimple::updateRotYLr() { }

float soPostureModuleSimple::getRotYLr() {
    return 90.0f * m_lr;
}

bool soPostureModuleSimple::isRotYLrDifferentInnerLr() {
    return false;
}

void soPostureModuleSimple::initRotYLr() { }

void soPostureModuleSimple::setRot(Vec3f* rot, u32 index) { }

Vec3f soPostureModuleSimple::getRot(u32 index) {
    return Vec3f(0.0f, 0.0f, 0.0f);
}

void soPostureModuleSimple::initRot() { }

void soPostureModuleSimple::initRot(u32 index) { }

void soPostureModuleSimple::setPrevRoughPos(Vec3f* roughPos) { }
