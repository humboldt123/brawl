#pragma once

#include <so/so_null.h>
#include <types.h>

// HYPOTHESIS: parameter lists of unreferenced slots.
class soDebugModule : public soNullable {
public:
    virtual ~soDebugModule() { }
    virtual void proc() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual bool isDamageLock() = 0;
    virtual bool isDisplayModel() = 0;
    virtual void sleep() = 0;
    virtual bool isSleep() = 0;
    virtual void addDebugRay() = 0;
    virtual void addDebugDamage() = 0;
    virtual int getColorBlendStatus() = 0;
    virtual const char* getStatusName() = 0;
    virtual const char* getMotionName() = 0;
    virtual const char* getKineticTypeName() = 0;
    virtual const char* getSituationName() = 0;
    virtual const char* getGroundCorrectName() = 0;
    virtual void debugPrintInsideEventInfo() = 0;
    virtual bool isPrintChangeStatus() = 0;
    virtual bool isPrintChangeMotion() = 0;
    virtual bool isDamageOrbit() = 0;
    virtual bool isDebugTarget();
    virtual bool isSubFlag();
};

class soDebugModuleNull : public soDebugModule {
public:
    virtual ~soDebugModuleNull();
    virtual void proc();
    virtual void update();
    virtual void render();
    virtual bool isDamageLock();
    virtual bool isDisplayModel();
    virtual void sleep();
    virtual bool isSleep();
    virtual void addDebugRay();
    virtual void addDebugDamage();
    virtual int getColorBlendStatus();
    virtual const char* getStatusName();
    virtual const char* getMotionName();
    virtual const char* getKineticTypeName();
    virtual const char* getSituationName();
    virtual const char* getGroundCorrectName();
    virtual void debugPrintInsideEventInfo();
    virtual bool isPrintChangeStatus();
    virtual bool isPrintChangeMotion();
    virtual bool isDamageOrbit();
};
