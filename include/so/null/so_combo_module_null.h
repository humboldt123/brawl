#pragma once

#include <ft/ft_combo_module_impl.h>

class soComboModuleNull : public soComboModule {
public:
    virtual ~soComboModuleNull();
    virtual void activate();
    virtual void set(int kind);
    virtual void update();
    virtual void clearSetting();
    virtual int getCount();
};
