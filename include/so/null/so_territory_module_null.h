#pragma once

#include <so/territory/so_territory_module_impl.h>

class soTerritoryModuleNull : public soTerritoryModule {
    soHyp::soTerritoryRect m_rect;
public:
    virtual ~soTerritoryModuleNull();
    virtual void createAreaDataSetAll();
    virtual void initAreaInstance();
    virtual void activate();
    virtual void createTerritory(int);
    virtual void setEnable(int);
    virtual bool isTouch(int area);
    virtual bool isOutSide();
    virtual soHyp::soTerritoryRect* getRect();
    virtual void setRect(soHyp::soTerritoryRect* rect);
    virtual bool checkArea(u32 area);
    virtual bool isDoCreate();
};
