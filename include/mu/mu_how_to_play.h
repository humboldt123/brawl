#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_boot_common.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The "how to play" movie (/movie/HowtoPlay.thp) that the game shows at the first boot, played on a model of the menu.
// (The symbol map calls the class "muISBNTask"; the name in the data of the unit is "muHowToPlayTask".)
class muHowToPlayTask : public gfTask {
public:
    gfFileIOHandle m_handleScene;               // 0x40 /menu/defaultcamera/AdvMovieWide.brres
    gfFileIOHandle m_handleChr;                 // 0x44 /menu/adventure/visual.brres
    nw4r::g3d::ResFile m_resScene;              // 0x48 the camera and the light of the scene
    nw4r::g3d::ResFile m_resChr;                // 0x4C the model that shows the movie
    MuObject* m_object;                         // 0x50 "MenAdvChrCmn1000_TopN"
    muMovie* m_movie;                           // 0x54
    int m_frameCount;                           // 0x58 how many frames the movie has
    int m_frame;                                // 0x5C the frame that was played
    int m_state;                                // 0x60
    int m_counter;                              // 0x64

    muHowToPlayTask(const char* name);
    static muHowToPlayTask* create();

    virtual void processDefault();
    virtual void renderOpa();
    virtual ~muHowToPlayTask();

    void initialize();
    void release();
    void openMovie();
    bool isLoadingMovie();
    void createData();
    void startMovie();
    void addMovieModel();
    void removeMovieModel();
    void skip();
};
static_assert(sizeof(muHowToPlayTask) == 0x68, "Class is wrong size!");
