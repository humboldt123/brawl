#pragma once

#include <havok/hkBase.h>

// Jacobian element (one solver row). Only the members needed so far are declared.
struct hkJacobianBuilder;

struct hkJacobianElement {
    // Returns this as the 0x30-byte linear/angular rows the contact builder fills.
    hkJacobianBuilder* as1Lin2Ang();
    hkJacobianElement* as2Ang();
};
