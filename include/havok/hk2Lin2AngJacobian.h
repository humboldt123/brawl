#pragma once

#include <havok/hkBase.h>

// Two-linear, two-angular Jacobian: 0x40 bytes per element.
struct hk2Lin2AngJacobian {
    hk2Lin2AngJacobian* next(int n) const;
};
