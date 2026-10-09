#include <st_halberd/gr_halberd.h>

grHalberd::grHalberd(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_frameWork = NULL;
    m_stateWork = NULL;
    setupMelee();
}

grHalberd::~grHalberd() {
}
