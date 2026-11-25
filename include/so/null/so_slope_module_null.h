#pragma once

#include <so/slope/so_slope_module_impl.h>

class soSlopeModuleNull : public soSlopeModule {
public:
    virtual ~soSlopeModuleNull();
    virtual void activate();
    virtual void setStatus(u32 statusMask, int, bool);
    virtual soSlopeStatusParam::StatusMask getStatus();
    virtual void update(bool);
    virtual void updateModelTopAngle();
    virtual void setTopAngleUnlimit(bool);
    virtual void setPartNode(int);
    virtual void setInvalidStatus(u32 statusMask);
};
