#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_friend_homerun.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqNetFriendHomerun* sqNetFriendHomerun::create() {
    return new (Heaps::GlobalMode) sqNetFriendHomerun("sqNetFriendHomerun");
}

void sqNetFriendHomerun::start() {
    unk8 = 0;
    fn_8004D010(g_GameGlobal);
}

void sqNetFriendHomerun::exit() { }
