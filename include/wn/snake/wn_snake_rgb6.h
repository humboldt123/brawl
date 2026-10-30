#pragma once

#include <mt/mt_vector.h>
#include <wn/weapon.h>

// HYPOTHESIS: opaque shell over the Weapon base; the weapon builder and observer bases are not modelled.
class wnSnakeRgb6 : public Weapon {
public:
    // HYPOTHESIS: the soMotionEventObserver override (the shell does not model the observer base).
    void notifyEventChangeMotion(int motionKind, int prevMotionKind, void* data, soModuleAccesser* moduleAccesser);
    virtual void processFixCamera();
    // HYPOTHESIS: no arguments besides the article (only the result slot and this are read).
    Vec3f getShootPos();
    virtual ~wnSnakeRgb6();
};
