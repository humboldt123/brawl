#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_friend_kumite.h>
#include <sr/sr_common.h>
#include <types.h>


sqNetFriendKumite* sqNetFriendKumite::create() {
    return new (Heaps::GlobalMode) sqNetFriendKumite("sqNetFriendKumite");
}

void sqNetFriendKumite::start() {
    unk8 = 0;
    g_GameGlobal->initSelCharData();
}

void sqNetFriendKumite::exit() { }
