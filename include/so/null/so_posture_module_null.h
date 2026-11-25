#pragma once

#include <so/posture/so_posture_module_simple.h>

class soPostureModuleNull : public soPostureModule {
public:
    virtual ~soPostureModuleNull();
    virtual void init(Vec3f* pos, float lr);
    virtual void initPos(Vec3f* pos);
    virtual Vec3f getPos();
    virtual void setPos(Vec3f* pos);
    virtual Vec3f getPrevPos();
    virtual Vec3f getPrevRoughPos();
    virtual void setPrevRoughPos(Vec3f* roughPos);
    virtual float getLr();
    virtual void setLr(float lr);
    virtual void reverseLr();
    virtual void initRot();
    virtual void initRot(u32 index);
    virtual Vec3f getRot(u32 index);
    virtual void setRot(Vec3f* rot, u32 index);
    virtual void initRotYLr();
    virtual bool isRotYLrDifferentInnerLr();
    virtual float getRotYLr();
    virtual void updateRotYLr();
    virtual void reverseRotYLr();
    virtual float getBaseScale();
    virtual float getScale();
    virtual void setScale(float scale);
    virtual void initScale();
    virtual float getOwnerScale();
    virtual void setOwnerScale(float ownerScale);
    virtual void begin();
    virtual void rewind();
    virtual void turnBack();
    virtual float interpolate(float);
    virtual bool setStickLR(float stickLR);
    virtual void setSyncConstraintNode(int);
    virtual void fixConstraintNodePosture(int, void*);
    virtual void fixRot();
};
