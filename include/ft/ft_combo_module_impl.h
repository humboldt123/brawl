#pragma once

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <types.h>

class soModuleAccesser;

// HYPOTHESIS: the order of the five pure virtuals follows ftComboModuleImpl's vtable and the sora_melee map names.
class soComboModule {
public:
    virtual ~soComboModule() { }
    virtual void activate() = 0;
    virtual void set(int kind) = 0;
    virtual void update() = 0;
    virtual void clearSetting() = 0;
    virtual int getCount() = 0;
};
static_assert(sizeof(soComboModule) == 4, "Class is wrong size!");

class ftComboModuleImpl : public soComboModule, public soStatusEventObserver, public soAnimCmdEventObserver {
public:
    soModuleAccesser* m_moduleAccesser; // 0x1C
    int m_kind;                         // 0x20
    int m_count;                        // 0x24
    int m_frame;                        // 0x28 HYPOTHESIS: frames left in the combo window
    int m_statusKind;                   // 0x2C

    ftComboModuleImpl(soModuleAccesser* moduleAccesser);
    virtual ~ftComboModuleImpl();
    virtual void activate();
    virtual void set(int kind);
    virtual void update();
    virtual void clearSetting();
    virtual int getCount();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual bool isObserv(char unk1);
};
static_assert(sizeof(ftComboModuleImpl) == 0x30, "Class is wrong size!");
