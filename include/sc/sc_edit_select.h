#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <mu/menu.h>
#include <types.h>

class MenuRoot;
class MuObject;
class muObjProcessTask;
class muStageEditSelectTask;
class muStageEditUI;

// The scene of the Stage Builder stage browser (module sora_scene): a menu scene that runs the stage file browser of
// sora_menu_edit.rel. Leaving it fills the stage edit record of GameGlobal for the editor.
class scEditSelect : public muMenu {
public:
    // What start makes.
    struct Work {
        MenuRoot* m_root;               // 0x00 the scene root of the menu
        muStageEditSelectTask* m_task;  // 0x04
        muObjProcessTask* m_objProcess; // 0x08 updates the objects of the menu
    };

    u8 m_unk350[0x35C - 0x350];
    Work* m_work;                       // 0x35C

    scEditSelect(const char* name) : muMenu(name), m_work(NULL) { }
    static scEditSelect* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    // Gives the object process of the menu the objects it updates.
    void setMotionObj(MuObject** objs, int num);
};
static_assert(sizeof(scEditSelect) == 0x360, "Class is wrong size!");

// The scene of the Stage Builder editor (module sora_scene): loads the parts archive and runs the editor task.
class scEditMenu : public gfScene {
public:
    muStageEditUI* m_ui;                // 0x08

    scEditMenu(const char* name) : gfScene(name) { }
    static scEditMenu* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
};
static_assert(sizeof(scEditMenu) == 0xC, "Class is wrong size!");
