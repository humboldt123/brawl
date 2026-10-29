// SHADOW of BrawlHeaders/so/visibility/so_visibility_module_impl.h: adds the sora_melee constructor/destructor declarations
// (fighter module builders without FT_REL_LINK_EXTERN keep the implicit inline destructor) and the dirty flag.
#pragma once

#include <StaticAssert.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/status/so_status_event_presenter.h>
#include <types.h>

class soVisibilityModuleImpl : public soVisibilityModuleSimple, public soStatusEventObserver {
public:
    soVisibilityModuleImpl(soModuleAccesser* acc, soVisibilityData* data, int n);
#if !defined(FT_MODULE_BUILDER) || defined(FT_REL_LINK_EXTERN)
    ~soVisibilityModuleImpl(); // MATCH-ONLY (fighter RELs): the destructor is a sora_melee function (not an inline weak copy)
#endif
#ifdef FT_MODULE_BUILDER
private:
#endif
    char _0x18[2];
    soVisibilityData* m_data;
    s8* m_setData;
    u32 m_numSets;
    // HYPOTHESIS: set when a set or status default changed, so setDefaultAll re-applies the status defaults.
    u8 unk2C;
    char _0x2d[3];

public:
    virtual void activate();
    virtual void setData(soVisibilityData*);
    virtual void set(u32, int);
    virtual void setStatusDefault(u32, int, bool);
    virtual void setDefaultAll();
    virtual void update();
    virtual void hideAll();
    virtual int get(u32);

    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soVisibilityModuleImpl) == 0x30, "Class is wrong size!");