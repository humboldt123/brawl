#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Special Brawl screen: eight settings (gmSetRule::m_spMeleeSetting1 ... 8), each with three values, and a ninth line
// under them that starts the match. Up and down move the cursor, left and right (or A) change the value of the line, Start
// jumps to the ninth line; the model of Mario shows the settings and the text at the top lists the ones that are not off.
// HYPOTHESIS: the names of the fields and of the steps follow what the functions do.
class muProcSpecial : public muProcMenu {
public:
    enum Step {
        Step_Come = 0,              // the panel comes in (from the main menu)
        Step_Select = 1,
        Step_Start = 2,             // Start was pressed: the match starts in the next frame
        Step_Exit = 3,
        Step_Leave = 4,             // B: the panel goes out, then back to the main menu
    };

    enum Entry {
        SettingCount = 8,
        ValueCount = 3,             // the values of a setting (0: off)
        Cursor_Start = 8,           // the line under the settings that starts the match
        Obj_Panel = 0xB4,           // the panel of the screen; its colour shows the cursor on the start line
        Obj_Cursor = 0xB5,          // node animation: the line of the cursor, visibility: the value under it
        Obj_Setting0 = 0xB6,        // the icons of the eight settings (colour: the value)
        Obj_Mario0 = 0xBE,          // the five parts of the model of Mario that show the settings
        Mdl_Panel = 0x17,           // the models of the screen in m_mdls
        Mdl_Mario = 0x18,
        Mdl_Mario2 = 0x19,
        Msg_Rules = 0,              // the text of the panel: the settings that are on
        Msg_Help = 1,               // the help of the line under the cursor
        Msg_Buf = 0x400,
        Msg_HelpBase = 0x22,        // the help of a setting: Msg_HelpBase + line * 3 + value
        Msg_HelpStart = 0x3A,
        Msg_RuleBase = 0x0,         // the name of a value that is on: Msg_RuleBase + line * 2 + value - 1
        Msg_RuleComma = 0x10,       // the same with a comma, when another setting after it is on
        Msg_RuleNone = 0x20,        // all of the settings are off
        Msg_RuleEnd = 0x21,
    };

    MuMsg* m_msg;                   // 0x66C
    u32 m_settings[SettingCount];   // 0x670 the values of the settings (copied to and from gmSetRule)
    int m_unk690;                   // 0x690 cleared by init

    muProcSpecial(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcSpecial() { delete m_msg; }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
    void setMarioModel();
    void printRuleMsg();
};
static_assert(sizeof(muProcSpecial) == 0x694, "Class is wrong size!");
