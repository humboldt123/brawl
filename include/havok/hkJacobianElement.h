#pragma once

#include <havok/hkBase.h>

// Jacobian element (one solver row). Only the members needed so far are declared.
struct hkJacobianElement {
    void as1Lin2Ang();
    hkJacobianElement* as2Ang();
};
