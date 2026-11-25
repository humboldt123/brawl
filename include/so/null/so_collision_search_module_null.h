#pragma once

#include <so/collision/so_collision_search_module_impl.h>

inline soCollisionSearchModule::~soCollisionSearchModule() { }

class soCollisionSearchModuleNull : public soCollisionSearchModule {
public:
    virtual ~soCollisionSearchModuleNull();
    virtual void activate(float, int taskId);
    virtual void deactivate();
    virtual void begin();
    virtual void clearAll();
    virtual void clear(u32 partIndex);
    virtual void set(u32 partIndex, u32 groupIndex, soCollisionSearchData* collisionSearchData);
    virtual bool isSearch(u32 partIndex);
    virtual float getSize(u32 partIndex);
    virtual void setSize(u32 partIndex, float size);
    virtual void setPos(int nodeIndex, Vec3f* startOffsetPos, Vec3f* endOffsetPos);
    virtual soCollisionSearchData* getData(u32 partIndex);
    virtual void update(float posX, float scale);
    virtual void checkLog();
    virtual bool check();
    virtual int isInflict();
    virtual void invalidLog(soCollisionLog* collisionLog, u32 groupIndex);
    virtual void setCheckInterval(int);
    virtual void renderDebug();
};
