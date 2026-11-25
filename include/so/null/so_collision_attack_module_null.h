#pragma once

#include <so/collision/so_collision_attack_module_impl.h>

inline soCollisionAttackModule::~soCollisionAttackModule() { }

class soCollisionAttackModuleNull : public soCollisionAttackModule {
public:
    virtual ~soCollisionAttackModuleNull();
    virtual void activate(float, float lr, int taskId, Vec2f* pos, u32 index, bool);
    virtual void deactivate();
    virtual void begin();
    virtual void clearAll();
    virtual void clear(u32 index);
    virtual void sleep(bool);
    virtual void set(u32 index, u32 groupIndex, soCollisionAttackData* attackData);
    virtual void set(u32 index, u32 groupIndex);
    virtual void setAbsolute(u32 index, u32 groupIndex, soCollisionAttackAbsoluteData* attackData);
    virtual void hitAbsolute(u32 index, Vec3f* nodeGlobalPos, int targetTaskId, int targetGroup, int targetNo);
    virtual void hitAbsolute(u32 index, int nodeId, int targetTaskId, int targetGroup, int targetNo);
    virtual void hitAbsolute(u32 index, const char* nodeName, int targetTaskId, int targetGroup, int targetNo);
    virtual void setWhole(u32 index, bool);
    virtual bool isAttack(u32 index, bool);
    virtual bool isInvalidInvincible(u32 index, bool);
    virtual bool isInvalidXlu(u32 index, bool);
    virtual void setPower(u32 index, int power, bool);
    virtual void setVector(u32 index, int vector);
    virtual float getSize(u32 index);
    virtual void setSize(u32 index, float size);
    virtual void setTargetCategory(u32 index, soCollision::CategoryMask categoryMask);
    virtual bool setOffset(u32 index, Vec3f* offsetPos);
    virtual bool setOffsetFromNode(u32 index, int node);
    virtual void setNode(u32 index, int node);
    virtual float getBasePower(u32 index, bool);
    virtual float getPower(u32 index, bool);
    virtual u32 getGroup(u32 index);
    virtual int getReactionEffect(u32 index, bool);
    virtual void setReactionEffect(u32 index, int reactionEffect, bool);
    virtual int getReactionFix(u32 index, bool);
    virtual void setReactionFix(u32 index, int reactionFix, bool);
    virtual int getReactionAdd(u32 index, bool);
    virtual void setReactionAdd(u32 index, int reactionAdd, bool);
    virtual float getReactionMul(u32 index);
    virtual void setPosX(u32 index, float posX);
    virtual float getPosX(u32 index, bool);
    virtual float getPosX();
    virtual Vec3f getCenterPos(u32 index, bool);
    virtual float getSpeedX();
    virtual Vec2f getSpeed();
    virtual void setSpeed(Vec2f* speed);
    virtual soCollisionAttackData* getData(u32 index, bool);
    virtual void setPowerMulStatus(float powerMulStatus);
    virtual float getPowerMulStatus();
    virtual void setPowerAddStatus(float powerAddStatus);
    virtual float getPowerSpeedStatus();
    virtual void setPowerSpeedStatus(float powerSpeedStatus);
    virtual void setPowerMul(float powerMul);
    virtual float getPowerMul();
    virtual void setPowerMul2nd(float powerMul2nd);
    virtual float getPowerMul2nd();
    virtual void setReactionMul(float reactionMul);
    virtual float getLr();
    virtual u32 getPartSize();
    virtual void update(Vec2f* pos, float scale, float lr, SituationKind kind, Vec2f* speed);
    virtual void checkLog();
    virtual bool check();
    virtual void refLogGroup(u32 groupIndex, u32 logGroupIndex);
    virtual void unRefLogGroup(u32 groupIndex);
    virtual int isInflict();
    virtual int isInflictStatus();
    virtual void setIndirectInfo(int indirectTaskId, int teamNo);
    virtual int getIndirectTaskId();
    virtual int getIndirectTeamNo();
    virtual soCollision* getCollision();
    virtual void addPastLog(void*, u32, u32 groupIndex);
    virtual void invalidLog(soCollisionLog* collisionLog, u32 groupIndex);
    virtual bool checkRebound(soModuleAccesser* moduleAccesser, StageObject*, const soCollisionLog* collisionLog, float power, float* out1, float* out2);
    virtual void setRestrict(bool isRestrict);
    virtual void renderDebug();
};
