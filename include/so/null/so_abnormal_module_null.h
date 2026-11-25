#pragma once

#include <so/so_null.h>
#include <types.h>

class soAbnormalModule : public soNullable {
public:
    virtual ~soAbnormalModule();
    virtual void activate() = 0;
    virtual void update() = 0;
    virtual float getAttackPowerMul() = 0;
    virtual float getDamageReactionMul() = 0;
    virtual bool isStatus(int) = 0;
    virtual void set(int) = 0;
    virtual void clear(int) = 0;
    virtual void clearAll() = 0;
};

class soAbnormalModuleNull : public soAbnormalModule {
public:
    virtual ~soAbnormalModuleNull();
    virtual void activate();
    virtual void update();
    virtual float getAttackPowerMul();
    virtual float getDamageReactionMul();
    virtual bool isStatus(int);
    virtual void set(int);
    virtual void clear(int);
    virtual void clearAll();
};
