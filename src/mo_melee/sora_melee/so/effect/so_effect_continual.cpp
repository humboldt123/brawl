#include <so/effect/so_effect_continual.h>
#include <so/so_module_accesser.h>
#include <types.h>

soEffectContinual::soEffectContinual() : m_isActive(false) { }

soEffectContinual::~soEffectContinual() { }

void soEffectContinual::active(EfID effectId, u32 nodeId, float f, bool b) {
    m_effectId = effectId;
    m_nodeId = nodeId;
    unk8 = f;
    unkC = 0.0f;
    unk2A = b;
    m_isActive = true;
    m_isFirst = true;
}

bool soEffectContinual::update(soModuleAccesser* moduleAccesser) {
    if (!m_isFirst) {
        m_prevPos = m_pos;
    }
    m_pos = moduleAccesser->getModelModule().getNodeGlobalPosition(m_nodeId, false);
    if (m_isFirst == true) {
        m_prevPos = m_pos;
        m_isFirst = false;
        return true;
    }
    return false;
}
