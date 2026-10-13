#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_boot_common.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The title screen of the game: the logo comes in with its background, the animation plays, and after a time without input
// the screen goes on with the demo (the time out).
class muTitleTask : public gfTask {
public:
    enum State {
        State_Idle = 0,
        State_Begin = 1,
        State_StartLogo = 2,        // the logo starts its animation
        State_WaitLogo = 3,
        State_StartFade = 4,
        State_FadeIn = 5,           // the alpha of the background and the logo goes up to 1
        State_FadeInEnd = 6,
        State_WaitAnim = 7,         // the animation of the logo plays until the frame 250
        State_Hold = 8,
        State_HoldEnd = 9,
        State_WaitTimeOut = 10,     // the screen waits for the player (about 16 seconds)
        State_TimeOut = 11,
        State_Replay = 12,          // the second part of the animation (frames 261 - 280)
        State_ReplayWait = 13,
        State_ReplayEnd = 14,
        State_Short = 15,           // the short title: the animation plays from the start
        State_ShortWait = 16,
        State_ShortEnd = 17,
        State_End = 18,
    };

    gfFileIOHandle m_handleA;                   // 0x40 HYPOTHESIS: not used by this unit
    gfFileIOHandle m_handleB;                   // 0x44
    nw4r::g3d::ResFile m_resScene;              // 0x48 the camera and the light of the scene
    nw4r::g3d::ResFile m_resTitle;              // 0x4C the models of the title (title.brres)
    MuObject* m_background;                     // 0x50 "TitleBg"
    MuObject* m_logo;                           // 0x54 "TitleFront"
    MuObject* m_logoShort;                      // 0x58 "TitleFront2s"
    nw4r::g3d::ResAnmScn m_anmScn;              // 0x5C
    float m_alpha;                              // 0x60 the alpha of the models
    float m_backgroundAlpha;                    // 0x64 the alpha of the vertex colors of the background
    u8** m_vertexColors;                        // 0x68 the vertex colors of the background, as they were made
    u32 m_vertexColorCount;                     // 0x6C
    u8 m_normalAdded;                           // 0x70 the models of the long title are in the scene
    u8 m_shortAdded;                            // 0x71 the model of the short title is in the scene
    u8 m_timeOut;                               // 0x72 the player waited (set by the task, read by the title scene)
    u8 m_short;                                 // 0x73 the title is the short one
    int m_counterA;                             // 0x74 the frames since the start
    int m_state;                                // 0x78
    int m_counterB;                             // 0x7C the frames since the state changed

    muTitleTask(const char* name);
    static muTitleTask* create();

    virtual void processDefault();
    virtual void renderPre();
    virtual ~muTitleTask();

    void initialize();
    void release();
    void createDataRequest();
    void createData(gfArchive* archive);
    void addModelNormal();
    void removeModelNormal();
    void addModelShort();
    void removeModelShort();
    void skip();
    void requestTimeOut();
    void initVertexColorParam();
    void setObjectBGVertexAlpha(float alpha);
    void resetAnimetionParam();
};
static_assert(sizeof(muTitleTask) == 0x80, "Class is wrong size!");
