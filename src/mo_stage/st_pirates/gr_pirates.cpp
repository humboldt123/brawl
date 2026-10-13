#include <gr/gr_pirates.h>

grPirates::grPirates(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_ctrlFrame = NULL;
    unk15C = NULL;
    setupMelee();
}

grPirates::~grPirates() { }
