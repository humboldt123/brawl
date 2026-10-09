#pragma once

#include <havok/hkBase.h>

// One-linear, two-angular Jacobian: 0x30 bytes per element.
struct hk1Lin2AngJacobian {
    hk1Lin2AngJacobian* next(int n) const;
};
