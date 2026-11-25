#pragma once

#include <so/area/so_area_module_impl.h>

class soAreaModuleNull : public soAreaModule {
public:
    virtual ~soAreaModuleNull();
    virtual void activate(int ownerId);
    virtual void deactivate();
    virtual void update();
    virtual void clearLog();
    virtual void check();
    virtual void clean();
    virtual void setup(soSet<soAreaData>*);
    virtual void addArea(soSet<soAreaData>*);
    virtual void setWhole(bool);
    virtual gfArea::Group getGroup(u32 index);
    virtual void enableArea(int, bool, int);
    virtual bool isEnableArea(int);
    virtual void resetArea(int);
    virtual void setAreaTargetGroupMask(u32 index, u32 mask);
    virtual void setAreaTargetLocalGroupMask(u32 index, u32 mask);
    virtual void setAreaShape(u32 index, Vec2f*, Vec2f*);
    virtual void setAreaShape(u32 index, Vec2f*, float);
    virtual void setAreaShapeType(u32 index, soAreaInstance::ShapeType);
    virtual void setCenterX0(u32 index, bool);
    virtual void setAreaDbgColor(u32 index, GXColor);
    virtual int getAreaId(u32 index);
    virtual bool isExistAreaInstance(int);
    virtual gfArea* getAreaInstance(u32 index);
    virtual int getAreaIndexFromId(int);
    virtual u32 getAreaContactCount(int);
    virtual soAreaContactLog* getAreaContactLog(int areaId, int index);
    virtual soAreaEnviromentElementChecker* getEnviromentElementChecker();
    virtual void setLogOverThrough(bool);
    virtual void eraseWind(int);
    virtual void createWind(u32, u32, stMovementData*);
};
