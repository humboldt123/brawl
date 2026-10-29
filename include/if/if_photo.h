#pragma once

// HYPOTHESIS: layout from the constructor, init, the destructor and the users of each field; names from the map
// (if_photo.o). IfPhoto is the photo (snapshot) mode of a match: pause camera, help display and the save task.

#include <StaticAssert.h>
#include <gf/gf_pad_status.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class CameraController;
class IfSnapSaveTask;
class MuMsg;
class MuObject;
struct IfSnapSaveMedia;
namespace nw4r { namespace g3d { class ScnGroup; } }

class IfPhoto {
public:
    nw4r::g3d::ScnGroup* m_scnGroup;    // 0x00
    MuObject* m_objects[11];            // 0x04
    MuMsg* m_msg;                       // 0x30
    s32 m_step;                         // 0x34
    s32 unk38;                          // 0x38
    s32 unk3C;                          // 0x3C
    s32 unk40;                          // 0x40
    s32 unk44;                          // 0x44
    u8 m_padNo;                         // 0x48
    s32 unk4C;                          // 0x4C
    s32 m_cameraInput;                  // 0x50 HYPOTHESIS: the CameraController input kind restored by exitPause
    nw4r::g3d::ResFile m_resFile;       // 0x54
    void* m_msgData;                    // 0x58
    IfSnapSaveTask* m_snapSaveTask;     // 0x5C
    s32 unk60;                          // 0x60
    u8 unk64;                           // 0x64
    nw4r::g3d::ResFile m_resFile2;      // 0x68
    u32 m_helpFlags;                    // 0x6C bit 0: snap, bit 1: retry, bit 2: reset
    s32 m_state;                        // 0x70 1: paused (IfMngr)

    IfPhoto();
    void init();
    // HYPOTHESIS: parameter types (the map has none).
    void startMelee(nw4r::g3d::ScnGroup* scnGroup);
    static CameraController* getCameraController();
    gfPadStatus getPadStatus();
    void removeAllObj();
    bool haveSnapHelp();
    bool haveRetryHelp();
    bool haveResetHelp();
    bool getSaveEnable(IfSnapSaveMedia* media, u32 size);
    void photoStepSnapSaveInit();
    s32 photoStepSnapSaveMain();

    virtual ~IfPhoto();
};
static_assert(sizeof(IfPhoto) == 0x78, "Class is the wrong size!");
