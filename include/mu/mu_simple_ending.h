#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The camera task of the menu that draws the models: it keeps the group of the scene that the models are inserted into.
// HYPOTHESIS: the name; the group is at 0x48.
class muEndCameraTask : public gfTask {
public:
    u32 m_unk40;
    u32 m_unk44;
    nw4r::g3d::ScnGroup* m_scnRoot;     // 0x48
};

class mvMoviePlayer;

// The picture of the ending that shows who is shown on it (one of the ending pictures of a mode that ends without a movie):
// the picture slides in, waits for a button and is taken away. For the boss battles the text on it is the clear time.
class muEndPicTask : public gfTask {
public:
    enum Step {
        Step_Create = 1,
        Step_Appear = 2,
        Step_WaitButton = 3,
        Step_Pressed = 4,
        Step_Leave = 5,
        Step_LeaveWait = 6,
        Step_Remove = 7,
        Step_Done = 8,
    };

    nw4r::g3d::ResFile m_resFile;               // 0x40 the picture
    nw4r::g3d::ResFile m_resFileTex;            // 0x44 the textures
    muEndCameraTask* m_cameraTask;              // 0x48
    MuObject* m_object;                         // 0x4C
    MuMsg* m_msg;                               // 0x50 the clear time
    int m_step;                                 // 0x54
    int m_frame;                                // 0x58

    muEndPicTask(const char* name) : gfTask(name, Category_Menu, 0xF, 8, true), m_resFile(static_cast<void*>(NULL)),
                     m_resFileTex(static_cast<void*>(NULL)), m_cameraTask(NULL), m_object(NULL), m_msg(NULL), m_step(0), m_frame(0) { }
    static muEndPicTask* create();

    virtual void processDefault();
    virtual ~muEndPicTask();

    void initialize();
    void release();
    void createData(gfArchive* archive);
    void setBossBattleClearTime(u32 frames);
};
static_assert(sizeof(muEndPicTask) == 0x5C, "Class is wrong size!");

// The movie of the ending of a character: a file for every character, played on a model of the menu.
class muEndMovieTask : public gfTask {
public:
    enum Step {
        Step_Create = 1,
        Step_Show = 2,
        Step_Play = 3,
        Step_Hide = 4,
        Step_Done = 5,
    };

    static const int PathNum = 50;
    static const int PathSize = 0x30;

    nw4r::g3d::ResFile m_resFile;               // 0x40 the camera
    nw4r::g3d::ResFile m_resFileModel;          // 0x44 the model that the movie is shown on
    MuObject* m_object;                         // 0x48
    char m_paths[PathNum][PathSize];            // 0x4C the file of the movie of each character
    mvMoviePlayer* m_player;                    // 0x9AC
    int m_frameNum;                             // 0x9B0 the number of frames of the movie
    int m_frameNo;                              // 0x9B4 the frame that was drawn last
    int m_step;                                 // 0x9B8
    int m_frame;                                // 0x9BC

    muEndMovieTask();
    static muEndMovieTask* create();

    virtual void processDefault();
    virtual void renderOpa();
    virtual ~muEndMovieTask();

    void initialize();
    void release();
    void load(int index);
    bool isLoading();
    void createData(gfArchive* archive);
};
static_assert(sizeof(muEndMovieTask) == 0x9C0, "Class is wrong size!");
