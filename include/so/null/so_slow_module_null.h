#pragma once

#include <so/slow/so_slow_module_simple.h>

inline soSlowModule::~soSlowModule() { }

class soSlowModuleNull : public soSlowModule {
public:
    virtual ~soSlowModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void update(bool);
    virtual void resetSkip();
    virtual bool isSlow();
    virtual void set(int mag, int frame);
    virtual int getMag();
    virtual int getFrame();
    virtual void clear();
    virtual float getRate();
    virtual bool isSkip();
    virtual void setWhole(int, int wholeFrame);
    virtual void clearWhole();
    virtual u8 getWholeMag();
    virtual void setWholeFrame(int wholeFrame);
    virtual int getWholeFrame();
};
