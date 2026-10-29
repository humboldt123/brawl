#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <sq/sq_net_friend_list.h>
#include <sr/sr_common.h>
#include <types.h>

sqNetFriendList* sqNetFriendList::create() {
    return new (Heaps::GlobalMode) sqNetFriendList("sqNetFriendList");
}

void sqNetFriendList::start() {
    s32 kind = gfSceneManager::getInstance()->unk31C;
    unk14 = kind;
    unk8 = 0;
    unkC = g_gfApplication->unk110[0] != 0;
    switch (kind) {
    case 0x18:
    default:
        unk10 = 0;
        break;
    case 0x19:
        unk10 = 1;
        break;
    case 0x1a:
        unk10 = 2;
        break;
    case 0x1b:
        unk10 = 3;
        break;
    }
}

void sqNetFriendList::exit() { }
