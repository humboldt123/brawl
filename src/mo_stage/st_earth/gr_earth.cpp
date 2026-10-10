#include <memory.h>
#include <types.h>

#include <st_earth/gr_earth.h>

grEarth::grEarth(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grEarth::~grEarth() {
}
