// SHADOW of BrawlHeaders/so/slow/so_slow_module_impl.h: declares the constructor (private fields only for FT_MODULE_BUILDER), the
// skip countdown field and the inline frame/magnification accessors that so_slow_module_impl.o emits.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_slow_manager.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_null.h>
#include <types.h>

class soModuleAccesser;

class soSlowModuleImpl : public soSlowModuleSimple, public soAnimCmdEventObserver {
public:
    soSlowModuleImpl(soModuleAccesser* acc);
#ifdef FT_MODULE_BUILDER
private:
#endif

    // HYPOTHESIS: frames left until the next frame that is not skipped while a local slow motion is set.
    int m_skipCount;
    int m_frame;
    gfSlowRequestId m_wholeSlowId;
    char _45[3];
    int m_wholeFrame;
    int m_mag;

public:
    virtual ~soSlowModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void update(bool);
    virtual void resetSkip();
    virtual void set(int mag, int frame) {
        m_mag = mag;
        m_frame = frame;
    }
    virtual int getMag() { return m_mag; }
    virtual int getFrame() { return m_frame; }
    virtual void clear();
    virtual void setWhole(int, int wholeFrame);
    virtual void clearWhole();
    virtual u8 getWholeMag();
    virtual void setWholeFrame(int wholeFrame);
    virtual int getWholeFrame() { return m_wholeFrame; }

    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
static_assert(sizeof(soSlowModuleImpl) == 56, "Class is wrong size!");
