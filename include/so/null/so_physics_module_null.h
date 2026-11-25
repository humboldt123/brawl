#pragma once

#include <so/physics/so_physics_module_impl.h>

inline soPhysicsModule::~soPhysicsModule() { }

class soPhysicsModuleNull : public soNull, public soNullable {
public:
    virtual ~soPhysicsModuleNull();
    virtual void constructController();
    virtual void destructController();
    virtual void deactivate();
    virtual void activate();
    virtual void begin();
    virtual void setUpMtxIK();
    virtual void setUpMtx2ndBoth();
    virtual bool isNcessarySetUpMtx2ndBoth();
    virtual bool correct2ndBoth();
    virtual void setUpMtx2ndPreCollision();
    virtual void setUpMtx2nd();
    virtual void setUpVisibility();
    virtual bool is2nd();
    virtual void set2nd(int on);
    virtual void set2ndDefault();
    virtual void set2ndDefault(int on);
    virtual void set2ndGravity(float gravity);
    virtual void set2ndSleep(bool sleep);
    virtual void set2ndType(int type);
    virtual void set2ndStatus(int status);
    virtual void set2ndEndPos(const Vec3f* pos);
    virtual void set2ndPreCollisionNode(int nodeId);
    virtual int get2ndLineCount();
    virtual float get2ndLineLength(int lineId);
    virtual void set2ndLineLength(int lineId, float length);
    virtual void set2ndLineInertia(int lineId, float inertia);
    virtual void set2ndLineInertiaValue(int lineId, float inertia);
    virtual void set2ndLineLengthBothAll(float length);
    virtual int get2ndActiveNodeNum();
    virtual void set2ndActiveNodeNum(int num);
    virtual void set2ndRewind(int rewind);
    virtual void off2ndWind();
    virtual bool is2ndBoth();
    virtual bool is2ndBothReverse();
    virtual void set2ndBoth(int on);
    virtual void set2ndBothStatus(int status);
    virtual int get2ndBothSituation();
    virtual int get2ndBothNodeMax();
    virtual void set2ndBothSpeed(const Vec3f* speed, int lineId, u8 flag);
    virtual void set2ndBothRewind(int rewind);
    virtual void set2ndBothWaver(int waver);
    virtual void set2ndBothHitStat(int hitStat);
    virtual void set2ndBothReverseFlag(int flag);
    virtual void set2ndPower(float power);
    virtual u32 get2ndNodeId(int lineId);
    virtual void set2ndLinePos(const Vec3f* pos);
    virtual void set2ndPosSplit(int split);
    virtual bool is2ndHitCollision();
    virtual void set2ndWind(const Vec3f* wind);
    virtual void set2ndWeaponScale(float scale);
    virtual void set2ndCliffDir(float lr);
    virtual void sync2ndSavedPosition();
    virtual bool isIK(int ikId);
    virtual bool isIK();
    virtual void setIK(int ikId, float value);
    virtual void setIKTargetPos(int ikId, const Vec3f* pos, const Vec3f* pos2, const Vec3f* pos3);
    virtual void setIKNormal(int ikId, const Vec3f* normal);
    virtual const char* getIKEndNodeName(int ikId, int index);
    virtual void setToeAngleLimit(int ikId, float limit);
};
