#pragma once

#include <so/collision/so_collision_catch_module_impl.h>

class soCollisionCatchModuleNull : public soCollisionCatchModule {
public:
    virtual ~soCollisionCatchModuleNull();
    virtual void activate(float posX, float scale, float lr, int taskId, bool isCheck);
    virtual void deactivate();
    virtual void begin();
    virtual void clearAll();
    virtual void clear(u32 partIndex);
    virtual void set(u32 partIndex, soCollisionCatchData* data);
    virtual bool isCatch(u32 partIndex);
    virtual float getSize(u32 partIndex);
    virtual void setSize(u32 partIndex, float size);
    virtual float getPosX();
    virtual Vec3f getCenterPos(u32 partIndex);
    virtual soCollisionCatchData* getData(u32 partIndex) const;
    virtual void update(float posX, float scale, float lr);
    virtual void renew();
    virtual void checkLog();
    virtual void renderDebug();
};
