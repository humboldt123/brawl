#include <gr/gr_yakumono.h>
#include <types.h>

#include <st_dxbigblue/gr_dxbigblue.h>

// Deadstripped function to emit Ground::setMdlIndex
static void wrapper(grDxBigBlue* ths, s32 mdlIndex) {
    ths->setMdlIndex(mdlIndex);
}

grDxBigBlue::grDxBigBlue(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grDxBigBlue::~grDxBigBlue() { }
