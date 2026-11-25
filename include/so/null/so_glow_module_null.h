#pragma once

#define SO_GLOW_MODULE_ATTACK_API
#include <so/glow/so_glow_module_simple.h>

struct soCollisionAttackData;

class soGlowModuleNull : public soGlowModule {
public:
    virtual ~soGlowModuleNull();
    virtual void clear();
    virtual void update();
    virtual void activate();
    virtual void setup();
    virtual s32 getGlowAttack();
    virtual s32 getGlowAttack(soCollisionAttackData*);
    virtual void setGlowAttack(s32 attack);
    virtual bool isGlow();
    virtual s32 getGlowDefense();
    virtual s32 getGlowDefense(int);
    virtual void setGlowDefense(s32 defense);
    virtual void setGlowPower(float, float, float);
    virtual float getGlowPowerEff();
    virtual float getGlowPowerMin();
    virtual float getGlowPowerMax();
    virtual void setGlowAttackMax(int);
    virtual int getGlowAttackMax();
    virtual void setGlowAttackAttr(int);
    virtual void setGlowAttackRegion(int);
    virtual void clearGlowAttackStatus();
    virtual void setGlowAttackStatus(int);
    virtual void addGlowAttackStatus(int);
    virtual int getGlowAttackAdd(soCollisionAttackData*);
    virtual void setWeaponGlowAttack(int);
    virtual void setGlowDefenseAttr(int);
    virtual float getPower(float power, soCollisionAttackData*, int);
    virtual float getPower(float power, soCollisionAttackData*, soGlowModule*);
    virtual void setRefLinkNo(s32);
    virtual void dbgReport();
};
