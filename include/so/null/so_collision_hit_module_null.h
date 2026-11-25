#pragma once

#include <so/collision/so_collision_hit_module_impl.h>

inline soCollisionHitModule::~soCollisionHitModule() { }

class soCollisionHitModuleNull : public soCollisionHitModule {
public:
    virtual ~soCollisionHitModuleNull();
    virtual void activate(float, float, float, u32 collisionIndex, u32 collisionHitGroupIndex);
    virtual void activate(float, float, float, int taskId, u32 collisionIndex);
    virtual void reset(float, float, float, u32 collisionIndex, u32 collisionHitGroupIndex);
    virtual void deactivate();
    virtual void begin();
    virtual void clean();
    virtual void sleep(bool);
    virtual void add(soSet<soCollisionHitData>*, u32 collisionHitGroupIndex);
    virtual void add(soSet<soCollisionHitData::Simple>*, u32 collisionHitGroupIndex);
    virtual void resetStatusAll(u32 collisionHitGroupIndex);
    virtual void set(u32 collisionHitPartIndex, soCollisionHitData* hitData, u32 collisionHitGroupIndex);
    virtual void set(soSet<soCollisionHitData>*, u32 collisionHitGroupIndex);
    virtual void set(soSet<soCollisionHitData::Simple>*, u32 collisionHitGroupIndex);
    virtual void setStatus(u32 collisionHitPartIndex, int status, u32 collisionHitGroupIndex);
    virtual void setStatusNode(int, int status, u32 collisionHitGroupIndex);
    virtual void setStatusNodeDefault(int, int status, u32 collisionHitGroupIndex);
    virtual void setStatusAll(int status, u32 collisionHitGroupIndex);
    virtual void setWhole(int, u32 collisionHitGroupIndex);
    virtual int getWhole(u32 collisionHitGroupIndex);
    virtual void setCheckCatch(bool, u32 collisionHitGroupIndex);
    virtual void update(float posX, float scale, float lr, SituationKind, u32 collisionHitGroupIndex);
    virtual void update(float posX, float scale, float lr, SituationKind);
    virtual void resetStatusGlobal(u32 collisionHitGroupIndex);
    virtual void setXluGlobal(u32 collisionHitGroupIndex);
    virtual void cancelXluGlobal(u32 collisionHitGroupIndex);
    virtual void setXluFrameGlobal(int xluFrameGlobal, u32 collisionHitGroupIndex);
    virtual void setInvincibleGlobal(u32 collisionHitGroupIndex);
    virtual void cancelInvincibleGlobal(u32 collisionHitGroupIndex);
    virtual void setInvincibleFrameGlobal(int invincibleFrameGlobal, bool override, u32 collisionHitGroupIndex);
    virtual int getInvincibleFrameGlobal(u32 collisionHitGroupIndex);
    virtual int getGlobal(u32 collisionHitGroupIndex);
    virtual int getStatus(u32 collisionHitPartIndex, u32 collisionHitGroupIndex);
    virtual int getTotalStatus(u32 collisionHitGroupIndex);
    virtual float getDurabilityMulFromAttackData(soCollisionAttackData* attackData, u32 partId);
    virtual int getPartSize(u32 collisionHitGroupIndex);
    virtual u32 getPartHeight(u32 collisionHitPartIndex, u32 collisionHitGroupIndex);
    virtual void setNoTeam(bool);
    virtual void setGlobalOffset(u8, u32 collisionHitGroupIndex);
    virtual void setMultiSituation(SituationKind situationKind, u32 collisionHitGroupIndex);
    virtual void initMultiSituation(u32 collisionHitGroupIndex);
    virtual void setOpponentCategory(u32 categoryMask, bool, u32 collisionHitGroupIndex);
    virtual void setSelfCategory(soCollision::Category, u32 collisionHitGroupIndex);
    virtual void checkLog();
    virtual u32 getGroupNum();
    virtual bool isReactionFrame();
    virtual int getReactionFrame();
    virtual void setReactionFrame(short reactionFrame);
    virtual void setPosX(float posX, u32 collisionGroupIndex);
    virtual float getPosX(u32 collisionGroupIndex);
    virtual Vec3f getCenterPos(u16 index, u32 collisionHitGroupIndex);
    virtual void renderDebug();
};
