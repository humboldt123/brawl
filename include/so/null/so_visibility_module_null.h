#pragma once

#include <so/visibility/so_visibility_module_simple.h>

class soVisibilityModuleNull : public soVisibilityModule {
public:
    virtual ~soVisibilityModuleNull();
    virtual void activate();
    virtual void setData(soVisibilityData*);
    virtual void set(u32, int);
    virtual void setStatusDefault(u32, int, bool);
    virtual void setDefaultAll();
    virtual void update();
    virtual void hideAll();
    virtual void setWhole(u8);
    virtual u8 getWhole();
    virtual bool isVisible();
    virtual int get(u32);
};
