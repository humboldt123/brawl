#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <mu/selchar/mu_select_character_name_entry.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

class muNameTask;

// The Controls option of the Options screen: the name menu (muNameTask) opens to choose the name whose controls are set;
// once a name is chosen (or made) it is written in the rules and the Controls scene opens. B in the name menu goes back.
// HYPOTHESIS: the names of the fields follow the constructor in muMenuMain (sora_scene) and what the functions do.
class muProcOptBtn : public muProcMenu {
public:
    enum Step {
        Step_Name = 0,              // the name menu is open
        Step_Done = 1,              // the name menu closed by itself; proc ends the screen
    };

    enum Obj {
        Obj_NameList0 = 0x4A,       // the first of the three objects of the name list in m_objs
        Obj_NameRemote = 0xAE,      // HYPOTHESIS: the object of the Wii Remote of the name menu
        Mdl_Name0 = 2,              // the first of the two models of the name menu in m_mdls
    };

    u8 m_unk66C_0 : 1;              // 0x66C cleared by init (HYPOTHESIS: never set)
    int m_unk670;                   // 0x670 cleared by init
    u8 m_unk674[0x678 - 0x674];
    void* m_unk678;                 // 0x678 a file of the menu archive (the constructor reads it; nothing here uses it)
    MuSelctChrNameEntry m_entry;    // 0x67C HYPOTHESIS: a keyboard of its own (the name menu builds the one it uses)
    u16 m_unk710;                   // 0x710 cleared by init
    u16 m_unk712;                   // 0x712 cleared by init
    u8 m_unk714[0x71C - 0x714];
    muNameTask* m_nameTask;         // 0x71C the name menu while it is open
    u8 m_unk720[0x724 - 0x720];
    bool m_shown;                   // 0x724 the name menu has finished coming in
    nw4r::g3d::ResFile* m_remoteRes;    // 0x728 the models of the Wii Remote (muMenuMain::m_resFileA8C)
    nw4r::g3d::ResFile* m_wndRes;       // 0x72C the models of the windows (muMenuMain::m_resFileA9C)
    void* m_msgData;                // 0x730 the texts of the name menu (a file of the menu archive)
    int m_msgNo;                    // 0x734 the help text in the panel

    virtual ~muProcOptBtn() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
    int selectNameMain();
};
static_assert(sizeof(muProcOptBtn) == 0x738, "Class is wrong size!");
