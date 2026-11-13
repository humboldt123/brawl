#pragma once
#include <so/status/so_status_module_impl.h>

struct stWaterData;
struct Vec2f;

// The native full RTTI chain is FinalSwim -> soStatusUniqProcess.
// FinalSwim also supplies the inherited position-fix callback.
class ftGameWatchStatusUniqProcessFinalSwim : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalSwim() { }
    // HYPOTHESIS: the water record argument of the three helpers (names from the map) is never written or read.
    bool getWaterAreaInfo(soModuleAccesser* moduleAccesser, stWaterData* water);
    float getWaterSurfaceOffsetPos(soModuleAccesser* moduleAccesser, stWaterData* water);
    bool calcWaterSurfacePos(soModuleAccesser* moduleAccesser, stWaterData* water, Vec2f* surface);
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};
