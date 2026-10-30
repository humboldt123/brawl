#include <so/reflect/so_reflect_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

soReflectModule::~soReflectModule() { }

soReflectModuleImpl::~soReflectModuleImpl() { }

void soReflectModuleImpl::activate() {
    resetInfo();
}

void soReflectModuleImpl::initInfo() {
    m_taskId = -1;
    unk3C = 0;
}

void soReflectModuleImpl::resetInfo() {
    m_taskId = -1;
    m_attackMul = 1.0f;
    m_speedMul = 1.0f;
    m_lifeMul = 0.0f;
    m_count = 0;
    m_noSpeedMul = false;
}

bool soReflectModuleImpl::isReflect() {
    return m_taskId != -1;
}

void soReflectModuleImpl::setNoSpeedMul(bool noSpeedMul) {
    m_noSpeedMul = noSpeedMul;
}

float soReflectModuleImpl::getLifeMul() {
    return m_lifeMul;
}

float soReflectModuleImpl::getSpeedMul() {
    return m_speedMul;
}

void soReflectModuleImpl::setAttackMul(float attackMul) {
    m_attackMul = attackMul;
}

float soReflectModuleImpl::getAttackMul() {
    return m_attackMul;
}

int soReflectModuleImpl::getTeam() {
    return m_team;
}

int soReflectModuleImpl::getTaskId() {
    return m_taskId;
}

void soReflectModuleImpl::deactivate() { }
