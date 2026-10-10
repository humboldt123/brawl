#include <memory.h>

#include <st_palutena/gr_palutena.h>

grPalutena::grPalutena(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    unk154 = 0.0f;
    setupMelee();
}

grPalutena::~grPalutena() {
}

grPalutena* grPalutena::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutena* ground = new (Heaps::StageInstance) grPalutena(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}
