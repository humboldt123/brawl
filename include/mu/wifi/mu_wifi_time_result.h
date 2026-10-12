#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// Local shadow of the header of the shared headers (this directory comes first in the include paths): the task is
// declared in full here.

// What the scene gives to the task. HYPOTHESIS: the names follow how the task uses the fields.
struct muWifiTimeResultTaskParam {
    u32 m_pad;                  // 0x00 the pad of the player (the controller of the menu)
    u32 m_charKind;             // 0x04 the character that is shown
    u32 m_time;                 // 0x08 the first number that is printed (the best time)
    u32 m_unkC;                 // 0x0C
    u32 m_unk10;                // 0x10 the second number that is printed
    u32 m_unk14;                // 0x14 the third number that is printed
    u32 m_unk18;                // 0x18 the fourth number that is printed (right aligned)

    muWifiTimeResultTaskParam() {
        m_pad = 0;
        m_charKind = 0;
        m_time = 0;
        m_unkC = 0;
        m_unk10 = 0;
        m_unk14 = 0;
        m_unk18 = 0;
    }
};
static_assert(sizeof(muWifiTimeResultTaskParam) == 0x1C, "Class is the wrong size!");

// The controller of the menu: the constructor of muMenuController, without its destructor (the task never destroys it).
extern "C" void fn_800B96E8(muMenuController* controller, int pad);
struct muWifiController {
    u8 m_data[0xA8];

    muWifiController() { fn_800B96E8(reinterpret_cast<muMenuController*>(this), 0xFF); }
};

// One model of the menu: the name, the slots of the task it fills (start, end), and the draw priority.
struct muWifiTimeResultModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The result screen of the time attack of the wifi mode: the panel with the character, the name of the character and the
// numbers of the result.
class muWifiTimeResultTask : public gfTask {
public:
    enum { FileNum = 5 };

    MuObject* m_object;                     // 0x40 the panel (it fills the slot of the model list)
    MuMsg* m_msg;                           // 0x44 the texts
    nw4r::g3d::ResFile m_resFileModel;      // 0x48 the models of the panel
    nw4r::g3d::ResFile m_resFileCamera;     // 0x4C the camera animation
    nw4r::g3d::ResFile m_resFileFace;       // 0x50 the face of the character
    nw4r::g3d::ResFile m_resFileName;       // 0x54 the textures of the name
    u32 m_pad;                              // 0x58
    u32 m_charKind;                         // 0x5C
    u32 m_unk60;                            // 0x60 numbers of the result
    u32 m_unk64;                            // 0x64
    u32 m_unk68;                            // 0x68
    u32 m_unk6C;                            // 0x6C
    int m_frame;                            // 0x70 the frame of the texture of the panel
    muWifiController m_controller;          // 0x74 the controller of the menu
    bool m_animChanged;                     // 0x11C the animation of the panel was changed
    char m_nameTex[0x23];                   // 0x11D the name of the texture of the name of the character
    gfFileIOHandle m_handles[FileNum];      // 0x140 the face, the textures of the name, the models, the camera and the texts
    bool m_loaded[FileNum];                 // 0x154
    bool m_started;                         // 0x159 the files were loaded and the menu was set up

    muWifiTimeResultTask();
    virtual void processDefault();
    virtual ~muWifiTimeResultTask();

    static muWifiTimeResultTask* create(const muWifiTimeResultTaskParam* param);
    void createObjResFile(const muWifiTimeResultModel* models, int count, nw4r::g3d::ResFile* resFile, int priority);
    void initMsg();
};
static_assert(sizeof(muWifiTimeResultTask) == 0x15C, "Class is wrong size!");
