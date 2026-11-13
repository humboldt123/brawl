#pragma once

#include <ft/ft_param_customize_module_impl.h>

// HYPOTHESIS: ftLucarioParamCustomizeModule is an ftParamCustomizeModuleImpl with the attack power overrides (the aura).
// Defined in ft_lucario_param_customize_module.cpp.
class ftLucarioParamCustomizeModule : public ftParamCustomizeModuleImpl {
public:
    ftLucarioParamCustomizeModule(soModuleAccesser* acc);
    virtual ~ftLucarioParamCustomizeModule();
    virtual float getAttackPowerMul(void* attack);
};
