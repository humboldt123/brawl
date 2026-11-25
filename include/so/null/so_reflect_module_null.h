#pragma once

#include <so/reflect/so_reflect_module.h>

inline soReflectModule::~soReflectModule() { }

class soReflectModuleNull : public soReflectModule {
public:
    virtual ~soReflectModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void initInfo();
    virtual void resetInfo();
    virtual int getTaskId();
    virtual int getTeam();
    virtual float getAttackMul();
    virtual void setAttackMul(float);
    virtual float getLifeMul();
    virtual bool isReflect();
    virtual float getSpeedMul();
    virtual bool isCountMax();
    virtual void setNoSpeedMul(bool);
};
