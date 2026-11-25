#pragma once

#include <so/stop/so_stop_module_impl.h>

inline soStopModule::~soStopModule() { }

class soStopModuleNull : public soStopModule {
public:
    virtual void setSpecialStop(u32 duration);
    virtual ~soStopModuleNull();
    virtual void activate();
    virtual void end(s32);
    virtual void setHitStop(float frame, int unk);
    virtual void cancelHitStop();
    virtual void setOtherStop(u32 duration);
    virtual void cancelOtherStop();
    virtual void setLinkStop(u32 duration);
    virtual bool isStop();
    virtual bool isSpecialStop();
    virtual bool isHit();
    virtual bool isDamage();
    virtual void update();
    virtual s32 getHitStopFrame();
    virtual s32 getHitStopRealFrame();
    virtual void setHitStopFrame(u32 duration, int allowSDI);
    virtual void setHitStopFrameFix(s32);
};
