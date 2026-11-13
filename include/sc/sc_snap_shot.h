#pragma once

#include <StaticAssert.h>
#include <mu/menu.h>
#include <types.h>

class MenuRoot;
class MuObject;
class muObjProcessTask;
class muSnapShotTask;

// The scene of the snapshot album (module sora_scene): a menu scene that runs the snapshot viewer task of
// sora_menu_snap_shot.rel. The viewer reaches the scene by the pointer it was created with.
class scSnapShot : public muMenu {
public:
    // What start makes.
    struct Work {
        MenuRoot* m_root;               // 0x00 the scene root of the menu
        muSnapShotTask* m_task;         // 0x04
        muObjProcessTask* m_objProcess; // 0x08 updates the objects of the menu
        u8 m_bgmStarted;                // 0x0C
    };

    u8 m_unk350[0x35C - 0x350];
    Work* m_work;                       // 0x35C

    scSnapShot(const char* name) : muMenu(name), m_work(NULL) { }
    static scSnapShot* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scSnapShot) == 0x360, "Class is wrong size!");
