#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>

class mvMoviePlayer;
namespace nw4r {
namespace g3d {
class ScnMdl;
}
} // namespace nw4r

// A movie (a THP file) that the adventure mode shows on a model of the stage, for example on a screen.
// setMovieFilename opens the movie, processDefault reads it step by step, playStart starts it once it is ready, and
// renderOpa plays a frame of it every frame.
class adVisual : public gfTask {
public:
    // The steps of processDefault (m_state).
    enum State {
        State_None = 0,             // there is no movie
        State_Open = 1,             // the file name is set, the player is made and opens the file
        State_Read = 2,             // HYPOTHESIS: the player reads the header of the file
        State_Prepare = 3,          // HYPOTHESIS: the player prepares the decoding
        State_Preload = 4,          // HYPOTHESIS: the player reads the first frames
        State_Wait = 5,             // waits until the movie is ready, then takes its size
        State_Ready = 6,            // the movie can be started (playStart)
        State_Play = 7,             // the movie plays
    };

    int m_state;                    // 0x40
    int m_unk44;                    // 0x44
    mvMoviePlayer* m_player;        // 0x48
    int m_frame;                    // 0x4C the frame that was played last
    int m_frameCount;               // 0x50 how many frames the movie has
    const char* m_filename;         // 0x54
    int m_x;                        // 0x58 where the movie is drawn and how big (the size of the movie once it is ready)
    int m_y;                        // 0x5C
    int m_width;                    // 0x60
    int m_height;                   // 0x64
    int m_unk68;                    // 0x68

    adVisual();
    virtual void processDefault();
    virtual void renderOpa();
    virtual ~adVisual();

    bool setMovieFilename(const char* filename);
    bool playStart();
    void setPause();
    void resetPause();
    void setLastFrame();
    void setScnMdl(nw4r::g3d::ScnMdl* model, int unk);
    void setVolume(int volume);
};
// HYPOTHESIS: the class ends after m_unk68 (the constructor sets nothing after it; nothing in the unit allocates one).
