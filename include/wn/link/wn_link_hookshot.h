#pragma once

// Link's hookshot (article kind 4, the Clawshot special). Only the members the fighter calls are declared so far; the
// article itself (src/mo_fighter/ft_link/wn_link_hookshot.cpp) is not reconstructed yet.

#include <wn/weapon.h>

class wnLinkHookshot : public Weapon {
public:
    // HYPOTHESIS: parameter names beyond the founder, resource and team; the position is the node the hook starts at and the
    // flag is set while Link is in one of the hookshot statuses.
    void activate(int founderTaskId, int resourceId, int team, float lr, const Vec3f& position, bool unk);
};
