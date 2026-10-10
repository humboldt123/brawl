#include <st_dxgarden/gr_dxgarden.h>

grDxGarden* grDxGarden::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGarden* ground = new (Heaps::StageInstance) grDxGarden(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGarden::grDxGarden(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grDxGarden::~grDxGarden() { }
