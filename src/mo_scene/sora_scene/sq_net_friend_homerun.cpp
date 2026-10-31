#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_friend_homerun.h>
#include <sr/sr_common.h>
#include <types.h>


sqNetFriendHomerun* sqNetFriendHomerun::create() {
    return new (Heaps::GlobalMode) sqNetFriendHomerun("sqNetFriendHomerun");
}

void sqNetFriendHomerun::start() {
    unk8 = 0;
    g_GameGlobal->initSelCharData();
}

void sqNetFriendHomerun::exit() { }
