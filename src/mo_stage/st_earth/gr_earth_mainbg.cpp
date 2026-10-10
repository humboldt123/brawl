#include <memory.h>
#include <types.h>

#include <st_earth/gr_earth.h>

grEarthMainBg* grEarthMainBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthMainBg* ground = new (Heaps::StageInstance) grEarthMainBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthMainBg::~grEarthMainBg() {
}
