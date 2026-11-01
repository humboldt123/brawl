#pragma once

#include <StaticAssert.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ScnMdl;
}
} // namespace nw4r

// Shadows include/Brawl/Include/mv/mv_movie_player.h (adds the members that the shared header lacks).
// The player of the THP movies. The callers allocate one byte for it (new (heap) mvMoviePlayer), the state of the
// player lives in main.dol. The names are those of the map; the parameter types follow the register use of the callers.
class mvMoviePlayer {
public:
    mvMoviePlayer();
    ~mvMoviePlayer();

    static mvMoviePlayer* getInstance();
    // HYPOTHESIS: the three numbers after the path (all callers pass 0, 0, 0).
    void openMovie(const char* path, int unk1, int unk2, int unk3);
    bool isOpened();
    void loadLastFrameInfo();
    bool isLoadedLastFrameInfo();
    void closeLastFrameInfo();
    void loadLastFrame();
    bool isLoadedLastFrame();
    void closeLastFrame();
    void getVideoSize(int* width, int* height);
    int getVideoState();                                    // 1 when the movie is ready to play
    int getTotalFrame();
    int drawFrame(int x, int y, int width, int height);     // plays a frame, returns the frame
    void play();
    void prepare(int unk);
    bool isPause() const;
    void setPause();
    void resetPause();
    void setLastFrame();
    void setScnMdl(nw4r::g3d::ScnMdl* model, int unk);      // the model that shows the movie
    void setVolume(int volume);
};
