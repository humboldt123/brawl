#pragma once

#include <wn/weapon.h>

// HYPOTHESIS: opaque shell over the Weapon base; the weapon builder and observer bases are not modelled.
class wnSnakeLadder : public Weapon {
public:
    virtual void processFixCamera();
    virtual ~wnSnakeLadder();
};
