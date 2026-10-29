// Local shadow: canonical full scalar visibility set argument; the pure soVisibilityModule interface and the
// non-virtual destructor follow the so_visibility_module_simple.o vtables.
#pragma once

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_common_data_accesser.h>
#include <types.h>

class soModuleAccesser;

class soVisibilityModule {
public:
    virtual void activate() = 0;
    virtual void setData(soVisibilityData*) = 0;
    virtual void set(u32, int) = 0;
    virtual void setStatusDefault(u32, int, bool) = 0;
    virtual void setDefaultAll() = 0;
    virtual void update() = 0;
    virtual void hideAll() = 0;
    virtual void setWhole(u8) = 0;
    virtual u8 getWhole() = 0;
    virtual bool isVisible() = 0;
    virtual int get(u32) = 0;
};

class soVisibilityModuleSimple : public soVisibilityModule, public soAnimCmdEventObserver {
protected:
    u8 m_whole;
    char _0xe[1];
    soModuleAccesser* m_moduleAccesser;

public:
    soVisibilityModuleSimple(soModuleAccesser* acc, soVisibilityData* data, int unk);
    ~soVisibilityModuleSimple();
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

    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);
};
static_assert(sizeof(soVisibilityModuleSimple) == 0x14, "Class is wrong size!");
