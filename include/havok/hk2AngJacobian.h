#pragma once

#include <havok/hkBase.h>

// Two-angular Jacobian: 0x20 bytes per element, angular rhs float at 0x1C.
struct hk2AngJacobian {
    hk2AngJacobian* next(int n) const;
    float getAngularRhs() const;
    void setAngularRhs(float f);
};
