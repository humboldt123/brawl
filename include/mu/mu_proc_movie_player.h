#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <types.h>

// The screen that plays a movie of the Movies menu (the Movies and Adventure Movies screens put the number of the movie
// in it before they close). Only the part that those screens use is known so far.
class muProcMoviePlayer : public muProcMenu {
public:
    int m_movieId;                  // 0x66C the movie to play (a GmMovieID)
};
