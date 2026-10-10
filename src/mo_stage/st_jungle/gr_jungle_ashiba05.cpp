#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba05::grJungleAshiba05(const char* taskName) : grJungleAshiba(taskName) {}

grJungleAshiba05* grJungleAshiba05::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba05* ground = new (Heaps::StageInstance) grJungleAshiba05(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba05::~grJungleAshiba05() {
}

void grJungleAshiba05::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_posGimmickWork != NULL) {
        getNodePosition(m_posGimmickWork, 0, "L5tatakuN");
    }
    if (m_posHashigoWork != NULL) {
        getNodePosition(m_posHashigoWork, 0, "L5Hashigo1");
        getNodePosition(&m_posHashigoWork[1], 0, "L5Hashigo2");
    }
}
