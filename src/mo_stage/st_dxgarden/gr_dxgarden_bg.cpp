#include <st_dxgarden/gr_dxgarden.h>

grDxGardenBg* grDxGardenBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGardenBg* ground = new (Heaps::StageInstance) grDxGardenBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGardenBg::~grDxGardenBg() { }
