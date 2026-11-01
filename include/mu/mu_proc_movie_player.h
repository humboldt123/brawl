#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_proc_main.h>
#include <mv/mv_movie_player.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The parameters of a movie of the Adventure Mode (movie/param/<language>/<movie>.pac, file 10 of the archive): up to
// four captions (telops) that fade in and out over the movie, and the music that starts at a frame of the movie.
// HYPOTHESIS: the names follow how muProcMoviePlayer uses the fields; the first 0x18 bytes are not read.
struct muMovieTelopData {
    struct Telop {
        u32 m_start;                // 0x0 the frame of the movie the caption comes at
        int m_fadeIn;               // 0x4 half the frames of the fade in
        u32 m_length;               // 0x8 the frames from m_start to the start of the fade out
        int m_fadeOut;              // 0xC half the frames of the fade out
    };

    u8 m_unk0[0x18];
    Telop m_telops[4];              // 0x18
    u32 m_bgmId;                    // 0x58 the music that starts during the movie
    int m_bgmFrame;                 // 0x5C the frame of the movie it starts at
    u32 m_bgmStart;                 // 0x60 HYPOTHESIS: where in the music it starts (sndSystem::startBGM)
    u8 m_unk64[4];
    bool m_hasBgm;                  // 0x68
    u8 m_unk69;
    u8 m_bgmVolume;                 // 0x6A in hundredths
    u8 m_movieVolume;               // 0x6B
};

// The task that draws the frames of the movie (drawFrame keeps the movie running and returns its frame).
// HYPOTHESIS: the task name is copied from adVisual, which does the same for the Adventure Mode stages.
class movieDraw : public gfTask {
public:
    mvMoviePlayer* m_player;        // 0x40
    int m_frame;                    // 0x44 the frame that was drawn last (-1 before the first)

    movieDraw() : gfTask("adVisual", Category_Menu, 0xE, 6, true), m_frame(-1) { }
    // HYPOTHESIS: a create helper (the player is read before the allocation and stored after the null check).
    static movieDraw* create(mvMoviePlayer* player) {
        movieDraw* draw = new (Heaps::MenuInstance) movieDraw();
        draw->m_player = player;
        return draw;
    }
    virtual void renderOpa();
    virtual ~movieDraw() { }
};
static_assert(sizeof(movieDraw) == 0x48, "Class is wrong size!");

// The screen that plays a movie of the Movies menu (the Movies and Adventure Movies screens put the number of the movie
// in it before they close). It loads the movie, the model the movie is shown on and, for the movies of the Adventure
// Mode, their captions and music; any of B, Start or A ends the movie and goes back to the screen it came from.
// HYPOTHESIS: the member names follow how the functions use them and the names of the map (setup, load, initAdv,
// procAdv, postProcAdv, isLoading, telopProc).
class muProcMoviePlayer : public muProcMenu {
public:
    enum {
        Id_AdvFirst = 5,            // the first GmMovieID of the Adventure Mode
        Id_AdvLast = 0x79,          // the last one
        Pad_End = Pad_Cancel | Pad_Start | Pad_A,
    };

    // How far the screen is (m_state).
    enum State {
        State_Load = 0,             // the files are read
        State_Prepare = 1,          // the music of the movie is prepared, then the movie is opened
        State_Open = 2,             // waits until the movie can play
        State_Play = 3,             // starts the movie
        State_Playing = 4,
        State_End = 5,              // the movie ended or was skipped
        State_Release = 6,          // takes everything away, then waits a few frames
        State_Exit = 7,
        State_Done = 8,
    };

    // How far the current caption is (m_telopState).
    enum TelopState {
        Telop_Show = 0,             // puts the caption on the screen
        Telop_FadeIn = 1,
        Telop_Wait = 2,             // waits for the frame of the fade out
        Telop_FadeOut = 3,
        Telop_Hide = 4,             // takes the caption away
        Telop_None = 5,             // no caption is shown
    };

    int m_movieId;                  // 0x66C the movie to play (a GmMovieID)
    mvMoviePlayer* m_player;        // 0x670
    u32 m_totalFrame;               // 0x674
    bool m_firstFrame;              // 0x678 set by init: the first proc returns 4
    int m_returnCode;               // 0x67C the screen to go back to
    int m_state;                    // 0x680 State
    int m_bgmId;                    // 0x684 the music that played before the movie
    int m_telopNo;                  // 0x688 the caption that comes next (also counts the frames of State_Release)
    int m_telopState;               // 0x68C TelopState
    int m_telopTexNo;               // 0x690 the texture of the caption
    u32 m_telopEnd;                 // 0x694 the frame the caption starts to fade out at
    u32 m_telopFadeIn;              // 0x698 the frames of the fade in (u32: the steps convert it as unsigned)
    u32 m_telopFadeOut;             // 0x69C
    float m_telopAlpha;             // 0x6A0
    float m_telopStep;              // 0x6A4
    bool m_screenShown;             // 0x6A8 the model of the movie is in the scene
    bool m_bgmStarted;              // 0x6A9
    bool m_isAdv;                   // 0x6AA a movie of the Adventure Mode: it has captions, music and its own camera
    u8 m_telopAlphaU8;              // 0x6AB
    gfFileIOHandle* m_handles;      // 0x6AC [0] the model, [1] the camera, [2] the parameters (Adventure Mode only)
    u8 m_unk6B0[4];
    MuObject* m_screen;             // 0x6B4 the model the movie is drawn on
    void* m_telopArcData;           // 0x6B8 the unpacked parameter archive
    gfArchive* m_telopArc;          // 0x6BC
    muMovieTelopData* m_telopData;  // 0x6C0
    nw4r::g3d::ResTex m_telopTex;   // 0x6C4
    MuObject* m_telop;              // 0x6C8 the model of the captions
    movieDraw* m_draw;              // 0x6CC

    virtual ~muProcMoviePlayer() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void playSE_CursorMove();

    void setup();
    void load();
    void initAdv();
    int procAdv();
    void postProcAdv();
    bool isLoading();
    void telopProc(int frame, bool end);
};
// HYPOTHESIS: the class ends after m_draw (the constructor is not in this unit).
static_assert(sizeof(muProcMoviePlayer) == 0x6D0, "Class is wrong size!");
