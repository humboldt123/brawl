#include <ad/ad_visual.h>
#include <memory.h>
#include <mv/mv_movie_player.h>
#include <string.h>
#include <types.h>

// The name of the movie file (setMovieFilename copies it here).
static char sFilename[32];

adVisual::adVisual()
    : gfTask("adVisual", Category_Visual, 0xE, 6, true), m_state(State_None), m_unk44(0), m_player(NULL), m_frame(0),
      m_frameCount(0), m_filename(NULL), m_x(0), m_y(8), m_width(640), m_height(464), m_unk68(0) { }

adVisual::~adVisual() {
    if (m_player != NULL) {
        delete m_player;
        m_player = NULL;
    }
}

void adVisual::processDefault() {
    switch (m_state) {
    case State_Open:
        if (m_filename != NULL) {
            m_player = new (Heaps::MenuInstance) mvMoviePlayer;
            m_player->openMovie(m_filename, 0, 1, 0);
            m_state = State_Read;
        }
        break;
    case State_Read:
        if (m_player->isOpened()) {
            m_frameCount = m_player->getTotalFrame();
            m_player->loadLastFrameInfo();
            m_state = State_Prepare;
        }
        break;
    case State_Prepare:
        if (m_player->isLoadedLastFrameInfo()) {
            m_player->closeLastFrameInfo();
            m_player->loadLastFrame();
            m_state = State_Preload;
        }
        break;
    case State_Preload:
        if (m_player->isLoadedLastFrame()) {
            m_player->closeLastFrame();
            m_player->prepare(0);
            m_state = State_Wait;
        }
        break;
    case State_Wait:
        if (m_player->getVideoState() == 1) {
            m_player->getVideoSize(&m_width, &m_height);
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
            m_frame = m_player->drawFrame(m_x, m_y, m_width, m_height);
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
        m_player->play();
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
    m_player->setLastFrame();
}

void adVisual::setScnMdl(nw4r::g3d::ScnMdl* model, int unk) {
    m_player->setScnMdl(model, unk);
}

void adVisual::setVolume(int volume) {
    m_player->setVolume(volume);
}
