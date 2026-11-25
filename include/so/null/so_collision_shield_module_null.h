#pragma once

#include <so/collision/so_collision_shield_module_impl.h>

inline soCollisionShieldModule::~soCollisionShieldModule() { }

class soCollisionShieldModuleNull : public soNull, public soNullable {
public:
    virtual ~soCollisionShieldModuleNull();
    virtual void activate(double, double, double);
    virtual void activate(double, double, double, u32 taskId);
    virtual void deactivate();
    virtual void begin();
    virtual void clean();
    virtual void add(soCollisionShieldGroupData*, u32 collisionShieldGroupIndex);
    virtual void add(soCollisionReflectorGroupData*, u32 collisionShieldGroupIndex);
    virtual void set(u32, soCollisionShieldData*, u32 collisionShieldGroupIndex);
    virtual void setReflectorData(soCollisionReflectorData*, u32 collisionShieldGroupIndex);
    virtual void update(double, double, double, u32 collisionShieldGroupIndex);
    virtual void update(double, double, double);
    virtual bool isShield(u32, u32 collisionShieldGroupIndex);
    virtual void setStatus(u32, u32, u32);
    virtual void setStatusAll(u32, u32);
    virtual u8 isTurn(u32 collisionShieldGroupIndex) const;
    virtual u8 isFront(u32 collisionShieldGroupIndex) const;
    virtual bool isHop(u32 collisionShieldGroupIndex) const;
    virtual float getHopAngle(u32 collisionShieldGroupIndex) const;
    virtual bool isNoHop(u32 collisionShieldGroupIndex) const;
    virtual void setTurn(bool isTurn, u32 collisionShieldGroupIndex);
    virtual void setFront(bool isFront, u32 collisionShieldGroupIndex);
    virtual void setHop(float hopAngle, bool isHop, u32 collisionShieldGroupIndex);
    virtual float getAttackMul(u32 collisionShieldGroupIndex) const;
    virtual float getSpeedMul(u32 collisionShieldGroupIndex) const;
    virtual float getLifeMul(u32 collisionShieldGroupIndex) const;
    virtual float getAttackLimit(u32 collisionShieldGroupIndex) const;
    virtual u8 isNoMBall(u32 collisionShieldGroupIndex) const;
    virtual int getPartSize(u32 collisionShieldGroupIndex) const;
    virtual u32 getTeam();
    virtual void checkLog();
    virtual bool check();
    virtual void renderDebug();
    virtual void clearAll();
    virtual void clearAll(u32 collisionShieldGroupIndex);
    virtual Vec3f getCenterPos(u32, u32 collisionShieldGroupIndex);
    virtual float getLr(u32 collisionShieldGroupIndex) const;
    virtual u32 getGroupNum();
    virtual float getPosX(u32 collisionShieldGroupIndex) const;
    virtual void setTargetProperty(u32, u32 collisionShieldGroupIndex);
};
