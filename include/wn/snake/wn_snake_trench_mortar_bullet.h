#pragma once

#include <wn/weapon.h>

class soCollisionLog;

// HYPOTHESIS: opaque shell over the Weapon base; the weapon builder and observer bases are not modelled.
class wnSnakeTrenchMortarBullet : public Weapon {
public:
    virtual void updateNodeSRT();
    virtual void processHit();
    virtual void processCollision();
    // HYPOTHESIS: the soCollisionSearchEventObserver overrides (the shell does not model the observer base).
    void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionSearchCheck();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual ~wnSnakeTrenchMortarBullet();
};
