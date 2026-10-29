#pragma once

#include <StaticAssert.h>
#include <ef/ef_id.h>
#include <types.h>

class soModuleAccesser;

// One timed effect slot of soEffectModuleImpl: the effect handle is ended when the frame count runs out.
// The array units use the plain layout of so/templates/so_array_value_soEffectTime.h instead.
class soEffectTime {
public:
    EfID m_effectId;
    int m_frame;
    u32 m_handle;

    soEffectTime();
    ~soEffectTime();
    // HYPOTHESIS: parameter types from the callers soEffectModuleImpl::reqTime/reqTimeFollow
    void set(EfID effectId, u32 handle, int frame);
    void end();
    bool update(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soEffectTime) == 0xC, "Class is wrong size!");
