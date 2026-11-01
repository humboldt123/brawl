#include <ad/ad_visual.h>
#include <memory.h>
#include <mv/mv_movie_player.h>
#include <string.h>
#include <types.h>

// mvMoviePlayer functions of main.dol that the shared header does not declare yet (unnamed in the symbols).
// include/mu/mu_boot_common.h declares the same ones for its muMovie handle.
extern "C" mvMoviePlayer* fn_8007E99C(mvMoviePlayer* player);                      // constructor
extern "C" void fn_8007EA78(mvMoviePlayer* player, int freeIt);                    // destructor
extern "C" void fn_8007EC08(mvMoviePlayer* player, const char* path, int unk1, int unk2, int unk3); // opens the file
// HYPOTHESIS: the steps of the reading of the file (each "is it done" test is followed by the start of the next step).
extern "C" bool fn_8007ED14(mvMoviePlayer* player);
extern "C" void fn_8007ED30(mvMoviePlayer* player);
extern "C" bool fn_8007EDA0(mvMoviePlayer* player);
extern "C" void fn_8007EDE8(mvMoviePlayer* player);
extern "C" void fn_8007EE24(mvMoviePlayer* player);
extern "C" bool fn_8007EEAC(mvMoviePlayer* player);
extern "C" void fn_8007EEFC(mvMoviePlayer* player);
extern "C" void fn_8007F15C(mvMoviePlayer* player, int unk);
extern "C" void fn_8007EF48(mvMoviePlayer* player, int* width, int* height);   // the size of the movie
extern "C" int fn_8007EF64(mvMoviePlayer* player);                             // 1 when the movie is ready to play
extern "C" int fn_8007EF68(mvMoviePlayer* player);                             // how many frames the movie has
extern "C" int fn_8007EF70(mvMoviePlayer* player, int x, int y, int width, int height); // plays a frame, returns the frame
extern "C" void fn_8007F148(mvMoviePlayer* player);                            // starts the movie
extern "C" void fn_8007F280(mvMoviePlayer* player);
extern "C" void fn_8007F2B4(mvMoviePlayer* player, nw4r::g3d::ScnMdl* model, int unk); // sets the model that shows the movie
extern "C" void fn_8007F2C8(mvMoviePlayer* player, int volume);

// The name of the movie file (setMovieFilename copies it here).
static char sFilename[32];

adVisual::adVisual()
    : gfTask("adVisual", Category_Visual, 0xE, 6, true), m_state(State_None), m_unk44(0), m_player(NULL), m_frame(0),
      m_frameCount(0), m_filename(NULL), m_x(0), m_y(8), m_width(640), m_height(464), m_unk68(0) { }

adVisual::~adVisual() {
    if (m_player != NULL) {
        fn_8007EA78(m_player, 1);
        m_player = NULL;
    }
}

void adVisual::processDefault() {
    switch (m_state) {
    case State_Open:
        if (m_filename != NULL) {
            // HYPOTHESIS: mvMoviePlayer has no data members (it keeps its state in globals), so the player is one byte
            // (as in mu_how_to_play.cpp); its constructor is the unnamed fn_8007E99C, called by hand.
            mvMoviePlayer* player = reinterpret_cast<mvMoviePlayer*>(new (Heaps::MenuInstance) u8);
            if (player != NULL) {
                player = fn_8007E99C(player);
            }
            m_player = player;
            fn_8007EC08(m_player, m_filename, 0, 1, 0);
            m_state = State_Read;
        }
        break;
    case State_Read:
        if (fn_8007ED14(m_player)) {
            m_frameCount = fn_8007EF68(m_player);
            fn_8007ED30(m_player);
            m_state = State_Prepare;
        }
        break;
    case State_Prepare:
        if (fn_8007EDA0(m_player)) {
            fn_8007EDE8(m_player);
            fn_8007EE24(m_player);
            m_state = State_Preload;
        }
        break;
    case State_Preload:
        if (fn_8007EEAC(m_player)) {
            fn_8007EEFC(m_player);
            fn_8007F15C(m_player, 0);
            m_state = State_Wait;
        }
        break;
    case State_Wait:
        if (fn_8007EF64(m_player) == 1) {
            fn_8007EF48(m_player, &m_width, &m_height);
            m_state = State_Ready;
        }
        break;
    // BYTES: the switch lists the two last states (its compare tree tests up to State_Play), State_None is not listed.
    case State_Ready:
    case State_Play:
        break;
    }
}

void adVisual::renderOpa() {
    switch (m_state) {
    case State_Play:
        if (m_player != NULL) {
            m_frame = fn_8007EF70(m_player, m_x, m_y, m_width, m_height);
        }
        break;
    }
}

bool adVisual::setMovieFilename(const char* filename) {
    if (m_player == NULL && m_state == State_None) {
        strcpy(sFilename, filename);
        m_filename = sFilename;
        m_state = State_Open;
        return true;
    }
    return false;
}

bool adVisual::playStart() {
    if (m_player != NULL && m_state == State_Ready) {
        fn_8007F148(m_player);
        m_state = State_Play;
        return true;
    }
    return false;
}

void adVisual::setPause() {
    m_player->setPause();
}

void adVisual::resetPause() {
    m_player->resetPause();
}

void adVisual::setLastFrame() {
    fn_8007F280(m_player);
}

void adVisual::setScnMdl(nw4r::g3d::ScnMdl* model, int unk) {
    fn_8007F2B4(m_player, model, unk);
}

void adVisual::setVolume(int volume) {
    fn_8007F2C8(m_player, volume);
}
