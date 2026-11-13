#include <ft/lucario/ft_lucario_param_customize_module.h>
#include <so/so_module_accesser.h>

ftLucarioParamCustomizeModule::ftLucarioParamCustomizeModule(soModuleAccesser* acc) : ftParamCustomizeModuleImpl(acc) { }

ftLucarioParamCustomizeModule::~ftLucarioParamCustomizeModule() { }

// Aura: attacks of element 0x15 are scaled by the aura multiplier Lucario keeps in work float 0x11000013.
// HYPOTHESIS: the argument is the attack's data; the low five bits of its word at +0x30 are the element.
float ftLucarioParamCustomizeModule::getAttackPowerMul(void* attack) {
    float mul = 1.0f;
    if ((*reinterpret_cast<u32*>(static_cast<u8*>(attack) + 0x30) & 0x1F) == 0x15) {
        mul = m_moduleAccsr->getWorkManageModule().getFloat(0x11000013);
    }
    return mul * ftParamCustomizeModuleImpl::getAttackPowerMul(attack);
}
