#include <ft/ft_param_customize_module_impl.h>
#include <ft/ft_util.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <ft/ft_common_data_accesser.h>

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

void ftParamCustomizeModuleImpl::applyRabbitCap() {
    float* scale = g_ftCommonData.dataCommon->rabbitCapScale;
    for (int i = 0; i < 2; i++) {
        m_paramFloat[i][4] *= scale[1];
        m_paramFloat[i][5] *= scale[2];
        m_paramFloat[i][6] *= scale[3];
        m_paramFloat[i][7] *= scale[4];
        m_paramFloat[i][11] *= scale[5];
        m_paramFloat[i][12] *= scale[6];
        m_paramFloat[i][15] *= scale[7];
        m_paramFloat[i][14] *= scale[8];
        m_paramFloat[i][22] *= scale[9];
        m_paramFloat[i][23] *= scale[10];
        m_paramFloat[i][30] *= scale[11];
        m_paramFloat[i][56] *= scale[12];
        m_paramFloat[i][57] *= scale[13];
        m_paramFloat[i][81] *= scale[14];
        m_paramFloat[i][82] *= scale[15];
    }
    m_walkSpeedMul = scale[0] * getWalkSpeedMul();
}

void ftParamCustomizeModuleImpl::applyMetal() {
    float* scale = g_ftCommonData.dataCommon->metalScale;
    for (int i = 0; i < 2; i++) {
        m_paramFloat[i][12] *= scale[1];
        m_paramFloat[i][15] *= scale[2];
        m_paramFloat[i][22] *= scale[3];
        m_paramFloat[i][23] *= scale[4];
        m_paramFloat[i][30] *= scale[5];
        m_paramFloat[i][40] *= scale[6];
        m_paramFloat[i][57] *= scale[7];
        m_paramFloat[i][82] *= scale[8];
    }
    m_walkSpeedMul = scale[0] * getWalkSpeedMul();
}

void ftParamCustomizeModuleImpl::applyThunder(float factor) {
    float* scale = g_ftCommonData.dataCommon->sizeCustomizeScale[1];
    for (int variation = 0; variation < 2; variation++) {
        {
            float& parameter = m_paramFloat[variation][4];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[4]);
        }
        {
            float& parameter = m_paramFloat[variation][7];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[5]);
        }
        {
            s32& parameter = m_paramInt[variation][1];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[6]);
        }
        {
            float& parameter = m_paramFloat[variation][12];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[7]);
        }
        {
            float& parameter = m_paramFloat[variation][15];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[8]);
        }
        {
            float& parameter = m_paramFloat[variation][22];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[9]);
        }
        {
            float& parameter = m_paramFloat[variation][23];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[10]);
        }
        {
            float& parameter = m_paramFloat[variation][26];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[11]);
        }
        {
            float& parameter = m_paramFloat[variation][27];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[12]);
        }
        {
            float& parameter = m_paramFloat[variation][28];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[13]);
        }
        {
            float& parameter = m_paramFloat[variation][30];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[14]);
        }
        {
            float& parameter = m_paramFloat[variation][40];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[15]);
        }
        {
            float& parameter = m_paramFloat[variation][46];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[16]);
        }
        {
            float& parameter = m_paramFloat[variation][56];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[17]);
        }
        {
            float& parameter = m_paramFloat[variation][57];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[18]);
        }
        {
            float& parameter = m_paramFloat[variation][64];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[19]);
        }
        {
            float& parameter = m_paramFloat[variation][65];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[20]);
        }
        {
            float& parameter = m_paramFloat[variation][94];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[21]);
        }
        {
            s32& parameter = m_paramInt[variation][11];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[22]);
        }
        {
            float& parameter = m_paramFloat[variation][72];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[23]);
        }
        {
            float& parameter = m_paramFloat[variation][73];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[24]);
        }
        {
            float& parameter = m_paramFloat[variation][74];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[25]);
        }
        {
            float& parameter = m_paramFloat[variation][75];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[26]);
        }
        {
            float& parameter = m_paramFloat[variation][76];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[27]);
        }
        {
            float& parameter = m_paramFloat[variation][77];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[28]);
        }
        {
            float& parameter = m_paramFloat[variation][78];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[29]);
        }
        {
            float& parameter = m_paramFloat[variation][110];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[30]);
        }
        {
            float& parameter = m_paramFloat[variation][111];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[31]);
        }
    }
    setAttackPowerMulScale(ftUtil::calcScalingParam(1.0f, factor, scale[1]));
}

void ftParamCustomizeModuleImpl::applyKinoko(float factor) {
    float* scale = g_ftCommonData.dataCommon->sizeCustomizeScale[0];
    for (int variation = 0; variation < 2; variation++) {
        {
            float& parameter = m_paramFloat[variation][4];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[4]);
        }
        {
            float& parameter = m_paramFloat[variation][7];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[5]);
        }
        {
            s32& parameter = m_paramInt[variation][1];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[6]);
        }
        {
            float& parameter = m_paramFloat[variation][12];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[7]);
        }
        {
            float& parameter = m_paramFloat[variation][15];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[8]);
        }
        {
            float& parameter = m_paramFloat[variation][22];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[9]);
        }
        {
            float& parameter = m_paramFloat[variation][23];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[10]);
        }
        {
            float& parameter = m_paramFloat[variation][26];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[11]);
        }
        {
            float& parameter = m_paramFloat[variation][27];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[12]);
        }
        {
            float& parameter = m_paramFloat[variation][28];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[13]);
        }
        {
            float& parameter = m_paramFloat[variation][30];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[14]);
        }
        {
            float& parameter = m_paramFloat[variation][40];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[15]);
        }
        {
            float& parameter = m_paramFloat[variation][46];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[16]);
        }
        {
            float& parameter = m_paramFloat[variation][56];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[17]);
        }
        {
            float& parameter = m_paramFloat[variation][57];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[18]);
        }
        {
            float& parameter = m_paramFloat[variation][64];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[19]);
        }
        {
            float& parameter = m_paramFloat[variation][65];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[20]);
        }
        {
            float& parameter = m_paramFloat[variation][94];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[21]);
        }
        {
            s32& parameter = m_paramInt[variation][11];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[22]);
        }
        {
            float& parameter = m_paramFloat[variation][72];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[23]);
        }
        {
            float& parameter = m_paramFloat[variation][73];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[24]);
        }
        {
            float& parameter = m_paramFloat[variation][74];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[25]);
        }
        {
            float& parameter = m_paramFloat[variation][75];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[26]);
        }
        {
            float& parameter = m_paramFloat[variation][76];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[27]);
        }
        {
            float& parameter = m_paramFloat[variation][77];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[28]);
        }
        {
            float& parameter = m_paramFloat[variation][78];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[29]);
        }
        {
            float& parameter = m_paramFloat[variation][110];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[30]);
        }
        {
            float& parameter = m_paramFloat[variation][111];
            parameter = ftUtil::calcScalingParam(parameter, factor, scale[31]);
        }
    }
    setAttackPowerMulScale(ftUtil::calcScalingParam(1.0f, factor, scale[1]));
    float walkSpeedScale = ftUtil::calcScalingParam(1.0f, factor, scale[3]);
    float walkSpeed = getWalkSpeedMul();
    m_walkSpeedMul = walkSpeed * walkSpeedScale;
}
