#pragma once

#include <havok/hkCollisionAgent.h>

// Box x box agent of the 3-axis variant (hkBoxBoxAgent3 TU). Only the members implemented so far are declared;
// the layout (manifold, 0x50 sub-object) is not recovered yet.
struct hkBoxBoxAgent3 : hkCollisionAgent {
    // Empty in the original (0x802FB45C).
    virtual void createZombie(u16 key); // 0x38
};
