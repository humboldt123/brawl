#pragma once

#include <mt/mt_vector.h>
#include <so/capture/so_capture_module.h>

inline soCaptureModule::~soCaptureModule() { }

class soCaptureModuleNull : public soCaptureModule {
public:
    virtual ~soCaptureModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();
    virtual bool isCapture();
    virtual int getCatchTaskId();
    virtual bool capture(int taskId, int node, bool);
    virtual Vec2f getNodeOffset();
    virtual float getCatchPosY();
    virtual void captureCut(bool, bool constrained);
    virtual bool checkDamage();
    virtual bool checkDamageThrown();
    virtual void thrown();
    virtual bool isThrown();
    virtual void updatePosThrown();
    virtual void updateLRThrown();
    virtual void thrownCut(bool);
    virtual bool check();
    virtual void setSendCutEvent(bool);
    virtual void setCatchNode(int);
    virtual void setUpdatePos(bool);
    virtual bool isReaction();
    virtual const Vec3f* getMotionOffset();
    virtual void setIgnoreCatching(bool);
    virtual bool isIgnoreCatching();
};
