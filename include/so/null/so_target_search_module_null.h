#pragma once

#include <so/so_null.h>
#include <types.h>

// HYPOTHESIS: parameter lists of unreferenced slots.
class soTargetSearchModule : public soNullable {
public:
    virtual ~soTargetSearchModule();
    virtual void createTargetSearchUnitAll() = 0;
    virtual void initInterval() = 0;
    virtual void resetIntervalCount() = 0;
    virtual void activate() = 0;
    virtual bool isFoundTarget() = 0;
    virtual bool isLostTarget() = 0;
    virtual void update() = 0;
    virtual void rayCheckCacheInit() = 0;
    virtual bool rayCheckCacheCheck() = 0;
    virtual void rayCheckCacheSet() = 0;
};

class soTargetSearchModuleNull : public soTargetSearchModule {
public:
    virtual ~soTargetSearchModuleNull();
    virtual void createTargetSearchUnitAll();
    virtual void initInterval();
    virtual void resetIntervalCount();
    virtual void activate();
    virtual bool isFoundTarget();
    virtual bool isLostTarget();
    virtual void update();
    virtual void rayCheckCacheInit();
    virtual bool rayCheckCacheCheck();
    virtual void rayCheckCacheSet();
};
