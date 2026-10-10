#include <types.h>

#include <st_famicom/gr_famicom.h>

grFamicom::grFamicom(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grFamicom::~grFamicom() {
}
