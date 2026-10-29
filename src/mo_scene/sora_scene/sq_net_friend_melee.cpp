#include <gf/gf_scene.h>
#include <sq/sq_net_friend_melee.h>
#include <sr/sr_common.h>
#include <types.h>

sqNetFriendMelee* sqNetFriendMelee::create() {
    return new (Heaps::GlobalMode) sqNetFriendMelee("sqNetFriendMelee");
}

void sqNetFriendMelee::exit() { }
