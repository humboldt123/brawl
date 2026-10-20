#pragma once

// Toon Link's hookshot (article kind 4, the Clawshot special). Only the members the fighter calls are declared so far; the
// article itself (src/mo_fighter/ft_toonlink/wn_toon_link_hook_shot.cpp) is not reconstructed yet.

#include <wn/weapon.h>

class wnToonLinkHookShot : public Weapon {
public:
    // HYPOTHESIS: same parameters as Link's hookshot.
    void activate(int founderTaskId, int resourceId, int team, float lr, const Vec3f& position, bool unk);
};
