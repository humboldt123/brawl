#pragma once

#include <so/area/so_area_enviroment_element_checker_impl.h>

class soAreaEnviromentElementCheckerNull : public soAreaEnviromentElementChecker {
public:
    virtual ~soAreaEnviromentElementCheckerNull();
    virtual void check(soModuleAccesser*);
    virtual void clearWaterInfo();
    virtual void clear();
    virtual void enable();
    virtual void unable();
    virtual bool isEnable();
    virtual bool isWater();
    virtual bool isWaterAreaIndex(int);
    virtual int getWaterAreaId();
    virtual float getWaterSurfaceY();
    virtual u32 getOnMovementGroundArea();
    virtual bool testWater(Vec2f*, float* surfaceY);
};

extern soAreaEnviromentElementCheckerNull g_soAreaEnviromentElementCheckerNull;
