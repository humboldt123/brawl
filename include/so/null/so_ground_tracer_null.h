#pragma once

#include <so/ground/so_ground_tracer_impl.h>

class soGroundTracerNull : public soGroundTracer {
public:
    virtual ~soGroundTracerNull();
    virtual bool isTraced();
    virtual bool canStart(u32, u8, soGroundModule* groundModule, u32 index, float);
    virtual bool start(u32, u8, soPostureModule* postureModule, soGroundModule* groundModule, u32 index, u8, float, float, float);
    virtual Vec2f getTraceTargetVec(soGroundModule* groundModule, u32 index, float);
    virtual Vec2f getTraceTargetPosturePos(soGroundModule* groundModule, u32 index, float);
    virtual bool isArrived(soPostureModule* postureModule, soGroundModule* groundModule, u32 index, float);
    virtual bool canIntercept(u32, u8, soGroundModule* groundModule, u32 index, float);
    virtual bool renew(u32, u8, soPostureModule* postureModule, soGroundModule* groundModule, u32 index, float, float);
    virtual void end();
    virtual void update(bool, soGroundModule* groundModule, u32 index);
    virtual Vec2f getGroundMovementSpeed();
    virtual float getDistanceFromCurrentLine(soGroundModule* groundModule, u32 index, bool);
    virtual Vec2f getVecFromCurrentLine(soGroundModule* groundModule, u32 index);
    virtual bool isSameNormalTraceLineCurrentAndNext(soGroundModule* groundModule, u32);
    virtual bool isOpenTraceLineCurrentAndNext(soGroundModule* groundModule, u32);
    virtual bool isTouchCurrentLine(soGroundModule* groundModule, u32 index);
};
