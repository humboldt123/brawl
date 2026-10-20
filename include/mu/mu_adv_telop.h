#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_restex.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <revolution/GX.h>
#include <types.h>

// What the adventure mode scene hands over to start a telop (the caption that fades in over the screen).
struct muAdvTelopParam {
    int m_texIndex;                 // 0x00 which picture of the menu archive is the caption
    u32 m_hold;                     // 0x04 frames that the caption stays (counted by m_frame of the task)
    u32 m_fadeIn;                   // 0x08 frames of the fade in (0 = 1)
    u32 m_fadeOut;                  // 0x0C frames of the fade out (0 = 1)
    gfArchive* m_archive;           // 0x10 the menu archive that has the model and the pictures
};

// The telop of the adventure mode: a single model ("Telop") whose picture is replaced by the caption and whose
// alpha fades in, holds and fades out, and is then removed.
class muAdvTelopTask : public gfTask {
public:
    enum Step {
        Step_Create = 0,
        Step_FadeIn = 1,
        Step_Hold = 2,
        Step_FadeOut = 3,
        Step_Remove = 4,
        Step_Delete = 5,
        Step_Done = 6,
    };

    gfArchive* m_archive;           // 0x40
    nw4r::g3d::ResTex m_tex;        // 0x44 the picture of the caption
    MuObject* m_object;             // 0x48 the model
    int m_step;                     // 0x4C
    int m_texIndex;                 // 0x50
    u32 m_hold;                     // 0x54
    u32 m_fadeIn;                   // 0x58
    u32 m_fadeOut;                  // 0x5C
    int m_frame;                    // 0x60 HYPOTHESIS: counted by the scene that owns the telop
    float m_alpha;                  // 0x64 0 - 255
    float m_alphaStep;              // 0x68 per frame
    int m_unk6C;
    u8 m_alphaByte;                 // 0x70
    u8 m_unk71;                     // 0x71

    muAdvTelopTask()
        : gfTask("Telop", Category_Menu, 0xF, 8, true), m_tex(), m_object(NULL), m_frame(0) { m_step = Step_Create; }
    static muAdvTelopTask* create(const muAdvTelopParam* param);

    virtual void processDefault();
    virtual ~muAdvTelopTask();

    bool isAlive() const;
    void kill();
    void createData(gfArchive* archive);
    void setAlpha();
};
static_assert(sizeof(muAdvTelopTask) == 0x74, "Class is wrong size!");
