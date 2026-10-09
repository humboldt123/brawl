#include <st_homerun/gr_homerun.h>

grHomerun::grHomerun(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grHomerun::~grHomerun() { }
