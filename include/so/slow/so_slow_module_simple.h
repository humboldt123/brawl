// SHADOW of BrawlHeaders/so/slow/so_slow_module_simple.h: adds the (sora_melee) constructor used by the article builders,
// the pure soSlowModule interface and the inline rate/skip getters that so_slow_module_impl.o emits.
#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>

class soModuleAccesser;

class soSlowModule : public soNull, public soNullable {
public:
    virtual ~soSlowModule();
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void update(bool) = 0;
    virtual void resetSkip() = 0;
    virtual bool isSlow() = 0;
    virtual void set(int mag, int frame) = 0;
    virtual int getMag() = 0;
    virtual int getFrame() = 0;
    virtual void clear() = 0;
    virtual float getRate() = 0;
    virtual bool isSkip() = 0;
    virtual void setWhole(int, int wholeFrame) = 0;
    virtual void clearWhole() = 0;
    virtual u8 getWholeMag() = 0;
    virtual void setWholeFrame(int wholeFrame) = 0;
    virtual int getWholeFrame() = 0;
};
static_assert(sizeof(soSlowModule) == 12, "Class is wrong size!");

class soSlowModuleSimple : public soSlowModule {
protected:
    soModuleAccesser* m_moduleAccesser;
    float m_rate;
    bool m_isSkip;
    char _pad[3];

public:
    soSlowModuleSimple(soModuleAccesser* moduleAccesser); // sora_melee
    virtual ~soSlowModuleSimple();
    virtual void activate();
    virtual void deactivate();
    virtual void update(bool);
    virtual void resetSkip();
    virtual bool isSlow() { return m_rate != 1.0f; }
    virtual void set(int mag, int frame);
    virtual int getMag();
    virtual int getFrame();
    virtual void clear();
    virtual float getRate() { return m_rate; }
    virtual bool isSkip() { return m_isSkip; }
    virtual void setWhole(int, int wholeFrame);
    virtual void clearWhole();
    virtual u8 getWholeMag();
    virtual void setWholeFrame(int wholeFrame);
    virtual int getWholeFrame();
};
static_assert(sizeof(soSlowModuleSimple) == 24, "Class is wrong size!");
