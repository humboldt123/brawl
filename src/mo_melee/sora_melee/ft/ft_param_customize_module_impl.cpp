#include <ft/ft_param_customize_module_impl.h>
#include <ft/ft_util.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

ftParamCustomizeModuleImpl::ftParamCustomizeModuleImpl(soModuleAccesser* accsr) :
    m_moduleAccsr(accsr), m_customizeBits(0), m_walkSpeedMul(1.0f) {
    reconstruct();
}

ftParamCustomizeModuleImpl::~ftParamCustomizeModuleImpl() { }

void ftParamCustomizeModuleImpl::activate() {
    soParamCustomizeModuleImpl::activate();
}

float ftParamCustomizeModuleImpl::getAttackPowerMul(void* p1) {
    return soParamCustomizeModuleImpl::getAttackPowerMul(p1) * ftUtil::getAttackPowerMul(m_moduleAccsr);
}

float ftParamCustomizeModuleImpl::getAttackPowerMulArticle() {
    return soValueAccesser::getConstantFloat(m_moduleAccsr, 0xDAE, 0);
}

float ftParamCustomizeModuleImpl::getAttackReactionMul() {
    return ftUtil::getAttackReactionMul(m_moduleAccsr);
}

void ftParamCustomizeModuleImpl::setDefault() {
    reconstruct();
    m_customizeBits = 0;
    float scale = m_moduleAccsr->getPostureModule().getBaseScale();
    if (1.0f != scale) {
        applyKinoko(scale);
        m_customizeBits |= 1;
    }
}

void ftParamCustomizeModuleImpl::addCustomize(u8 kind) {
    m_customizeBits |= 1 << kind;
    calc();
}

void ftParamCustomizeModuleImpl::removeCustomize(u8 kind) {
    u32 bit = 1 << kind;
    if (m_customizeBits & bit) {
        m_customizeBits &= ~bit;
        calc();
    }
}

void ftParamCustomizeModuleImpl::calc() {
    reconstruct();
    float scale = m_moduleAccsr->getPostureModule().getBaseScale();
    if (m_customizeBits & 1) {
        applyKinoko(scale);
    } else if (m_customizeBits & 2) {
        applyThunder(scale);
    } else if (1.0f != scale) {
        applyKinoko(scale);
        m_customizeBits |= 1;
    }
    if (m_customizeBits & 4) {
        applyRabbitCap();
    }
    if (m_customizeBits & 8) {
        applyMetal();
    }
    m_moduleAccsr->getLinkModule().setNodeScalePowerMul(getAttackPowerMulScale());
}

int ftParamCustomizeModuleImpl::getParamInt(u32 variation, u32 id) {
    return m_paramInt[variation][id - 23001];
}

float ftParamCustomizeModuleImpl::getParamFloat(u32 variation, u32 id) {
    return m_paramFloat[variation][id - 3001];
}

void* ftParamCustomizeModuleImpl::getParamIndefinite(u32 variation, u32 id) {
    return m_paramIndefinite[variation][id - 43001];
}
