#pragma once

#include <wn/weapon.h>

// HYPOTHESIS: opaque shell over the Weapon base; the weapon builder and observer bases are not modelled.
class wnSnakeRgb6Bullet : public Weapon {
public:
    virtual void onDeactivate();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual ~wnSnakeRgb6Bullet();
};
