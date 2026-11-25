#pragma once

#include <so/jostle/so_jostle_module.h>

class soJostleModuleNull : public soJostleModule {
public:
    virtual ~soJostleModuleNull();
    virtual void unk10();
    virtual void setByte47(u8 value);
    virtual void setByte46(int value);
    virtual void setByte48(bool value);
    virtual void setByte4B(u8 value);
    virtual void setByte4A(bool value);
    virtual void unk30();
    virtual float getSpeedX();
    virtual void unk38();
    virtual float getPosZ();
    virtual void unk40();
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void activate();
    virtual void setStatus(u8);
    virtual void setLayer(int);
    virtual void setFix(bool);
    virtual void setIgnoreSpeedX(u8);
    virtual void sleep(bool);
    virtual void update();
    virtual void setRefer(int);
    virtual void setPushSpeedX(float);
    virtual void setPriority(int);
    virtual void setJostleData(void*);
    virtual void setForceGap(float);
};
