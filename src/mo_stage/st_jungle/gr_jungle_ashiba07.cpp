#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba07::grJungleAshiba07(const char* taskName) : grJungleAshiba(taskName) {}

grJungleAshiba07* grJungleAshiba07::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba07* ground = new (Heaps::StageInstance) grJungleAshiba07(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba07::~grJungleAshiba07() {
}

void grJungleAshiba07::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_posGimmickWork != NULL) {
        getNodePosition(m_posGimmickWork, 0, "L7move1N");
        getNodePosition(&m_posGimmickWork[1], 0, "L7move2N");
    }
    if (m_posHashigoWork != NULL) {
        getNodePosition(m_posHashigoWork, 0, "L7Hashigo");
    }
}
