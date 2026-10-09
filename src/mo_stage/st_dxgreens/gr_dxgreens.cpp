#include <memory.h>

#include <st_dxgreens/gr_dxgreens.h>

grDxGreens* grDxGreens::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGreens* ground = new (Heaps::StageInstance) grDxGreens(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGreens::grDxGreens(const char* taskName) : grYakumono(taskName) {
    setupMelee();
}

grDxGreens::~grDxGreens() {
}
